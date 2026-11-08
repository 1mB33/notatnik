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
};

State::State() {}

static State g_State = {};

bool CreateFile( filesystem::path &path, const string &content )
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

bool ReadFile( filesystem::path &path, string &content )
{
    ifstream file( path, ios::ate );
    if ( !file.is_open() || file.bad() )
    {
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
        return false;
    }
    return true;
}

bool DbMan::InitializeState()
{
    try
    {
        g_State.NotesStoragePath = ".";

        g_State.Config = make_shared<sqlpp::postgresql::connection_config>();

        g_State.Config->dbname   = "notepad_dev_db";
        g_State.Config->user     = "dev";
        g_State.Config->password = "dev";
#if defined( _DEBUG )
        g_State.Config->debug = true;
#endif

        g_State.Db = State::Connection( g_State.Config, 5 );
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
        using namespace filesystem;

        path pathDir  = g_State.NotesStoragePath.string() + "/" + to_string( ownerId );
        path pathNote = pathDir.string() + "/" + noteName + ".txt";


        if ( !filesystem::exists( pathDir ) &&
             ( !filesystem::create_directory( pathDir ) || !CreateFile( pathNote, "" ) ) )
        {
            return false;
        }

        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::postgresql::insert_into( tab )
                                          .set( tab.localPath = pathNote.string(), tab.ownerId = ownerId )
                                          .returning( tab.id ) );
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

bool DbMan::ListNotes( std::vector<std::string> &notes, int64_t ownerId )
{
    try
    {
        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::select( tab.localPath ).from( tab ).where( tab.ownerId == ownerId ) );

        notes.resize( res.size() );

        size_t i = 0;
        for ( const auto &n : res )
        {
            notes[ i ] = n.localPath;
            notes[ i ] = notes[ i ].substr( notes[ i ].find_last_of( '/' ) + 1 );
        }

        return true;
    }
    catch ( ... )
    {
        return false;
    }
}

bool DbMan::ReadNote( const std::string &noteName, int64_t ownerId, std::string &content )
{
    try
    {
        using namespace filesystem;
        path pathNote = g_State.NotesStoragePath.string() + "/" + to_string( ownerId ) + "/" + noteName + ".txt";

        auto &tab = g_State.NoteTab;
        auto  res = g_State.Db.get()( sqlpp::select( tab.localPath )
                                          .from( tab )
                                          .where( tab.ownerId == ownerId and tab.localPath == pathNote.string() ) );

        if ( res.empty() )
        {
            return false;
        }

        return filesystem::exists( pathNote ) && ReadFile( pathNote, content );
    }
    catch ( ... )
    {
        return false;
    }
}

bool DbMan::UpdateNote( const std::string &noteName, int64_t ownerId, const std::string &content )
{
    try
    {
        using namespace filesystem;
        path pathNote = g_State.NotesStoragePath.string() + "/" + to_string( ownerId ) + "/" + noteName + ".txt";
        return filesystem::exists( pathNote ) && CreateFile( pathNote, content );
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
        path pathNote = g_State.NotesStoragePath.string() + "/" + to_string( ownerId ) + "/" + noteName + ".txt";

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
