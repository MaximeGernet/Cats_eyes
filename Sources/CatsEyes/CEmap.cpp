/*
 * map.cpp
 *
 *  Created on: May 12, 2016
 *      Author: root
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <curl/curl.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <ImageMagick-6/Magick++.h>


#include <string>
#include <vector>





#include "basics.h"
#include "vertex.h"
#include "events.h"

#include "CEdata.h"
#include "CEmap.h"

using namespace Magick;

Map::Map(glm::vec2 _pos, float _w, float _h)
{
	pos = _pos;
	w = _w;
	h = _h;
	tile_cnt = 0;
	hovered = 0;
}

void Map::setMapCoord(float _lat_org, float _lon_org, int _z)
{
	lat_org = _lat_org;
	lon_org = _lon_org;
	z = _z;
}

void Map::loadTiles()
{
	int x_org, y_org;		// Top left tile
	float i_f, j_f;			// Dimensions of the matrix of tiles
	float x, y;				// tile position
	float tex_x, tex_y; 	// origin of the tile portion used
	float tex_w, tex_h; 	// width and height of the tile portion used
	char tile_file[64];
	char url[256];
	int i_max, j_max;
	float x_scale, y_scale;
	FILE* fp = NULL;

	tile_cnt = 0;

	x_orgf = (lon_org + 180.0f) / 360.0f * pow(2.0f, z);
	//y_orgf = (1 - log(tan(lat_org * PI / 180.0f) + 1.0f / cos(lat_org * PI / 180.0f)) / PI) * pow(2.0f, z - 1);
	y_orgf = (1 - log(tan(lat_org * PI / 180.0f) + 1.0f / cos(lat_org * PI / 180.0f)) / PI) * pow(2.0f, z - 1);
	x_org = floor(x_orgf);
	y_org = floor(y_orgf);

	j_f = (w + (x_orgf - x_org) * 256.0f / screenWidth) * screenWidth / 256.0f;
	i_f = (h + (y_orgf - y_org) * 256.0f / screenHeight) * screenHeight / 256.0f;

	i_max = ceil(i_f);
	j_max = ceil(j_f);

	printf("i_max: %f, j_max: %d\n", i_f, j_max);

	x_scale = 1.0f / w;
	y_scale = 1.0f / h;

	for(int i = 0; i < i_max; i++)
	{
		for(int j = 0; j < j_max; j++)
		{
			if(z < 12)
			{
				sat_tiles[tile_cnt].setTexCoord(glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
				sprintf(tile_file, "Maps/sat/%d_%d_%d.png", z, x_org + j, y_org + i);

				fp = fopen(tile_file, "r+");
				if (fp == NULL)
				{
					sprintf(url, "http://otile1.mqcdn.com/tiles/1.0.0/sat/%d/%d/%d.jpg", z, x_org + j, y_org + i);
					std::cout << "url : " << url << std::endl;
					//loadServerTile(url, tile_file);
					Image url_image(url);
					url_image.write(tile_file);
				}
				else
				{
					fclose(fp);
				}
				sat_tiles[tile_cnt].updateTexture(tile_file, x_scale, y_scale);
				x = (((float)x_org - x_orgf) * 256.0f + (float)j * 255.0f) / (w * screenWidth);
				y = (h * screenHeight - (256.0f - (y_orgf - (float)y_org) * 256.0f) - (float)i * 256.0f) / (h * screenHeight);
				sat_tiles[tile_cnt].setPosition(glm::vec2(x, y));
				sat_tiles[tile_cnt].init();
			}


			osm_tiles[tile_cnt].setTexCoord(glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
			sprintf(tile_file, "Maps/osm/%d_%d_%d.png", z, x_org + j, y_org + i);

			fp = fopen(tile_file, "r+");
			if (fp == NULL)
			{
				sprintf(url, "http://tile.openstreetmap.org/%d/%d/%d.png", z, x_org + j, y_org + i);
				std::cout << "url : " << url << std::endl;
				loadServerTile(url, tile_file);
			}
			else
			{
			    fclose(fp);
			}
			osm_tiles[tile_cnt].updateTexture(tile_file, x_scale, y_scale);
			x = (((float)x_org - x_orgf) * 256.0f + (float)j * 255.0f) / (w * screenWidth);
			y = (h * screenHeight - (256.0f - (y_orgf - (float)y_org) * 256.0f) - (float)i * 256.0f) / (h * screenHeight);
			osm_tiles[tile_cnt].setPosition(glm::vec2(x, y));
			osm_tiles[tile_cnt].init();

			tile_cnt++;

		}
	}
}

void Map::loadServerTile(std::string _url, std::string _file)
{
	CURL*  easyhandle = NULL;curl_easy_init();
	CURLcode imgresult;
	FILE *fp = NULL;

	easyhandle = curl_easy_init();

	fp = fopen(_file.c_str(), "wb");
	if( fp == NULL )
	{
		std::cout << "File cannot be opened" << std::endl;
		curl_easy_cleanup(easyhandle);
		return;
	}

	curl_easy_setopt(easyhandle, CURLOPT_URL, _url.c_str());

	curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, fp);

	imgresult = curl_easy_perform(easyhandle);

	if( imgresult )
	{
		std::cout << "Cannot grab the image\n";
	}

	curl_easy_cleanup(easyhandle);
	fclose(fp);
}

void Map::loadTrace(Track* _track)
{
	float x_scale, y_scale;

	x_scale = 1.0f / w;
	y_scale = 1.0f / h;

	trace.setOrg(lat_org, lon_org, z);
	trace.setPos(glm::vec2(0.0f));
	for(int i = 0; i < _track->getNumberOfPoints(); i++)
	{
		Acquisition* acq = _track->acquisition(i);
		if(acq->isUpdated(LATITUDE))
		{
			trace.addPoint(acq->readValue(LATITUDE), acq->readValue(LONGITUDE), x_scale, y_scale);
			//printf("lat: %0.2f, lon: %0.2f\n", acq->readValue(LATITUDE), acq->readValue(LONGITUDE));
		}
	}
	trace.init();
}

int Map::update(float _transparency, input_s* _input, Track* _track)
{
	float mx = x_orgf + ((float)_input->mousex - pos.x * screenWidth) / 256.0f;
	float my = y_orgf + ((float)_input->mousey - (1.0f - (pos.y + h)) * screenHeight) / 256.0f;
	float mlon, mlat;
	float new_mx, new_my;
	int new_z = z;
	transparency = _transparency;

	if(isOver(pos, w, h, _input))
	{
		if(hovered == 0)
		{
			hovered = 1;
			_input->scrolly = 0;
		}
		else
		{
			new_z += _input->scrolly;
			_input->scrolly = 0;
			if(new_z > 13)
				new_z = 13;
			if(new_z < 7)
				new_z = 7;
			if(new_z != z)
			{
				mlon = mx/pow(2.0f, z) * 360.0f - 180.0f;
				mlat = atan(sinh(PI - my/pow(2.0f, z) * 2*PI)) * 180.0f / PI;
				if(new_z > z)
				{
					z = new_z;
					new_mx = (mlon + 180.0f) / 360.0f * pow(2.0f, z);
					new_my = (1 - log(tan(mlat * PI / 180.0f) + 1.0f / cos(mlat * PI / 180.0f)) / PI) * pow(2.0f, z - 1);
					//x_orgf = new_mx - w * screenWidth / (2.0f * 256.0f);
					//y_orgf = new_my - h * screenHeight / (2.0f * 256.0f);
					x_orgf = new_mx - w * screenWidth / (2.0f * 256.0f) * ((mx - x_orgf)/(w/2.0f * screenWidth/256.0f));
					y_orgf = new_my - h * screenHeight / (2.0f * 256.0f) * ((my - y_orgf)/(h/2.0f * screenHeight/256.0f));
					lon_org = x_orgf/pow(2.0f, z) * 360.0f - 180.0f;
					lat_org = atan(sinh(PI - y_orgf/pow(2.0f, z) * 2*PI)) * 180.0f / PI;
					loadTiles();
				}
				else
				{
					z = new_z;
					new_mx = (mlon + 180.0f) / 360.0f * pow(2.0f, z);
					new_my = (1 - log(tan(mlat * PI / 180.0f) + 1.0f / cos(mlat * PI / 180.0f)) / PI) * pow(2.0f, z - 1);
					x_orgf = new_mx - w * screenWidth / (2.0f * 256.0f) * ((mx - x_orgf)/(w/2.0f * screenWidth/256.0f));
					y_orgf = new_my - h * screenHeight / (2.0f * 256.0f) * ((my - y_orgf)/(h/2.0f * screenHeight/256.0f));
					lon_org = x_orgf/pow(2.0f, z) * 360.0f - 180.0f;
					lat_org = atan(sinh(PI - y_orgf/pow(2.0f, z) * 2*PI)) * 180.0f / PI;
					loadTiles();
				}
				loadTrace(_track);
			}
		}
	}
	else
	{
		hovered = 0;
	}

	return z;
}

void Map::draw(GLuint _program)
{
	glViewport(pos.x * screenWidth, pos.y * screenHeight, w * screenWidth, h * screenHeight);
	for(int i = 0; i < tile_cnt; i++)
	{
		osm_tiles[i].draw(_program);
	}
	if(z < 12)
	{
		glUniform1i(glGetUniformLocation(_program, "override_transparency"), 1);
		glUniform1f(glGetUniformLocation(_program, "transparency"), transparency);
		for(int i = 0; i < tile_cnt; i++)
		{
			sat_tiles[i].draw(_program);
		}
		glUniform1i(glGetUniformLocation(_program, "override_transparency"), 0);
	}
	trace.draw(_program);
	glViewport(0, 0, screenWidth, screenHeight);
}
