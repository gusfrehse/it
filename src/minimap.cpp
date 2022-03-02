#include "minimap.h"

#include <cstdio>

int init_minimap_things(minimap_settings &set) {
    // Create a framebuffer
    glGenFramebuffers(1, &(set.framebuffer_name));
    glBindFramebuffer(GL_FRAMEBUFFER, set.framebuffer_name);
    
    // Create the color attachment texture we are rendering to
    glGenTextures(1, &(set.color_texture));
    glBindTexture(GL_TEXTURE_2D, set.color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, set.framebuffer_width,
                 set.framebuffer_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         set.color_texture, 0);
    
    // Create Depth and Stencil texture
    glGenTextures(1, &(set.depth_stencil_texture));
    glBindTexture(GL_TEXTURE_2D, set.depth_stencil_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, set.framebuffer_width,
                 set.framebuffer_height, 0, GL_DEPTH_STENCIL,
                 GL_UNSIGNED_INT_24_8, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         set.depth_stencil_texture, 0);
    

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        std::fprintf(stderr, "FRAMEBUFFER NOT COMPLETE\n");
        switch (result) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::fprintf(stderr, "ATTACHMENT INCOMPLETE\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::fprintf(stderr, "ATTACHMENT MISSING\n");
            break;
        default:
            std::fprintf(stderr, "OTHER");
            break;
        }

        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 1;
}