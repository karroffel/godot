#ifndef ENTITY_STREAM_H
#define ENTITY_STREAM_H

#include "bitset.h"
#include "ecs_defs.h"
#include "entity.h"

class EcsWorld;

struct EntityStream {

	EcsWorld *world;

	BitSet used_components;
	BitSet disallowed_components;

	BitSet used_tags;
	BitSet disallowed_tags;

	EntityIndex next_to_search_entity;

	bool next(Entity &r_entity);
};

#endif // ENTITY_STREAM_H
