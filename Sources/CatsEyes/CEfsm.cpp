#include <stdlib.h>
#include <stdio.h>

#include "CEbtns.h"
#include "CEfsm.h"

char next_state(char current_state, ButtonSet* btns)
{
	switch(current_state)
	{
		case FSM_LOAD_MAIN_MENU:
			return FSM_MAIN_MENU;

		case FSM_MAIN_MENU:
			if(btns->isBtnClicked(LIVE_VISION_BUTTON))
				return FSM_LOAD_LIVE_VISION;
			else if(btns->isBtnClicked(SETTINGS_BUTTON))
				return FSM_LOAD_SETTINGS;
			else if(btns->isBtnClicked(GLOBAL_VISION_BUTTON))
				return FSM_LOAD_GLOBAL_VISION;
			else if(btns->isBtnClicked(TRACK_VISION_BUTTON))
				return FSM_LOAD_TRACK_VISION;
			else
				return FSM_MAIN_MENU;

		case FSM_LOAD_LIVE_VISION:
			return FSM_LIVE_VISION;

		case FSM_LIVE_VISION:
			if(btns->isBtnClicked(BACK_BUTTON))
				return FSM_LOAD_MAIN_MENU;
			else
				return FSM_LIVE_VISION;

		case FSM_LOAD_SETTINGS:
			return FSM_SETTINGS;

		case FSM_SETTINGS:
			if(btns->isBtnClicked(BACK_BUTTON))
				return FSM_LOAD_MAIN_MENU;
			else
				return FSM_SETTINGS;

		case FSM_LOAD_GLOBAL_VISION:
			return FSM_GLOBAL_VISION;

		case FSM_GLOBAL_VISION:
			if(btns->isBtnClicked(BACK_BUTTON))
				return FSM_LOAD_MAIN_MENU;
			else
				return FSM_GLOBAL_VISION;

		case FSM_LOAD_TRACK_VISION:
			return FSM_TRACK_VISION;

		case FSM_TRACK_VISION:
			if(btns->isBtnClicked(BACK_BUTTON))
				return FSM_LOAD_MAIN_MENU;
			else
				return FSM_TRACK_VISION;

		default:
			break;
	}
	return FSM_LOAD_MAIN_MENU;
}
