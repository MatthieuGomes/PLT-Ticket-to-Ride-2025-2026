#include <iostream>
#include "Map.h"

// player mock for tests
class Player {
public:
    std::string name;
    Player(const std::string& n) : name(n) {}
};

int main() {
    std::cout << "==== PLT MAP TEST ====\n\n";

    // creates map
    Map map;
    map.generateMap();

    map.printMap();

    /*std::cout << "\n--- Station list ---\n";
    for (auto* station : map.listStations()) {
        std::cout << " - " << station->data->name
                  << " (ID " << station->data->ID << ")\n";
    }

    std::cout << "\n--- Road list ---\n";
    for (auto* road : map.listRoads()) {
        std::cout << " - " << road->data->ID
                  << " (" << road->data->color << ", "
                  << road->data->length << " pieces) connects "
                  << road->data->endpoints[0]->name << " ↔ "
                  << road->data->endpoints[1]->name << "\n";
    }*/

    // searches station
    std::cout << "\n--- Station search test ---\n";
    auto* berlin = map.getStationByName("Berlin");
    if (berlin) {
        std::cout << "Station found: " << berlin->data->name << "\n";
    } else {
        std::cout << "Station not found!\n";
    }

    // adjacent station test
    std::cout << "\n--- Stations adjacent to Paris ---\n";
    auto* paris = map.getStationByName("Paris");
    if (paris) {
        auto adj = map.getAdjacentStations(paris);
        for (auto* s : adj) {
            std::cout << " - " << s->data->name << "\n";
        }
    }

    // road claim test
    std::cout << "\n--- Road claiming test ---\n";
    Player p1("Alice");
    auto* madrid = map.getStationByName("Madrid");
    if (map.claimRoad(&p1, paris->data, madrid->data)) {
        std::cout << "Alice has taken posession of the Paris <-> Madrid road\n";
    }

    //check changes after claim
    map.printMap();

    // shortest path rest
    std::cout << "\n--- Shortest path Paris -> Rome ---\n";
    auto* rome = map.getStationByName("Rome");
    Path path = map.findShortestPath(paris->data, rome->data);

    std::cout << "Path found: ";
    for (auto* s : path.STATIONS) {
        std::cout << s->data->name;
        if (s != path.STATIONS.back()) std::cout << " -> ";
    }
    std::cout << "\nTotal length: " << path.TOTALLENGTH
              << "\nNumber of edges: " << path.NUMEDGES << "\n";

    std::cout << "\n==== END OF TEST ====\n";

    //test tostring
    std::string snapshot = map.toString();
    
    // check formatted state
    std::cout << "\n[STATE SNAPSHOT]\n" << snapshot << std::endl;
    return 0;
}
