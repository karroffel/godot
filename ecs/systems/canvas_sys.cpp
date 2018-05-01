#include "canvas_sys.h"

#include "ecs/components/canvas_comps.h"
#include "ecs/components/transform_comps.h"

#include "core/os/os.h"
#include "servers/visual_server.h"

using namespace components;

systems::Canvas::Canvas() {

	vs = VS::get_singleton();
	viewport = vs->viewport_create();
	canvas = vs->canvas_create();

	Size2i screen_size = OS::get_singleton()->get_window_size();
	vs->viewport_attach_canvas(viewport, canvas);
	vs->viewport_set_size(viewport, screen_size.x, screen_size.y);
	vs->viewport_attach_to_screen(viewport, Rect2(Vector2(), screen_size));
	vs->viewport_set_active(viewport, true);

	canvas_item = vs->canvas_item_create();
	vs->canvas_item_set_parent(canvas_item, canvas);
}

void systems::Canvas::init(EcsWorld *world) {
	world->register_tag<CanvasItemTag>();

	world->register_component<CanvasLine>();
	world->register_component<CanvasPolyLine>();
	world->register_component<CanvasMultiLine>();
	world->register_component<CanvasRect>();
	world->register_component<CanvasCircle>();
	world->register_component<CanvasTextureRect>();
	world->register_component<CanvasTextureRectRegion>();
	world->register_component<CanvasNinePath>();
	world->register_component<CanvasPrimitive>();
	world->register_component<CanvasPolygon>();
	world->register_component<CanvasTriangleArray>();
	world->register_component<CanvasMesh>();
	world->register_component<CanvasMultimesh>();
	world->register_component<CanvasParticles>();

	world->system_add_required_tag<Canvas, CanvasItemTag>();
	world->system_add_reading_component<Canvas, Transform2dComp>();
}

void systems::Canvas::update(EcsWorld *world, EntityStream &entities) {

	Entity a;

	vs->canvas_item_clear(canvas_item);

	while (entities.next(a)) {

		auto *transform = world->get_component<Transform2dComp>(a);
		vs->canvas_item_add_set_transform(canvas_item, transform->transform);

		auto *circle = world->get_component<CanvasCircle>(a);
		auto *rect = world->get_component<CanvasRect>(a);

		if (circle) {
			vs->canvas_item_add_circle(canvas_item, circle->position, circle->radius, circle->color);
		}

		if (rect) {
			vs->canvas_item_add_rect(canvas_item, rect->rect, rect->color);
		}
	}
}
