#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cstdint>
#include <vector>
#include <random>

#include "maze.h"

#define XPOSITIVE 0x01
#define XNEGATIVE 0x02
#define YPOSITIVE 0x04
#define YNEGATIVE 0x08
#define ZPOSITIVE 0x10
#define ZNEGATIVE 0x20

std::random_device rd;
std::mt19937 rng(rd());

static void append_wall(std::vector<glm::vec3> &vertices, int d, float wall_size, int x, int y, int z);
	
uint32_t opposite(uint32_t d) {
	switch (d) {
		case XNEGATIVE: return XPOSITIVE;
		case XPOSITIVE: return XNEGATIVE;
		case YPOSITIVE: return YNEGATIVE;
		case YNEGATIVE: return YPOSITIVE;
		case ZPOSITIVE: return ZNEGATIVE;
		case ZNEGATIVE: return ZPOSITIVE;
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
		case XPOSITIVE: return glm::ivec3(  1,  0,  0);
		case XNEGATIVE: return glm::ivec3( -1,  0,  0);
		case YPOSITIVE: return glm::ivec3(  0,  1,  0);
		case YNEGATIVE: return glm::ivec3(  0, -1,  0);
		case ZPOSITIVE: return glm::ivec3(  0,  0,  1);
		case ZNEGATIVE: return glm::ivec3(  0,  0, -1);
		default:
			std::fprintf(
					stderr,
					"ERROR: Maze: no direction vector for %x\n",
					d);
			return glm::ivec3(0);
	}
}

static float quad_angle_to_rotate(int d) {
	switch (d) {
		case XPOSITIVE: return  90.0f;
		case XNEGATIVE: return -90.0f;
		case YPOSITIVE: return -90.0f;
		case YNEGATIVE: return  90.0f;
		case ZPOSITIVE: return   0.0f;
		case ZNEGATIVE: return 180.0f;
		default:
			std::fprintf(
					stderr,
					"ERROR: Maze: no angle for %x\n",
					d);
			return 0.0f;
	}
	
}

static glm::vec3 quad_vector_to_rotate(uint32_t d) {
	switch (d) {
		case XPOSITIVE: return glm::vec3( 0, 1, 0);
		case XNEGATIVE: return glm::vec3( 0, 1, 0);
		case YPOSITIVE: return glm::vec3( 1, 0, 0);
		case YNEGATIVE: return glm::vec3( 1, 0, 0);
		case ZPOSITIVE: return glm::vec3( 0, 1, 0);
		case ZNEGATIVE: return glm::vec3( 0, 1, 0);
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

void maze::create_paths(glm::ivec3 start) {
	std::vector dirs{ XPOSITIVE, XNEGATIVE, YPOSITIVE, YNEGATIVE, ZPOSITIVE, ZNEGATIVE };

	shuffle(dirs);

	for (int i = 0; i < dirs.size(); i++) {
		auto next = start + direction(dirs[i]);
		if (in_bounds(next) && !data[next.x][next.y][next.z]) {
			data[start.x][start.y][start.z] |= dirs[i];
			data[next.x][next.y][next.z] |= opposite(dirs[i]);

			create_paths(next);
		}
	}
}

bool maze::in_bounds(glm::ivec3 p) const {
	return p.x >= 0 && p.x < MAZE_WIDTH &&
		   p.y >= 0 && p.y < MAZE_HEIGHT &&
		   p.z >= 0 && p.z < MAZE_LENGTH;
}

void maze::gen_vertices(float wall_size) {
	for (int z = 0; z < MAZE_LENGTH; z++) {
		for (int y = 0; y < MAZE_HEIGHT; y++) {
			for (int x = 0; x < MAZE_WIDTH; x++) {

				if (!(data[x][y][z] & XNEGATIVE)) {
					append_wall(vertices, XNEGATIVE, wall_size, x, y, z);
				}
				
				if (!(data[x][y][z] & YNEGATIVE)) {
					append_wall(vertices, YNEGATIVE, wall_size, x, y, z);
				}

				if (!(data[x][y][z] & ZNEGATIVE)) {
					append_wall(vertices, ZNEGATIVE, wall_size, x, y, z);
				}
			}
		}
	}
	
	// xpositive wall
	for (int y = 0; y < MAZE_HEIGHT; y++) {
		for (int z = 0; z < MAZE_LENGTH; z++) {
			append_wall(vertices, XPOSITIVE, wall_size, MAZE_WIDTH - 1, y, z);
			}
	}
	
	// ypositive wall
	for (int x = 0; x < MAZE_WIDTH; x++) {
		for (int z = 0; z < MAZE_LENGTH; z++) {
			append_wall(vertices, YPOSITIVE, wall_size, x, MAZE_HEIGHT - 1, z);
			}
	}
	
	// zpositive wall
	for (int x = 0; x < MAZE_WIDTH; x++) {
		for (int y = 0; y < MAZE_HEIGHT; y++) {
			append_wall(vertices, ZPOSITIVE, wall_size, x, y, MAZE_LENGTH - 1);
			}
	}
}

static void append_wall(std::vector<glm::vec3> &vertices, int d, float wall_size, int x, int y, int z) {
	const glm::vec3 quad_vertices[] = {
		glm::vec3( 1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3( 1.0f, -1.0f,  1.0f),

		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3( 1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
	};
	
	for (int i = 0; i < 6; i++) {
		float angle = glm::radians(quad_angle_to_rotate(d));
		glm::vec3 dir = quad_vector_to_rotate(d);
		glm::vec3 vertex = glm::rotate(quad_vertices[i], angle, dir);
		vertices.push_back((wall_size / 2) * vertex + wall_size * glm::vec3(x, y, z));
		vertices.push_back((glm::vec3)glm::abs(direction(d)));
	}
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
    std::printf("\n");

    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            for (int z = 0; z < MAZE_HEIGHT; z++) {
                std::printf("%d %d %d: ", x, y, z);
                if (data[x][y][z] & XPOSITIVE)
                    std::printf("XPOSITIVE ");
                if (data[x][y][z] & XNEGATIVE)
                    std::printf("XNEGATIVE ");
                if (data[x][y][z] & YPOSITIVE)
                    std::printf("YPOSITIVE ");
                if (data[x][y][z] & YNEGATIVE)
                    std::printf("YNEGATIVE ");
                if (data[x][y][z] & ZPOSITIVE)
                    std::printf("ZPOSITIVE ");
                if (data[x][y][z] & ZNEGATIVE)
                    std::printf("ZNEGATIVE ");
                std::printf("\n");
            }
        }
    }
}

void maze::init_gl() {
    // create a vertex buffer object and initialize it
    glGenBuffers(1, &vbo);

    // create a vertex array object
    glGenVertexArrays(1, &vao);

    // bind vao
    glBindVertexArray(vao);

    // bind vbo to current array_buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // set the data of the current array_buffer
    glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(glm::vec3),
            vertices.data(),
            GL_DYNAMIC_DRAW);

    // in shader: (location = vertex_attrib_index)
    const GLuint maze_vertex_attrib_index = 0;
    const GLuint maze_color_attrib_index = 1;

    // enable attrib
    glEnableVertexAttribArray(maze_vertex_attrib_index);
    glEnableVertexAttribArray(maze_color_attrib_index);

    // show opengl how to interpret the attrib
    glVertexAttribPointer(
            maze_vertex_attrib_index,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            (void*)0);

    glVertexAttribPointer(
            maze_color_attrib_index,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            (void*)(3 * sizeof(float)));
}

void maze::render() const {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
