#include <iostream>
#include <string>
#include "window.h"
#include "big103.h"

int main(int argc, char *argv[]){

    // Initialize Big103
    
    Initialize();

    bool ui = true;
    bool get = false;

    int i = 0;
    while (i < argc) {
        if(i == 1) {
            std::string getstr = "get";
            if(std::string(argv[i]) == getstr) {
                ui = false;
                get = true;
            }
        }

        i++;
    }

    if(get) {
        std::string at, aa;
        std::string r = request(&at, &aa);
        std::cout << at << " - " << aa;
    }

    if(ui) {
        CreateSFMLWindow();
    }

    return 0;
}
