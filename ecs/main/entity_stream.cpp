#include "entity_stream.h"

#include "ecs_world.h"

bool EntityStream::next(Entity &r_entity) {

	EntityIndex idx = next_to_search_entity;

	for (; idx < world->entity_generation.size(); idx++) {

		if ((world->entity_component_set[idx] & used_components) != used_components) {
			continue;
		}

		if ((world->entity_component_set[idx] & disallowed_components) != BitSet()) {
			continue;
		}

		if ((world->entity_tag_set[idx] & used_tags) != used_tags) {
			continue;
		}

		if ((world->entity_tag_set[idx] & disallowed_tags) != BitSet()) {
			continue;
		}

		next_to_search_entity = idx + 1;
		r_entity.id = idx;
		r_entity.generation = world->entity_generation[idx];
		return true;
	}

	next_to_search_entity = idx;

	return false;
}
