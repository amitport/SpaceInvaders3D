#pragma once

#include "OFF_File.h"
#include "Laser.h"
#include <iostream>

class Alien
{
	static bool useVBO;
	static unsigned int g_nAlienVerticesVBO;
	static unsigned int g_nAlienNormalsVBO;
	static unsigned int g_nAlienFacesVBO;

	bool laserReady;
	float endH;
	Laser laser;
	static OFF_File g_objModel;
	float speedFactor;
	int timeToFire;
public:
	static float currentSpeedFactor;

	float height;
	float Xpos;
	float Zpos;

	Alien(void);
	~Alien(void);
	static void load(std::string,bool);
	bool draw(unsigned long,bool,bool);
	void init(float,float,float);
};
