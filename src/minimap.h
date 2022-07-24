#ifndef IT_MINIMAP_H
#define IT_MINIMAP_H

#include <GL/glew.h>
#include <glm/ext/quaternion_common.hpp>

#include "maze.h"

class Minimap {
private:
public:
    GLuint fb_name;
    GLuint color_texture;
    GLuint depth_stencil_texture;
    GLuint depth_renderbuffer;
    GLuint vao;

    int fb_width;
    int fb_height;



public:
    Minimap(int fb_width, int fb_height);

    int create();

    // TODO: change quad_vao to vao (lol)
    void render(GLuint quad_vao, GLuint program_ids[], GLint mvp_location, const maze& m);
};

#endif

