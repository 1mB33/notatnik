#include <iostream>
#include <manager.hpp>

using namespace std;

int main( int, char *[] )
{
    cout << "Starting" << endl;

    DbMan::InitializeState();

    cout << "Exiting" << endl;
    return 0;
}
