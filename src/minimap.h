#ifndef IT_MINIMAP_H
#define IT_MINIMAP_H

#include <gl/glew.h>

struct minimap_settings {
    GLuint framebuffer_name;
    GLuint color_texture;
    GLuint depth_stencil_texture;
    GLuint depth_renderbuffer;
    int framebuffer_width;
    int framebuffer_height;
};

int init_minimap_things(minimap_settings &set);

#endif