//
//  text.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 4/15/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef text_hpp
#define text_hpp

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"

using namespace GL; // TODO: remove

class Text {
    GLuint VAO, VBO;
public:
    Text();
    void renderText(const Shader& shader,
                    const std::string& text,
                    float x,
                    float y,
                    const float scale,
                    glm::vec3& color);
};

#endif /* text_hpp */
