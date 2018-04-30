#ifndef SYSTEM_SCHEDULER_H
#define SYSTEM_SCHEDULER_H

#include "ecs_defs.h"
#include "system.h"

struct SystemScheduler {
	Vector<SystemHandle> components; // TODO: use graph theory to make dependency tree

	void recalculate_components(const Vector<SystemData> &systems);

	void dispatch_update(EcsWorld *p_world);
};

#endif // SYSTEM_SCHEDULER_H
