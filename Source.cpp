#include <iostream>
#include <memory>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "imgui-SFML.h"
#include <imgui-SFML.h>





//testing Vec2. Remove later
#include "Vec2.h"
#include "Game.h"

int main() {

    std::string enter;
    std::cout << "Enter file to run" << std::endl;
    std::cin >> enter;

    Game game(enter);
    




    return 0;
}