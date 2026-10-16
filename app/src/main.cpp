#include <iostream>
#include <manager.hpp>
#include <thread>

using namespace std;

int main( int, char *[] )
{
    using namespace chrono_literals;

    cout << "Starting" << endl;

    this_thread::sleep_for( 10s );

    cout << "Hello app: " << fn() << endl;

    this_thread::sleep_for( 10s );

    cout << "Exiting" << endl;
    return 0;
}
