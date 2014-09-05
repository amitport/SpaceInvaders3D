#pragma once

#include <iostream>
#include <gl/glew.h> 
#include <GL/glut.h>
#include <math.h>
#include "OFF_File.h"

const int MAX_N = 256;
const float SMOOTH_FALLOUT = 0.5f;

class Terrain
{
private:
	
	GLuint g_nHouseVerticesVBO;
	GLuint g_nHouseNormalsVBO;
	GLuint g_nHouseFacesVBO;
	OFF_File g_house;

	GLuint g_nLampVerticesVBO;
	GLuint g_nLampNormalsVBO;
	GLuint g_nLampFacesVBO;
	OFF_File g_lightPost;

	GLfloat normals[MAX_N][MAX_N][3];
	GLfloat vertices[MAX_N][MAX_N][3];
	GLfloat texCoords[MAX_N][MAX_N][2];
	GLuint elements[(MAX_N-1)*MAX_N*2];
	GLuint g_nTerrainVerticesVBO;
	GLuint g_nTerrainNormalsVBO;
	GLuint g_nTerrainTexVBO;
	GLuint g_nTerrainFacesVBO[(MAX_N-1)];
	GLuint g_nTextures[1];

	bool g_useVBO;

	void drawLightPost(float ,float ,float ,int );
	void drawHouse(float ,float ,float );

public:	
	int height,width;

	Terrain(void);
	~Terrain(void);
	void load(std::string,std::string,std::string,std::string,float,bool);
	void drawTerrain();
	void drawEnviroment();
    GLfloat getHeight(float,float);
	void getNormal(float,float,float[3]);
	float getXAngle(float,float);
	float getZAngle(float,float);
	void Transformations();
	bool isLegalPos(float, float);
};
