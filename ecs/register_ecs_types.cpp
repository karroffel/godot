#include "register_ecs_types.h"

#include "core/class_db.h"

#include "ecs/main/ecs_main_loop.h"

void register_ecs_types() {
	ClassDB::register_class<EcsMainLoop>();
}

void unregister_ecs_types() {
}
