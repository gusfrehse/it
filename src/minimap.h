#ifndef IT_MINIMAP_H
#define IT_MINIMAP_H

#include <gl/glew.h>

class Minimap {
private:
    GLuint fb_name;
    GLuint color_texture;
    GLuint depth_stencil_texture;
    GLuint depth_renderbuffer;
    int fb_width;
    int fb_height;


public:
    Minimap(int fb_width, int fb_height);

    int create();

    void render(GLuint quad_vao, GLuint program_ids[]);
};

#endif

