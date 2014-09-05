#pragma once

class Laser
{
	bool alien;
public:	
	float x;
	float z;
	float h;
	Laser(float,float,float,bool);
	~Laser(void);
	void draw(void);
};
