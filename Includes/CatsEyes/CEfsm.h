#ifndef CEFSM_H_
#define CEFSM_H_

#include "CEbtns.h"

// States:
#define FSM_LOAD_MAIN_MENU		1
#define FSM_MAIN_MENU			2
#define FSM_LOAD_LIVE_VISION	3
#define FSM_LIVE_VISION 		4
#define FSM_DEBUG				5
#define FSM_LOAD_SETTINGS		6
#define FSM_SETTINGS			7
#define FSM_LOAD_GLOBAL_VISION	8
#define FSM_GLOBAL_VISION		9
#define FSM_LOAD_TRACK_VISION	10
#define FSM_TRACK_VISION		11

char next_state(char current_state, ButtonSet* btns);

#endif
