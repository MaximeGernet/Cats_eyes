/*
 * fonts.h
 *
 *  Created on: Apr 10, 2016
 *      Author: root
 */

#ifndef FONTS_H_
#define FONTS_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Justification types
#define LEFT_JUSTIFIED		0
#define RIGHT_JUSTIFIED		1
#define CENTERED			2
#define LEFT_CENTERED		3

struct Character
{
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

class CharacterSet
{
private:
	std::map<GLchar, Character> Characters;
public:
	CharacterSet(const GLchar* _file_path, GLuint _size);
	CharacterSet();
	void load(const GLchar* _file_path, GLuint _size);
	Character glyph(char c);
};

class String2D
{
public:
	std::string s;
	GLuint vao;
	GLuint vbo;
	glm::vec2 orig;
	glm::vec3 color;
	char justification;
	CharacterSet ch_set;
	CharacterSet* ch_set_p;

	bool using_p;

	void init();
public:
	String2D(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet _ch_set, char _justification);
	String2D();
	~String2D();

	void setOrig(glm::vec2 _orig);
	void setColor(glm::vec3 _color);
	void setCharacterSet(CharacterSet _ch_set);
	void setCharacterSet(CharacterSet* _ch_set);
	void setJustification(char _justification);
	void setString(std::string _s);

	void draw(GLuint _program);
	void draw(GLuint _program, glm::vec3 _color);
	void draw(GLuint _program, std::string _s);
	void draw(GLuint _program, std::string _s, glm::vec3 _color);
	void tmpDraw(GLuint _program, glm::vec3 _color);
	void tmpDraw(GLuint _program, glm::vec2 _pos, glm::vec3 _color);
};

class StringSet
{
public:
	//std::vector<String2D> strings;
	String2D strings[64];

public:
	char string_cnt;
	StringSet();

	void addString(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet _ch_set, char _justification);
	void addString(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet* _ch_set, char _justification);
	void replaceString(std::string _s, glm::vec2 _orig, glm::vec3 _color, CharacterSet _ch_set, char _justification, char _string_id);
	void loadMainMenuStrings();
	void loadLiveVisionStrings();
	void loadGlobalVisionStrings();
	void loadTrackVisionStrings();
	void loadSettingsStrings();

	void draw(GLuint _program);
	void draw(GLuint _program, glm::vec3 _color, char _string_id);
	void draw(GLuint _program, glm::vec2 _pos, glm::vec3 _color, char _string_id);
};


#endif /* FONTS_H_ */

