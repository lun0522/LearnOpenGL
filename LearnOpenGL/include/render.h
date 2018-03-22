//
//  render.h
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/13/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef render_h
#define render_h

#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>

class Render {
    GLFWwindow *window;
    void processKeyboardInput();
public:
    Render();
    void renderLoop();
};

#endif /* render_h */
