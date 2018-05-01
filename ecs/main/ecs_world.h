#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include "ecs_defs.h"

#include "entity.h"
#include "system.h"
#include "system_scheduler.h"

#include "bitset.h"

#include "hash_map.h"
#include "oa_hash_map.h"
#include "vector.h"

#include "os/input_event.h"

#include "ecs/resources/basic_required.h"
#include "state_machine.h"

#include <typeinfo>

#define MINIMUM_FREE_ENTITIES 1024

struct ComponentStorage {
	size_t size_of_individual_component;
	HashMap<EntityIndex, uint32_t> component_index;
	Vector<uint8_t> component_data;
};

class EcsWorld {

	bool _quit = false;

public:
	// entity stuff

	Vector<EntityGeneration> entity_generation;
	Vector<EntityIndex> entity_free_indices;
	Vector<BitSet> entity_component_set;
	Vector<BitSet> entity_tag_set;

	// comonent stuff

	ComponentHandle component_last_handle = 0;
	HashMap<StringName, ComponentHandle> component_names;
	Vector<ComponentStorage> components;
	HashMap<size_t, ComponentHandle> component_handles;

	// resource stuff
	Vector<uint8_t> resource_data;
	HashMap<size_t, ResourceHandle> resource_handles;
	HashMap<ResourceHandle, size_t> resource_size;
	HashMap<StringName, ResourceHandle> resource_names;

	// tag stuff
	HashMap<StringName, TagHandle> tag_names;
	HashMap<size_t, TagHandle> tag_handles;
	Vector<Set<EntityIndex> > tags;

	// system stuff

	Vector<StringName> system_names;
	Vector<System *> systems;
	HashMap<size_t, SystemHandle> system_handles;
	Vector<SystemData> system_data;

	// system scheduler

	SystemScheduler system_scheduler;

	// state stuff

	StateMachine state;

	void handle_event(const Ref<InputEvent> &p_event);
	void handle_notification(int p_notification);
	void on_start();
	void fixed_update(float p_time);
	void update(float p_time);
	void on_stop();

	void update_system_scheduler();

	_FORCE_INLINE_ bool requested_quit() {
		return _quit;
	}

	_FORCE_INLINE_ void quit() {
		_quit = true;
	}

	// dealing with entities
	Entity create_entity();
	void destroy_entity(Entity p_entity);
	bool is_entity_alive(Entity p_entity);

	// dealing with components

	ComponentHandle register_component(const StringName &p_name, size_t p_size);

	void *add_component(Entity p_entity, ComponentHandle p_component);
	void *get_component(Entity p_entity, ComponentHandle p_component);
	void remove_component(Entity p_entity, ComponentHandle p_component);

	// template convenience wrappers
	template <class T>
	_FORCE_INLINE_ void register_component() {
		ComponentHandle handle = register_component(typeid(T).name(), sizeof(T));
		component_handles.set(typeid(T).hash_code(), handle);
	}

	template <class T>
	_FORCE_INLINE_ T *add_component(Entity p_entity) {
		ComponentHandle handle = component_handles[typeid(T).hash_code()];

		return (T *)add_component(p_entity, handle);
	}

	template <class T>
	_FORCE_INLINE_ T *add_component_with_data(Entity p_entity, const T &p_data) {
		ComponentHandle handle = component_handles[typeid(T).hash_code()];

		T *data_ptr = (T *)add_component(p_entity, handle);
		*data_ptr = p_data;
		return data_ptr;
	}

	template <class T>
	_FORCE_INLINE_ T *get_component(Entity p_entity) {
		ComponentHandle handle = component_handles[typeid(T).hash_code()];

		return (T *)get_component(p_entity, handle);
	}

	template <class T>
	_FORCE_INLINE_ void remove_component(Entity p_entity) {
		ComponentHandle handle = component_handles[typeid(T).hash_code()];

		remove_component(p_entity, handle);
	}

	// dealing with resources

	ResourceHandle register_resource(const StringName &p_resource_name, size_t p_size);

	void *get_resource(ResourceHandle p_resource);

	template <class T>
	_FORCE_INLINE_ T &register_resource() {
		ResourceHandle handle = register_resource(StringName(typeid(T).name()), sizeof(T));
		resource_handles.set(typeid(T).hash_code(), handle);
		return *(T *)get_resource(handle);
	}

	template <class T>
	_FORCE_INLINE_ T &get_resource() {
		ResourceHandle handle = resource_handles[typeid(T).hash_code()];
		return *(T *)get_resource(handle);
	}

	template <class T>
	_FORCE_INLINE_ void add_resource(const T &p_resource_data) {
		register_resource<T>();
		get_resource<T>() = p_resource_data;
	}

	// dealing with tags
	TagHandle register_tag(const StringName &p_tag_name);
	void add_tag(Entity p_entity, TagHandle p_tag);
	void remove_tag(Entity p_entity, TagHandle p_tag);
	bool has_tag(Entity p_entity, TagHandle p_tag);

	template <class T>
	_FORCE_INLINE_ void register_tag() {
		TagHandle handle = register_tag(StringName(typeid(T).name()));
		tag_handles.set(typeid(T).hash_code(), handle);
	}

	template <class T>
	_FORCE_INLINE_ void add_tag(Entity p_entity) {
		TagHandle handle = tag_handles[typeid(T).hash_code()];

		add_tag(p_entity, handle);
	}

	template <class T>
	_FORCE_INLINE_ void remove_tag(Entity p_entity) {
		TagHandle handle = tag_handles[typeid(T).hash_code()];

		remove_tag(p_entity, handle);
	}

	template <class T>
	_FORCE_INLINE_ bool has_tag(Entity p_entity) {
		TagHandle handle = tag_handles[typeid(T).hash_code()];

		return has_tag(p_entity, handle);
	}
	// dealing with systems

	SystemHandle register_system(const StringName &p_system_name, System *p_system);
	void system_run_after(SystemHandle p_system, SystemHandle p_before);

	void system_add_disallowed_component(SystemHandle p_system, ComponentHandle p_comp);

	void system_add_reading_component(SystemHandle p_system, ComponentHandle p_comp);
	void system_add_writing_component(SystemHandle p_system, ComponentHandle p_comp);

	void system_add_required_tag(SystemHandle p_system, TagHandle p_tag);
	void system_add_disallowed_tag(SystemHandle p_system, TagHandle p_tag);

	void system_add_reading_resource(SystemHandle p_system, ResourceHandle p_res);
	void system_add_writing_resource(SystemHandle p_system, ResourceHandle p_res);

	template <class S>
	_FORCE_INLINE_ void register_system(const StringName &p_system_name) {
		SystemHandle handle = register_system(p_system_name, new S());
		system_handles.set(typeid(S).hash_code(), handle);
	}

	template <class S>
	_FORCE_INLINE_ void register_system_custom(const StringName &p_system_name, S *p_system) {
		SystemHandle handle = register_system(p_system_name, p_system);
		system_handles.set(typeid(S).hash_code(), handle);
	}

	template <class S, class B>
	_FORCE_INLINE_ void system_run_after() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		SystemHandle before_handle = system_handles[typeid(B).hash_code()];

		system_run_after(handle, before_handle);
	}

	template <class S, class C>
	_FORCE_INLINE_ void system_add_reading_component() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		ComponentHandle comp_handle = component_handles[typeid(C).hash_code()];

		system_add_reading_component(handle, comp_handle);
	}

	template <class S, class C>
	_FORCE_INLINE_ void system_add_writing_component() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		ComponentHandle comp_handle = component_handles[typeid(C).hash_code()];

		system_add_writing_component(handle, comp_handle);
	}
	template <class S, class T>
	_FORCE_INLINE_ void system_add_required_tag() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		TagHandle tag_handle = tag_handles[typeid(T).hash_code()];

		system_add_required_tag(handle, tag_handle);
	}
	template <class S, class C>
	_FORCE_INLINE_ void system_add_disallowed_component() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		ComponentHandle comp_handle = component_handles[typeid(C).hash_code()];

		system_add_disallowed_component(handle, comp_handle);
	}
	template <class S, class T>
	_FORCE_INLINE_ void system_add_disallowed_tag() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		TagHandle tag_handle = tag_handles[typeid(T).hash_code()];

		system_add_disallowed_tag(handle, tag_handle);
	}

	template <class S, class R>
	_FORCE_INLINE_ void system_add_reading_resource() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		ResourceHandle resource_handle = resource_handles[typeid(R).hash_code()];

		system_add_reading_resource(handle, resource_handle);
	}

	template <class S, class R>
	_FORCE_INLINE_ void system_add_writing_resource() {
		SystemHandle handle = system_handles[typeid(S).hash_code()];

		ResourceHandle resource_handle = resource_handles[typeid(R).hash_code()];

		system_add_writing_resource(handle, resource_handle);
	}

	// dealing with states

	void set_initial_state(State *p_state);

	EcsWorld();
	~EcsWorld();
};

#endif // ECS_WORLD_H
