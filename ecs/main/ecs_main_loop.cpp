#include "ecs_main_loop.h"

#include "ecs/components/viewport_comp.h"

#include "ecs/systems/viewport_sys.h"

#include "core/os/input.h"
#include "core/os/os.h"
#include "servers/visual_server.h"

struct MoveTag {};

class MoveSystem : public System {
public:
	void update(EcsWorld *p_world, EntityStream &r_entity_stream) {

		Entity e;

		float delta = p_world->get_resource<DeltaTime>().delta;
		while (r_entity_stream.next(e)) {
			Position2dComponent *pos = p_world->get_component<Position2dComponent>(e);

			pos->position.x += 100 * delta;
		}
	}
};

class TestState : public State {

	Entity circle_a;

	Entity circle_b;

	// State interface
public:
	virtual void on_start(EcsWorld *p_world) {

		p_world->register_tag<MoveTag>();

		p_world->register_system<MoveSystem>("Move");
		p_world->system_add_writing_component<MoveSystem, Position2dComponent>();
		p_world->system_add_required_tag<MoveSystem, MoveTag>();

		p_world->update_system_scheduler();

		circle_a = p_world->create_entity();
		circle_b = p_world->create_entity();

		p_world->add_component_with_data(circle_a, Position2dComponent{ Point2(100, 100) });
		p_world->add_component_with_data(circle_b, Position2dComponent{ Point2(200, 200) });

		p_world->add_component_with_data(circle_a, CircleComponent{ 50.0, Color(1.0, 0.0, 0.0) });
		p_world->add_component_with_data(circle_b, CircleComponent{ 25, Color(0.0, 0.0, 1.0) });
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

		return Transition::none;
	}

	virtual Transition handle_event(EcsWorld *p_world, const Ref<InputEvent> &p_event) {
		const InputEventKey *key = Object::cast_to<InputEventKey>(*p_event);
		if (key) {
			if (key->is_pressed() && !key->is_echo()) {
				p_world->add_tag<MoveTag>(circle_b);
			}

			if (!key->is_pressed()) {
				p_world->remove_tag<MoveTag>(circle_b);
			}
		}
		return Transition::none;
	}
};

void EcsMainLoop::init() {

	// TODO load level file

	// create worlds
	EcsWorld *world = create_world();

	// register components
	world->register_component<Position2dComponent>();
	world->register_component<CircleComponent>();

	VisualServer *vs = VS::get_singleton();
	RID vp = vs->viewport_create();
	RID canvas = vs->canvas_create();

	Size2i screen_size = OS::get_singleton()->get_window_size();
	vs->viewport_attach_canvas(vp, canvas);
	vs->viewport_set_size(vp, screen_size.x, screen_size.y);
	vs->viewport_attach_to_screen(vp, Rect2(Vector2(), screen_size));
	vs->viewport_set_active(vp, true);

	RID ci = vs->canvas_item_create();
	vs->canvas_item_set_parent(ci, canvas);

	// register systems
	world->register_system_custom("ViewportSystem", new ViewportSystem(ci));
	world->system_add_reading_component<ViewportSystem, CircleComponent>();
	world->system_add_reading_component<ViewportSystem, Position2dComponent>();

	world->update_system_scheduler();

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

void EcsMainLoop::_notification(int p_notification) {
	for (int i = 0; i < worlds.size() && !requested_quit; i++) {
		worlds[i].handle_notification(p_notification);

		requested_quit = requested_quit || worlds[i].requested_quit();
	}
}

EcsWorld *EcsMainLoop::create_world() {
	worlds.resize(worlds.size() + 1);

	EcsWorld *world = &worlds[worlds.size() - 1];

	world->on_start();

	return world;
}
