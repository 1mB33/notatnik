#include <httplib.h>
#include <manager.hpp>
#include <single_include/nlohmann/json.hpp>

using namespace std;

void HelloAPI( const httplib::Request &, httplib::Response &res )
{
    res.set_content( "Hello World!", "text/plain" );
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

        int64_t        ownerId = -1;
        vector<string> notes;
        nlohmann::json requestJson( nlohmann::json::parse( req.body ) );
        nlohmann::json responseJson;

        if ( !requestJson.contains( "ownerId" ) )
        {
            res.status = httplib::BadRequest_400;
            return;
        }

        ownerId = requestJson[ "ownerId" ];
        if ( !DbMan::ListNotes( notes, ownerId ) )
        {
            res.status = httplib::Forbidden_403;
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
    DbMan::InitializeState();
    httplib::Server().Get( "/hi", HelloAPI ).Get( "/list", ListNotes ).listen( "0.0.0.0", 8000 );
    return 0;
}
