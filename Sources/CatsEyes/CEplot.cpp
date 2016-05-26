/*
 * CEplot.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: root
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#include "basics.h"
#include "events.h"

#include "CEbtns.h"
#include "CEdata.h"
#include "CEplot.h"


Plot::Plot()
{
	initialized = 0;
	plot_object = 0;
}

void Plot::init(GLfloat _x, GLfloat _y, GLfloat _w, GLfloat _h, int plot_object)
{
	if(initialized)
	{
		delete tex_curve[0];
		delete tex_curve[1];
		delete tex_curve;
	}

	initialized = 1;
	w_power = 1;
	h_power = 1;
	coord.x = _x;
	coord.y = _y;
	w = _w;
	h = _h;

	/*
	t = SDL_GetTicks();
	t_start = SDL_GetTicks();
	t_startBuffer = SDL_GetTicks();
	*/

	if(plot_object < 3)
	{
		y_start = -20;
		y_end = 20;
		y_range = 40;
		x_range = 10000;
		Tacq = 50;
	}
	else if(plot_object < 6)
	{
		y_start = -180;
		y_end = 180;
		y_range = 360;
		x_range = 10000;
		Tacq = 50;
	}
	else if(plot_object < 9)
	{
		y_start = -200;
		y_end = 200;
		y_range = 400;
		x_range = 10000;
		Tacq = 50;
	}
	else if(plot_object == 9)
	{
		y_start = 10;
		y_end = 30;
		y_range = 20;
		x_range = 10000;
		Tacq = 50;
	}
	else if(plot_object == 10)
	{
		y_start = 1000;
		y_end = 1100;
		y_range = 100;
		x_range = 10000;
		Tacq = 50;
	}
	else if(plot_object == 11)
	{
		y_start = 0;
		y_end = 200;
		y_range = 200;
		x_range = 10000;
		Tacq = 50;
	}
	else if(plot_object == 12)
	{
		y_start = 2;
		y_end = 4;
		y_range = 2;
		x_range = 10000;
		Tacq = 50;
	}

	w_pixel = _w * screenWidth;
	h_pixel = _h * screenHeight;

	last_x_coord = -2;
	last_y_coord = h_pixel / 2;


	pixel_step = ((GLfloat)w_pixel * Tacq) / (x_range);
	w_pixel += pixel_step;

	while(w_power < w_pixel)
		w_power *= 2;
	while(h_power < h_pixel)
		h_power *= 2;

	split_pos = w_pixel - (pixel_step - 1);

	progression = 0;
	progression_step = ((w_pixel - pixel_step) * 1000) / (FPS * x_range);

	active_buffer = 1;

	tex_curve = new GLubyte* [2];
	tex_curve[0] = new GLubyte [4 * w_power * h_power];
	tex_curve[1] = new GLubyte [4 * w_power * h_power];

	memset(tex_curve[0], 0, 4 * w_power * h_power);
	memset(tex_curve[1], 0, 4 * w_power * h_power);
	/*
	glGenTextures(1, &id1);
	glBindTexture(GL_TEXTURE_2D, id1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_power, h_power, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_curve[1 - active_buffer]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glGenTextures(1, &id2);
	glBindTexture(GL_TEXTURE_2D, id2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_power, h_power, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_curve[active_buffer]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	*/
	curve[active_buffer].setTexture(tex_curve[active_buffer], w_power, h_power);
	curve[active_buffer].init();
	curve[1 - active_buffer].setTexture(tex_curve[1 - active_buffer], w_power, h_power);
	curve[1 - active_buffer].init();

	CharacterSet ch_set("Fonts/Larke Neue Regular.ttf", 10.0f * screenWidth / 1366.0f);
	strings.string_cnt = 0;
	int n = 1;
	char n_s[32];
	sprintf(n_s, "%d", (int)y_start);
	strings.addString(n_s, glm::vec2(_x - 0.005, _y), glm::vec3(1.0f, 1.0f, 1.0f), ch_set, RIGHT_JUSTIFIED);
	GLubyte* grid_tex = new GLubyte [4 * w_power * h_power];
	memset((void*)grid_tex, 0, 4 * w_power * h_power);
	int i, j, k;
	for(i = h_pixel/4; i < h_pixel + 1; i += h_pixel/4)
	{
		for(j = 0; (j + 4) < w_pixel - pixel_step; j+= 12)
		{
			for(k = 0; k < 4; k++)
			{
				grid_tex[0 + (w_power * i + j + k) * 4] = 128;
				grid_tex[1 + (w_power * i + j + k) * 4] = 128;
				grid_tex[2 + (w_power * i + j + k) * 4] = 128;
				grid_tex[3 + (w_power * i + j + k) * 4] = 255;
			}
		}
		sprintf(n_s, "%d", (int)(y_start + n * y_range / 4));
		strings.addString(n_s, glm::vec2(_x - 0.005f, _y + (GLfloat)i * 0.95f / screenHeight), glm::vec3(1.0f, 1.0f, 1.0f), ch_set, RIGHT_JUSTIFIED);
		n++;
	}

	if(plot_object < 3)
	{
		sprintf(n_s, "m/s ");
		n_s[3] = '²';
	}
	else if(plot_object < 6)
	{
		sprintf(n_s, "°/s");
		n_s[0] = ' ';
	}
	else if(plot_object < 9)
	{
		sprintf(n_s, " T");
		n_s[0] = 'µ';
	}
	else if(plot_object == 9)
	{
		sprintf(n_s, " C");
		n_s[0] = '°';
	}
	else if(plot_object == 10)
	{
		sprintf(n_s, "hPa");
	}
	else if(plot_object == 11)
	{
		sprintf(n_s, "m");
	}
	else if(plot_object == 12)
	{
		sprintf(n_s, "V");
	}
	strings.addString(n_s, glm::vec2(_x + 0.01f, _y + h_pixel / screenHeight + 0.01f), glm::vec3(1.0f, 1.0f, 1.0f), ch_set, LEFT_JUSTIFIED);

	for(i = (w_pixel - pixel_step)/5; i < w_pixel - pixel_step; i += (w_pixel - pixel_step)/5)
	{
		for(j = 0; (j + 4) < h_pixel; j+= 12)
		{
			for(k = 0; k < 4; k++)
			{
				grid_tex[0 + (w_power * (j+k) + i) * 4] = 128;
				grid_tex[1 + (w_power * (j+k) + i) * 4] = 128;
				grid_tex[2 + (w_power * (j+k) + i) * 4] = 128;
				grid_tex[3 + (w_power * (j+k) + i) * 4] = 255;
			}
		}
	}

	for(j = 0; j < w_pixel - pixel_step; j++)
	{
		grid_tex[0 + j * 4] = 128;
		grid_tex[1 + j * 4] = 128;
		grid_tex[2 + j * 4] = 128;
		grid_tex[3 + j * 4] = 255;
	}

	for(j = 0; j < h_pixel; j++)
	{
		grid_tex[0 + w_power * j * 4] = 128;
		grid_tex[1 + w_power * j * 4] = 128;
		grid_tex[2 + w_power * j * 4] = 128;
		grid_tex[3 + w_power * j * 4] = 255;
	}

	grid.setPosition(glm::vec2(_x, _y), _w, _h);
	grid.setTexCoord(_w, _h, w_power, h_power);
	grid.setTexture(grid_tex, w_power, h_power);
	grid.init();

	//TTF_Font *font_title = TTF_OpenFont("Fonts/space age.ttf", 20);
	//TTF_Font *font_title = TTF_OpenFont("Fonts/good_times_rg.ttf", 20);
	//TTF_Font *font_title = TTF_OpenFont("Fonts/falconpunchout3d.ttf", 20);
	//TTF_Font *font_title = TTF_OpenFont("Fonts/masque.ttf", 20);
	//plot_tex.makeTextTextureBlended(_x + _w / 2, _y + _h + 0.025, {0, 230, 190}, "  Acceleration",font_title, CENTERED);
	plot_object_menu.loadPlotMenu(glm::vec2(_x + 0.1f, _y + _h + 0.0125f));

	//y_axis = new GLubyte [4 * 128 * h_power];
	//memset((void*)y_axis, 0, 4 * 128 * h_power);
	//updateYAxis();

	//plot_tex.makeTexture(_x - 2.0 * 128.0 / WIDTH, _y, 2.0 * 128.0 / WIDTH, 2.0 * h_power / HEIGHT, y_axis, 128, h_power);
	data_ready = 0;
}

void Plot::updatePlot(input_s* _input)
{
	previous_plot_object = plot_object;
	plot_object_menu.updateMenu(_input);
	plot_object = plot_object_menu.readCurrentSelection();

	if(plot_object != previous_plot_object)
	{
		init(coord.x, coord.y, w, h, plot_object);
	}
}

Plot::~Plot()
{
	if(initialized)
	{
		delete tex_curve[0];
		delete tex_curve[1];
		delete tex_curve;
	}
}

void Plot::addPoint(Acquisition* acq)
{
	int y_coord;
	int x_coord;
	int x_rel, y_rel;
	int x_in, y_in;
	int delta_x;
	int delta_y;
	float value;

	if(acq->isUpdated(plot_object))
		value = acq->readValue(plot_object);
	else
		return;

	if(last_x_coord == -2)
	{
		last_x_coord = 0;
		last_y_coord = ((value - y_start) * h_pixel)/y_range;
		return;
	}

	y_coord = ((value - y_start) * h_pixel)/y_range;
	x_coord = w_pixel - split_pos;
	delta_x = x_coord - last_x_coord;
	delta_y = y_coord - last_y_coord;
	x_in = last_x_coord;
	y_in = last_y_coord;
	x_rel = 0;
	y_rel = 0;

	if((delta_y >= 0 && delta_x >= delta_y) || (delta_y <= 0 && delta_x >= -delta_y))
	{
		while(x_in < x_coord)
		{
			x_in++;
			x_rel++;
			y_in = last_y_coord + (x_rel * delta_y) / delta_x;
			createDot(x_in, y_in, 1, tex_curve[active_buffer], w_pixel, w_power);
		}
	}
	else
	{
		if(delta_y > 0)
		{
			while(y_in < y_coord)
			{
				y_in++;
				y_rel++;
				x_in = last_x_coord + (y_rel * delta_x) / delta_y;
				createDot(x_in, y_in, 1, tex_curve[active_buffer], w_pixel, w_power);
				//printf("x: %d\ty: %d\n", x_in, y_in);
			}
		}
		else
		{
			while(y_in > y_coord)
			{
				y_in--;
				y_rel--;
				x_in = last_x_coord + (y_rel * delta_x) / delta_y;
				createDot(x_in, y_in, 1, tex_curve[active_buffer], w_pixel, w_power);
			}
		}
	}

	last_x_coord = x_coord;
	last_y_coord = y_coord;
	//last_x_coord = progression;
	//progression += (double)x_rel * progression;
	if(split_pos <= pixel_step)//pixel_step)
	{
		swapCurveBuffers();
	}
	else
	{
		split_pos -= pixel_step;
	}
}

/*
void fonction(float* target)
{
	progression += progression_step;
	double delta_x = x_target - x_progression;
	double delta_y = y_target - y_progression;

	if(delta_x > delta_y)
	{

	}
}
*/

void Plot::swapCurveBuffers()
{
	//split_pos = w_pixel - 2 * pixel_step;
	split_pos = w_pixel - (pixel_step - 1);
	if(active_buffer == 1)
		active_buffer = 0;
	else
		active_buffer = 1;
	memset(tex_curve[active_buffer], 0, 4 * w_power * h_power);
	//texCopy(tex_curve[active_buffer], tex_curve[1 - active_buffer], pixel_step, h_pixel, w_power, w_pixel - pixel_step);
	//last_x_coord = pixel_step;
	last_x_coord = 0;//-1;
	progression = 0;
	//t_start = SDL_GetTicks();
	//t_startBuffer = SDL_GetTicks();

	GLuint id3;
	id3 = id2;
	id2 = id1;
	id1 = id3;
}

void Plot::draw(GLuint _program)
{
	grid.draw(_program);
	strings.draw(_program);

	//progression += progression_step;
	//progression = progression + ((double)((SDL_GetTicks() - t) * 60)) * progression_step / 1000.0 * 0.97;
	//progression = ((double)w_pixel - pixel_step)/x_range * ((double)SDL_GetTicks() - t_start);

	if(progression < w_pixel - split_pos)
		progression += pixel_step;
	//progression = (double)w_pixel * ((double)SDL_GetTicks() - t_start) / (((double)w_pixel/(double)pixel_step) * (double)Tacq);
	//if(progression > w_pixel)
	//	printf("progression: %d\n", (int)progression);
	//t = SDL_GetTicks();
	//if(progression <= 0)
	//	return;
	/*
	Quad2D quad;
	//GLubyte* quad_data = new GLubyte [4 * w_power * h_power];
	//memset((void*)quad_data, 255, 4 * w_power * h_power);
	quad.setPosition(glm::vec2(coord.x + ((GLfloat)w_pixel - progression) / screenWidth, coord.y), w - ((GLfloat)w_pixel - progression) / screenWidth, h);
	//quad.setPosition(glm::vec2(0.5f), 0.5f, 0.5f);
	//quad.setTexCoord(((GLfloat)progression - pixel_step) / screenWidth, h, w_power, h_power);
	quad.setTexCoord(((GLfloat)(progression - pixel_step)) / screenWidth, h, w_power, h_power);
	//quad.setTexCoord(w, h, w_power, h_power);
	quad.setTexture(tex_curve[active_buffer], w_power, h_power);
	quad.init();
	quad.draw(_program);
	*/
	//delete quad_data;

	if(progression > pixel_step)
	{
		curve[active_buffer].setPosition(glm::vec2(coord.x + ((GLfloat)(w_pixel - progression)) / screenWidth, coord.y), w - ((GLfloat)(w_pixel - progression)) / screenWidth, h);
		curve[active_buffer].setTexCoord(((GLfloat)(progression - pixel_step)) / screenWidth, h, w_power, h_power);
		curve[active_buffer].updateTexture(tex_curve[active_buffer], w_power, h_power);
		curve[active_buffer].init();	// ??
		curve[active_buffer].draw(_program);

		curve[1 - active_buffer].setPosition(coord, ((GLfloat)(w_pixel + pixel_step - progression)) / screenWidth, h);
		curve[1 - active_buffer].setTexCoord(glm::vec2((GLfloat)progression/w_power, 0.0f), glm::vec2((GLfloat)w_pixel/w_power, (GLfloat)h_pixel/h_power));
		curve[1 - active_buffer].updateTexture(tex_curve[1 - active_buffer], w_power, h_power);
		curve[1 - active_buffer].init();	// ??
		curve[1 - active_buffer].draw(_program);
	}
	else
	{
		curve[1 - active_buffer].setPosition(coord, w, h);
		curve[1 - active_buffer].setTexCoord(glm::vec2((GLfloat)progression/w_power, 0.0f), glm::vec2((GLfloat)(w_pixel - pixel_step + progression)/w_power, (GLfloat)h_pixel/h_power));
		curve[1 - active_buffer].updateTexture(tex_curve[1 - active_buffer], w_power, h_power);
		curve[1 - active_buffer].init();	// ??
		curve[1 - active_buffer].draw(_program);
	}

	plot_object_menu.drawMenu(_program);
}

/*
void Plot::updateYAxis()
{
	int i, j, k;
	memset(y_axis, 0, 4 * 128 * h_power);
	TTF_Font *font = TTF_OpenFont("Fonts/Larke Neue Regular.ttf", 20);
	SDL_Surface* sdl_text = NULL;
	SDL_Surface* sdl_axis = SDL_CreateRGBSurface(SDL_HWSURFACE, 128, h_power, 32, 0, 0, 0, 0);
	SDL_FillRect(sdl_axis, NULL, SDL_MapRGBA(sdl_axis->format, 0, 0, 0, 0));

	for(i = 0; i < h_pixel; i++)
	{
		y_axis[0 + (128 * i + 127) * 4] = 128;
		y_axis[1 + (128 * i + 127) * 4] = 128;
		y_axis[2 + (128 * i + 127) * 4] = 128;
		y_axis[3 + (128 * i + 127) * 4] = 255;
	}

	SDL_Color color_sdl = {128, 128, 128};

	SDL_Rect pos;
	for(i = h_pixel/4; i < h_pixel; i += h_pixel/4)
	{
		sdl_text = TTF_RenderText_Blended(font, "42", color_sdl);
		pos.x = 128 - sdl_text->w;
		pos.y = h_pixel - i;
		SDL_BlitSurface(sdl_text, NULL, sdl_axis, &pos);
	}

	SDL_Surface* flipped = flipSurface(sdl_axis);
	memcpy(y_axis, flipped->pixels, 4 * 128 * h_power);

	TTF_CloseFont(font);
	SDL_FreeSurface(sdl_text);
	SDL_FreeSurface(sdl_axis);
}
*/



TrackPlot::TrackPlot()
{
	active_buffer = 1;
	tex_curve = NULL;
	trackFiles = NULL;
	track = NULL;
	ch_set = NULL;
	acq_start = 0;
	clicked = 0;
}
TrackPlot::~TrackPlot()
{
	if(tex_curve)
	{
		delete tex_curve[0];
		delete tex_curve[1];
		delete tex_curve;
	}
	if(trackFiles)
	{
		delete trackFiles;
	}
	if(track)
	{
		delete track;
	}
	if(ch_set)
	{
		delete ch_set;
	}
}

void TrackPlot::init(GLfloat _x, GLfloat _y, GLfloat _w, GLfloat _h, int _plot_object)
{
	width = _w;
	height = _h;
	pos.x = _x;
	pos.y = _y;

	w_power = 1;
	h_power = 1;

	w_pixel = width * screenWidth;
	h_pixel = height * screenHeight;

	while(w_power < w_pixel)
		w_power *= 2;
	while(h_power < h_pixel)
		h_power *= 2;

	tex_curve = new GLubyte* [2];
	tex_curve[0] = new GLubyte [4 * w_power * h_power];
	tex_curve[1] = new GLubyte [4 * w_power * h_power];
	memset(tex_curve[0], 0, 4 * w_power * h_power);
	memset(tex_curve[1], 0, 4 * w_power * h_power);

	plot_object = _plot_object;

	last_x_coord = -2;
	split_pos = w_pixel - 1;

	trackFiles = new TrackFiles;
	track = new Track(trackFiles);
	acq_end = track->getNumberOfPoints() - 1;
	ch_set = new CharacterSet("Fonts/Larke Neue Regular.ttf", 15.0f * screenWidth / 1366.0f);

	initTexCurve();

	curve[active_buffer].setPosition(glm::vec2(_x, _y), _w, _h);
	curve[active_buffer].setTexCoord(_w, _h, w_power, h_power);
	curve[active_buffer].setTexture(tex_curve[active_buffer], w_power, h_power);
	curve[active_buffer].init();
}

void TrackPlot::initTexCurve()
{
	x_range = track->duration();
	x_end = x_start + x_range;
	//printf("x_range: %f\n", x_range);

	GLuint number_of_points = track->getNumberOfPoints();
	float data_step = (float)number_of_points / (float)w_pixel; // 1 point by pixel
	//printf("data_step: %f\n", data_step);
	acq_start = 0;
	acq_end = number_of_points - 1;
	//printf("points: %d\n", number_of_points);
	//printf("step: %f\n", data_step);
	fitY(0, acq_end, data_step);
	fitX(0, acq_end);
	updateGrid(1);
	float i = 0.0f;
	while((i < number_of_points) && (i / data_step < w_pixel))
	{
		addPoint(track->acquisition((int)i), 1);
		i += data_step;
	}
}

void TrackPlot::update(input_s* _input)
{
	int number_of_points = acq_end - acq_start + 1;
	float acq_step = (float)number_of_points / (float)w_pixel;

	updateXAxis(_input);
	updateYAxis(_input);
	updateGrid(0);
	updateTexCurve();
	/*
	float i = 0.0f;
	while((i < number_of_points) && (i / acq_step < w_pixel))
	{
		addPoint(track->acquisition((int)i), 1);
		i += acq_step;
	}
	*/
}

void TrackPlot::fitY(int _acq_start, int _acq_end, float _acq_step)
{
	float i = _acq_start;// + _acq_step * 10.0f;
	Acquisition* acq = track->acquisition((int)i);
	GLfloat max_value = acq->readValue(plot_object);
	GLfloat min_value = acq->readValue(plot_object);

	for(i = _acq_start + _acq_step * 10.0f; i < _acq_end; i += _acq_step)
	{
		//printf("value: %f\n", acq->readValue(plot_object));
		acq = track->acquisition((int)i);
		max_value = (acq->readValue(plot_object) > max_value) ? acq->readValue(plot_object) : max_value;
		min_value = (acq->readValue(plot_object) < min_value) ? acq->readValue(plot_object) : min_value;
	}


	y_range = (max_value - min_value) * 1.2f;
	y_start = min_value - y_range * 0.1f;
	y_end = y_start + y_range;
	//printf("y_start: %f\n", y_start);
	//printf("y_range: %f\n", y_range);
}

void TrackPlot::fitX(int _acq_start, int _acq_end)
{
	Acquisition* start = track->acquisition(_acq_start);
	Acquisition* end = track->acquisition(_acq_end);
	x_start = start->readTime();
	x_range = end->readTime() - start->readTime();
	//printf("x_start: %f\n", x_start);
	//printf("x_range: %f\n", x_range);
}

void TrackPlot::updateXAxis(input_s* _input)
{
	//x_start = start->readTime();
	//x_range = end->readTime() - start->readTime();
	float sensitivity = 10.0f;

	if(_input->mousebuttons[GLFW_MOUSE_BUTTON_RIGHT])
	{
		if(clicked == 0)
		{
			clicked = 1;
			x_click = _input->mousex - pos.x * screenWidth;
			y_click = (screenHeight - _input->mousey) - pos.y * screenHeight;
		}
		else
		{
			acq_start = 0;
			acq_end = track->getNumberOfPoints() - 1;
			Acquisition* start = track->acquisition(acq_start);
			Acquisition* end = track->acquisition(acq_end);
			//printf("moving x_start\n");
			//printf("x_click: %f\n", x_click);
			x_start += _input->mousexrel * sensitivity/2.0f * x_range/1000.0f * x_click / w_pixel;
			x_end -= _input->mousexrel * sensitivity/2.0f * x_range/1000.0f * (1.0f - x_click / w_pixel);

			y_start += _input->mouseyrel * sensitivity/2.0f * y_range/1000.0f * y_click / h_pixel;
			y_end -= _input->mouseyrel * sensitivity/2.0f * y_range/1000.0f * (1.0f - y_click / h_pixel);

			if(x_end < x_start + 50.0f * track->duration() / track->getNumberOfPoints())
			{
				//x_end += 5.0f * track->duration() / track->getNumberOfPoints();
				//x_start -= 5.0f * track->duration() / track->getNumberOfPoints();

				x_start -= _input->mousexrel * sensitivity/2.0f * x_range/1000.0f * x_click / w_pixel;
				x_end += _input->mousexrel * sensitivity/2.0f * x_range/1000.0f * (1.0f - x_click / w_pixel);
			}
			if(x_start < start->readTime())
				x_start = start->readTime();
			if(x_end > end->readTime())
				x_end = end->readTime();
			x_range = x_end - x_start;
			y_range = y_end - y_start;
			//printf("x_start: %f\n", x_start);
			//printf("x_range: %f\n", x_range);

			while(start->readTime() < x_start)
			{
				acq_start ++;
				start = track->acquisition(acq_start);
			}
			//printf("x_end: %f\n", x_end);
			while(end->readTime() > x_end)
			{
				acq_end --;
				end = track->acquisition(acq_end);
			}


			//printf("acq_start: %d; acq_end: %d\n", acq_start, acq_end);
		}
	}
	else if(_input->mousebuttons[GLFW_MOUSE_BUTTON_LEFT])
	{
		if(clicked == 0)
		{
			clicked = 1;
			x_click = _input->mousex - pos.x * screenWidth;
			y_click = (screenHeight - _input->mousey) - pos.y * screenHeight;
		}
		else
		{
			acq_start = 0;
			acq_end = track->getNumberOfPoints() - 1;
			Acquisition* start = track->acquisition(acq_start);
			Acquisition* end = track->acquisition(acq_end);
			//printf("moving x_start\n");
			//printf("x_click: %f\n", x_click);
			float delta_x = _input->mousexrel * x_range/w_pixel;
			float delta_y = _input->mouseyrel * y_range/h_pixel;
			x_start -= delta_x;
			x_end -= delta_x;
			y_start -= delta_y;
			y_end -= delta_y;

			if(x_start < start->readTime())
			{
				x_start += delta_x;
				x_end += delta_x;
			}
			if(x_end > end->readTime())
			{
				x_start += delta_x;
				x_end += delta_x;
			}
			x_range = x_end - x_start;
			y_range = y_end - y_start;
			//printf("x_start: %f\n", x_start);
			//printf("x_range: %f\n", x_range);

			while(start->readTime() < x_start)
			{
				acq_start ++;
				start = track->acquisition(acq_start);
			}
			//printf("x_end: %f\n", x_end);
			while(end->readTime() > x_end)
			{
				acq_end --;
				end = track->acquisition(acq_end);
			}
			//printf("acq_start: %d; acq_end: %d\n", acq_start, acq_end);
		}
	}
	else
	{
		clicked = 0;
	}
	_input->mousexrel = 0;
	_input->mouseyrel = 0;
}

void TrackPlot::updateYAxis(input_s* _input)
{

}

void TrackPlot::updateTexCurve()
{
	GLuint number_of_points = acq_end - acq_start;
	float data_step = (float)number_of_points / (float)w_pixel; // 1 point by pixel
	float i = acq_start;

	last_x_coord = -2;
	split_pos = w_pixel - 1;
	//printf("\n\nUpdating\n");
	//printf("%f\n", data_step);
	memset(tex_curve[active_buffer], 0, 4 * w_power * h_power);
	while((i < acq_end))// && (i / data_step < w_pixel))
	{
		addPoint(track->acquisition((int)i), 1);
		i += data_step;
	}
	curve[active_buffer].updateTexture(tex_curve[active_buffer], w_power, h_power);
	curve[active_buffer].init();
}

void TrackPlot::updateGrid(char _init)
{
	//printf("wrinting texture...\n");
	GLubyte* grid_tex = new GLubyte [4 * w_power * h_power];
	memset((void*)grid_tex, 50, 4 * w_power * h_power);
	// Values in file untis
	float Y_grid_spacing;
	float X_grid_spacing;
	float Y_grid_start;
	float X_grid_start;
	// Values in pixels
	float y_grid_spacing;
	float x_grid_spacing;
	float y_grid_start;
	float x_grid_start;
	float x_coord, y_coord;

	int x_power = -2;
	int y_power = -2;

	// Calcul des paramètres de la grille
	Y_grid_spacing = y_range / 1.0f;
	while(pow(10, y_power) < Y_grid_spacing)
		y_power++;
	y_power--;
	//printf("y_power: %d; ", y_power);

	Y_grid_spacing = pow(10, y_power);
	y_grid_spacing = Y_grid_spacing * (float)h_pixel / y_range;
	if(y_start > 0)
	{
		Y_grid_start = ((int)(y_start/Y_grid_spacing) + 1) * Y_grid_spacing;
	}
	else
	{
		Y_grid_start = ((int)(y_start/Y_grid_spacing)) * Y_grid_spacing;
	}
	y_grid_start = (Y_grid_start - y_start) * (float)h_pixel / y_range;
	//printf("y_grid_spacing: %f; y_grid_start: %f\n", y_grid_spacing, y_grid_start);

	x_power = -2;
	X_grid_spacing = x_range / 1.0f;
	while(pow(10, x_power) < X_grid_spacing)
		x_power++;
	x_power--;
	//printf("x_power: %d; ", x_power);

	X_grid_spacing = pow(10, x_power);
	x_grid_spacing = X_grid_spacing * (float)w_pixel / x_range;
	if(x_start > 0)
	{
		X_grid_start = ((int)(x_start/X_grid_spacing) + 1) * X_grid_spacing;
	}
	else
	{
		X_grid_start = ((int)(x_start/X_grid_spacing)) * X_grid_spacing;
	}
	x_grid_start = (X_grid_start - x_start) * (float)w_pixel / x_range;
	//printf("x_grid_spacing: %f; x_grid_start: %f\n", x_grid_spacing, x_grid_start);
	if(x_grid_start < 0)
		x_grid_start = 0;

	// Drawing the grid
	// y axis
	for(y_coord = y_grid_start; y_coord < h_pixel; y_coord += y_grid_spacing)
	{
		for(x_coord = 0; (x_coord + 4) < (w_pixel - 1); x_coord += 12)
		{
			for(int k = 0; k < 4; k++)
			{
				grid_tex[0 + (w_power * (int)y_coord + (int)x_coord + k) * 4] = 128;
				grid_tex[1 + (w_power * (int)y_coord + (int)x_coord + k) * 4] = 128;
				grid_tex[2 + (w_power * (int)y_coord + (int)x_coord + k) * 4] = 128;
				grid_tex[3 + (w_power * (int)y_coord + (int)x_coord + k) * 4] = 255;
			}
		}
	}
	for(y_coord = 0; y_coord < h_pixel; y_coord++)
	{
		grid_tex[0 + w_power * (int)y_coord * 4] = 128;
		grid_tex[1 + w_power * (int)y_coord * 4] = 128;
		grid_tex[2 + w_power * (int)y_coord * 4] = 128;
		grid_tex[3 + w_power * (int)y_coord * 4] = 255;
	}

	// x axis
	for(x_coord = x_grid_start; x_coord < w_pixel; x_coord += x_grid_spacing)
	{
		for(y_coord = 0; (y_coord + 4) < (h_pixel - 1); y_coord += 12)
		{
			for(int k = 0; k < 4; k++)
			{
				grid_tex[0 + (w_power * ((int)y_coord + k) + (int)x_coord) * 4] = 128;
				grid_tex[1 + (w_power * ((int)y_coord + k) + (int)x_coord) * 4] = 128;
				grid_tex[2 + (w_power * ((int)y_coord + k) + (int)x_coord) * 4] = 128;
				grid_tex[3 + (w_power * ((int)y_coord + k) + (int)x_coord) * 4] = 255;
			}
		}
	}
	for(x_coord = 0; x_coord < w_pixel; x_coord++)
	{
		grid_tex[0 + (int)x_coord * 4] = 128;
		grid_tex[1 + (int)x_coord * 4] = 128;
		grid_tex[2 + (int)x_coord * 4] = 128;
		grid_tex[3 + (int)x_coord * 4] = 255;
	}
	//printf("done writing texture\n");

	// Setting the strings
	strings.string_cnt = 0;
	char value[64];
	float i = 0.0f;
	for(y_coord = y_grid_start; y_coord < h_pixel; y_coord += y_grid_spacing)
	{
		switch(y_power)
		{
		case -2:
			sprintf(value, "%0.2f", Y_grid_start + i * Y_grid_spacing);
			break;
		case -1:
			sprintf(value, "%0.1f", Y_grid_start + i * Y_grid_spacing);
			break;
		default:
			sprintf(value, "%0.0f", Y_grid_start + i * Y_grid_spacing);
			break;
		}
		i++;
		strings.addString(value, glm::vec2(pos.x - 0.01f, pos.y + y_coord / screenHeight), glm::vec3(0.3f, 1.0f, 0.8f), ch_set, RIGHT_JUSTIFIED);
	}
	i = 0.0f;
	//printf("X_grid_start: %f; X_grid_spacing: %f\n", Y_grid_start, y_start);
	for(x_coord = x_grid_start; x_coord < w_pixel; x_coord += x_grid_spacing)
	{
		switch(x_power)
		{
		case -2:
			sprintf(value, "%0.2f", X_grid_start + i * X_grid_spacing);
			break;
		case -1:
			sprintf(value, "%0.1f", X_grid_start + i * X_grid_spacing);
			break;
		default:
			sprintf(value, "%0.0f", X_grid_start + i * X_grid_spacing);
			break;
		}
		//printf("%s\n", value);
		i++;
		strings.addString(value, glm::vec2(pos.x + x_coord / screenWidth, pos.y - 0.02f), glm::vec3(0.3f, 1.0f, 0.8f), ch_set, CENTERED);
	}


	if(_init)
	{
		grid.setPosition(pos, width, height);
		grid.setTexCoord(width, height, w_power, h_power);
		grid.setTexture(grid_tex, w_power, h_power);
		grid.init();
	}
	else
	{
		grid.updateTexture(grid_tex, w_power, h_power);
		grid.init();
	}

	delete grid_tex;
}

void TrackPlot::addPoint(Acquisition* acq, int _pixel_step)
{
	int y_coord;
	int x_coord;
	int x_rel, y_rel;
	int x_in, y_in;
	int delta_x;
	int delta_y;
	float value;

	// On suppose que la valeur est constamment mise à jour
	if(acq->isUpdated(plot_object))
	{
		value = acq->readValue(plot_object);
	}
	else
		return;

	if(value < y_start)
	{
		value = y_start;
	}
	else if(value > y_start + y_range)
	{
		value = y_start + y_range - y_range/h_pixel;
	}

	if(last_x_coord == -2)
	{
		last_x_coord = 0;
		last_y_coord = ((value - y_start) * h_pixel)/y_range;
		return;
	}

	y_coord = ((value - y_start) * h_pixel)/y_range;
	//x_coord = w_pixel - split_pos + _pixel_step;
	x_coord = last_x_coord + _pixel_step;
	//printf("x_coord: %d ; y_coord: %d\n", x_coord, y_coord);
	delta_x = x_coord - last_x_coord;
	delta_y = y_coord - last_y_coord;
	x_in = last_x_coord;
	y_in = last_y_coord;
	x_rel = 0;
	y_rel = 0;

	if((delta_y >= 0 && delta_x >= delta_y) || (delta_y <= 0 && delta_x >= -delta_y))
	{
		while(x_in < x_coord)
		{
			x_in++;
			x_rel++;
			y_in = last_y_coord + (x_rel * delta_y) / delta_x;
			createDot(x_in, y_in, 1, tex_curve[active_buffer], w_pixel, w_power);
		}
	}
	else
	{
		if(delta_y > 0)
		{
			while(y_in < y_coord)
			{
				y_in++;
				y_rel++;
				x_in = last_x_coord + (y_rel * delta_x) / delta_y;
				createDot(x_in, y_in, 1, tex_curve[active_buffer], w_pixel, w_power);
				//printf("x: %d\ty: %d\n", x_in, y_in);
			}
		}
		else
		{
			while(y_in > y_coord)
			{
				y_in--;
				y_rel--;
				x_in = last_x_coord + (y_rel * delta_x) / delta_y;
				createDot(x_in, y_in, 1, tex_curve[active_buffer], w_pixel, w_power);
			}
		}
	}

	last_x_coord = x_coord;
	last_y_coord = y_coord;

	/*
	if(split_pos <= pixel_step)//pixel_step)
	{
		swapCurveBuffers();
	}
	else
	{
		split_pos -= pixel_step;
	}
	*/
	split_pos -= _pixel_step;
}

void TrackPlot::draw(GLuint _program)
{
	grid.draw(_program);
	strings.draw(_program);
	curve[active_buffer].draw(_program);
}
