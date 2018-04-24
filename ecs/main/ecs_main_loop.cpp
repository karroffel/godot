#include "ecs_main_loop.h"

struct Position {
	float x;
	float y;
};

struct TestTag {};

class TestState : public State {

	Entity a;

	// State interface
public:
	virtual void on_start(EcsWorld *p_world) {
		p_world->register_tag<TestTag>();

		a = p_world->create_entity();
		p_world->add_component_with_data(a, Position{ 0.0, 1337.42 });
		p_world->add_tag<TestTag>(a);
	}
	virtual Transition handle_notification(EcsWorld *p_world, int p_notification) {
		switch (p_notification) {
			case MainLoop::NOTIFICATION_WM_QUIT_REQUEST: {
				return Transition::quit;
			} break;
		}

		return Transition::none;
	}
	virtual Transition update(EcsWorld *p_world) {

		Position *pos = p_world->get_component<Position>(a);
		pos->x += 1.0 * p_world->get_resource<DeltaTime>().delta;

		print_line("position: " + String::num(pos->x) + ", " + String::num(pos->y));

		return Transition::none;
	}
};

void EcsMainLoop::init() {

	// TODO load level file

	// create worlds
	EcsWorld *world = create_world();
	world->register_component_type<Position>();

	// register components

	// register systems

	// set states
	TestState *test_state = new TestState;
	world->set_initial_state(test_state);
}

bool EcsMainLoop::iteration(float p_time) {

	for (int i = 0; i < worlds.size() && !requested_quit; i++) {
		worlds[i].fixed_update(p_time);

		requested_quit = requested_quit || worlds[i].requested_quit();
	}

	return requested_quit;
}

bool EcsMainLoop::idle(float p_time) {

	for (int i = 0; i < worlds.size() && !requested_quit; i++) {
		worlds[i].update(p_time);

		requested_quit = requested_quit || worlds[i].requested_quit();
	}

	return requested_quit;
}

void EcsMainLoop::finish() {
	for (int i = 0; i < worlds.size(); i++) {
		worlds[i].on_stop();
	}
}

void EcsMainLoop::input_event(const Ref<InputEvent> &p_event) {
	for (int i = 0; i < worlds.size() && !requested_quit; i++) {
		worlds[i].handle_event(p_event);

		requested_quit = requested_quit || worlds[i].requested_quit();
	}
}

void EcsMainLoop::input_text(const String &p_text) {
}

EcsWorld *EcsMainLoop::create_world() {
	worlds.resize(worlds.size() + 1);

	EcsWorld *world = &worlds[worlds.size() - 1];

	world->on_start();

	return world;
}
