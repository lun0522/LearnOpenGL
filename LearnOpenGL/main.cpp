//
//  main.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/7/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <iostream>

#include "render.h"

int main(int argc, const char * argv[]) {
  try {
    Render render;
    render.MainLoop();
    glfwTerminate();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  glfwTerminate();
  return -1;
}
