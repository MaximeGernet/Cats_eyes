/*
 * map.h
 *
 *  Created on: May 12, 2016
 *      Author: root
 */

#ifndef CEMAP_H_
#define CEMAP_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "vertex.h"
#include "events.h"

#include "CEdata.h"

class Map
{
private:
	Quad2D osm_tiles[64];
	Quad2D sat_tiles[64];
	int tile_cnt;

	glm::vec2 pos;
	float w, h;

	int z;
	float lat_org, lon_org;
	float x_orgf, y_orgf;

	Line trace;

	float transparency;
	bool hovered;

public:
	Map(glm::vec2 _pos, float _w, float _h);

	void setMapCoord(float _lat_org, float _lon_org, int _z);
	void loadTiles();
	void loadServerTile(std::string _url, std::string _file);
	void loadTrace(Track* _track);
	int update(float _transparency, input_s* _input, Track* _track);
	void draw(GLuint _program);
};





#endif /* CEMAP_H_ */
