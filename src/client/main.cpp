#include <iostream>

// The following lines are here to check that SFML is installed and working
#include <SFML/Graphics.hpp>
#include "client/Client.h"  
#include <cstring>

void testSFML() {
    sf::Texture texture;
}
// end of test SFML

using namespace std;

int main(int argc,char* argv[])
{
    cout << "Client application started." << endl;
    if (argc<=1) {
        cout << "So far, nothing here..." << endl;
        cout << "Testing map" << endl;
        mapState::MapState map;
        map.printMapState();
        std::vector<mapState::Station*> stations = map.listStations();
        cout << "Testing station:" << &stations[1] << endl;
        std::vector<mapState::Road*> roads = map.listRoads();
        cout << "Testing road:" << &roads[2] << endl;
        return EXIT_FAILURE;
    }
    client::Client client = client::Client();
    if (strcmp(argv[1],"hello")==0) {
        client.helloWorld();
        return EXIT_SUCCESS;
    }
    if (strcmp(argv[1],"state")==0) {
        client.printState();
        return EXIT_SUCCESS;
    }



    return EXIT_SUCCESS;

}
