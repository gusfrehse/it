#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdbool.h>
#include <cstdlib>
#include <cstdio>

#include "SDL_events.h"
#include "SDL_keycode.h"
#include "maze.h"

#define CODE(...) #__VA_ARGS__

#define WIDTH 1280
#define HEIGHT 720

// quad data
float quad_vertices[] = {
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,

    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
};

GLuint quad_vao;
GLuint quad_vbo;

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
			"Modern Opengl",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			WIDTH,
			HEIGHT,
			SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1);

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
	m.construct(glm::ivec3(0));
	m.print();
	auto vertices = m.gen_vertices(1.0f);

	SDL_Window* window;
	SDL_GLContext context;

	init(window, context);

    // create a vertex buffer object and initialize it
    unsigned int vbo;
    glGenBuffers(1, &vbo);

    // create a vertex array object
    glGenVertexArrays(1, &quad_vao);

	// bind vao
    glBindVertexArray(quad_vao);

	// bind vbo to current array_buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// set the data of the current array_buffer
    glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec3),
			vertices.data(),
			GL_DYNAMIC_DRAW);

    // in shader: (location = vertex_attrib_index)
	GLuint vertex_attrib_index = 0;
	GLuint color_attrib_index = 1;

    // enable attrib
	glEnableVertexAttribArray(vertex_attrib_index);
	glEnableVertexAttribArray(color_attrib_index);

    // show opengl how to interpret the attrib
	glVertexAttribPointer(
			vertex_attrib_index,
			3,
			GL_FLOAT,
			GL_FALSE,
			6 * sizeof(float),
			(void*)0);

	glVertexAttribPointer(
			color_attrib_index,
			3,
			GL_FLOAT,
			GL_FALSE,
			6 * sizeof(float),
			(void*)(3 * sizeof(float)));

    // vertex shader
    const GLchar* vs_code = "#version 450 core\n" CODE(
        layout (location = 0) in vec3 attrib_pos;
        layout (location = 1) in vec3 attrib_color;

		out vec4 vertex_color;

		uniform mat4 mvp;

        void main(){
			vec4 pos = mvp * vec4(attrib_pos.x, attrib_pos.y, attrib_pos.z, 1.0);
			vertex_color = vec4(attrib_color / pos.z, 1.0);
            gl_Position = pos;
        }
    );

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_code, NULL);
    glCompileShader(vs);

    int success;
    char infolog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vs, 512, NULL, infolog);
		std::fprintf(stderr, "%s\n", infolog);
    }

    // fragment shader
    const GLchar* fs_code = "#version 450 core\n" CODE(
		in  vec4 vertex_color;
        out vec4 color;
        void main()
        {
            color = vertex_color;
        } 
    );

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_code, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fs, 512, NULL, infolog);
		std::fprintf(stderr, "%s\n", infolog);
    }

    // create program
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if(!success){
        glGetProgramInfoLog(program, 512, NULL, infolog);
		std::fprintf(stderr, "%s\n", infolog);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);


	glm::mat4 proj = glm::perspective(
			glm::radians(90.0f),
			(float) WIDTH / HEIGHT,
			0.1f,
			100.0f);

	glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::lookAt(cam_pos, glm::vec3(0.0f), cam_up);
	glm::vec3 model1_pos = glm::vec3(0.0f);
	glm::mat4 model1 = glm::translate(glm::mat4(1.0f), model1_pos);
	glm::mat4 m1vp = proj * view * model1;

	GLint mvp_uniform_loc = glGetUniformLocation(program, "mvp");

	long long frame_count = 0;

	float speed = 0.1f;

    // start render loop
    bool quit = false;
    while(!quit) {
        SDL_Event event;

        while(SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_w:
					cam_pos += speed * glm::vec3(0.0f, 0.0f, -1.0f);
					break;
				case SDLK_a:
					cam_pos += speed * glm::vec3(-1.0f, 0.0f, 0.0f);
					break;
				case SDLK_s:
					cam_pos += speed * glm::vec3(0.0f, 0.0f, 1.0f);
					break;
				case SDLK_d:
					cam_pos += speed * glm::vec3(1.0f, 0.0f, 0.0f);
					break;
				case SDLK_SPACE:
					cam_pos += speed * glm::vec3(0.0f, 1.0f, 0.0f);
					break;
				case SDLK_LSHIFT:
					cam_pos += speed * glm::vec3(0.0f, -1.0f, 0.0f);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				// TODO
				break;
			}
        }

        glClearColor(1.0, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        glBindVertexArray(quad_vao);

		// send mvp
		glUniformMatrix4fv(
				mvp_uniform_loc,
				1,
				GL_FALSE,
				glm::value_ptr(m1vp));

		// draw quad 1
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        SDL_GL_SwapWindow(window);

		//model1 = glm::rotate(
		//		model1,
		//		0.1f,
		//		glm::vec3(0.0f, 1.0f, 0.0f));
		//cam_pos = glm::vec3(
		//		5.0f,
		//		10.0f * glm::sin(0.01f * frame_count),
		//		10.0f * glm::cos(0.01f * frame_count));
		glm::mat4 view = glm::lookAt(
				cam_pos,
				glm::vec3(0.0f, 0.0f, -1.0f) + cam_pos,
				cam_up);
		m1vp = proj * view * model1;

		frame_count++;
    }

    glDeleteProgram(program);
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
