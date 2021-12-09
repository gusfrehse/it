#include <gl/glew.h>
#include <SDL2/SDL.h>

#include <stdbool.h>
#include <cstdlib>
#include <cstdio>
#define CODE(...) #__VA_ARGS__

#define WIDTH 1280
#define HEIGHT 720

void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

void init(SDL_Window*& window, SDL_GLContext& context) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    // create window
    window = SDL_CreateWindow("Modern Opengl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::fprintf(stderr, "glew error: %s\n", glewGetErrorString(err));
		std::exit(1);
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(opengl_message_callback, nullptr);
    glViewport(0, 0, WIDTH, HEIGHT);
}

int main(int argc, char** argv) {
	SDL_Window* window;
	SDL_GLContext context;

	init(window, context);
    
    // triangle data
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    // create a vertex buffer object and initialize it
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // create a vertex array object
    unsigned int vao;
    glGenVertexArrays(1, &vao);

	// bind vao
    glBindVertexArray(vao);

	// bind vbo to vao's array_buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLuint attribindex = 0; // layout thing in shader I think...
    glEnableVertexAttribArray(attribindex);
    // add the vertex data to the vertex buffer
	
	glVertexArrayVertexBuffer(vao, bindingindex, vbo, 0, 3 * sizeof(GL_FLOAT)); // NOLINT

	glVertexArrayAttribFormat(vao, bindingindex, 3, GL_FLOAT, GL_FALSE, 0);

	// attrib array, size, type, normalize, stride
    glVertexAttribPointer(attribindex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), NULL); // NOLINT 'suspicious' sizeof madness

    // vertex shader
    const GLchar* vs_code = "#version 330 core\n" CODE(
        layout (location = 0) in vec3 aPos;
        void main(){
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
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
        printf("%s\n", infolog);
    }

    // fragment shader
    const GLchar* fs_code = "#version 330 core\n" CODE(
        out vec4 color;
        void main()
        {
            color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        } 
    );

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_code, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fs, 512, NULL, infolog);
        printf("%s\n", infolog);
    }

    // create program
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if(!success){
        glGetProgramInfoLog(program, 512, NULL, infolog);
         printf("%s\n", infolog);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    // start render loop
    bool quit = false;
    while(!quit){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT)
                quit = true;
        }
        glClearColor(1.0, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(window);
    }
    glDeleteProgram(program);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}

void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		default: return "what??";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		default: return "what??";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		default: return "what??";
		}
	}();
	std::fprintf(stderr, "OPENGL CALLBACK: %s, %s, %s, %d, %s\n", src_str, type_str, severity_str, id, message);
}
