#include <iostream>
#include "window.h"

int main(int argc, char *argv[]){

    //CreateSFMLWindow();

    std::cout << "arguments\n";
    std::cout << argc << "\n";
    int i = 0;
    while (i < argc) {
        std::cout << "Argument " << i + 1  << ": " << argv[i] << std::endl;
        i++;
    }

    return 0;
}
