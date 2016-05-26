/*
 * basics.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: root
 */

#include <chrono>
#include <thread>
#include <math.h>
#include <iostream>

#include "basics.h"

void sleep_millis(int64_t millis)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

float dist(glm::vec2 a, glm::vec2 b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

