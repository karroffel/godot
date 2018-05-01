#ifndef CANVAS_SYS_H
#define CANVAS_SYS_H

#include "ecs/main/ecs_world.h"
#include "ecs/main/system.h"

#include "core/rid.h"
#include "servers/visual_server.h"

namespace systems {

class Canvas : public System {

	RID viewport;
	RID canvas;
	RID canvas_item;

	VisualServer *vs;

public:
	Canvas();

	virtual void init(EcsWorld *world);
	virtual void update(EcsWorld *world, EntityStream &entities);
};

} // namespace systems

#endif // CANVAS_SYS_H
