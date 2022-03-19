#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cstdint>
#include <vector>
#include <random>

#include "maze.h"

#define ZPOSITIVE 0x01
#define ZNEGATIVE 0x02
#define XNEGATIVE 0x04
#define XPOSITIVE 0x08
#define YPOSITIVE 0x10
#define YNEGATIVE 0x20

std::random_device rd;
std::mt19937 rng(rd());

uint32_t opposite(uint32_t d) {
	switch (d) {
		case ZPOSITIVE: return ZNEGATIVE;
		case ZNEGATIVE: return ZPOSITIVE;
		case XNEGATIVE: return XPOSITIVE;
		case XPOSITIVE: return XNEGATIVE;
		case YPOSITIVE: return YNEGATIVE;
		case YNEGATIVE: return YPOSITIVE;
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
		case ZPOSITIVE: return glm::ivec3(  0,  1,  0);
		case ZNEGATIVE: return glm::ivec3(  0, -1,  0);
		case XPOSITIVE: return glm::ivec3(  1,  0,  0);
		case XNEGATIVE: return glm::ivec3( -1,  0,  0);
		case YPOSITIVE: return glm::ivec3(  0,  0,  1);
		case YNEGATIVE: return glm::ivec3(  0,  0, -1);
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
	std::vector dirs{ ZPOSITIVE, ZNEGATIVE, XNEGATIVE, XPOSITIVE, YPOSITIVE, YNEGATIVE };

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
	glm::vec3 quad_vertices[] = {
		glm::vec3( 1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3( 1.0f, -1.0f,  1.0f),

		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3( 1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
	};

	std::vector<glm::vec3> vertices;

	for (int z = 0; z < MAZE_LENGTH; z++) {
		for (int y = 0; y < MAZE_HEIGHT; y++) {
			for (int x = 0; x < MAZE_WIDTH; x++) {


				if (!(data[x][y][z] & XNEGATIVE)) {
					// West face -> -X
					for (int i = 0; i < 6; i++) {
						glm::vec3 vertex = glm::rotate(quad_vertices[i], glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
						vertices.push_back((wall_size / 2) * vertex + wall_size * glm::vec3(x, y, z));
						vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
					}
				}

				if (!(data[x][y][z] & YNEGATIVE)) {
					// Down face -> -Y
					for (int i = 0; i < 6; i++) {
						glm::vec3 vertex = glm::rotate(quad_vertices[i], glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
						vertices.push_back((wall_size / 2) * vertex + wall_size * glm::vec3(x, y, z));
						vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
					}
				}

				if (!(data[x][y][z] & ZNEGATIVE)) {
					// North face -> -Z
					for (int i = 0; i < 6; i++) {
						glm::vec3 vertex = glm::rotate(quad_vertices[i], glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
						vertices.push_back((wall_size / 2) * quad_vertices[i] + wall_size * glm::vec3(x, y, z));
						vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
					}
				}
			}
		}
	}

	return vertices;
}

void maze::print() const {
    /*
	for (int z = 0; z < MAZE_LENGTH; z++) {
		for (int i = 0; i < MAZE_WIDTH; i++)
			std::printf("__");

		printf(" z = %d\n", z);

		for (int y = 0; y < MAZE_HEIGHT; y++) {
			std::putchar('|');

			for (int x = 0; x < MAZE_WIDTH; x++) {
				std::putchar((data[x][y][z] & ZPOSITIVE) != 0 ? ' ' : '_');

				if ((data[x][y][z] & XPOSITIVE) != 0) {
					std::putchar((((data[x][y][z] | data[x + 1][y][z]) & ZPOSITIVE) != 0) ? ' ' : '_');
				} else {
					std::putchar('|');
				}
			}
			std::putchar('\n');
		}
	}
    */


    // nine by nine, very ugly probably.
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        std::printf("\n\nY: %d\n", y);
        for (int z = 0; z < MAZE_HEIGHT; z++) {

            // top line
            for (int x = 0; x < MAZE_WIDTH; x++) {
                std::putchar('+');

                if (data[x][y][z] & ZNEGATIVE)
                    std::putchar('.');
                else
                    std::putchar('-');
            }

            std::putchar('+');
            std::putchar('\n');

            if (data[0][y][z] & XNEGATIVE)
                std::putchar('.');
            else
                std::putchar('|');

            // bottom line
            for (int x = 0; x < MAZE_WIDTH; x++) {
                if ((data[x][y][z] & YPOSITIVE) && (data[x][y][z] & YNEGATIVE))
                    std::putchar('B');
                else if (data[x][y][z] & YPOSITIVE)
                    std::putchar('U');
                else if (data[x][y][z] & YNEGATIVE)
                    std::putchar('D');
                else 
                    std::putchar('.');

                if (data[x][y][z] & XPOSITIVE)
                    std::putchar('.');
                else
                    std::putchar('|');
            }

            std::putchar('\n');
        }

        for (int x = 0; x < MAZE_WIDTH; x++) {
            std::putchar('+');
            
            if (data[x][y][MAZE_LENGTH - 1] & ZPOSITIVE)
                std::putchar('.');
            else
                std::putchar('-');
        }

        std::putchar('+');
    }
}
