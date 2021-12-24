#ifndef IT_MAZE_H
#define IT_MAZE_H

#include <glm/glm.hpp>

#include <vector>

#define MAZE_WIDTH  16
#define MAZE_HEIGHT 16
#define MAZE_LENGTH 2

struct maze {
	uint32_t data[MAZE_WIDTH][MAZE_HEIGHT][MAZE_LENGTH];

	uint32_t get(glm::ivec3 pos) const {
		return data[pos.x][pos.y][pos.z];
	}

	maze();
	void construct(glm::ivec3 start);
	void reset();
	bool in_bounds(glm::ivec3 p) const;
	std::vector<glm::vec3> gen_vertices(float wall_size) const;
	void print() const;
};

#endif
