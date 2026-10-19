#include <iostream>
#include <manager.hpp>

using namespace std;

int main( int, char *[] )
{
    cout << "Starting" << endl;

    DbMan::InitializeState();
    for ( int i = 0; i < 2; ++i )
    {
        DbMan::CreateNote();
    }

    cout << "Exiting" << endl;
    return 0;
}
