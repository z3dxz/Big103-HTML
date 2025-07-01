#include <iostream>
#include <string>
#include "window.h"
#include "big103.h"

int main(int argc, char *argv[]){

    // Initialize Big103
    
    Initialize();

    bool ui = true;
    bool get = false;
    bool gett = false;
    bool geta = false;

    int i = 0;
    while (i < argc) {
        if(i == 1) {
            if(std::string(argv[i]) == "get") {
                ui = false;
                get = true;
            }
            if(std::string(argv[i]) == "title") {
                ui = false;
                gett = true;
            }
            if(std::string(argv[i]) == "artist") {
                ui = false;
                geta = true;
            }

        }

        i++;
    }

    if(get) {
        std::string at, aa;
        std::string r = request(&at, &aa);
        std::cout << at << " - " << aa;
    }

    if(gett) {
        std::string at, aa;
        std::string r = request(&at, &aa);
        std::cout << at;
    }

    if(geta) {
        std::string at, aa;
        std::string r = request(&at, &aa);
        std::cout << aa;
    }

    if(ui) {
        CreateSFMLWindow();
    }

    return 0;
}
