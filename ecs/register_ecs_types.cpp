#include "register_ecs_types.h"

#include "core/class_db.h"

#include "ecs/main/ecs_world.h"

#include <stdio.h>

void register_ecs_types() {
	ClassDB::register_class<EcsWorld>();
}

void unregister_ecs_types() {
}
