#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <random>

#include "maze.h"

#define SOUTH 0x01
#define NORTH 0x02
#define WEST  0x04
#define EAST  0x08
#define UP    0x10
#define DOWN  0x20

std::random_device rd;
std::mt19937 rng(rd());

uint32_t opposite(uint32_t d) {
	switch (d) {
		case SOUTH: return NORTH;
		case NORTH: return SOUTH;
		case WEST:  return EAST;
		case EAST:  return WEST;
		case UP:    return DOWN;
		case DOWN:  return UP;
		default:
			std::fprintf(
					stderr,
					"ERROR: Maze: there is no opposite of %x\n",
					d);
			return 0;
	}
}

glm::ivec3 direction(uint32_t d) {
	switch (d) {
		case SOUTH: return glm::ivec3(  0,  1,  0);
		case NORTH: return glm::ivec3(  0, -1,  0);
		case EAST:  return glm::ivec3(  1,  0,  0);
		case WEST:  return glm::ivec3( -1,  0,  0);
		case UP:    return glm::ivec3(  0,  0,  1);
		case DOWN:  return glm::ivec3(  0,  0, -1);
		default:
			std::fprintf(
					stderr,
					"ERROR: Maze: no direction vector for %x\n",
					d);
			return glm::ivec3(0);
	}
}

template <typename T>
void shuffle(std::vector<T>& v) {
	int n = v.size();
	for (int i = 0; i <= n - 2; i++) {
		std::uniform_int_distribution<uint32_t> dist(i, n - 1);
		int j = dist(rng);
		auto tmp = v[i];
		v[i] = v[j];
		v[j] = tmp;
	}
}

maze::maze() : data{0} {}

void maze::construct(glm::ivec3 start) {
	std::vector dirs{ SOUTH, NORTH, WEST, EAST, UP, DOWN };

	shuffle(dirs);

	for (int i = 0; i < dirs.size(); i++) {
		auto next = start + direction(dirs[i]);
		if (in_bounds(next) && !data[next.x][next.y][next.z]) {
			data[start.x][start.y][start.z] |= dirs[i];
			data[next.x][next.y][next.z] |= opposite(dirs[i]);

			construct(next);
		}
	}
}

void maze::reset() {
	std::memset(data, 0, MAZE_WIDTH * MAZE_HEIGHT * MAZE_LENGTH);
}

bool maze::in_bounds(glm::ivec3 p) const {
	return p.x >= 0 && p.x < MAZE_WIDTH &&
		   p.y >= 0 && p.y < MAZE_HEIGHT &&
		   p.z >= 0 && p.z < MAZE_LENGTH;
}

std::vector<glm::vec3> maze::gen_vertices(float wall_size) const {
	std::vector<glm::vec3> vertices;

	for (int z = 0; z < MAZE_LENGTH; z++) {
		for (int y = 0; y < MAZE_HEIGHT; y++) {
			for (int x = 0; x < MAZE_WIDTH; x++) {

				if (!(data[x][y][z] & NORTH)) {
					// North face -> +Z
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y - wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
					vertices.push_back(glm::vec3(
								x - wall_size / 2,
								y - wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

					vertices.push_back(glm::vec3(
								x - wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
					vertices.push_back(glm::vec3(
								x - wall_size / 2,
								y - wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
				}

				if (!(data[x][y][z] & WEST)) {
					// West face -> +X
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z - wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y - wall_size / 2,
								z - wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 1.0f));

					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y - wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y - wall_size / 2,
								z - wall_size / 2));
					vertices.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
				}

				if (!(data[x][y][z] & DOWN)) {
					// Down face -> +Y
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x - wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z - wall_size / 2));
					vertices.push_back(glm::vec3(0.0f, 1.0f, 1.0f));

					vertices.push_back(glm::vec3(
								x - wall_size / 2,
								y + wall_size / 2,
								z - wall_size / 2));
					vertices.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x + wall_size / 2,
								y + wall_size / 2,
								z - wall_size / 2));
					vertices.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
					vertices.push_back(glm::vec3(
								x - wall_size / 2,
								y + wall_size / 2,
								z + wall_size / 2));
					vertices.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
				}
			}
		}
	}

	return vertices;
}

void maze::print() const {
	for (int z = 0; z < MAZE_LENGTH; z++) {
		for (int i = 0; i < MAZE_WIDTH; i++)
			std::printf("__");

		printf(" z = %d\n", z);

		for (int y = 0; y < MAZE_HEIGHT; y++) {
			std::putchar('|');

			for (int x = 0; x < MAZE_WIDTH; x++) {
				std::putchar((data[x][y][z] & SOUTH) != 0 ? ' ' : '_');

				if ((data[x][y][z] & EAST) != 0) {
					std::putchar((((data[x][y][z] | data[x + 1][y][z]) & SOUTH) != 0) ? ' ' : '_');
				} else {
					std::putchar('|');
				}
			}
			std::putchar('\n');
		}
	}
}
