#ifndef SYSTEM_H
#define SYSTEM_H

#include "ecs_defs.h"
#include "entity.h"
#include "vector.h"

#include "bitset.h"
#include "entity_stream.h"

class EcsWorld;

struct SystemData {

	BitSet depending_on_systems;

	BitSet disallowed_components;

	BitSet reading_components;
	BitSet writing_components;

	BitSet required_tags;
	BitSet disallowed_tags;

	BitSet reading_resources;
	BitSet writing_resources;
};

class System {
public:
	virtual void init(EcsWorld *world) {}
	virtual void update(EcsWorld *world, EntityStream &entities) = 0;
	virtual void fixed_update() {}
	virtual void finish() {}
};
#endif // SYSTEM_H
