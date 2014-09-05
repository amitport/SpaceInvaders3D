#pragma once

#include "Alien.h"
#include "Terrain.h"

#include <vector>

class AlienHive
{


	Terrain* pTerrain;

public:
	static unsigned int maxAliensOnScreen;
	unsigned int aliensLeft;
	static std::vector<Alien> aliens;

	AlienHive(void);
	~AlienHive(void);

	void load(std::string,bool);
	void init(Terrain*);
	bool draw(unsigned long , float , float , bool );
};
