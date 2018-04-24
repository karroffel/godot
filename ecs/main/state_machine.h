#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "core/os/input_event.h"
#include "vector.h"

class State;

struct Transition {
	enum class Type {
		NONE,
		QUIT,
		PUSH,
		POP,
		SWITCH,
	} type;

	State *argument = NULL;

	_FORCE_INLINE_ Transition(Type p_type = Type::NONE) {
		type = p_type;
		argument = nullptr;
	}

	_FORCE_INLINE_ Transition(Type p_type, State *p_argument) {
		type = p_type;
		argument = p_argument;
	}

	// convenience instances
	static Transition quit;
	static Transition none;
};

class EcsWorld;

class State {
public:
	virtual void on_start(EcsWorld *p_world) {}
	virtual void on_stop(EcsWorld *p_world) {}
	virtual void on_pause(EcsWorld *p_world) {}
	virtual void on_resume(EcsWorld *p_world) {}

	virtual Transition handle_event(EcsWorld *p_world, const Ref<InputEvent> &p_event) {
		return Transition(Transition::Type::NONE);
	}

	virtual Transition handle_notification(EcsWorld *p_world, int p_notification) {
		return Transition(Transition::Type::NONE);
	}
	virtual Transition update(EcsWorld *p_world) {
		return Transition(Transition::Type::NONE);
	}
	virtual Transition fixed_update(EcsWorld *p_world) {
		return Transition(Transition::Type::NONE);
	}
};

class StateMachine {

	Vector<State *> states;
	State *current_state;

	void handle_transition(EcsWorld *p_world, Transition p_transition);

public:
	// managing states

	void push_state(EcsWorld *p_world, State *p_state);
	void pop_state(EcsWorld *p_world);
	void switch_state(EcsWorld *p_world, State *p_state);

	// handlers

	void update(EcsWorld *p_world);
	void fixed_update(EcsWorld *p_world);
	void handle_event(EcsWorld *p_world, const Ref<InputEvent> &p_event);
	void handle_notification(EcsWorld *p_world, int p_notification);
	void on_stop(EcsWorld *p_world);

	// misc

	void quit(EcsWorld *p_world);

	_FORCE_INLINE_ bool is_running() {
		return current_state != nullptr;
	}
};

#endif // STATE_MACHINE_H
