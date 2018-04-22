#include "ecs_world.h"

void EcsWorld::input_event(const Ref<InputEvent> &p_event) {
}

void EcsWorld::input_text(const String &p_text) {
}

void EcsWorld::init() {
	print_line("Hello from EcsWorld!!!");
}

bool EcsWorld::iteration(float p_time) {
	return quit;
}

bool EcsWorld::idle(float p_time) {
	return quit;
}

void EcsWorld::finish() {
}
