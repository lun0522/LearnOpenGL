//
//  loader.hpp
//  LearnOpenGL
//
//  Created by Pujun Lun on 3/31/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef loader_hpp
#define loader_hpp

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Loader {
    struct Character {
        GLuint texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        GLuint advance;
    };
    void set2DTexParameter(const GLenum wrapMode, const GLenum interpMode);
    Character& loadCharacter(const char character);
    GLuint loadTexture(const std::string& path, const bool gammaCorrection);
    GLuint loadCubemap(const std::string& path,
                       const std::vector<std::string>& filename,
                       const bool gammaCorrection);
}

#endif /* loader_hpp */
