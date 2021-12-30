#include "input_controller.h"

input_controller::input_controller() :
	pressed(NUM_MOTIONS), holded(NUM_MOTIONS) {}

void input_controller::key_down(motions m) {
	pressed[m] = 1;
	holded[m] = 1;
}

void input_controller::key_up(motions m) {
	holded[m] = 0;
}

void input_controller::reload() {
	for (int m = 0; m < NUM_MOTIONS; m++) {
		pressed[m] = 0;
	}

	num_pressed = 0;
}

bool input_controller::is_active(motions m) {
	return pressed[m] || holded[m];
}

bool input_controller::is_pressed(motions m) {
	return pressed[m];
}
bool input_controller::is_holded(motions m) {
	return holded[m];
}
