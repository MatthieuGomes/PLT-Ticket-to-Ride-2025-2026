#include <iostream>

// The following lines are here to check that SFML is installed and working
#include <SFML/Graphics.hpp>
#include "client/Client.h"  
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>

#include "tui/Terminal.h"
#include "tui/TUIManager.h"

void testSFML() {
    sf::Texture texture;
}
// end of test SFML

static void getTerminalSize(int& cols, int& rows) {
    cols = 100;
    rows = 30;

    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_col > 0) {
            cols = static_cast<int>(ws.ws_col);
        }
        if (ws.ws_row > 0) {
            rows = static_cast<int>(ws.ws_row);
        }
    }
}

using namespace std;

int main(int argc,char* argv[])
{
    cout << "Client application started." << endl;
    if (argc<=1) {
        cout << "So far, nothing here..." << endl;
        cout << "Testing map" << endl;
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
    if (strcmp(argv[1],"tui")==0) {
        int cols = 100;
        int rows = 30;
        getTerminalSize(cols, rows);

        tui::Terminal term;
        tui::TUIManager manager(&term, cols, rows);
        manager.runMainLoop();

        return EXIT_SUCCESS;
    }



    return EXIT_SUCCESS;

}
