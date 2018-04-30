#include "system_scheduler.h"

#include "ecs_world.h"

#include "entity_stream.h"

void SystemScheduler::recalculate_components(const Vector<SystemData> &systems) {

	components.resize(systems.size());

	for (SystemHandle handle = 0; handle < systems.size(); handle++) {
		components[handle] = handle;
	}
}

void SystemScheduler::dispatch_update(EcsWorld *p_world) {

	for (SystemHandle i = 0; i < components.size(); i++) {
		System *s = p_world->systems[i];

		EntityStream entity_stream;
		entity_stream.next_to_search_entity = 0;

		SystemData &data = p_world->system_data[i];

		entity_stream.used_components = data.reading_components & data.writing_components;
		entity_stream.disallowed_components = data.disallowed_components;

		entity_stream.used_tags = data.required_tags;
		entity_stream.disallowed_tags = data.disallowed_tags;

		s->update(p_world, entity_stream);
	}
}
