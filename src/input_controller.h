#ifndef IT_INPUT_H
#define IT_INPUT_H

#include <vector>

enum motions {FORWARD, BACKWARD, RIGHT, LEFT, UP, DOWN, NUM_MOTIONS};

class input_controller {
	int num_pressed = 0;
	std::vector<bool> pressed;
	std::vector<bool> holded;


public:
	input_controller();
	void key_down(motions m);
	void key_up(motions m);
	void reload();

	bool is_active(motions m);
	bool is_pressed(motions m);
	bool is_holded(motions m);

	void reset();
};

#endif

