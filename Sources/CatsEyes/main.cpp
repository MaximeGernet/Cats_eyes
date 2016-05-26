/********************************* Cat's Eyes S8 Project **********************************
*
*	Auteurs : Karim Kharboush, Maxime Gernet
* Encadrant : Jérémie Crennes
*
* Description :
*			Interface graphique permettant principalement l'affichage de diverses données
*			enregistrées par les capteurs embarqués sur un vélo
*
******************************************************************************************/

/************************************ Dependencies ***************************************
 *
 * GLFW3
 * GLEW
 * GLM
 * Freetype
 * SOIL
 * ASSIMP
 * ImageMagick
 *
******************************************************************************************/

using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <SOIL/SOIL.h>
#include <curl/curl.h>

// Custom libraries
#include "basics.h"
#include "shader.h"
#include "events.h"
#include "vertex.h"
#include "textures.h"
#include "fonts.h"
#include "camera.h"
#include "lighting.h"
#include "model.h"

#include "CEbtns.h"
#include "CEfsm.h"
#include "CEdata.h"
#include "CEacqDisplay.h"
#include "CEplot.h"
#include "CEmap.h"

float angleZ ;
float acc_angleX ;
float acc_angleY ;

float acc_x=0;
float acc_y=13880;
float acc_z=0;

float gyro_x=0;
float gyro_y=0;
float gyro_z=0;

float filter_x;
float filter_y;
float filter_z;

float alpha=0.8;

GLfloat screenWidth = 1366.0f;
GLfloat screenHeight = 768.0f;

int main()
{
	// glfw initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_REFRESH_RATE, 60);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	screenWidth = mode->width;
	screenHeight = mode->height - 64;

	GLFWwindow* window = NULL;
	window = glfwCreateWindow(screenWidth, screenHeight, "Cat's Eyes", nullptr, nullptr);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	input_s input;
	memset(&input, 0, sizeof(input));
	input.mousex = screenWidth / 2;
	input.mousey = screenHeight / 2;
	input.init = 1;

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowUserPointer(window, &input);

	// glew initialization
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}

	// OpenGL initialization
	glClearColor(27.0f/255.0f, 25.0f/255.0f, 25.0f/255.0f, 1.0f);
	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_MULTISAMPLE);

	glLineWidth(3.0);

	// curl initialization
	curl_global_init(CURL_GLOBAL_ALL);

	// Shaders initialization
	Shader shader_font("Shaders/vertex_shader_font.glsl", "Shaders/fragment_shader_font.glsl");
	Shader shader_model("Shaders/vertex_shader_model.glsl", "Shaders/fragment_shader_model.glsl");

	// Drawing objects initialization
	Model model3D("cycle03/cycle_ce.obj");
	glm::mat4 model;

	//Transformation matrices initialization
	glm::mat4 font_projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);

	// FreeType initialization
	CharacterSet character_set("Fonts/digital-7 (mono).ttf", 48);
	CharacterSet cell_ch_set("Fonts/Larke Neue Thin.ttf", (25 * screenWidth)/1366);

	// 2D Strings initialization
	StringSet strings;
	String2D text;
	text.setOrig(glm::vec2(0.005f, 0.95f));
	text.setColor(glm::vec3(0.0f, 1.0f, 0.9f));
	text.setCharacterSet(character_set);
	text.setJustification(LEFT_JUSTIFIED);

	// Buttons initialization
	ButtonSet btns;
	RadioBtnSet cells(&cell_ch_set);

	// Live vision objects initialization
	AcqDisplay acq_display;
	Plot plotX;
	Plot plotY;
	Plot plotZ;

	// Global vision objects initialization
	TrackPlot track_plot;
	track_plot.init(0.25f, 0.25f, 0.5f, 0.5f, TEMPERATURE);

	// Camera
	Camera_FPV camera(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-0.1f, 1.5f, 2.0f), -90.0f, -25.0f, (screenWidth/2.0f + screenWidth/10.0f) / (screenHeight/2.0f + screenHeight/10.0f));
	glm::mat4 projection;
	glm::mat4 view;

	// Lighting
	Scene_lights scene_lights;
	glm::vec3 light_position = glm::vec3(2.0f, 2.0f, 2.0f);
	scene_lights.newDirectionalLight(-light_position, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	// Data related objects
	Acquisition last_acq;
	Sniffer sniffer;
	sleep_millis(200);
	sniffer.connectBluetooth(); // Only call once. If the sniffer fails connecting, it retries every 5 seconds.

	// Some stuff
	ACC_s acc;
	GYR_s gyr;
	char current_state = FSM_LOAD_MAIN_MENU;
	bool updateDisplay;
	bool connection_status = DISCONNECTED;
	CharacterSet ch_set_status("Fonts/Larke Neue Thin.ttf", (25 * screenWidth)/1366);
	String2D status("DISCONNECTED", glm::vec2(1.0f, 0.950f), glm::vec3(1.0f, 0.0f, 0.0f), ch_set_status, RIGHT_JUSTIFIED);
	bool fullscreen = 0;

	// Map initialization
	TrackFiles trackFiles;
	Track track(&trackFiles);
	Map map_track(glm::vec2(0.25f), 0.5f, 0.5f);
	map_track.setMapCoord(44.9f, -1.3f, 10);
	map_track.loadTiles();
	map_track.loadTrace(&track);
	int zoom;
	Slider slider;
	slider.init(glm::vec2(0.5f, 0.20f), 0.2f);
	slider.setString("Transparency", glm::vec3(0.0f, 0.9f, 0.7f), &cell_ch_set);

	float t_ref_display = glfwGetTime();

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		/************************** FSM **************************/

		switch(current_state)
		{
			case FSM_LOAD_MAIN_MENU:
				/*
				if(ctrl_settings.resetVideoMode)
				{
					setVideoMode(&ctrl_settings);
				}
				glViewport(0, 0, WIDTH, HEIGHT);
				*/
				btns.loadMainMenuBtns();
				strings.loadMainMenuStrings();
				current_state = next_state(current_state, &btns);
				updateDisplay = false;
				break;

			case FSM_MAIN_MENU:
				connection_status = sniffer.updateConnectionStatus();
				glClear(GL_COLOR_BUFFER_BIT);
				shader_font.Use();
				glUniformMatrix4fv(glGetUniformLocation(shader_font.id(), "projection"), 1, GL_FALSE, glm::value_ptr(font_projection));
				strings.draw(shader_font.id());
				btns.updateBtnsStates(&input);
				btns.draw(shader_font.id());
				if(connection_status == CONNECTED)
					status.draw(shader_font.id(), "CONNECTED", glm::vec3(0.0f, 1.0f, 0.0f));
				else
					status.draw(shader_font.id(), "DISCONNECTED", glm::vec3(1.0f, 0.0f, 0.0f));
				//line.draw(shader_font.id());

				current_state = next_state(current_state, &btns);
				updateDisplay = true;
				break;

			case FSM_LOAD_LIVE_VISION:
				/*
				sniffer.configureSensors(GYR_MASK | ACC_MASK | MAG_MASK | BAR_MASK);
				sniffer.enterBurstMode();
				glViewport(0, 0, WIDTH, HEIGHT);
				plotX.init(-0.935, 0.2, 0.6, 0.4, 0);
				plotY.init(-0.27, 0.2, 0.6, 0.4, 0);
				plotZ.init(0.4, 0.2, 0.6, 0.4, 0);
				acq_display.init(-0.935, -0.85, 0.9, 1);
				*/
				sniffer.configureSensors(GPS_MASK | GYR_MASK | ACC_MASK | MAG_MASK | BAR_MASK);
				sniffer.enterBurstMode();
				strings.loadLiveVisionStrings();
				btns.loadLiveVisionBtns();
				plotX.init(0.035f, 0.6f, 0.3f, 0.2f, 0);
				plotY.init(0.36, 0.6, 0.3, 0.2, 0);
				plotZ.init(0.7, 0.6, 0.3, 0.2, 0);
				acq_display.init(glm::vec2(0.035f, 0.075f), 0.45f, 0.5f);
				current_state = next_state(current_state, &btns);
				updateDisplay = false;
				break;

			case FSM_LIVE_VISION:
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				btns.updateBtnsStates(&input);
				last_acq = sniffer.getLastAcq();
				plotX.updatePlot(&input);
				plotY.updatePlot(&input);
				plotZ.updatePlot(&input);
				//sleep_millis(50);
				plotX.addPoint(&last_acq);
				plotY.addPoint(&last_acq);
				plotZ.addPoint(&last_acq);


				shader_font.Use();
				glUniformMatrix4fv(glGetUniformLocation(shader_font.id(), "projection"), 1, GL_FALSE, glm::value_ptr(font_projection));
				strings.draw(shader_font.id());
				btns.draw(shader_font.id());

				acq_display.update(&last_acq);
				acq_display.draw(shader_font.id());
				plotX.draw(shader_font.id());
				plotY.draw(shader_font.id());
				plotZ.draw(shader_font.id());

				acc = last_acq.readAcc();
				gyr = last_acq.readGyr();
				acc_angleY = atan2(acc.ax,sqrt(pow(acc.ay,2)+pow(acc.az,2))) * 180/PI ;
				acc_angleX = atan2(acc.ay,sqrt(pow(acc.ax,2)+pow(acc.az,2))) * 180/PI ;

				filter_x = filter_x - gyr.gx / 40.0;
				filter_y = filter_y + gyr.gy / 40.0;
				filter_z = filter_z - gyr.gz / 40.0;
				filter_x=filter_x*alpha+(1-alpha)*acc_angleX;
				filter_y=filter_y*alpha+(1-alpha)*acc_angleY;

				glViewport(screenWidth/2 - screenWidth/10, 0, screenWidth/2.0f + screenWidth/10.0f, screenHeight/2.0f + screenHeight/10.0f);
				shader_model.Use();
				//camera.update(&input, deltaTime);
				model = glm::mat4();
				//model = glm::rotate(model, (GLfloat)(2.0 * 3.14 * 0.1 * glfwGetTime()), glm::vec3(1.0f, 0.0f, 0.0f));
				//model = glm::rotate(model, glm::radians(filter_y), glm::vec3(0.0f, 1.0f, 0.0f));
				//model = glm::rotate(model, glm::radians(filter_z), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(filter_z), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(filter_y), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, glm::radians(filter_x), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
				//model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
				//model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
				glUniformMatrix4fv(glGetUniformLocation(shader_model.id(), "model"), 1, GL_FALSE, glm::value_ptr(model));
				view = camera.view();
				glUniformMatrix4fv(glGetUniformLocation(shader_model.id(), "view"), 1, GL_FALSE, glm::value_ptr(view));
				projection = camera.projection();
				glUniformMatrix4fv(glGetUniformLocation(shader_model.id(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniform3f(glGetUniformLocation(shader_model.id(), "viewPos"), camera.position.x, camera.position.y, camera.position.z);
				scene_lights.sendShaderData(shader_model.id());

				model3D.Draw(shader_model.id());

				glViewport(0, 0, screenWidth, screenHeight);
				shader_font.Use();
				glUniformMatrix4fv(glGetUniformLocation(shader_font.id(), "projection"), 1, GL_FALSE, glm::value_ptr(font_projection));

				current_state = next_state(current_state, &btns);
				updateDisplay = true;
				break;

			case FSM_LOAD_TRACK_VISION:
				strings.loadTrackVisionStrings();
				btns.loadLiveVisionBtns();
				cells.loadTrackVisionCells();
				current_state = next_state(current_state, &btns);
				updateDisplay = false;
				break;

			case FSM_TRACK_VISION:
				glClear(GL_COLOR_BUFFER_BIT);
				shader_font.Use();
				glUniformMatrix4fv(glGetUniformLocation(shader_font.id(), "projection"), 1, GL_FALSE, glm::value_ptr(font_projection));
				cells.update(&input);
				cells.draw(shader_font.id());
				strings.draw(shader_font.id());
				btns.updateBtnsStates(&input);
				btns.draw(shader_font.id());

				if(cells.isActive(PLOT_CELL))
				{
					track_plot.update(&input);
					track_plot.draw(shader_font.id());
				}

				if(cells.isActive(MAP_CELL))
				{
					zoom = map_track.update(slider.value(), &input, &track);
					if(zoom < 12)
					{
						slider.update(&input);
						slider.draw(shader_font.id());
					}
					map_track.draw(shader_font.id());
				}


				current_state = next_state(current_state, &btns);
				updateDisplay = true;
				break;

			case FSM_LOAD_GLOBAL_VISION:
				strings.loadGlobalVisionStrings();
				btns.loadLiveVisionBtns();
				current_state = next_state(current_state, &btns);
				updateDisplay = false;
				break;

			case FSM_GLOBAL_VISION:
				glClear(GL_COLOR_BUFFER_BIT);
				shader_font.Use();
				glUniformMatrix4fv(glGetUniformLocation(shader_font.id(), "projection"), 1, GL_FALSE, glm::value_ptr(font_projection));
				strings.draw(shader_font.id());
				btns.updateBtnsStates(&input);
				btns.draw(shader_font.id());

				current_state = next_state(current_state, &btns);
				updateDisplay = true;
				break;

			case FSM_LOAD_SETTINGS:
				btns.loadLiveVisionBtns();
				strings.loadSettingsStrings();
				cells.loadsettingsCells();
				current_state = next_state(current_state, &btns);
				updateDisplay = false;
				break;

			case FSM_SETTINGS:
				glClear(GL_COLOR_BUFFER_BIT );
				shader_font.Use();
				glUniformMatrix4fv(glGetUniformLocation(shader_font.id(), "projection"), 1, GL_FALSE, glm::value_ptr(font_projection));
				strings.draw(shader_font.id());
				btns.updateBtnsStates(&input);
				btns.draw(shader_font.id());
				cells.update(&input);
				cells.draw(shader_font.id());
				current_state = next_state(current_state, &btns);

				if(fullscreen == 0 && cells.isActive(1))
				{
					GLFWwindow* new_window = NULL;
					const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
					screenWidth = mode->width;
					screenHeight = mode->height;
					new_window = glfwCreateWindow(screenWidth, screenHeight, "Cat's Eyes", glfwGetPrimaryMonitor(), window);
					glfwDestroyWindow(window);
					window = new_window;
					glfwMakeContextCurrent(window);
					fullscreen = 1;
					glfwSetKeyCallback(window, key_callback);
					glfwSetCursorPosCallback(window, mouse_callback);
					glfwSetMouseButtonCallback(window, mouse_button_callback);
					glfwSetWindowUserPointer(window, &input);
					glfwMakeContextCurrent(window);
					current_state = FSM_LOAD_SETTINGS;
				}
				else if(fullscreen == 1 && cells.isActive(0))
				{
					GLFWwindow* new_window = NULL;
					screenWidth = 1366.0f;
					screenHeight = 768.0f;
					new_window = glfwCreateWindow(screenWidth, screenHeight, "Cat's Eyes", nullptr, window);
					glfwDestroyWindow(window);
					window = new_window;
					glfwMakeContextCurrent(window);
					fullscreen = 0;
					glfwSetKeyCallback(window, key_callback);
					glfwSetCursorPosCallback(window, mouse_callback);
					glfwSetMouseButtonCallback(window, mouse_button_callback);
					glfwSetWindowUserPointer(window, &input);
					current_state = FSM_LOAD_SETTINGS;
				}

				if (window == NULL)
				{
					std::cout << "Failed to create GLFW window" << std::endl;
					glfwTerminate();
					exit(-1);
				}

				updateDisplay = true;
				break;

			case FSM_DEBUG:
				updateDisplay = false;
				break;

			default:
				break;
		}

		// Update display
		if(updateDisplay)
		{
			while(glfwGetTime() - t_ref_display < 0.016f)
				sleep_millis(1);
			t_ref_display = glfwGetTime();
			glfwSwapBuffers(window);
		}
	}

	curl_global_cleanup();
	glfwTerminate();
	return 0;
}
