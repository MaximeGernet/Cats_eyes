/*
 * CEbtns.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: root
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "basics.h"
#include "events.h"
#include "fonts.h"
#include "textures.h"

#include "CEbtns.h"


Button::Button()
{
	button_state = DEFAULT_BTN;
	previous_button_state = DEFAULT_BTN;
	setButtonColors(glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f, 0.8f, 0.7f), glm::vec3(0.0f, 1.0f, 0.9f));
}

void Button::setButtonColors(glm::vec3 _default_color, glm::vec3 _hovered_color, glm::vec3 _pressed_color)
{
	color[DEFAULT_BTN] = _default_color;
	color[HOVERED_BTN] = _hovered_color;
	color[PRESSED_BTN] = _pressed_color;
}

void Button::makeButton(glm::vec2 _pos, GLfloat _w, GLfloat _h, std::string _s, CharacterSet _ch_set)
{
	pos = _pos;
	w = _w;
	h = _h;
	string.setOrig(glm::vec2(_pos.x + _w / 2.0f, _pos.y + _h / 2.0f));
	string.setCharacterSet(_ch_set);
	string.setJustification(CENTERED);
	string.setString(_s);
}

bool Button::isOverButton(input_s* input)
{
	// On ramène les coordonnées de la souris entre 0 et 1
	float mx = (float)input->mousex/screenWidth;
	float my = 1.0f - (float)input->mousey/screenHeight;

	if((mx > pos.x) && (my > pos.y))
	{
		if((mx < pos.x + w) && (my < pos.y + h))
		{
			return true;
		}
	}
	return false;
}

bool Button::isButtonClicked()
{
	if(button_state == HOVERED_BTN && previous_button_state == PRESSED_BTN)
		return true;
	else
		return false;
}

void Button::updateButtonState(input_s* _input)
{
	previous_button_state = button_state;

	if(isOverButton(_input))
	{
		if(_input->mousebuttons[GLFW_MOUSE_BUTTON_LEFT])
		{
			button_state = PRESSED_BTN;
		}
		else
		{
			button_state = HOVERED_BTN;
		}
	}
	else
	{
		button_state = DEFAULT_BTN;
	}
	return;
}

void Button::drawButton(GLuint _program)
{
	string.draw(_program, color[(int)button_state]);
}



ButtonSet::ButtonSet()
{
	btn_cnt = 0;
}

void ButtonSet::newBtn(glm::vec2 _pos, GLfloat _w, GLfloat _h, std::string _s, CharacterSet _ch_set)
{
	btn[btn_cnt].makeButton(_pos, _w, _h, _s, _ch_set);
	btn_cnt++;
}

void ButtonSet::updateBtnsStates(input_s* _input)
{
	for(int i = 0; i < btn_cnt; i++)
	{
		btn[i].updateButtonState(_input);
	}
}
// Dessine tous les boutons de la liste
void ButtonSet::draw(GLuint _program)
{
	for(int i = 0; i < btn_cnt; i++)
	{
		btn[i].drawButton(_program);
	}
}

void ButtonSet::loadMainMenuBtns()
{
	CharacterSet ch_set("Fonts/zekton rg.ttf", (50.0f * screenWidth)/1366);
	btn_cnt = 0;
	newBtn(glm::vec2(0.35f, 0.6f), 0.3f, 0.07f, "LIVE VISION", ch_set);
	newBtn(glm::vec2(0.35f, 0.5f), 0.3f, 0.07f, "TRACK VISION", ch_set);
	//newBtn(glm::vec2(0.35f, 0.4f), 0.3f, 0.07f, "GLOBAL VISION", ch_set);
	newBtn(glm::vec2(0.35f, 0.2f), 0.3f, 0.07f, "SETTINGS", ch_set);
}


void ButtonSet::loadLiveVisionBtns()
{
	CharacterSet ch_set("Fonts/zekton rg.ttf", (30.0f * screenWidth)/1366);
	btn_cnt = 0;
	newBtn(glm::vec2(0.005f, 0.01f), 0.07f, 0.05f, "BACK", ch_set);
}

/*
void ButtonSet::loadSettingsBtns();
*/

bool ButtonSet::isBtnClicked(char _button_id)
{
	return btn[(int)_button_id].isButtonClicked();
}

DropDownMenu::DropDownMenu()
{
	item_cnt = 0;
	current_selection = 3;
	dropped = 0;
	menu_state = DEFAULT_MENU;
}

void DropDownMenu::loadMenu(glm::vec2 _pos, GLfloat _w, GLfloat _h, std::string* _items, int _item_cnt)
{
	int i, j;
	int w_power, h_power;

	item_cnt = _item_cnt;
	pos = _pos;
	w = _w;
	h = _h;

	w_power = 1;
	h_power = 1;

	while(w_power < w * screenWidth)
		w_power *= 2;
	while(h_power < h * item_cnt * screenHeight)
		h_power *= 2;

	GLubyte* background_dropped = new GLubyte [4 * w_power * h_power];

	//memset(background_dropped, 64, 4 * w_power * h_power);
	for(i = 0; i < h_power; i++)
	{
		for(j = 0; j < w_power; j++)
		{
			background_dropped[0 + (w_power * i + j) * 4] = 64;
			background_dropped[1 + (w_power * i + j) * 4] = 64;
			background_dropped[2 + (w_power * i + j) * 4] = 64;
			background_dropped[3 + (w_power * i + j) * 4] = 200;
		}
	}

	addFrame(background_dropped, w, h * item_cnt, w_power);

	background_tex.setPosition(glm::vec2(pos.x, pos.y - h * item_cnt), w, h * item_cnt);
	background_tex.setTexCoord(w, h * item_cnt, w_power, h_power);
	background_tex.setTexture(background_dropped, w_power, h_power);
	background_tex.init();

	CharacterSet ch_set("Fonts/Larke Neue Regular.ttf", 20.0 * screenWidth / 1366.0);
	menu_items.string_cnt = 0;
	for(i = 0; i < item_cnt; i++)
	{
		menu_items.addString(_items[i], glm::vec2(pos.x + 4.0f/screenWidth, pos.y - h * (i + 1) + 0.01f), {200, 200, 200}, ch_set, LEFT_JUSTIFIED);
	}
}

void DropDownMenu::loadPlotMenu(glm::vec2 _pos)
{
	int i;
	std::string item [13];
	//item = new std::string [13];

	item[0] = "X Acceleration";
	item[1] = "Y Acceleration";
	item[2] = "Z Acceleration";
	item[3] = "X Rotation";
	item[4] = "Y Rotation";
	item[5] = "Z Rotation";
	item[6] = "X Magnetic Flux";
	item[7] = "Y Magnetic Flux";
	item[8] = "Z Magnetic Flux";
	item[9] = "Temperature";
	item[10] = "Pressure";
	item[11] = "Altitude";
	item[12] = "Battery Voltage";

	loadMenu(_pos, 0.15f, 0.0325f, item, 13);
	//delete item;
}

void DropDownMenu::updateMenu(input_s* _input)
{
	int i = 0;
	float my = 1.0f - (float)_input->mousey/screenHeight;

	if(!dropped)	// When the menu is not dropped
	{
		updateMenuState(_input);
		if(isMenuClicked())
		{
			dropped = true;
			return;
		}
	}
	else			// When the menu is dropped
	{
		if(background_tex.isOverQuad(glm::vec2((float)_input->mousex/screenWidth, my)))
		{
			hovered_item = (pos.y - my)/h;
			// while(!default_tex.isOverTexture(_input, i) && i < item_cnt)
			// {
			// 	i++;
			// }
			// hovered_item = i;
		}
		else
		{
			hovered_item = -1;
		}
		if(_input->mousebuttons[GLFW_MOUSE_BUTTON_LEFT])		// When the user clicks
		{
			dropped = false;
			if(hovered_item >= 0)								// An item is clicked
			{
				current_selection = hovered_item;
			}
		}
	}
}

void DropDownMenu::drawMenu(GLuint _program)
{
	if(!dropped)	// When the menu is not dropped
	{
		menu_items.draw(_program, glm::vec2(pos.x + 4.0/screenWidth, pos.y), glm::vec3(0.0f, 0.9f, 0.7f), current_selection);
	}
	else
	{
		background_tex.draw(_program);
		menu_items.draw(_program, glm::vec2(pos.x + 4.0/screenWidth, pos.y), glm::vec3(0.0f, 0.9f, 0.7f), current_selection);
		menu_items.draw(_program);
		if(hovered_item >= 0)
		{
			menu_items.draw(_program, glm::vec3(0.0f, 0.9f, 0.7f), hovered_item);
		}
	}
}

bool DropDownMenu::isMenuClicked()
{
	if(menu_state == HOVERED_MENU && previous_menu_state == PRESSED_MENU)
		return true;
	else
		return false;
}

void DropDownMenu::updateMenuState(input_s* _input)
{
	previous_menu_state = menu_state;

	if(isOver(pos, w, h, _input))
	{
		if(_input->mousebuttons[GLFW_MOUSE_BUTTON_LEFT])
		{
			menu_state = PRESSED_MENU;
		}
		else
		{
			menu_state = HOVERED_MENU;
		}
	}
	else
	{
		menu_state = DEFAULT_MENU;
	}
	return;
}

int DropDownMenu::readCurrentSelection()
{
	return current_selection;
}

RadioBtn::RadioBtn()
{
	hovered = 0;
	btn_state = 0;
	w = 0.02f;
}

void RadioBtn::init(glm::vec2 _pos, bool _init_state)
{
	pos = _pos;
	int w_power = 1;
	int w_pixel = w * screenWidth;

	printf("w_pixel: %d\n", w_pixel);
	while(w_power < w_pixel)
		w_power *= 2.0f;
	printf("w_power: %d\n", w_power);

	GLchar* tex_data = new GLchar [4 * w_power * w_power];
	memset(tex_data, 0, 4 * w_power * w_power);
	for(int i =  0; i < w_pixel; i++)
	{
		for(int j = 0; j < w_pixel; j++)
		{
			if((dist(glm::vec2(i, j), glm::vec2(w_pixel / 2, w_pixel / 2)) < (float)w_pixel/2.0f - 1.0f) && (dist(glm::vec2(i, j), glm::vec2(w_pixel / 2, w_pixel / 2)) > (float)(3.5f * w_pixel) / 10.0f))
			{
				tex_data[(i * w_power + j) * 4 + 0] = 0;
				tex_data[(i * w_power + j) * 4 + 1] = 200;
				tex_data[(i * w_power + j) * 4 + 2] = 175;
				tex_data[(i * w_power + j) * 4 + 3] = 255;
			}
		}
	}
	inactive_cell.setPosition(pos, w, w * screenWidth / screenHeight);
	inactive_cell.setTexCoord(w, w * screenWidth / screenHeight, w_power, w_power);
	inactive_cell.setTexture((unsigned char*)tex_data, w_power, w_power);
	inactive_cell.init();


	for(int i =  0; i < w_pixel; i++)
	{
		for(int j = 0; j < w_pixel; j++)
		{
			if(dist(glm::vec2(i, j), glm::vec2(w_pixel / 2, w_pixel / 2)) < (float)2.0f * w_pixel / 10.0f)
			{
				tex_data[(i * w_power + j) * 4 + 0] = 0;
				tex_data[(i * w_power + j) * 4 + 1] = 200;
				tex_data[(i * w_power + j) * 4 + 2] = 175;
				tex_data[(i * w_power + j) * 4 + 3] = 255;
			}
		}
	}
	active_cell.setPosition(pos, w, w * screenWidth / screenHeight);
	active_cell.setTexCoord(w, w * screenWidth / screenHeight, w_power, w_power);
	active_cell.setTexture((unsigned char*)tex_data, w_power, w_power);
	active_cell.init();

	//memset(tex_data, 0, 4 * w_power * w_power);
	/*
	for(int i =  0; i < w_pixel; i++)
	{
		for(int j = 0; j < w_pixel; j++)
		{
			if((dist(glm::vec2(i, j), glm::vec2(w_pixel / 2, w_pixel / 2)) < (float)w_pixel/2.0f - 1.0f) && (dist(glm::vec2(i, j), glm::vec2(w_pixel / 2, w_pixel / 2)) > (float)3.0f * w_pixel / 10.0f))
			{
				tex_data[(i * w_power + j) * 4 + 0] = 50;
				tex_data[(i * w_power + j) * 4 + 1] = 255;
				tex_data[(i * w_power + j) * 4 + 2] = 255;
				tex_data[(i * w_power + j) * 4 + 3] = 255;
			}
		}
	}
	*/
	for(int i =  0; i < w_pixel; i++)
	{
		for(int j = 0; j < w_pixel; j++)
		{
			if(dist(glm::vec2(i, j), glm::vec2(w_pixel / 2, w_pixel / 2)) < (float)2.0f * w_pixel / 10.0f)
			{
				tex_data[(i * w_power + j) * 4 + 0] = 50;
				tex_data[(i * w_power + j) * 4 + 1] = 255;
				tex_data[(i * w_power + j) * 4 + 2] = 255;
				tex_data[(i * w_power + j) * 4 + 3] = 100;
			}
		}
	}
	hovered_cell.setPosition(pos, w, w * screenWidth / screenHeight);
	hovered_cell.setTexCoord(w, w * screenWidth / screenHeight, w_power, w_power);
	hovered_cell.setTexture((unsigned char*)tex_data, w_power, w_power);
	hovered_cell.init();

	btn_state = _init_state;

	delete tex_data;
}

void RadioBtn::setString(std::string _s, glm::vec3 _color, CharacterSet* _ch_set)
{
	string.setOrig(glm::vec2(pos.x + w + 0.001f, pos.y + w * screenWidth / screenHeight / 2.0f));
	string.setColor(glm::vec3(0.0f, 0.7f, 1.0f));
	string.setCharacterSet(_ch_set);
	string.setJustification(LEFT_CENTERED);
	string.setString(_s);
}

void RadioBtn::setState(bool _cell_state)
{
	btn_state = _cell_state;
}

void RadioBtn::update(input_s* _input)
{
	float mx = (float)_input->mousex/screenWidth;
	float my = 1.0f - (float)_input->mousey/screenHeight;

	if(active_cell.isOverQuad(glm::vec2(mx, my)))
	{
		hovered = 1;
		if(_input->mousebuttons[GLFW_MOUSE_BUTTON_LEFT])
		{
			pressed = 1;
		}
		else
		{
			if(pressed)
			{
				pressed = 0;
				if(btn_state)
					btn_state = 0;
				else
					btn_state = 1;
			}
		}
	}
	else
		hovered = 0;
}

void RadioBtn::draw(GLuint _program)
{
	if(btn_state)
		active_cell.draw(_program);
	else
	{
		if(hovered)
			hovered_cell.draw(_program);
		else
			inactive_cell.draw(_program);
	}
	string.draw(_program);
}

bool RadioBtn::isActive()
{
	return btn_state;
}

RadioBtnSet::RadioBtnSet(CharacterSet* _ch_set, char _type)
{
	ch_set = _ch_set;
	cell_cnt = 0;
	type = _type;
}

void RadioBtnSet::addCell(std::string _s, glm::vec2 _pos, glm::vec3 _color, bool _init_state)
{
	cells[cell_cnt].init(_pos, _init_state);
	cells[cell_cnt].setString(_s, _color, ch_set);

	cell_cnt++;
}

void RadioBtnSet::loadTrackVisionCells()
{
	cell_cnt = 0;
	type = SINGLE_CHOICE;
	addCell("Map", glm::vec2(0.03f, 0.8f), glm::vec3(0.0f, 0.7f, 0.8f), 1);
	addCell("Plot", glm::vec2(0.03f, 0.8 - 50.0f / screenHeight), glm::vec3(0.0f, 0.7f, 0.8f), 0);
}

void RadioBtnSet::loadsettingsCells()
{
	cell_cnt = 0;
	type = SINGLE_CHOICE;
	addCell("Windowed", glm::vec2(0.4f, 0.6f), glm::vec3(0.0f, 0.8f, 0.6f), 1);
	addCell("Fullscreen", glm::vec2(0.4f, 0.55f), glm::vec3(0.0f, 0.8f, 0.6f), 0);
}

void RadioBtnSet::update(input_s* _input)
{
	if(type == MULTIPLE_CHOICE)
	{
		for(int i = 0; i < cell_cnt; i++)
			cells[i].update(_input);
	}
	else if(type == SINGLE_CHOICE)
	{
		for(int i = 0; i < cell_cnt; i++)
		{
			cells[i].update(_input);
			if(cells[i].isActive() && i != single_active_cell)
			{
				cells[single_active_cell].setState(0);
				single_active_cell = i;
			}
		}
	}
}

void RadioBtnSet::draw(GLuint _program)
{
	for(int i = 0; i < cell_cnt; i++)
		cells[i].draw(_program);
}

bool RadioBtnSet::isActive(char _cell_id)
{
	return cells[(int)_cell_id].isActive();
}

char RadioBtnSet::activeCell()
{
	return single_active_cell;
}

Slider::Slider()
{
	hovered = 0;
	clicked = 0;
	slider_pos = 0.0f;
	h = 30.0f / screenHeight;
}

void Slider::init(glm::vec2 _pos, float _w)
{
	pos = _pos;
	float h_bar = 3.0f / screenHeight;
	w = _w;
	int w_power_slider = 1;
	int w_pixel_slider = h * screenHeight;

	int w_power_bar = 1;
	int h_power_bar = 1;
	int w_pixel_bar = w * screenWidth;
	int h_pixel_bar = h_bar * screenHeight;

	while(w_power_slider < w_pixel_slider)
		w_power_slider *= 2.0f;

	while(w_power_bar < w_pixel_bar)
		w_power_bar *= 2.0f;

	while(h_power_bar < h_pixel_bar)
		h_power_bar *= 2.0f;

	GLchar* tex_data = new GLchar [4 * w_power_slider * w_power_slider];
	memset(tex_data, 0, 4 * w_power_slider * w_power_slider);
	for(int i =  0; i < w_pixel_slider; i++)
	{
		for(int j = 0; j < w_pixel_slider; j++)
		{
			if(dist(glm::vec2(i, j), glm::vec2(w_pixel_slider / 2, w_pixel_slider / 2)) < (float)w_pixel_slider/2.0f - 1.0f)
			{
				tex_data[(i * w_power_slider + j) * 4 + 0] = 0;
				tex_data[(i * w_power_slider + j) * 4 + 1] = 200;
				tex_data[(i * w_power_slider + j) * 4 + 2] = 175;
				tex_data[(i * w_power_slider + j) * 4 + 3] = 200;
			}
		}
	}
	slider.setPosition(glm::vec2(pos.x - (w_pixel_slider + w_pixel_bar)/(2.0f * screenWidth), pos.y - w_pixel_slider/(2.0f * screenHeight)), h * screenHeight / screenWidth, h);
	slider.setTexCoord(h * screenHeight / screenWidth, h, w_power_slider, w_power_slider);
	slider.setTexture((unsigned char*)tex_data, w_power_slider, w_power_slider);
	slider.init();

	delete tex_data;
	tex_data = new GLchar [4 * w_power_bar * h_power_bar];
	memset(tex_data, 255, 4 * w_power_bar * h_power_bar);
	bar.setPosition(glm::vec2(pos.x - w_pixel_bar/(2.0f * screenWidth), pos.y - h_pixel_bar/(2.0f * screenHeight)), w, h_bar);
	bar.setTexCoord(w, h_bar, w_power_bar, w_power_bar);
	bar.setTexture((unsigned char*)tex_data, w_power_bar, w_power_bar);
	bar.init();
}

void Slider::setString(std::string _s, glm::vec3 _color, CharacterSet* _ch_set)
{
	string.setOrig(glm::vec2(pos.x + w / 2.0f + 0.01f, pos.y));
	string.setColor(_color);
	string.setCharacterSet(_ch_set);
	string.setJustification(LEFT_CENTERED);
	string.setString(_s);
}


void Slider::update(input_s* _input)
{
	float mx = (float)_input->mousex/screenWidth;
	float my = 1.0f - (float)_input->mousey/screenHeight;
	float min_pos, max_pos;
	min_pos = pos.x - (w + h * screenHeight/screenWidth)/2.0f;
	max_pos = pos.x + (w - h * screenHeight/screenWidth)/2.0f;

	if(_input->mousebuttons[GLFW_MOUSE_BUTTON_LEFT])
	{
		if(hovered == 1)
		{
			clicked = 1;
		}
	}
	else
	{
		clicked = 0;
		if(slider.isOverQuad(glm::vec2(mx, my)))
		{
			hovered = 1;
		}
		else
		{
			hovered = 0;
		}
	}

	if(clicked == 1)
	{
		if(mx < pos.x - w / 2.0f)
			slider_pos = 0.0f;
		else if(mx > pos.x + w / 2.0f)
			slider_pos = 1.0f;
		else
			slider_pos = (mx - (pos.x - w / 2.0f)) / w;
		slider.setPosition(glm::vec2(min_pos + slider_pos * (max_pos - min_pos), pos.y - h / 2.0f));
		slider.init();
	}
}

void Slider::draw(GLuint _program)
{
	bar.draw(_program);
	slider.draw(_program);
	string.draw(_program);
}

float Slider::value()
{
	return slider_pos;
}
