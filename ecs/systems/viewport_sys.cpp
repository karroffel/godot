#include "viewport_sys.h"

#include "ecs/components/viewport_comp.h"
#include "ecs/main/ecs_world.h"

#include "servers/visual_server.h"

ViewportSystem::ViewportSystem(RID p_canvas_item) {
	canvas_item = p_canvas_item;
}

void ViewportSystem::update(EcsWorld *p_world, EntityStream &r_entity_stream) {

	VisualServer *vs = VS::get_singleton();

	vs->canvas_item_clear(canvas_item);

	Entity e;

	while (r_entity_stream.next(e)) {
		Position2dComponent *pos = p_world->get_component<Position2dComponent>(e);
		CircleComponent *circle = p_world->get_component<CircleComponent>(e);

		vs->canvas_item_add_circle(canvas_item, pos->position, circle->radius, circle->color);
	}
}
