#include "ecs_main_loop.h"

#include "core/os/input.h"
#include "core/os/os.h"
#include "servers/visual_server.h"

#include "ecs/systems/canvas_sys.h"

#include "ecs/components/canvas_comps.h"
#include "ecs/components/transform_comps.h"

struct VelocityComp {
	Vector2 velocity;
};

class MoveSystem : public System {
public:
	void update(EcsWorld *world, EntityStream &entities) {

		Entity e;

		float delta = world->get_resource<DeltaTime>().delta;
		while (entities.next(e)) {
			components::Transform2dComp *trans = world->get_component<components::Transform2dComp>(e);
			const VelocityComp *velocity = world->get_component<VelocityComp>(e);

			trans->transform.translate(velocity->velocity * delta);
		}
	}

	virtual void init(EcsWorld *world) {
		world->register_component<VelocityComp>();

		world->system_add_writing_component<MoveSystem, components::Transform2dComp>();
		world->system_add_reading_component<MoveSystem, VelocityComp>();
		world->system_add_reading_resource<MoveSystem, DeltaTime>();
	}
};

class TestState : public State {

	Entity circle_a;

	Entity circle_b;

	// State interface
public:
	virtual void on_start(EcsWorld *world) {

		world->register_system<MoveSystem>("Move");

		circle_a = world->create_entity();
		circle_b = world->create_entity();

		world->add_component_with_data(circle_a,
		                components::CanvasCircle{
		                                Point2(100, 100),
		                                50,
		                                Color(0.5, 1.0, 1.0) });
		world->add_component_with_data(circle_a,
		                components::CanvasRect{
		                                Rect2(Point2(0, 0), Point2(50, 50)),
		                                Color(0.0, 1.0, 0.0) });
		world->add_component_with_data(circle_b,
		                components::CanvasRect{
		                                Rect2(Point2(0, 0), Point2(50, 50)),
		                                Color(1.0, 1.0, 0.0) });

		world->add_component_with_data(circle_a, components::Transform2dComp{ Transform2D(0.0, Vector2(100, 100)) });
		world->add_component_with_data(circle_b, components::Transform2dComp{ Transform2D(0.0, Vector2(200, 50)) });

		world->add_component_with_data(circle_a, VelocityComp{ Vector2(50, 25) });
		world->add_component_with_data(circle_b, VelocityComp{ Vector2(0, 50) });

		world->add_tag<components::CanvasItemTag>(circle_a);
		world->add_tag<components::CanvasItemTag>(circle_b);
	}

	virtual Transition handle_notification(EcsWorld *world, int p_notification) {
		switch (p_notification) {
			case MainLoop::NOTIFICATION_WM_QUIT_REQUEST: {
				return Transition::quit;
			} break;
		}

		return Transition::none;
	}
	virtual Transition update(EcsWorld *world) {

		return Transition::none;
	}

	virtual Transition handle_event(EcsWorld *world, const Ref<InputEvent> &p_event) {
		return Transition::none;
	}
};

void EcsMainLoop::init() {

	// TODO load level file

	// create worlds
	EcsWorld *world = create_world();

	// register components
	world->register_component<components::TransformComp>();
	world->register_component<components::Transform2dComp>();

	// register systems
	world->register_system<systems::Canvas>("Canvas");

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
