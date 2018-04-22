#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include "os/main_loop.h"

#include "entity.h"

#include "oa_hash_map.h"

typedef uint32_t EntityIndex;
typedef uint16_t EntityGeneration;
typedef uint32_t ComponentHandle;

#define MINIMUM_FREE_ENTITIES 1024

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
	OAHashMap<ComponentHandle, size_t> component_sizes;

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

	enum ComponentStorageType {
		COMPONENT_STORAGE_HASHMAP,

		COMPONENT_STORAGE_MAX
	};

	ComponentHandle register_component_type(const StringName &p_name,
			size_t p_size,
			ComponentStorageType p_storage_type = COMPONENT_STORAGE_HASHMAP);

	void *add_component(Entity p_entity, ComponentHandle p_component);

	void *get_component(Entity p_entity, ComponentHandle p_component);

	void remove_component(Entity p_entity, ComponentHandle p_component);

	void quit();

	EcsWorld();
	~EcsWorld();
};

#endif // ECS_WORLD_H
