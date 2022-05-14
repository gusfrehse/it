#ifndef IT_MAZE_H
#define IT_MAZE_H

#include <gl/glew.h>
#include <glm/glm.hpp>

#include <vector>

#define MAZE_WIDTH  10
#define MAZE_HEIGHT 10
#define MAZE_LENGTH 10

class maze {
	uint32_t data[MAZE_WIDTH][MAZE_HEIGHT][MAZE_LENGTH];
    std::vector<glm::vec3> vertices;
    GLuint vao;
    GLuint vbo;

public:
	maze();
	void create_paths(glm::ivec3 start);
	bool in_bounds(glm::ivec3 p) const;
	void gen_vertices(float wall_size);
	void print() const;
    void init_gl();
    void render() const;
};

#endif
