#include "shaders.h"
#include <gl/glew.h>
#include <cstdio>

bool compile_shaders(GLuint shader_ids[SHADER_COUNT]) {
    GLint success = 0;
    GLchar info_log[512];
    
    bool no_failures = true;

    for (int shader = 0; shader < SHADER_COUNT; shader++) {
        shader_ids[shader] = glCreateShader(pre_shaders[shader].type);
        glShaderSource(shader_ids[shader], 1, &pre_shaders[shader].source, 0);
        glCompileShader(shader_ids[shader]);
        
        glGetShaderiv(shader_ids[shader], GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader_ids[shader], 512, 0, info_log);

            std::fprintf(stderr,
                         "%sSHADER COMPILATION FAILED. SHADER: %d\n%s\n",
                         pre_shaders[shader].type != GL_FRAGMENT_SHADER ?
                             pre_shaders[shader].type != GL_VERTEX_SHADER ?
                                 "" : "VERTEX " : "FRAGMENT ",
                         shader,
                         info_log);
            no_failures = false;
        }
    }
    
    return no_failures;
}

bool delete_shaders(GLuint shader_ids[SHADER_COUNT]) {
    for (int shader = 0; shader < SHADER_COUNT; shader++) {
        glDeleteShader(shader_ids[shader]);
    }

    return true;
}

/**
 * @brief Link shaders from pre_shaders. Uses the program information from the
 * pre_shader struct. O(SHADER_COUNT * PROGRAM_COUNT)), could be better but too
 * much hassle.
 * 
 * @return true If all linking were successful.
 * @return false If any linking failed.
 */
bool link_shaders(GLuint shader_ids[SHADER_COUNT],
                  GLuint program_ids[PROGRAM_COUNT]) {
    GLint success = 0;
    GLchar info_log[512];
    
    bool no_failures = true;

    // Creating programs.
    for (int program = 0; program < PROGRAM_COUNT; program++) {
        program_ids[program] = glCreateProgram();
    }

    // Attaching each shader to the corresponding program.
    for (int shader = 0; shader < SHADER_COUNT; shader++) {
        glAttachShader(program_ids[pre_shaders[shader].program],
                       shader_ids[shader]);
    }
    
    // Linking and checking for errors.
    for (int program = 0; program < PROGRAM_COUNT; program++) {
        glLinkProgram(program_ids[program]);
        glGetProgramiv(program_ids[program], GL_LINK_STATUS, &success);
        
        if (!success) {
            glGetProgramInfoLog(program_ids[program], 512, NULL, info_log);
            std::fprintf(stderr, "%s\n", info_log);
            std::fprintf(stderr,
                         "PROGRAM LINKNING FAILED. PROGRAM: %d\n%s\n",
                         program,
                         info_log);
            no_failures = false;
        }
    }
    
    return no_failures;
}

bool compile_shaders_and_link_programs(GLuint program_ids[PROGRAM_COUNT]) {
    GLuint shader_ids[SHADER_COUNT];
    return compile_shaders(shader_ids)
        && link_shaders(shader_ids, program_ids)
        && delete_shaders(shader_ids);
}