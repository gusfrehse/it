#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <stdbool.h>
#include <cstdlib>
#include <cstdio>

#include "SDL_events.h"
#include "SDL_keycode.h"
#include "input_controller.h"
#include "maze.h"
#include "minimap.h"
#include "shaders.h"

#define WIDTH 1280
#define HEIGHT 720

bool process_event(SDL_Event event, input_controller& icontroller);

// quad data
//float quad_pos[] = {
//     1.0f,  1.0f, 0.0f,
//    -1.0f,  1.0f, 0.0f,
//     1.0f, -1.0f, 0.0f,
//
//    -1.0f, -1.0f, 0.0f,
//     1.0f, -1.0f, 0.0f,
//    -1.0f,  1.0f, 0.0f,
//};
float quad_pos[] = {
     0.4,  0.4, 0.0f,
    -0.4,  0.4, 0.0f,
     0.4, -0.4, 0.0f,

    -0.4, -0.4, 0.0f,
     0.4, -0.4, 0.0f,
    -0.4,  0.4, 0.0f,
};

float quad_uv[] = {
    1.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,

    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
};

float arrow_pos[] = {
    0.0f,  0.0f, 0.0f,
    1.0f,  0.0f, 0.0f,
    0.8f,  0.2f, 0.0f,
    1.0f,  0.0f, 0.0f,
    0.8f, -0.2f, 0.0f,
};

float arrow_color[] = {
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
};

void opengl_message_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        GLchar const* message,
        void const* user_param);

void init(SDL_Window*& window, SDL_GLContext& context) {

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    // create window
    window = SDL_CreateWindow(
            "it",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::fprintf(
                stderr,
                "glew error: %s\n",
                glewGetErrorString(err));
        std::exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_message_callback, nullptr);
    glViewport(0, 0, WIDTH, HEIGHT);
}

int main(int argc, char** argv) {
    maze m;
    m.create_paths(glm::ivec3(0));
    m.print();
    m.gen_vertices(10.0f);

    SDL_Window* window;
    SDL_GLContext context;

    init(window, context);

    GLuint program_ids[PROGRAM_COUNT];
    if (!compile_shaders_and_link_programs(program_ids)) {
        return 1;
    }

    Minimap minimap(WIDTH, HEIGHT);


    if (!minimap.create()) 
        return 1;

    // init things
    m.init_gl();
    
    // quad things for minimap
    GLuint quad_vao;
    GLuint quad_pos_vbo;
    GLuint quad_uv_vbo;
    {
        GLuint quad_pos_attrib_index = 0;
        GLuint quad_uv_attrib_index = 1;

        glGenBuffers(1, &quad_pos_vbo);
        glGenBuffers(1, &quad_uv_vbo);
        glGenVertexArrays(1, &quad_vao);
    
        glBindVertexArray(quad_vao);    
    
        // pos bufffer
        glBindBuffer(GL_ARRAY_BUFFER, quad_pos_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(quad_pos),
                     quad_pos,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(quad_pos_attrib_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(quad_pos_attrib_index);

        // uv buffer
        glBindBuffer(GL_ARRAY_BUFFER, quad_uv_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(quad_uv),
                     quad_uv,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(quad_uv_attrib_index, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(quad_uv_attrib_index);
    }
    
    // arrow vao
    GLuint arrow_vao;
    GLuint arrow_pos_vbo;
    GLuint arrow_color_vbo;
    {
    
        GLuint arrow_pos_attrib_index = 0;
        GLuint arrow_color_attrib_index = 1;


        glGenBuffers(1, &arrow_pos_vbo);
        glGenBuffers(1, &arrow_color_vbo);
        glGenVertexArrays(1, &arrow_vao);
    
        glBindVertexArray(arrow_vao);   
    
        // pos bufffer
        glBindBuffer(GL_ARRAY_BUFFER, arrow_pos_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(arrow_pos),
                     arrow_pos,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(arrow_pos_attrib_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(arrow_pos_attrib_index);

        // uv buffer
        glBindBuffer(GL_ARRAY_BUFFER, arrow_color_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(arrow_color),
                     arrow_color,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(
                arrow_color_attrib_index,
                3,
                GL_FLOAT,
                GL_FALSE,
                6 * sizeof(float),
                (void*)(3 * sizeof(float)));
        
        glVertexAttribPointer(arrow_color_attrib_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(arrow_color_attrib_index);
    }
    

    glm::mat4 proj = glm::perspective(
            glm::radians(90.0f),
            (float) WIDTH / HEIGHT,
            0.1f,
            100.0f);

    glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cam_right = glm::cross(cam_front, cam_up);
    glm::mat4 view = glm::lookAt(cam_pos, glm::vec3(0.0f), cam_up);
    glm::vec3 model_pos = glm::vec3(0.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), model_pos);
    glm::mat4 mvp = proj * view * model;

    glm::ivec2 dmouse(0);

    glUseProgram(program_ids[PROGRAM_MINIMAP]);
    glUniform1i(glGetUniformLocation(program_ids[PROGRAM_MINIMAP], "color_texture"), 0);
    glUniform1i(glGetUniformLocation(program_ids[PROGRAM_MINIMAP], "depth_texture"), 1);
    glUseProgram(program_ids[PROGRAM_BASIC]);
    GLint mvp_uniform_loc = glGetUniformLocation(program_ids[PROGRAM_BASIC], "mvp");
    GLint cam_pos_uniform_loc = glGetUniformLocation(
                                                    program_ids[PROGRAM_BASIC],
                                                    "cam_pos");

    glLineWidth(2.0f);

    long long frame_count = 0;

    float speed = 0.1f;
    float sensitivity = 0.005f;

    input_controller icontroller;

    // start render loop
    bool quit = false;
    while(!quit) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            quit = process_event(event, icontroller);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program_ids[PROGRAM_BASIC]);

        // send mvp
        glUniformMatrix4fv(
                mvp_uniform_loc,
                1,
                GL_FALSE,
                glm::value_ptr(mvp));

        // send cam_pos
        glUniform3fv(
                cam_pos_uniform_loc,
                1,
                glm::value_ptr(cam_pos));

        // draw maze
        m.render();
        
        // draw minimap
        minimap.render(quad_vao, program_ids, m);

        // draw arrow in perspective, but not in viewport.
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(arrow_vao);
        glUseProgram(program_ids[PROGRAM_BASIC]);

        glm::mat4 arrow_shift = glm::translate(glm::vec3(0.8f,-0.8f, 0.0f));
        glm::mat4 arrow_model = glm::mat4(1.0f);
        glm::mat4 arrow_view = glm::lookAt(
                -5.0f * cam_front,
                glm::vec3(0.0f, 0.0f, 0.0f),
                cam_up);

        glm::mat4 arrow_mvp = arrow_shift * proj * arrow_view * arrow_model;
        glUniformMatrix4fv(
                mvp_uniform_loc,
                1,
                GL_FALSE,
                glm::value_ptr(arrow_mvp));
        glDrawArrays(GL_LINE_STRIP, 0, 5);

        arrow_model = glm::rotate(arrow_model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        arrow_mvp = arrow_shift * proj * arrow_view * arrow_model;
        glUniformMatrix4fv(
                mvp_uniform_loc,
                1,
                GL_FALSE,
                glm::value_ptr(arrow_mvp));
        glDrawArrays(GL_LINE_STRIP, 0, 5);

        arrow_model = glm::rotate(arrow_model, glm::radians(90.0f), glm::vec3(0.0f,-1.0f, 0.0f));
        arrow_mvp = arrow_shift * proj * arrow_view * arrow_model;
        glUniformMatrix4fv(
                mvp_uniform_loc,
                1,
                GL_FALSE,
                glm::value_ptr(arrow_mvp));
        glDrawArrays(GL_LINE_STRIP, 0, 5);
        glEnable(GL_DEPTH_TEST);


        SDL_GL_SwapWindow(window);

        if (icontroller.is_active(FORWARD)) {
            cam_pos += speed * cam_front;
        }
        
        if (icontroller.is_active(BACKWARD)) {
            cam_pos -= speed * cam_front;
        }
        
        if (icontroller.is_active(RIGHT)) {
            cam_pos += speed * cam_right;
        }
        
        if (icontroller.is_active(LEFT)) {
            cam_pos -= speed * cam_right;
        }
        
        if (icontroller.is_active(UP)) {
            cam_pos += speed * cam_up;
        }
        
        if (icontroller.is_active(DOWN)) {
            cam_pos -= speed * cam_up;
        }

        icontroller.reload();
        SDL_GetRelativeMouseState(&dmouse.x, &dmouse.y);

        cam_right = glm::rotate(glm::rotate(cam_right, sensitivity * -dmouse.x, cam_up), sensitivity * -dmouse.y, cam_right);
        cam_front = glm::rotate(glm::rotate(cam_front, sensitivity * -dmouse.x, cam_up), sensitivity * -dmouse.y, cam_right);
        cam_up = glm::cross(cam_right, cam_front);
        
        view = glm::lookAt(
                cam_pos,
                cam_front + cam_pos,
                cam_up);
        mvp = proj * view * model;

        frame_count++;
    }

    glDeleteProgram(program_ids[PROGRAM_BASIC]);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void opengl_message_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        GLchar const* message,
        void const* user_param)
{
    auto const src_str = [source]() {
        switch (source)
        {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
        default:
            return "what??";
        }
    }();

    auto const type_str = [type]() {
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
        default:
            return "what??";
        }
    }();

    auto const severity_str = [severity] () {
        switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        default:
            return "what??";
        }
    }();

    std::fprintf(
            stderr,
            "OPENGL CALLBACK: %s, %s, %s, %d, %s\n",
            src_str,
            type_str,
            severity_str,
            id,
            message);
}

bool process_event(SDL_Event event, input_controller& icontroller) {
    switch (event.type) {
    case SDL_QUIT:
        return true;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            return true;
        case SDLK_w:
            icontroller.key_down(FORWARD);
            break;
        case SDLK_a:
            icontroller.key_down(LEFT);
            break;
        case SDLK_s:
            icontroller.key_down(BACKWARD);
            break;
        case SDLK_d:
            icontroller.key_down(RIGHT);
            break;
        case SDLK_SPACE:
            icontroller.key_down(UP);
            break;
        case SDLK_LSHIFT:
            icontroller.key_down(DOWN);
            break;
        }
        break;
    case SDL_KEYUP:
        switch (event.key.keysym.sym) {
        case SDLK_w:
            icontroller.key_up(FORWARD);
            break;
        case SDLK_a:
            icontroller.key_up(LEFT);
            break;
        case SDLK_s:
            icontroller.key_up(BACKWARD);
            break;
        case SDLK_d:
            icontroller.key_up(RIGHT);
            break;
        case SDLK_SPACE:
            icontroller.key_up(UP);
            break;
        case SDLK_LSHIFT:
            icontroller.key_up(DOWN);
            break;
        }
        break;
    }

    return false;
}
