#ifndef ECS_WORLD_H
#define ECS_WORLD_H

#include "os/main_loop.h"

class EcsWorld : public MainLoop {

	GDCLASS(EcsWorld, MainLoop)

	bool quit = false;

public:
	virtual void input_event(const Ref<InputEvent> &p_event);
	virtual void input_text(const String &p_text);
	virtual void init();
	virtual bool iteration(float p_time);
	virtual bool idle(float p_time);
	virtual void finish();
};

#endif // ECS_WORLD_H
