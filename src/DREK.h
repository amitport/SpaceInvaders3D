#pragma once

#include "OFF_File.h"
#include "Laser.h"
#include <vector>
#include <iostream>

class DREK
{
private:
	OFF_File g_objModel;
	std::vector<Laser> lasers;
	bool useVBO;

	unsigned int g_nDREKVerticesVBO;
	unsigned int g_nDREKNormalsVBO;
	unsigned int g_nDREKFacesVBO;

public:
	DREK(void);
	~DREK(void);
	void load(std::string,bool);
	void draw(float,float,float,float,float,float,unsigned long);
	void addLaser(float,float,float,unsigned long);
};
