//
//  text.cpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 4/15/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include "loader.hpp"
#include "text.hpp"

using glm::vec2;

Text::Text() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * (2 + 2) * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Text::renderText(const Shader& shader,
                      const std::string& text,
                      float x,
                      float y,
                      const float scale,
                      glm::vec3& color) {
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    shader.setInt("text", 0);
    shader.setVec3("color", color);
    glBindVertexArray(VAO);

    for (auto c = text.begin(); c != text.end(); ++c) {
        Loader::Character ch = Loader::loadCharacter(*c);
        
        float xPos = x + ch.bearing.x * scale;
        float yPos = y - (ch.size.y - ch.bearing.y) * scale;
        vec2 size = vec2(ch.size) * scale;
        x += ch.advance * scale;
        
        float vertexAttrib[6][4] = {
            { xPos,          yPos + size.y,  0.0, 0.0 },
            { xPos,          yPos,           0.0, 1.0 },
            { xPos + size.x, yPos,           1.0, 1.0 },
            
            { xPos,          yPos + size.y,  0.0, 0.0 },
            { xPos + size.x, yPos,           1.0, 1.0 },
            { xPos + size.x, yPos + size.y,  1.0, 0.0 },
        };
        
        glBindTexture(GL_TEXTURE_2D, ch.texture);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexAttrib), vertexAttrib);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
