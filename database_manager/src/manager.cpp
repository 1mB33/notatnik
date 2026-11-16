#include "manager.hpp"
#include "schema.h"

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/default_value.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/null.h>
#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/postgresql/postgresql.h>
#include <sqlpp11/sqlpp11.h>
#include <string>

using namespace std;

#if 1
inline void debugLog( const string &msg )
{
    cout << msg << endl;
}
#else

inline void debugLog( const string & ) {}

#endif // defined (_DEBUG)

static std::string env_or( const char *name, const char *fallback )
{
    const char *v = std::getenv( name );
    return v ? v : fallback;
}

struct State
{
    using Connection        = sqlpp::postgresql::connection_pool;
    using Connection_config = std::shared_ptr<sqlpp::postgresql::connection_config>;
    using Path              = std::filesystem::path;

    State();
    ~State() noexcept = default;

    State( State && ) noexcept                 = default;
    State &operator=( State && ) noexcept      = default;
    State( const State & ) noexcept            = delete;
    State &operator=( const State & ) noexcept = delete;

    Connection_config          Config;
    Connection                 Db;
    notepad_dev_db::PublicNote NoteTab;
    Path                       NotesStoragePath;
    unordered_map<char, bool>  Banned;
};

State::State() {}

static State g_State = {};

bool CreateFile( const filesystem::path &path, const string &content )
{
    ofstream file( path, ios_base::trunc );
    if ( !file.is_open() || file.bad() )
    {
        return false;
    }

    file.write( content.c_str(), content.size() );

    file.close();
    if ( file.bad() )
    {
        return false;
    }
    return true;
}

bool ReadFile( const filesystem::path &path, string &content )
{
    ifstream file( path, ios::ate );
    if ( !file.is_open() || file.bad() )
    {
        debugLog("isnt open, or bad");
        return false;
    }
    const auto fileEnd = file.tellg();
    file.seekg( ios::beg );
    const auto fileLen = fileEnd - file.tellg();
    content.resize( fileLen );

    file.read( content.data(), content.size() );

    file.close();
    if ( file.bad() )
    {
        debugLog("is bad at the end");
        return false;
    }
    return true;
}

void SanatizeString( string &str, const unordered_map<char, bool> &banned )
{
    string sanatized;

    for ( const char c : str )
    {
        const auto bannedC = banned.find( c );

        if ( bannedC != banned.end() && bannedC->second )
        {
            continue;
        }

        sanatized += c;
    }

    str.swap( sanatized );
}

bool DbMan::InitializeState()
{
    try
    {
        g_State.NotesStoragePath = "/var/lib/notes/data";

        g_State.Config = make_shared<sqlpp::postgresql::connection_config>();

        g_State.Config->host     = env_or( "DB_HOST", "127.0.0.1" );
        g_State.Config->dbname   = env_or( "DB_NAME", "notepad_dev_db" );
        g_State.Config->user     = env_or( "DB_USER", "dev" );
        g_State.Config->password = env_or( "DB_PASSWORD", "dev" );
        g_State.Config->debug    = true;

        g_State.Db = State::Connection( g_State.Config, 5 );

        g_State.Banned = { { '\\', false } };

        filesystem::create_directories(g_State.NotesStoragePath);

        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

bool DbMan::CreateNewNote( const string &noteName, int64_t ownerId, int64_t *noteId )
{
    try
    {
        debugLog( "creating new note" );
        using namespace filesystem;

        debugLog( "sanatizing" );
        string sanatized = noteName;
        SanatizeString( sanatized, g_State.Banned );

        debugLog( "path create" );
        path pathDir  = g_State.NotesStoragePath.string() + "/" + to_string( ownerId );
        path pathNote = pathDir.string() + "/" + sanatized + ".txt";


        debugLog( "file check" );
        if ( !filesystem::exists( pathDir ) &&
             ( !filesystem::create_directory( pathDir ) || !CreateFile( pathNote, "" ) ) )
        {
            return false;
        }

        debugLog( "db call" );
        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::postgresql::insert_into( tab )
                                          .set( tab.localPath = pathNote.string(), tab.ownerId = ownerId )
                                          .returning( tab.id ) );
        debugLog( "result" );
        if ( noteId )
        {
            *noteId = res.front().id.value();
        }
        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

bool DbMan::ListNotes( unordered_map<string, int64_t> &notes, int64_t ownerId )
{
    try
    {
        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::select( tab.localPath, tab.id ).from( tab ).where( tab.ownerId == ownerId ) );

        for ( const auto &n : res )
        {
            auto note = n.localPath.value();

            const auto lastSlash = note.find_last_of( '/' );
            const auto lastDot   = note.find_last_of( '.' );
            note                 = note.substr( lastSlash + 1, lastDot - lastSlash - 1 );
            notes[ note ]        = n.id.value();
        }

        return true;
    }
    catch ( exception &e )
    {
        cerr << e.what() << endl;
        return false;
    }
}

bool DbMan::ReadNote( int64_t noteId, int64_t ownerId, std::string &content )
{
    try
    {
        using namespace filesystem;


        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::select( tab.localPath )
                                          .from( tab )
                                          .where( tab.ownerId == ownerId and tab.id == noteId ) );

        if ( res.empty() )
        {
            debugLog( "sql: empty" );
            return false;
        }
        debugLog( res.front().localPath.value() );
        
        const auto doesExsists = filesystem::exists( res.front().localPath.value() );
        debugLog(string() + static_cast<char>(('0' + doesExsists)));

        return doesExsists && ReadFile( filesystem::path( res.front().localPath.value() ), content );
    }
    catch ( const exception& e )
    {
        debugLog(e.what());
        return false;
    }
}

bool DbMan::UpdateNote( int64_t noteId, int64_t ownerId, const std::string &content )
{
    try
    {
        using namespace filesystem;
        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::select( tab.localPath )
                                          .from( tab )
                                          .where( tab.ownerId == ownerId and tab.id == noteId ) );

        if ( res.empty() )
        {
            debugLog("sql: empty");
            return false;
        }
        debugLog(res.front().localPath.value());
        return filesystem::exists( res.front().localPath.value() ) &&
               CreateFile( filesystem::path(res.front().localPath.value()), content );
    }
    catch ( ... )
    {
        return false;
    }
}

bool DbMan::DeleteNote( const std::string &noteName, int64_t ownerId )
{
    try
    {
        using namespace filesystem;
        string sanatized = noteName;
        SanatizeString( sanatized, g_State.Banned );

        path pathNote = g_State.NotesStoragePath.string() + "/" + to_string( ownerId ) + "/" + sanatized + ".txt";

        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::select( tab.localPath )
                                          .from( tab )
                                          .where( tab.ownerId == ownerId and tab.localPath == pathNote.string() ) );

        if ( res.empty() )
        {
            return false;
        }

        return filesystem::exists( pathNote ) && filesystem::remove( pathNote );
    }
    catch ( ... )
    {
        return false;
    }
}
