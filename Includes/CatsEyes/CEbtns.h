/*
 * CEbtns.h
 *
 *  Created on: Apr 19, 2016
 *      Author: root
 */

#ifndef CEBTNS_H_
#define CEBTNS_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include "events.h"
#include "fonts.h"
#include "vertex.h"

// Button states
#define DEFAULT_BTN		0
#define HOVERED_BTN		1
#define PRESSED_BTN		2

// Drop-down menu states
#define DEFAULT_MENU	0
#define HOVERED_MENU	1
#define PRESSED_MENU 	2

// Main menu buttons
#define LIVE_VISION_BUTTON 		0
#define TRACK_VISION_BUTTON 	1
#define GLOBAL_VISION_BUTTON	3
#define SETTINGS_BUTTON 		2

// Live vision buttons
#define BACK_BUTTON		0

// Track cell buttons
#define MAP_CELL		0
#define PLOT_CELL		1

// RadioBtnSet types
#define MULTIPLE_CHOICE	0
#define SINGLE_CHOICE	1

class Button
{
private:
	glm::vec2	pos;
	GLfloat		w, h;
	char 		button_state;
	char		previous_button_state;
	String2D 	string;
	glm::vec3	color[3];

public:
	Button();

	void setButtonColors(glm::vec3 _default_color, glm::vec3 _hovered_color, glm::vec3 _pressed_color);

	// Création d'un bouton comportant du texte à la position (x, y), de largeur _w et de hauteur _h
	void makeButton(glm::vec2 _pos, GLfloat _w, GLfloat _h, std::string _s, CharacterSet _ch_set);

	// Renvoie true si la souris est au dessus du bouton, false sinon
	bool isOverButton(input_s* input);

	bool isButtonClicked();

	void updateButtonState(input_s* _input);

	// Dessine le bouton
	void drawButton(GLuint _program);
};

class ButtonSet
{
private:
	Button btn[16];
	char btn_cnt;

public:
	ButtonSet();

	// Ajoute un bouton à la liste
	void newBtn(glm::vec2 _pos, GLfloat _w, GLfloat _h, std::string _s, CharacterSet _ch_set);

	void updateBtnsStates(input_s* _input);

	// Dessine tous les boutons de la liste
	void draw(GLuint _program);
	void loadMainMenuBtns();
	void loadLiveVisionBtns();
	/*
	void loadSettingsBtns();
	*/
	bool isBtnClicked(char _button_id);
};


class DropDownMenu
{
private:
	glm::vec2 pos;
	GLfloat w, h;
	int item_cnt;
	int hovered_item;
	int current_selection;
	bool dropped;

	Quad2D background_tex;
	StringSet menu_items;

	char menu_state;
	char previous_menu_state;

public:
	DropDownMenu();

	void loadMenu(glm::vec2 _pos, GLfloat _w, GLfloat _h, std::string* _items, int _item_cnt);

	//void loadFullscreenMenu();

	void loadPlotMenu(glm::vec2 _pos);

	bool isMenuClicked();

	void updateMenuState(input_s* _input);

	void updateMenu(input_s* _input);

	void drawMenu(GLuint _program);

	int readCurrentSelection();
};

class RadioBtn
{
private:
	glm::vec2 pos;
	float w;
	bool hovered;
	bool btn_state;
	bool pressed;

	Quad2D active_cell;
	Quad2D inactive_cell;
	Quad2D hovered_cell;
	String2D string;

public:
	RadioBtn();
	void init(glm::vec2 _pos, bool _init_state = 0);
	void setString(std::string _s, glm::vec3 _color, CharacterSet* _ch_set);
	void setState(bool _cell_state);
	void update(input_s* input);
	void draw(GLuint _program);

	bool isActive();
};

class RadioBtnSet
{
private:
	RadioBtn cells[64];
	int cell_cnt;
	CharacterSet* ch_set;
	char type;
	char single_active_cell;

public:
	RadioBtnSet(CharacterSet* _ch_set, char _type = MULTIPLE_CHOICE);
	void addCell(std::string _s, glm::vec2 _pos, glm::vec3 _color, bool _init_state);
	void loadTrackVisionCells();
	void loadsettingsCells();
	void update(input_s* _input);
	void draw(GLuint _program);

	bool isActive(char _cell_id);
	char activeCell();
};

class Slider
{
private:
	glm::vec2 pos; // Position of the center of the bar
	float w;
	float h;
	bool hovered;
	bool clicked;
	float slider_pos;

	Quad2D bar;
	Quad2D slider;
	String2D string;

public:
	Slider();
	void init(glm::vec2 _pos, float _w);
	void setString(std::string _s, glm::vec3 _color, CharacterSet* _ch_set);
	void update(input_s* input);
	void draw(GLuint _program);

	float value();
};



#endif /* CEBTNS_H_ */
