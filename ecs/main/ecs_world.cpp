#include "ecs_world.h"

void EcsWorld::handle_event(const Ref<InputEvent> &p_event) {
	state.handle_event(this, p_event);
}

void EcsWorld::handle_notification(int p_notification) {
	state.handle_notification(this, p_notification);
}

void EcsWorld::on_start() {
	add_resource<DeltaTime>(DeltaTime{ 0.00001 });
	add_resource<FixedDeltaTime>(FixedDeltaTime{ 0.00001 });
}

void EcsWorld::fixed_update(float p_time) {

	get_resource<FixedDeltaTime>().delta = p_time;

	// TODO run fixed update on systems

	state.fixed_update(this);
}

void EcsWorld::update(float p_time) {

	get_resource<DeltaTime>().delta = p_time;

	for (int i = 0; i < systems.size(); i++) {
		// TODO create entity streams that match the system

		systems[i]->update();
	}

	state.update(this);
}

void EcsWorld::on_stop() {
	print_line("Goodbye from EcsWorld!!");

	state.on_stop(this);

	for (int i = systems.size() - 1; i >= 0; i--) {
		systems[i]->finish();
	}
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

ComponentHandle EcsWorld::register_component_type(const StringName &p_name, size_t p_size) {

	ComponentHandle handle = component_last_handle;
	component_names[p_name] = handle;

	component_last_handle += 1;

	components.resize(components.size() + 1);
	components[handle].size_of_individual_component = p_size;

	return handle;
}

void *EcsWorld::add_component(Entity p_entity, ComponentHandle p_component) {

	ComponentStorage &comp_storage = components[p_component];

	uint32_t idx = 0;
	bool entity_has_component = comp_storage.component_index.lookup(p_entity.id, &idx);
	if (!entity_has_component) {
		uint32_t number_components = comp_storage.component_data.size() / comp_storage.size_of_individual_component;

		comp_storage.component_data.resize(comp_storage.component_data.size() + comp_storage.size_of_individual_component);

		idx = number_components;
		comp_storage.component_index.set(p_entity.id, idx);
	}

	return (void *)&comp_storage.component_data[idx * comp_storage.size_of_individual_component];
}

void *EcsWorld::get_component(Entity p_entity, ComponentHandle p_component) {

	ComponentStorage &comp_storage = components[p_component];

	uint32_t idx = 0;
	bool entity_has_component = comp_storage.component_index.lookup(p_entity.id, &idx);
	if (!entity_has_component) {
		return nullptr;
	}

	return (void *)&comp_storage.component_data[idx * comp_storage.size_of_individual_component];
}

void EcsWorld::remove_component(Entity p_entity, ComponentHandle p_component) {

	ComponentStorage &comp_storage = components[p_component];

	comp_storage.component_index.remove(p_entity.id);
}

ResourceHandle EcsWorld::register_resource(const StringName &p_resource_name, size_t p_size) {
	ResourceHandle handle = resource_data.size();

	resource_size[handle] = p_size;
	resource_names[p_resource_name] = handle;

	resource_data.resize(resource_data.size() + p_size);

	return handle;
}

void *EcsWorld::get_resource(ResourceHandle p_resource) {
	return (void *)&resource_data[p_resource];
}

TagHandle EcsWorld::register_tag(const StringName &p_tag_name) {
	TagHandle handle = tags.size();

	tag_names[p_tag_name] = handle;
	tags.push_back(Set<EntityIndex>());

	return handle;
}

void EcsWorld::add_tag(Entity p_entity, TagHandle p_tag) {
	tags[p_tag].insert(p_entity.id);
}

void EcsWorld::remove_tag(Entity p_entity, TagHandle p_tag) {
	tags[p_tag].erase(p_entity.id);
}

bool EcsWorld::has_tag(Entity p_entity, TagHandle p_tag) {
	return tags[p_tag].has(p_entity.id);
}

SystemHandle EcsWorld::register_system(const StringName &p_system_name, System *p_system) {

	SystemHandle handle = systems.size();

	system_names.push_back(p_system_name);
	systems.push_back(p_system);

	p_system->init();

	return handle;
}

void EcsWorld::set_initial_state(State *p_state) {
	state.push_state(this, p_state);
}

EcsWorld::EcsWorld() {
}

EcsWorld::~EcsWorld() {
}
