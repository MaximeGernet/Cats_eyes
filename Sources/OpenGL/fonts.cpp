/*
 * font.cpp
 *
 *  Created on: Apr 10, 2016
 *      Author: root
 */

//using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <algorithm>

#include "basics.h"
#include "textures.h"
#include "fonts.h"

CharacterSet::CharacterSet(const GLchar* _file_path, GLuint _size)
{
	load(_file_path, _size);
}

CharacterSet::CharacterSet()
{

}

void CharacterSet::load(const GLchar* _file_path, GLuint _size)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	FT_Face face;
	if (FT_New_Face(ft, _file_path, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	FT_Set_Pixel_Sizes(face, 0, _size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character =
		{
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

Character CharacterSet::glyph(char c)
{
	return Characters[c];
}

String2D::String2D()
{
	justification = LEFT_JUSTIFIED;
	color = glm::vec3(0.0f, 0.0f, 0.0f);
	init();
}

String2D::String2D(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet _ch_set, char _justification)
{
	setString(_s);
	setOrig(_orig);
	setColor(_color);
	setCharacterSet(_ch_set);
	setJustification(_justification);
	init();
}

void String2D::setOrig(glm::vec2 _orig)
{
	orig.x = _orig.x * screenWidth;
	orig.y = _orig.y * screenHeight;
}

void String2D::setColor(glm::vec3 _color)
{
	color = _color;
}

void String2D::setCharacterSet(CharacterSet _ch_set)
{
	ch_set = _ch_set;
	using_p = 0;
}

void String2D::setCharacterSet(CharacterSet* _ch_set)
{
	ch_set_p = _ch_set;
	using_p = 1;
}

void String2D::setJustification(char _justification)
{
	justification = _justification;
}

void String2D::setString(std::string _s)
{
	s = _s;
}

void String2D::init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void String2D::draw(GLuint _program)
{
	std::string::const_iterator c;
	GLfloat length = 0.0f;
	GLfloat height = 0.0f;
	GLfloat x;
	GLfloat y;
	Character ch;
	for(c = s.begin(); c != s.end(); c++)
	{
		if(using_p)
			ch = ch_set_p->glyph(*c);
		else
			ch = ch_set.glyph(*c);
		length += (ch.Advance >> 6);
		height = (height > ch.Size.y) ? height : ch.Size.y;
	}

	if(justification == LEFT_JUSTIFIED)
	{
		x = orig.x;
		y = orig.y;
	}
	else if(justification == RIGHT_JUSTIFIED)
	{
		x = orig.x - length;
		y = orig.y;
	}
	else if(justification == CENTERED)
	{
		x = orig.x - length / 2.0f;
		y = orig.y - height / 2.0f;
	}
	else if(justification == LEFT_CENTERED)
	{
		x = orig.x;
		y = orig.y - height / 2.0f;
	}

	// Activate corresponding render state
	glDisable(GL_DEPTH_TEST);
	//glUseProgram(program);
	glUniform1i(glGetUniformLocation(_program, "type"), TEXTURE_FROM_FONT);
	glUniform3f(glGetUniformLocation(_program, "textColor"), color.x, color.y, color.z);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	// Iterate through all characters
	for (c = s.begin(); c != s.end(); c++)
	{
		if(using_p)
			ch = ch_set_p->glyph(*c);
		else
			ch = ch_set.glyph(*c);
		GLfloat xpos = x + ch.Bearing.x;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y);
		GLfloat w = ch.Size.x;
		GLfloat h = ch.Size.y;
		// Update VBO for each character
		GLfloat vertices[6][4] =
		{
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos,	ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
}

void String2D::draw(GLuint _program, glm::vec3 _color)
{
	setColor(_color);
	draw(_program);
}


void String2D::draw(GLuint _program, std::string _s)
{
	setString(_s);
	draw(_program);
}

void String2D::draw(GLuint _program, std::string _s, glm::vec3 _color)
{
	setColor(_color);
	setString(_s);
	draw(_program);
}

void String2D::tmpDraw(GLuint _program, glm::vec3 _color)
{
	glm::vec3 memColor = color;
	draw(_program, _color);
	setColor(memColor);
}

void String2D::tmpDraw(GLuint _program, glm::vec2 _pos, glm::vec3 _color)
{
	glm::vec2 memPos = orig;
	glm::vec3 memColor = color;
	setOrig(_pos);
	draw(_program, _color);
	orig = memPos;
	color = memColor;
}

String2D::~String2D()
{
	glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

StringSet::StringSet()
{
	string_cnt = 0;
}

void StringSet::addString(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet _ch_set, char _justification)
{
	strings[string_cnt].setString(_s);
	strings[string_cnt].setOrig(_orig);
	strings[string_cnt].setColor(_color);
	strings[string_cnt].setCharacterSet(_ch_set);
	strings[string_cnt].setJustification(_justification);
	string_cnt++;
}

void StringSet::addString(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet* _ch_set, char _justification)
{
	strings[string_cnt].setString(_s);
	strings[string_cnt].setOrig(_orig);
	strings[string_cnt].setColor(_color);
	strings[string_cnt].setCharacterSet(_ch_set);
	strings[string_cnt].setJustification(_justification);
	string_cnt++;
}

void StringSet::replaceString(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet _ch_set, char _justification, char _string_id)
{
	strings[_string_id].setString(_s);
	strings[_string_id].setOrig(_orig);
	strings[_string_id].setColor(_color);
	strings[_string_id].setCharacterSet(_ch_set);
	strings[_string_id].setJustification(_justification);
	string_cnt = (string_cnt > (_string_id + 1)) ? string_cnt : (_string_id + 1);
}

void StringSet::loadMainMenuStrings()
{
	CharacterSet title_character_set("Fonts/caviarDreams.ttf", (128 * screenWidth)/1366);
	CharacterSet author_character_set("Fonts/Larke Neue Thin.ttf", (32 * screenWidth)/1366);
	string_cnt = 0;
	addString("Cat's Eyes", glm::vec2(0.5f, 0.9f), glm::vec3(0.0f, 0.8f, 0.7f), title_character_set, CENTERED);
	addString("Karim Kharboush", glm::vec2(0.01f, 0.01f), glm::vec3(0.3f, 0.3f, 0.3f), author_character_set, LEFT_JUSTIFIED);
	addString("Maxime Gernet", glm::vec2(0.01f, 0.06f), glm::vec3(0.3f, 0.3f, 0.3f), author_character_set, LEFT_JUSTIFIED);

	/*
	if(_bluetooth_status == CONNECTED)
		makeTextTextureBlended(1, 0.95, {0, 200, 0}, "CONNECTED", author_font, RIGHT_JUSTIFIED);
	else
		makeTextTextureBlended(1, 0.95, {200, 0, 0}, "DISCONNECTED", author_font, RIGHT_JUSTIFIED);
	*/
}


void StringSet::loadLiveVisionStrings()
{
	CharacterSet title_character_set("Fonts/abeakrg.ttf", (80.0f * screenWidth)/1366);
	string_cnt = 0;
	addString("Live Vision", glm::vec2(0.5f, 0.9f), glm::vec3(0.0f, 0.8f, 0.7f), title_character_set, CENTERED);
}

void StringSet::loadGlobalVisionStrings()
{
	CharacterSet title_character_set("Fonts/abeakrg.ttf", (80.0f * screenWidth)/1366);
	string_cnt = 0;
	addString("Global Vision", glm::vec2(0.5f, 0.9f), glm::vec3(0.0f, 0.8f, 0.7f), title_character_set, CENTERED);
}

void StringSet::loadTrackVisionStrings()
{
	CharacterSet title_character_set("Fonts/abeakrg.ttf", (80.0f * screenWidth)/1366);
	string_cnt = 0;
	addString("Track Vision", glm::vec2(0.5f, 0.9f), glm::vec3(0.0f, 0.8f, 0.7f), title_character_set, CENTERED);
}

void StringSet::loadSettingsStrings()
{
	CharacterSet title_character_set("Fonts/abeakrg.ttf", (80.0f * screenWidth)/1366);
	string_cnt = 0;
	addString("Settings", glm::vec2(0.5f, 0.9f), glm::vec3(0.0f, 0.8f, 0.7f), title_character_set, CENTERED);
}

void StringSet::draw(GLuint _program)
{
	for(int i = 0; i < string_cnt; i++)
	{
		strings[i].draw(_program);
	}
}

void StringSet::draw(GLuint _program, glm::vec3 _color, char _string_id)
{
	strings[_string_id].tmpDraw(_program, _color);
}

void StringSet::draw(GLuint _program, glm::vec2 _pos, glm::vec3 _color, char _string_id)
{
	strings[_string_id].tmpDraw(_program, _pos, _color);
}
