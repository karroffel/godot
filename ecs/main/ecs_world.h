#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include "entity.h"

#include "hash_map.h"
#include "oa_hash_map.h"
#include "vector.h"

#include "os/input_event.h"

#include "ecs/resources/basic_required.h"
#include "state_machine.h"

#include <typeinfo>

typedef uint32_t EntityIndex;
typedef uint16_t EntityGeneration;

typedef uint32_t ComponentHandle;

typedef uint32_t ResourceHandle;

typedef uint32_t SystemHandle;

class System {
public:
	virtual void init() {}
	virtual void update() = 0;
	virtual void fixed_update() {}
	virtual void finish() {}
};

#define MINIMUM_FREE_ENTITIES 1024

struct ComponentStorage {
	size_t size_of_individual_component;
	OAHashMap<EntityIndex, uint32_t> component_index;
	Vector<uint8_t> component_data;
};

class EcsWorld {

	bool _quit = false;

	// entity stuff

	Vector<EntityGeneration> entity_generation;
	Vector<EntityIndex> entity_free_indices;

	// comonent stuff

	ComponentHandle component_last_handle = 0;
	HashMap<StringName, ComponentHandle> component_names;
	Vector<ComponentStorage> components;
	OAHashMap<size_t, ComponentHandle> component_handles;

	// resource stuff
	Vector<uint8_t> resource_data;
	OAHashMap<size_t, ResourceHandle> resource_handles;
	HashMap<ResourceHandle, size_t> resource_size;
	HashMap<StringName, ResourceHandle> resource_names;

	// tag stuff

	// system stuff

	Vector<StringName> system_names;
	Vector<System *> systems;

	// state stuff

	StateMachine state;

public:
	void handle_event(const Ref<InputEvent> &p_event);
	void handle_notification(int p_notification);
	void on_start();
	void fixed_update(float p_time);
	void update(float p_time);
	void on_stop();

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

	ComponentHandle register_component_type(const StringName &p_name, size_t p_size);

	void *add_component(Entity p_entity, ComponentHandle p_component);
	void *get_component(Entity p_entity, ComponentHandle p_component);
	void remove_component(Entity p_entity, ComponentHandle p_component);

	// template convenience wrappers
	template <class T>
	_FORCE_INLINE_ void register_component_type() {
		ComponentHandle handle = register_component_type(typeid(T).name(), sizeof(T));
		component_handles.set(typeid(T).hash_code(), handle);
	}

	template <class T>
	_FORCE_INLINE_ T *add_component(Entity p_entity) {
		ComponentHandle handle = 0;
		component_handles.lookup(typeid(T).hash_code(), &handle);

		return (T *)add_component(p_entity, handle);
	}

	template <class T>
	_FORCE_INLINE_ T *add_component_with_data(Entity p_entity, const T &p_data) {
		ComponentHandle handle = 0;
		component_handles.lookup(typeid(T).hash_code(), &handle);

		T *data_ptr = (T *)add_component(p_entity, handle);
		*data_ptr = p_data;
		return data_ptr;
	}

	template <class T>
	_FORCE_INLINE_ T *get_component(Entity p_entity) {
		ComponentHandle handle = 0;
		component_handles.lookup(typeid(T).hash_code(), &handle);

		return (T *)get_component(p_entity, handle);
	}

	template <class T>
	_FORCE_INLINE_ void remove_component(Entity p_entity) {
		ComponentHandle handle = 0;
		component_handles.lookup(typeid(T).hash_code(), &handle);

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
		ResourceHandle handle = 0;
		resource_handles.lookup(typeid(T).hash_code(), &handle);
		return *(T *)get_resource(handle);
	}

	template <class T>
	_FORCE_INLINE_ void add_resource(const T &p_resource_data) {
		register_resource<T>();
		get_resource<T>() = p_resource_data;
	}

	// dealing with systems

	SystemHandle register_system(const StringName &p_system_name, System *p_system);

	// dealing with states

	void set_initial_state(State *p_state);

	EcsWorld();
	~EcsWorld();
};

#endif // ECS_WORLD_H
