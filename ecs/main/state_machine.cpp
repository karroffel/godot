#include "state_machine.h"

#include "ecs_world.h"

Transition Transition::none = { Transition::Type::NONE, nullptr };
Transition Transition::quit = { Transition::Type::QUIT, nullptr };

void StateMachine::handle_transition(EcsWorld *p_world, Transition p_transition) {
	switch (p_transition.type) {
		case Transition::Type::NONE: {

		} break;
		case Transition::Type::QUIT: {
			quit(p_world);
		} break;
		case Transition::Type::PUSH: {
			push_state(p_world, p_transition.argument);
		} break;
		case Transition::Type::POP: {
			pop_state(p_world);
		} break;
		case Transition::Type::SWITCH: {
			switch_state(p_world, p_transition.argument);
		} break;
	}
}

void StateMachine::push_state(EcsWorld *p_world, State *p_state) {
	if (current_state) {
		current_state->on_pause(p_world);
	}

	states.push_back(p_state);
	current_state = p_state;

	current_state->on_start(p_world);
}

void StateMachine::pop_state(EcsWorld *p_world) {
	if (current_state) {
		current_state->on_stop(p_world);
		delete current_state;
	}

	states.remove(states.size() - 1);

	if (states.size() > 0) {
		current_state = states[states.size() - 1];
		current_state->on_resume(p_world);
	}
}

void StateMachine::switch_state(EcsWorld *p_world, State *p_state) {
	if (current_state) {
		current_state->on_stop(p_world);
		delete current_state;
	}

	if (states.size() == 0) {
		return;
	}

	states[states.size() - 1] = p_state;
	current_state = p_state;

	current_state->on_start(p_world);
}

void StateMachine::update(EcsWorld *p_world) {
	if (current_state) {
		Transition trans = current_state->update(p_world);

		handle_transition(p_world, trans);
	}
}

void StateMachine::fixed_update(EcsWorld *p_world) {
	if (current_state) {
		Transition trans = current_state->fixed_update(p_world);

		handle_transition(p_world, trans);
	}
}

void StateMachine::handle_event(EcsWorld *p_world, const Ref<InputEvent> &p_event) {
	if (current_state) {
		Transition trans = current_state->handle_event(p_world, p_event);

		handle_transition(p_world, trans);
	}
}

void StateMachine::handle_notification(EcsWorld *p_world, int p_notification) {
	if (current_state) {
		Transition trans = current_state->handle_notification(p_world, p_notification);

		handle_transition(p_world, trans);
	}
}

void StateMachine::on_stop(EcsWorld *p_world) {
	while (states.size() > 0) {
		pop_state(p_world);
	}
}

void StateMachine::quit(EcsWorld *p_world) {
	p_world->quit();
}
