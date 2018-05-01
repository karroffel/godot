#ifndef VIEWPORT_SYS_H
#define VIEWPORT_SYS_H

#include "ecs/main/entity_stream.h"
#include "ecs/main/system.h"

#include "core/rid.h"

class ViewportSystem : public System {

	RID canvas_item;

public:
	ViewportSystem(RID p_canvas_item);

	void update(EcsWorld *p_world, EntityStream &r_entity_stream);
};

#endif // VIEWPORT_SYS_H
