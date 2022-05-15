#include "minimap.h"
#include "shaders.h"

#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

Minimap::Minimap(int fb_width, int fb_height) : fb_width(fb_width), fb_height(fb_height) {}

int Minimap::create() {
    // Create a framebuffer
    glGenFramebuffers(1, &(fb_name));
    glBindFramebuffer(GL_FRAMEBUFFER, fb_name);
    
    // Create the color attachment texture we are rendering to
    glGenTextures(1, &(color_texture));
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fb_width,
                 fb_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         color_texture, 0);
    
    // Create Depth and Stencil texture
    glGenTextures(1, &(depth_stencil_texture));
    glBindTexture(GL_TEXTURE_2D, depth_stencil_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, fb_width,
                 fb_height, 0, GL_DEPTH_STENCIL,
                 GL_UNSIGNED_INT_24_8, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         depth_stencil_texture, 0);
    

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

// TODO: remove these (program_ids, vao) to something sensible
void Minimap::render(GLuint quad_vao, GLuint program_ids[], GLint mvp_location, const maze& m) {

    // TODO: should be temp
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 20.0f, -30.0f), glm::vec3(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // TODO: REMOVE 16:9 from hardcode
    glm::mat4 proj = glm::perspective(
            glm::radians(90.0f),
            16.0f / 9.0f,
            0.1f,
            100.0f);

    glUniformMatrix4fv(
            mvp_location,
            1,
            GL_FALSE,
            glm::value_ptr(proj * view));

    // bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fb_name);
    glClearColor(0.3f, 0.3f, 0.8f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw maze to framebuffer in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram(program_ids[PROGRAM_BASIC]);

    m.render();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // draw minimap
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);
    glUseProgram(program_ids[PROGRAM_MINIMAP]);
    glBindVertexArray(quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_stencil_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}
