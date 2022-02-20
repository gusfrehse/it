#ifndef IT_SHADERS_H
#define IT_SHADERS_H

#include <gl/glew.h>

#define SHADER_CODE(...) #__VA_ARGS__

enum shader_names { SHADER_BASIC_VERT, SHADER_BASIC_FRAG, SHADER_COUNT };
enum program_names { PROGRAM_BASIC, PROGRAM_COUNT };

/**
 * @brief Shader information before compilation.
 * 
 */
struct pre_shader {
    GLenum type;
    program_names program;
    const char* source;
};

const pre_shader pre_shaders[SHADER_COUNT] = {
    {
        // SHADER_BASIC_VERT
        GL_VERTEX_SHADER,
        PROGRAM_BASIC,
        "#version 450 core\n" SHADER_CODE(
        layout (location = 0) in vec3 attrib_pos;
        layout (location = 1) in vec3 attrib_color;

        out vec4 vertex_color;
        out vec3 pos;

        uniform mat4 mvp;
        uniform vec3 cam_pos;

        void main(){
            vertex_color = vec4(attrib_color, 1.0);
            vec4 position = mvp * vec4(attrib_pos, 1.0);
            pos = attrib_pos;
            gl_Position = position;
        }),
    },
    {
        // SHADER_BASIC_FRAG
        GL_FRAGMENT_SHADER,
        PROGRAM_BASIC,
        "#version 450 core\n" SHADER_CODE(
        in  vec4 vertex_color;
        in  vec3 pos;
        layout(location = 0) out vec3 color;
        uniform vec3 cam_pos;

        void main()
        {
            float dist = length(pos - cam_pos);
            dist = dist * dist;
            color = vertex_color.xyz * 90.0 / (dist + 50.0);
        }),
    }
};

bool compile_shaders(GLuint shaders_ids[SHADER_COUNT]);
bool link_shaders(GLuint shader_ids[SHADER_COUNT], GLuint program_ids[PROGRAM_COUNT]);
bool delete_shaders(GLuint shader_ids[SHADER_COUNT]);
bool compile_shaders_and_link_programs(GLuint program_ids[PROGRAM_COUNT]);

#endif