/*
 * CEplot.h
 *
 *  Created on: Apr 20, 2016
 *      Author: root
 */

#ifndef CEPLOT_H_
#define CEPLOT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "events.h"

#include "CEbtns.h"

// Premier type de graphe
// Ce graphe est une texture mise à jour à chaque nouvelle donée apportée
class Plot
{
private:
	Quad2D y_axis;//(6);
	Quad2D grid;
	Quad2D curve[2];
	StringSet strings;
	GLubyte** tex_curve;	// Curve texture data, 2 buffers
	//GLubyte* grid;
	//GLubyte* y_axis;

	DropDownMenu plot_object_menu;
	int plot_object;
	int previous_plot_object;

	GLfloat x_start, x_end;
	GLfloat x_range;
	GLfloat y_start, y_end;
	GLfloat y_range;

	int w_power;
	int h_power;
	GLfloat Tacq;
	int pixel_step;
	GLfloat progression_step;// Pixels/frame
	GLfloat progression;		// Pixels since the last point
	GLfloat w, h;			// Dimensions du graphe à l'écran
	int w_pixel, h_pixel;	// Dimensions du graphe en pixels
	glm::vec2 coord;			// Position du graphe à l'écran
	GLfloat last_x_coord, last_y_coord; // in pixels
	int split_pos;			// position de la première colonne de pixel du buffer le plus récent
	char active_buffer;

	char x_label[16];
	char y_label[16];
	char title[16];

	GLfloat next_data;
	char data_ready;

	unsigned int t;
	unsigned int t_start;

	char initialized;

public:
	GLuint id1, id2;

	void swapCurveBuffers();

	int t_startBuffer;
	Plot();
	void init(GLfloat _x, GLfloat _y, GLfloat _w, GLfloat _h, int plot_object);
	~Plot();

	// Add a point to the plot. TODO: use a template
	void addPoint(Acquisition* _acq);
	// Add a data set to the plot
	//void addSet(Track _track);
	//void addSet(LiveTrack _track);

	void draw(GLuint _program);

	//void updateYAxis();

	void updatePlot(input_s* _input);
};

// Deuxième type de graphe
// Ce graphe est crée à partir de données stockées dans un fichier (charger toutes les données une seule fois ?)
// Il est possible de zoomer sur ce graphe, et de se déplacer sur l'axe des abscisses
class TrackPlot
{
private:
	GLfloat x_start;	// The following values are expressed in the units used in the file
	GLfloat x_end;
	GLfloat y_start;
	GLfloat y_end;
	GLfloat x_range;
	GLfloat y_range;

	glm::vec2 pos;		// The following values are expressed in screen coordinates (between 0.0f and 1.0f)
	GLfloat width;
	GLfloat height;

	Quad2D grid;
	Quad2D curve[2];
	StringSet strings;
	GLubyte** tex_curve;	// Curve texture data, 2 buffers

	TrackFiles* trackFiles;
	Track* track;

	GLuint w_power, h_power;
	GLuint w_pixel, h_pixel;

	char active_buffer;
	char plot_object;

	GLuint split_pos;
	int last_x_coord;
	GLfloat last_y_coord;

	int acq_start;
	int acq_end;

	CharacterSet* ch_set;

	bool clicked;
	float x_click, y_click;

public:
	TrackPlot();
	~TrackPlot();

	void init(GLfloat _x, GLfloat _y, GLfloat _w, GLfloat _h, int _plot_object);
	void update(input_s* _input);
	void fitY(int _acq_start, int _acq_end, float _acq_step);
	void fitX(int _acq_start, int _acq_end);
	void updateYAxis(input_s* _input);
	void updateXAxis(input_s* _input);
	void updateGrid(char _init);
	void initTexCurve();
	void updateTexCurve();
	void addPoint(Acquisition* _acq, int _pixel_step);
	void draw(GLuint _program);

};

#endif /* CEPLOT_H_ */
