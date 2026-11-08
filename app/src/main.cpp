#include <httplib.h>
#include <iostream>
#include <manager.hpp>


using namespace std;

int main( int, char *[] )
{
    httplib::Server server;
    server.Get( "/hi",
                []( const httplib::Request &, httplib::Response &res )
                {
                    res.set_content( "Hello World!", "text/plain" );
                } );
    server.listen( "0.0.0.0", 80 );

    return 0;
}

// int main( int, char *[] )
// {
//     cout << "Starting" << endl;
//
//     DbMan::InitializeState();
//
//     vector<string> notes;
//     string         note;
//     cout << "Result: " << DbMan::CreateNewNote( "Nowa notatka", 1, nullptr ) << endl;
//     cout << "Result: " << DbMan::UpdateNote( "Nowa notatka", 1, "Hello skolim" ) << endl;
//
//     cout << "Result: " << DbMan::ReadNote( "Nowa notatka", 1, note ) << endl;
//     cout << note << endl;
//
//     cout << "Result: " << DbMan::ListNotes( notes, 1 ) << endl;
//     for ( const auto &n : notes )
//     {
//         cout << n << endl;
//     }
//
//     cout << "Exiting" << endl;
//     return 0;
// }
