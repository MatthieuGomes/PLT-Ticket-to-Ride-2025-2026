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
        mapState::Station * b_stat = stations[1];
        cout << "Testing station:" << b_stat->data->name << endl;
        std::vector<mapState::Road*> roads = map.listRoads();
        cout << "Testing road color:" << roads[2]->data->color << endl;
        cout << "print berlin:" << map.getStationByName("Berlin")->data->name << endl;
        mapState::Road* roadptr = map.getRoad(map.getStationByName("Berlin"), map.getStationByName("Rome"));
        if (roadptr == nullptr) {
            cout << "getroad search failed" << endl;
        } else {
            cout << "getroad berlin-rome: " << roadptr->data->ID << endl;
        }
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
