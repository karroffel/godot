#ifndef ECS_MAIN_LOOP_H
#define ECS_MAIN_LOOP_H

#include "os/main_loop.h"

#include "ecs_world.h"

class EcsMainLoop : public MainLoop {
	GDCLASS(EcsMainLoop, MainLoop)

	Vector<EcsWorld> worlds;

	bool requested_quit = false;

public:
	virtual void init();
	virtual bool iteration(float p_time);
	virtual bool idle(float p_time);
	virtual void finish();
	virtual void input_event(const Ref<InputEvent> &p_event);
	virtual void input_text(const String &p_text);

	void _notification(int p_notification);

	EcsWorld *create_world();
};

#endif // ECS_MAIN_LOOP_H
