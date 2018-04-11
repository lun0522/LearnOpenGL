//
//  render.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef render_hpp
#define render_hpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Render {
    GLFWwindow *window;
    void processKeyboardInput();
public:
    Render();
    void renderLoop();
};

#endif /* render_hpp */
