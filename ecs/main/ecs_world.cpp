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

	system_scheduler.dispatch_update(this);

	state.update(this);
}

void EcsWorld::on_stop() {
	print_line("Goodbye from EcsWorld!!");

	state.on_stop(this);

	for (int i = systems.size() - 1; i >= 0; i--) {
		systems[i]->finish();
	}
}

void EcsWorld::update_system_scheduler() {
	system_scheduler.recalculate_components(system_data);
}

Entity EcsWorld::create_entity() {

	uint32_t index = 0;

	if (entity_free_indices.size() > MINIMUM_FREE_ENTITIES) {
		index = entity_free_indices[0];
		entity_free_indices.remove(0);

		entity_component_set[index] = BitSet();
		entity_tag_set[index] = BitSet();
	} else {
		entity_generation.push_back(0);
		index = entity_generation.size() - 1;

		entity_component_set.push_back(BitSet());
		entity_tag_set.push_back(BitSet());
	}

	return Entity{ index, entity_generation[index] };
}

void EcsWorld::destroy_entity(Entity p_entity) {

	entity_generation[p_entity.id] += 1;
	entity_free_indices.push_back(p_entity.id);

	entity_component_set[p_entity.id] = BitSet();
	entity_tag_set[p_entity.id] = BitSet();
}

bool EcsWorld::is_entity_alive(Entity p_entity) {
	return entity_generation[p_entity.id] == p_entity.generation;
}

ComponentHandle EcsWorld::register_component(const StringName &p_name, size_t p_size) {

	ComponentHandle handle = component_last_handle;
	component_names[p_name] = handle;

	component_last_handle += 1;

	components.resize(components.size() + 1);
	components[handle].size_of_individual_component = p_size;

	return handle;
}

void *EcsWorld::add_component(Entity p_entity, ComponentHandle p_component) {

	ComponentStorage &comp_storage = components[p_component];

	uint32_t *idx = comp_storage.component_index.getptr(p_entity.id);
	uint32_t index = 0;

	if (!idx) {
		uint32_t number_components = comp_storage.component_data.size() / comp_storage.size_of_individual_component;

		comp_storage.component_data.resize(comp_storage.component_data.size() + comp_storage.size_of_individual_component);

		index = number_components;
		comp_storage.component_index.set(p_entity.id, index);
		entity_component_set[p_entity.id].set(p_component, true);
	} else {
		index = *idx;
	}

	return (void *)&comp_storage.component_data[index * comp_storage.size_of_individual_component];
}

void *EcsWorld::get_component(Entity p_entity, ComponentHandle p_component) {

	ComponentStorage &comp_storage = components[p_component];

	uint32_t *idx = comp_storage.component_index.getptr(p_entity.id);
	if (!idx) {
		return nullptr;
	}

	return (void *)&comp_storage.component_data[*idx * comp_storage.size_of_individual_component];
}

void EcsWorld::remove_component(Entity p_entity, ComponentHandle p_component) {

	entity_component_set[p_entity.id].set(p_component, false);
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
	entity_tag_set[p_entity.id].set(p_tag, true);
}

void EcsWorld::remove_tag(Entity p_entity, TagHandle p_tag) {
	tags[p_tag].erase(p_entity.id);
	entity_tag_set[p_entity.id].set(p_tag, false);
}

bool EcsWorld::has_tag(Entity p_entity, TagHandle p_tag) {
	return entity_tag_set[p_entity.id].get(p_tag);
}

SystemHandle EcsWorld::register_system(const StringName &p_system_name, System *p_system) {

	SystemHandle handle = systems.size();

	system_names.push_back(p_system_name);
	systems.push_back(p_system);
	system_data.push_back(SystemData());

	p_system->init();

	return handle;
}

void EcsWorld::system_run_after(SystemHandle p_system, SystemHandle p_before) {
	system_data[p_system].depending_on_systems.set(p_before, true);
}

void EcsWorld::system_add_reading_component(SystemHandle p_system, ComponentHandle p_comp) {
	system_data[p_system].reading_components.set(p_comp, true);
}

void EcsWorld::system_add_writing_component(SystemHandle p_system, ComponentHandle p_comp) {
	system_data[p_system].writing_components.set(p_comp, true);
}

void EcsWorld::system_add_required_tag(SystemHandle p_system, TagHandle p_tag) {
	system_data[p_system].required_tags.set(p_tag, true);
}

void EcsWorld::system_add_disallowed_component(SystemHandle p_system, ComponentHandle p_comp) {
	system_data[p_system].disallowed_components.set(p_comp, true);
}

void EcsWorld::system_add_disallowed_tag(SystemHandle p_system, TagHandle p_tag) {
	system_data[p_system].disallowed_tags.set(p_tag, true);
}

void EcsWorld::system_add_reading_resource(SystemHandle p_system, ResourceHandle p_res) {
	system_data[p_system].reading_resources.set(p_res, true);
}

void EcsWorld::system_add_writing_resource(SystemHandle p_system, ResourceHandle p_res) {
	system_data[p_system].writing_resources.set(p_res, true);
}

void EcsWorld::set_initial_state(State *p_state) {
	state.push_state(this, p_state);
}

EcsWorld::EcsWorld() {
}

EcsWorld::~EcsWorld() {
}
