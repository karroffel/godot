#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include "os/main_loop.h"

#include "entity.h"

#include "hash_map.h"
#include "oa_hash_map.h"
#include "vector.h"

#include <typeinfo>

typedef uint32_t EntityIndex;
typedef uint16_t EntityGeneration;

typedef uint32_t ComponentHandle;

typedef uint32_t SystemHandle;

class System {
public:
	virtual void init() = 0;
	virtual void run() = 0;
	virtual void finish() = 0;
};

#define MINIMUM_FREE_ENTITIES 1024

struct ComponentStorage {
	size_t size_of_individual_component;
	OAHashMap<EntityIndex, uint32_t> component_index;
	Vector<uint8_t> component_data;
};

class EcsWorld : public MainLoop {

	GDCLASS(EcsWorld, MainLoop)

	bool _quit = false;

	uint64_t frame = 0;

	// entity stuff

	Vector<EntityGeneration> entity_generation;
	Vector<EntityIndex> entity_free_indices;

	// comonent stuff

	ComponentHandle component_last_handle = 0;
	HashMap<StringName, ComponentHandle> component_names;
	HashMap<ComponentHandle, ComponentStorage> components;
	OAHashMap<size_t, ComponentHandle> component_handles;

	// resource stuff

	// tag stuff

	// system stuff
	Vector<StringName> system_names;
	Vector<System *> systems;

public:
	// MainLoop stuff
	virtual void input_event(const Ref<InputEvent> &p_event);
	virtual void input_text(const String &p_text);
	virtual void init();
	virtual bool iteration(float p_time);
	virtual bool idle(float p_time);
	virtual void finish();

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

	// dealing with systems

	SystemHandle register_system(const StringName &p_system_name, System *p_system);

	void quit();

	EcsWorld();
	~EcsWorld();
};

#endif // ECS_WORLD_H
