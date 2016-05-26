/*
 * CEacqDisplay.h
 *
 *  Created on: Apr 19, 2016
 *      Author: root
 */

#ifndef CEACQDISPLAY_H_
#define CEACQDISPLAY_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "fonts.h"
#include "vertex.h"

// Objects
#define OBJECTS_CNT		21
#define X_ACCELERATION	0
#define Y_ACCELERATION	1
#define Z_ACCELERATION	2
#define X_ROTATION		3
#define Y_ROTATION		4
#define Z_ROTATION		5
#define X_MAGNETIC_FLUX	6
#define Y_MAGNETIC_FLUX	7
#define Z_MAGNETIC_FLUX	8
#define TEMPERATURE		9
#define PRESSURE		10
#define ALTITUDE		11
#define BATTERY_LVL		12
// Not for plots
#define DATE 			13
#define TIME 			14
#define SATELLITES 		15
#define LATITUDE 		16
#define LONGITUDE 		17
#define ALT_GPS 		18
#define SPEED 			19
#define ANGLE 			20

class AcqDisplay
{
private:
	StringSet obj;
	StringSet value;
	Quad2D background;
	bool gps_valid;
	bool initialized;
	char sensors;
	CharacterSet ch_set;

	glm::vec2 pos;
	GLfloat w, h;

public:
	AcqDisplay();
	void init(glm::vec2 _pos, GLfloat _w, GLfloat _h);
	void update(Acquisition* _acq);
	void draw(GLuint _program);
	void deleteTextures();
};



#endif /* CEACQDISPLAY_H_ */
