#include "ecs_world.h"

void EcsWorld::input_event(const Ref<InputEvent> &p_event) {
}

void EcsWorld::input_text(const String &p_text) {
}

void EcsWorld::init() {
	print_line("Hello from EcsWorld!!");

	register_component_type("speed", sizeof(float));
}

bool EcsWorld::iteration(float p_time) {
	return _quit;
}

bool EcsWorld::idle(float p_time) {

	frame++;

	Entity e = create_entity();

	ERR_FAIL_COND_V(!is_entity_alive(e), true);

	printf("entity: %d, %d\n", e.id, e.generation);
	fflush(stdout);

	destroy_entity(e);

	return _quit;
}

void EcsWorld::finish() {
	print_line("Goodbye from EcsWorld!!");
}

void EcsWorld::quit() {
	_quit = true;
}

Entity EcsWorld::create_entity() {

	uint32_t index = 0;

	if (entity_free_indices.size() > MINIMUM_FREE_ENTITIES) {
		index = entity_free_indices[0];
		entity_free_indices.remove(0);
	} else {
		entity_generation.push_back(0);
		index = entity_generation.size() - 1;
	}

	return Entity{ index, entity_generation[index] };
}

void EcsWorld::destroy_entity(Entity p_entity) {

	entity_generation[p_entity.id] += 1;
	entity_free_indices.push_back(p_entity.id);
}

bool EcsWorld::is_entity_alive(Entity p_entity) {
	return entity_generation[p_entity.id] == p_entity.generation;
}

ComponentHandle EcsWorld::register_component_type(const StringName &p_name,
		size_t p_size,
		EcsWorld::ComponentStorageType p_storage_type) {
	ComponentHandle handle = component_last_handle;
	component_names[p_name] = handle;
	component_sizes.set(handle, p_size);

	component_last_handle += 1;

	return handle;
}

void *EcsWorld::add_component(Entity p_entity, ComponentHandle p_component) {
	return NULL;
}

void *EcsWorld::get_component(Entity p_entity, ComponentHandle p_component) {
	return NULL;
}

void EcsWorld::remove_component(Entity p_entity, ComponentHandle p_component) {
}

EcsWorld::EcsWorld() {
}

EcsWorld::~EcsWorld() {
}
