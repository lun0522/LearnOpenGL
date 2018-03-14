//
//  main.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/7/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <iostream>

#include "render.h"

using std::string;

int main(int argc, const char * argv[]) {
    try {
        Render render;
        render.renderLoop();
        glfwTerminate();
        return 0;
    } catch (string err) {
        std::cerr << err << std::endl;
        glfwTerminate();
        return -1;
    }
}
