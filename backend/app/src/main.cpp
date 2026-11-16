#include <cstdint>
#include <httplib.h>
#include <manager.hpp>
#include <single_include/nlohmann/json.hpp>

using namespace std;

#if defined( _DEBUG )
inline void debugLog( const string &msg )
{
    cout << msg << endl;
}
#else

inline void debugLog( const string & ) {}

#endif // defined (_DEBUG)

void HelloAPI( const httplib::Request &, httplib::Response &res )
{
    res.set_content( "Hello World!", "text/plain" );
}

void CreateNote( const httplib::Request &req, httplib::Response &res )
{
    try
    {
        if ( req.body.empty() )
        {
            res.status = httplib::BadRequest_400;
            return;
        }

        int64_t        ownerId  = -1;
        string         noteName = {};
        int64_t        noteId   = -1;
        nlohmann::json requestJson( nlohmann::json::parse( req.body ) );
        nlohmann::json responseJson;

        debugLog( "Checking for ownerId" );
        if ( !requestJson.contains( "ownerId" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }
        ownerId = requestJson[ "ownerId" ];

        debugLog( "Checking for noteName" );
        if ( !requestJson.contains( "noteName" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }
        noteName = requestJson[ "noteName" ];

        debugLog( "requesting new note" );
        if ( !DbMan::CreateNewNote( noteName, ownerId, &noteId ) )
        {
            res.status = httplib::InternalServerError_500;
            return;
        }
        responseJson[ "noteId" ] = noteId;

        res.set_content( responseJson.dump(), "application/json" );
    }
    catch ( exception &e )
    {
        cout << "E: " << e.what() << endl;
        res.status = httplib::InternalServerError_500;
        return;
    }
}

void ReadNote( const httplib::Request &req, httplib::Response &res )
{
    try
    {
        if ( req.body.empty() )
        {
            res.status = httplib::BadRequest_400;
            return;
        }

        int64_t        noteId  = -1;
        int64_t        ownerId = -1;
        string         noteContent;
        nlohmann::json requestJson( nlohmann::json::parse( req.body ) );
        nlohmann::json responseJson;

        if ( !requestJson.contains( "ownerId" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }
        ownerId = requestJson[ "ownerId" ];

        if ( !requestJson.contains( "noteId" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }
        noteId = requestJson[ "noteId" ];

        if ( !DbMan::ReadNote( noteId, ownerId, noteContent ) )
        {
            res.status = httplib::InternalServerError_500;
            return;
        }
        responseJson[ "noteContent" ] = noteContent;

        res.set_content( responseJson.dump(), "application/json" );
    }
    catch ( exception &e )
    {
        cout << "E: " << e.what() << endl;
        res.status = httplib::InternalServerError_500;
        return;
    }
}

void UpdateNote( const httplib::Request &req, httplib::Response &res )
{
    try
    {
        if ( req.body.empty() )
        {
            res.status = httplib::BadRequest_400;
            return;
        }

        int64_t        noteId  = -1;
        int64_t        ownerId = -1;
        string         noteContent;
        nlohmann::json requestJson( nlohmann::json::parse( req.body ) );
        nlohmann::json responseJson;

        if ( !requestJson.contains( "ownerId" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }
        ownerId = requestJson[ "ownerId" ];

        if ( !requestJson.contains( "noteId" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }
        noteId = requestJson[ "noteId" ];

        if ( !requestJson.contains( "noteContent" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }
        noteContent = requestJson[ "noteContent" ];

        if ( !DbMan::UpdateNote( noteId, ownerId, noteContent ) )
        {
            res.status = httplib::InternalServerError_500;
            return;
        }

        res.set_content( responseJson.dump(), "application/json" );
    }
    catch ( exception &e )
    {
        cout << "E: " << e.what() << endl;
        res.status = httplib::InternalServerError_500;
        return;
    }
}

void ListNotes( const httplib::Request &req, httplib::Response &res )
{
    try
    {
        if ( req.body.empty() )
        {
            res.status = httplib::BadRequest_400;
            return;
        }

        int64_t                        ownerId = -1;
        unordered_map<string, int64_t> notes;
        nlohmann::json                 requestJson( nlohmann::json::parse( req.body ) );
        nlohmann::json                 responseJson;

        if ( !requestJson.contains( "ownerId" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }

        ownerId = requestJson[ "ownerId" ];
        if ( !DbMan::ListNotes( notes, ownerId ) )
        {
            res.status = httplib::InternalServerError_500;
            return;
        }
        responseJson[ "notes" ] = notes;

        res.set_content( responseJson.dump(), "application/json" );
    }
    catch ( exception &e )
    {
        cout << "E: " << e.what() << endl;
        res.status = httplib::InternalServerError_500;
        return;
    }
}

int main( int, char *[] )
{
    cout << "Starting notatnik" << endl;
    thread(
        []()
        {
            DbMan::InitializeState();
            if ( httplib::Server()
                     .set_logger(
                         []( const httplib::Request &req, const httplib::Response &res )
                         {
                             std::cout << req.method << " " << req.path << " -> " << res.status << std::endl;
                         } )
                     .Get( "/hi", HelloAPI )
                     .Post( "/create", CreateNote )
                     .Post( "/read", ReadNote )
                     .Patch( "/update", UpdateNote )
                     .Post( "/list", ListNotes )
                     .listen( "0.0.0.0", 8000, 0 ) )
            {
                cerr << "listen failed" << endl;
            }
        } )
        .join();
    return 0;
}
