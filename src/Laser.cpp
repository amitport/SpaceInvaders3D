
#include "Laser.h"
#include "math.h"
#include <vector>
#include "AlienHive.h"

using std::vector;

#define PI 3.1415926538f


Laser::Laser(float xt,float zt,float ht,bool alient)
{
	x = xt;	z = zt;	h = ht; alien = alient;
}

Laser::~Laser(void)
{
}

void Laser::draw(void)
{
	glPushMatrix();


	glTranslatef(x,h,z);

	float xt,yt,zt;
	float radius = 1.6f;
	float angle;

	if (alien)
		glColor3f(1.f, 1.0f, 0.0f);
	else
		glColor3f(0.3f, 0.6f, 0.8f);

	glBegin(GL_TRIANGLE_STRIP);	
	for(int i = 0; i <= 10; i++) { 
        angle = i*2*PI/10; 

		xt = cos(angle) * radius;	
		yt = 0; 
		zt = (sin(angle) * radius);
		glNormal3f(xt,0,zt);
		
        glVertex3f(xt,yt,zt); //end

		yt = 5.0f;
		glVertex3f(xt,yt,zt); //start
    } 
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(0,1,0);
	for(int i = 0; i < 10; i++) { 
        angle = i*2*PI/1; 
		xt = cos(angle) * radius;	
		yt = 2.f; 
		zt = (sin(angle) * radius);
        glVertex3f(xt,yt,zt); 
    } 
	glEnd();

	glPopMatrix();


	//check if the playe's laser hit someone
	if (!alien){
		vector<Alien>::iterator iter = AlienHive::aliens.begin();
		while (iter != AlienHive::aliens.end()){

			if (iter->height <= h+2 && abs(iter->Xpos-x)<=10  && abs(iter->Zpos-z)<=10){
				AlienHive::aliens.erase(iter);
				AlienHive::maxAliensOnScreen++;
				break;
			}
			iter++;
		}
	}
}