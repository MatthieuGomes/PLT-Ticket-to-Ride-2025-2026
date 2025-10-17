#include <iostream>

// The following lines are here to check that SFML is installed and working
#include <SFML/Graphics.hpp>
#include <string.h>

#include "client.h"
#include "state/State.h"


void testSFML() {
    sf::Texture texture;
}
// end of test SFML


using namespace std;

int main(int argc,char* argv[])
{
    client::Client client;
    state::State state;
    if(argc<1){
        cout << "No arguments provided." << endl;
        return EXIT_FAILURE;
    }
    client = client::Client();
    state = state::State();

    if(strcmp(argv[1], "hello") == 0) {
        client.helloWorld();
        return EXIT_SUCCESS;
    } else if(strcmp(argv[1], "state") == 0) {
        client.printState();
        return EXIT_SUCCESS;
    } else {
        cout << "Unknown argument: " << argv[1] << endl;
        return EXIT_FAILURE;
    }
}