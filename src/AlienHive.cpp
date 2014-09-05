#include "AlienHive.h"
#include <time.h>

using namespace std;


vector<Alien> AlienHive::aliens;
unsigned int AlienHive::maxAliensOnScreen = 1;
#define PI 3.1415926538f

AlienHive::AlienHive(void)
{
}

AlienHive::~AlienHive(void)
{
}

void AlienHive::init(Terrain* pTerraint){
	aliens.clear();

	maxAliensOnScreen = 1;
	aliensLeft = 24;
	pTerrain = pTerraint;

	srand ( time(NULL) );
	Alien::currentSpeedFactor=0.4f;
}

void AlienHive::load(std::string model,bool useVBO){
	
	Alien::load(model,useVBO);
	unsigned int alienToCreate = min(maxAliensOnScreen,aliensLeft);

	for (unsigned int i = 0; i<alienToCreate;i++) //we use 6 aliens at a time
	{
		Alien alien;
		float x = rand() % 255;
		float z = rand() % 255;
		if (pTerrain->isLegalPos(x,z)){
			//add the alien
			alien.init(x,z,pTerrain->getHeight(x,z));
			aliens.push_back(alien);
			aliensLeft--;
		} else {
			i--;
		}
			
	}
}

bool AlienHive::draw(unsigned long deltaTime, float user_posX, float user_posY, bool kDown){
	bool lostGame = false;
	//check if we can add aliens 
	int aliensToRevive = maxAliensOnScreen - aliens.size();
	if (aliensToRevive != 0) {
		for (int i = 0; aliensLeft!=0 && i<aliensToRevive;i++)
		{
			Alien alien;
			float x = rand() % 255;
			float z = rand() % 255;
			if (pTerrain->isLegalPos(x,z)){
				//add the alien
				alien.init(x,z,pTerrain->getHeight(x,z));
				aliens.push_back(alien);
				aliensLeft--;
				//update level
			} else {
				i--;
			}
		}
	}

	for(vector<Alien>::iterator iter = aliens.begin();iter != aliens.end();iter++){
		lostGame |= iter->draw(deltaTime,(abs(user_posX-iter->Xpos)<=10)&&(abs(user_posY-iter->Zpos)<=10),kDown);
	}

	return lostGame;
}