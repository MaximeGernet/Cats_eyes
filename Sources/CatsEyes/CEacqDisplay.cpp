/*
 * CEacqDisplay.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: root
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "basics.h"
#include "vertex.h"

#include "CEdata.h"
#include "CEacqDisplay.h"

AcqDisplay::AcqDisplay()
{
	initialized = 0;
	gps_valid = 0;
	sensors = ACC_MASK | GYR_MASK | MAG_MASK | BAR_MASK;
}

void AcqDisplay::init(glm::vec2 _pos, GLfloat _w, GLfloat _h)
{
	ch_set.load("Fonts/DroidSansMono.ttf", 21.0 * screenWidth / 1366.0);
	// Background initialization
	int w_pow, h_pow;

	pos = _pos;
	w = _w;
	h = _h;

	w_pow = 1;
	h_pow = 1;

	while(w_pow < w * screenWidth)
		w_pow *= 2;
	while(h_pow < h * screenHeight)
		h_pow *= 2;

	GLubyte* background_data = new GLubyte [4 * w_pow * h_pow];

	int i, j;
	for(i = 0; i < h_pow; i++)
	{
		for(j = 0; j < w_pow; j++)
		{
			background_data[0 + (w_pow * i + j) * 4] = 75;
			background_data[1 + (w_pow * i + j) * 4] = 100;
			background_data[2 + (w_pow * i + j) * 4] = 100;
			background_data[3 + (w_pow * i + j) * 4] = 64;
		}
	}

	addFrame(background_data, w, h, w_pow);

	background.setPosition(pos, w, h);
	background.setTexCoord(w, h, w_pow, h_pow);
	background.setTexture(background_data, w_pow, h_pow);
	background.init();
	delete background_data;

	// Obj initialization
	CharacterSet character_set("Fonts/Larke Neue Regular.ttf", 20.0 * screenWidth / 1366.0);

	obj.addString("* Acceleration", glm::vec2(pos.x + w * 0.02f, pos.y + h * 0.92f), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("X:", glm::vec2(pos.x + w * 0.01, pos.y + h * 0.85), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Y:", glm::vec2(pos.x + w * 0.01 + w/2.7, pos.y + h * 0.85), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Z:", glm::vec2(pos.x + w * 0.01 + w * 2.0/2.7, pos.y + h * 0.85), {200, 200, 200}, character_set, LEFT_JUSTIFIED);

	obj.addString("* Rotation", glm::vec2(pos.x + w * 0.02, pos.y + h * 0.77), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("X:", glm::vec2(pos.x + w * 0.01, pos.y + h * 0.7), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Y:", glm::vec2(pos.x + w * 0.01 + w/2.7, pos.y + h * 0.7), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Z:", glm::vec2(pos.x + w * 0.01 + w * 2.0/2.7, pos.y + h * 0.7), {200, 200, 200}, character_set, LEFT_JUSTIFIED);

	obj.addString("* Magnetic Flux", glm::vec2(pos.x + w * 0.02, pos.y + h * 0.62), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("X:", glm::vec2(pos.x + w * 0.01, pos.y + h * 0.55), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Y:", glm::vec2(pos.x + w * 0.01 + w/2.7, pos.y + h * 0.55), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Z:",glm::vec2(pos.x + w * 0.01 + w * 2.0/2.7, pos.y + h * 0.55), {200, 200, 200},  character_set, LEFT_JUSTIFIED);

	obj.addString("* Atmospheric Data", glm::vec2(pos.x + w * 0.02, pos.y + h * 0.47), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Temp:", glm::vec2(pos.x + w * 0.01, pos.y + h * 0.4), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Press:", glm::vec2(pos.x + w * 0.01 + w/2.7, pos.y + h * 0.4), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Alt:", glm::vec2(pos.x + w * 0.01 + w * 2.0/2.7, pos.y + h * 0.4), {200, 200, 200}, character_set, LEFT_JUSTIFIED);

	obj.addString("* GPS", glm::vec2(pos.x + w * 0.02, pos.y + h * 0.32), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Date:", glm::vec2(pos.x + w * 0.01, pos.y + h * 0.25), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Time:", glm::vec2(pos.x + w * 0.01 + w/2.7, pos.y + h * 0.25), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Sat:", glm::vec2(pos.x + w * 0.01 + w * 2.0/2.7, pos.y + h * 0.25), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Latitude:", glm::vec2(pos.x + w * 0.01, pos.y + h * 0.18), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Longitude:", glm::vec2(pos.x + w * 0.01 + w/2.7, pos.y + h * 0.18), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Alt:", glm::vec2(pos.x + w * 0.01, pos.y + h * 0.11), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Speed:", glm::vec2(pos.x + w * 0.01 + w/2.7, pos.y + h * 0.11), {200, 200, 200}, character_set, LEFT_JUSTIFIED);
	obj.addString("Angle:", glm::vec2(pos.x + w * 0.01 + w * 2.0/2.7, pos.y + h * 0.11), {200, 200, 200}, character_set, LEFT_JUSTIFIED);

	initialized = 1;
}

void AcqDisplay::draw(GLuint _program)
{
	background.draw(_program);
	obj.draw(_program);
	value.draw(_program);
}

void AcqDisplay::update(Acquisition* _acq)
{
	int plot_object;
	char value_s[64];

	for(plot_object = 0; plot_object < OBJECTS_CNT; plot_object++)
	{
		if(_acq->isUpdated(plot_object))
		{
			if(plot_object < 3 && (sensors & ACC_MASK))
			{
				sprintf(value_s, "% -5.1fm/s", _acq->readValue(plot_object));
				value_s[strlen(value_s)+1] = 0;
				value_s[strlen(value_s)] = '²';
				value.replaceString(value_s, glm::vec2(pos.x + w * 0.05 + w * plot_object/2.7, pos.y + h * 0.85), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
			}
			else if(plot_object < 6 && (sensors & GYR_MASK))
			{
				sprintf(value_s, "% -5.1f /s", _acq->readValue(plot_object));
				value_s[strlen(value_s) - 3] = '°';
				value.replaceString(value_s, glm::vec2(pos.x + w * 0.05 + w * (plot_object - 3)/2.7, pos.y + h * 0.7), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
			}
			else if(plot_object < 9 && (sensors & MAG_MASK))
			{
				sprintf(value_s, "% -5.1f T", _acq->readValue(plot_object));
				value_s[strlen(value_s) - 2] = 'µ';
				value.replaceString(value_s, glm::vec2(pos.x + w * 0.05 + w * (plot_object - 6)/2.7, pos.y + h * 0.55), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
			}
			else if(plot_object < 12 && (sensors & BAR_MASK))
			{
				if(plot_object == TEMPERATURE)
				{
					sprintf(value_s, "% -5.1f C", _acq->readValue(plot_object));
					value_s[strlen(value_s) - 2] = '°';
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.095, pos.y + h * 0.4), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == PRESSURE)
				{
					sprintf(value_s, "% -5.1fhPa", _acq->readValue(plot_object));
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.465, pos.y + h * 0.4), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else
				{
					sprintf(value_s, "% -5.1fm", _acq->readValue(plot_object));
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.8, pos.y + h * 0.4), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
			}
			else if(sensors & GPS_MASK)
			{
				if(plot_object == DATE)
				{
					_acq->readDate(value_s);
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.095, pos.y + h * 0.25), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == TIME)
				{
					_acq->readTime(value_s);
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.095 + w/2.7, pos.y + h * 0.25), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == SATELLITES)
				{
					sprintf(value_s, "%-2d", (int)_acq->readValue(plot_object));
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.095 + 2.0*w/2.7, pos.y + h * 0.25), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == LATITUDE)
				{
					_acq->readLatitude(value_s);
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.15, pos.y + h * 0.18), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == LONGITUDE)
				{
					_acq->readLongitude(value_s);
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.15 + w/2.7, pos.y + h * 0.18), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == ALT_GPS)
				{
					sprintf(value_s, "% -5.1fm", _acq->readValue(plot_object));
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.095, pos.y + h * 0.11), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == SPEED)
				{
					sprintf(value_s, "% -5.1fm/s", _acq->readValue(plot_object));
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.095, pos.y + h * 0.11), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
				else if(plot_object == ANGLE)
				{
					sprintf(value_s, "% -5.1f ", _acq->readValue(plot_object));
					value_s[strlen(value_s) - 1] = '°';
					value.replaceString(value_s, glm::vec2(pos.x + w * 0.095, pos.y + h * 0.11), {0, 175, 175}, ch_set, LEFT_JUSTIFIED, plot_object);
				}
			}
		}
	}
}
