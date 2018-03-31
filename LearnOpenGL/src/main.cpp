//
//  main.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/7/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <iostream>

#include "render.hpp"

using std::string;

int main(int argc, const char * argv[]) {
    try {
        Render render;
        render.renderLoop();
        glfwTerminate();
        return 0;
    } catch (const string& err) {
        std::cerr << "Error: " << err << std::endl;
    } catch (const char *err) {
        std::cerr << "Error: " << err << std::endl;
    }
    glfwTerminate();
    return -1;
}
