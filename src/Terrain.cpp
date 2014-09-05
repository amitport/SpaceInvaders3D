#include "Terrain.h"
#include "BMPLoader.h"

using namespace std;

#define PI 3.1415926538f

const float HOUSE1_X = 145,
			HOUSE1_Z = 95,
			HOUSE2_X = 40,
			HOUSE2_Z = 200,

			LIGHT_POST1_X = 15,
			LIGHT_POST1_Z = 20,
			LIGHT_POST2_X =200,
			LIGHT_POST2_Z = 26,
			LIGHT_POST3_X = 200,
			LIGHT_POST3_Z = 200;

//helper functions
void normalize(float Normal[3])
{	
	float m = sqrt(Normal[0] * Normal[0] + Normal[1] * Normal[1] + Normal[2] * Normal[2]);

	Normal[0] = Normal[0] / m;
	Normal[1] = Normal[1] / m;
	Normal[2] = Normal[2] / m;
}
void CalcNormal(float P1[3],float P2[3],float P3[3],float Normal[3])
{
	float V1[] = {0,0,0};
	float V2[] = {0,0,0};

	V1[0] = P2[0] - P1[0];
	V1[1] = P2[1] - P1[1];
	V1[2] = P2[2] - P1[2];

	V2[0] = P3[0] - P1[0];
	V2[1] = P3[1] - P1[1];
	V2[2] = P3[2] - P1[2];

	Normal[0] = V1[1]*V2[2] - V1[2]*V2[1];
	Normal[1] = V1[2]*V2[0] - V1[0]*V2[2];
	Normal[2] = V1[0]*V2[1] - V1[1]*V2[0];

	//normalize results
	normalize(Normal);
}

//static objects draw functions
void Terrain::drawLightPost(float X,float Z,float Yrotation,int lightType){
	glPushMatrix();
	
	glColor3f(0.68f, 0.99f, 0.7f);
	//go to starting position (+12.5 cause get_center don't work well)
	const Vertex* pCenter = g_lightPost.get_center();
	glTranslatef(-(pCenter->m_arrPosition[0]),-(pCenter->m_arrPosition[1]),-(pCenter->m_arrPosition[2])+12.5);

	//go to current position
	glTranslatef(X,getHeight(X,Z)+7.8,Z);

	//rotate with terrain angle
	glRotatef(getZAngle(X,Z)*360.f/(2.f*PI),0.0f,0.0f,1.0f);
	glRotatef(getXAngle(X,Z)*360.f/(2.f*PI),1.0f,0.0f,0.0f);
	glRotatef(Yrotation,0,1,0);

	GLfloat lightColor0[] = {0.8f, 0.0f, 0.0f, 1.f};
	GLfloat lightPos0[] = {-10.f, 0.f, 0.f, 1.0f};
	glLightf(lightType, GL_QUADRATIC_ATTENUATION, 1.0f);
	glLightfv(lightType, GL_DIFFUSE, lightColor0);
	glLightfv(lightType, GL_POSITION, lightPos0);

	glRotatef(180,0,0,1);
	glScalef(1.3,1.3,1.3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	unsigned int nNumberOfFaces = g_lightPost.get_Number_Of_Faces();

	if(g_useVBO){
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nLampVerticesVBO);
		glVertexPointer(3,GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nLampNormalsVBO);
		glNormalPointer(GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nLampFacesVBO);
		glDrawElements(GL_TRIANGLES,nNumberOfFaces*3,GL_UNSIGNED_INT,(char*)NULL);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	} else {

		float* pVertexArray = g_lightPost.get_Vertex_Array();
		float* pNormalArray = g_lightPost.get_Normal_Array();
		unsigned int* pFacesArray	= g_lightPost.get_Faces_Array();

		glVertexPointer(3,GL_FLOAT,0,pVertexArray);
		glNormalPointer(GL_FLOAT,0,pNormalArray);
		glDrawElements(GL_TRIANGLES,nNumberOfFaces*3,GL_UNSIGNED_INT,pFacesArray);
	}


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
}

void Terrain::drawHouse(float X,float Z,float Yrotation){
	glPushMatrix();
	
	glColor3f(0.7f, 0.7f, 0.6f);
	//go to starting position (+12.5 cause get_center don't work well)
	const Vertex* pCenter = g_house.get_center();
	glTranslatef(-(pCenter->m_arrPosition[0]),-(pCenter->m_arrPosition[1]),-(pCenter->m_arrPosition[2])+12.5);

	//go to current position
	glTranslatef(X,getHeight(X,Z),Z);

	//rotate with terrain angle
	glRotatef(getZAngle(X,Z)*360.f/(2.f*PI),0.0f,0.0f,1.0f);
	glRotatef(getXAngle(X,Z)*360.f/(2.f*PI),1.0f,0.0f,0.0f);
	glRotatef(Yrotation,0,1,0);

	//repair model
	glRotatef(180,0,0,1);
	glScalef(2.3,2.3,2.3);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	unsigned int nNumberOfFaces = g_house.get_Number_Of_Faces();

	if(g_useVBO){
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nHouseVerticesVBO);
		glVertexPointer(3,GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nHouseNormalsVBO);
		glNormalPointer(GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nHouseFacesVBO);
		glDrawElements(GL_TRIANGLES,nNumberOfFaces*3,GL_UNSIGNED_INT,(char*)NULL);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	} else {
		float* pVertexArray = g_house.get_Vertex_Array();
		float* pNormalArray = g_house.get_Normal_Array();
		unsigned int* pFacesArray	= g_house.get_Faces_Array();

		glVertexPointer(3,GL_FLOAT,0,pVertexArray);
		glNormalPointer(GL_FLOAT,0,pNormalArray);
		glDrawElements(GL_TRIANGLES,nNumberOfFaces*3,GL_UNSIGNED_INT,pFacesArray);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();
}

//Class functions
Terrain::Terrain(void)
{
	memset(normals,0,sizeof(float)*MAX_N*MAX_N*3);
	memset(vertices,0,sizeof(float)*MAX_N*MAX_N*3);
	memset(texCoords,0,sizeof(float)*MAX_N*MAX_N*3);
}

Terrain::~Terrain(void)
{
}

//creates vertex,Normal Tex Coord arrays from height map and texture
void Terrain::load(string hightMap, string texture,string houseOFF,string lightPostOFF,float heightTresh,bool g_useVBOt) 
{
	//init static objects
	g_house.load_OFF(houseOFF);
	g_lightPost.load_OFF(lightPostOFF);
	g_useVBO = g_useVBOt;

	BMPClass bmp;

	//open the heightMap
	if (BMPNOERROR != BMPLoad(hightMap,bmp) || MAX_N < bmp.height || MAX_N < bmp.width)
	{
		cout << "an error occured while trying to open" << hightMap << endl;
		exit(-1);
	}

	height = min(bmp.height,MAX_N);
	width = min(bmp.width,MAX_N);

	//init a temp height array (keeps only the y of each vertex) (on heap cause stack isn't big enough)
	float ** heights = new float*[MAX_N];
	for(int i = 0; i < MAX_N; i++)
		heights[i] = new float[MAX_N];
	
	for(int n = 0; n < height; n++) {
		for(int m = 0; m < width; m++) {
			BYTE color = bmp.pixel(m,n,0);
			float h = heightTresh * ((color / 255.0f) - 0.5f);
			heights[n][m] = h;
		}
	}

	//get the normal for each vertex (by summing all the faces' normals around it)
	for(int z = 0; z < height; z++) {
		for(int x = 0; x < width; x++) {
			float normalsSum[3] = {0,0,0}; //going to be current vertex's normal
			 //some temp vars
			float normal[3] = {0,0,0}; 
			float p1[3]; float p2[3]; float p3[3];

			//this is how our terrain is mapped
			// *----> 
			// |    X
			// |
			// \/ Z

			//when there is a valid face calculate it's normalized normal and add it to normalsSum

			if (x > 0 && z > 0) {
			//	 \  ^
			//	  \ |
			//     \|
			// <----*

				p1[0]=x;		p1[1]=heights[z][x];			p1[2]=z;
				p2[0]=x;		p2[1]=heights[(z-1)][x];		p2[2]=(z-1); 
				p3[0]=(x-1);	p3[1]=heights[(z-1)][(x-1)];	p3[2]=(z-1);

				CalcNormal(p1,p2,p3,normal);
				normalsSum[0]+=normal[0];
				normalsSum[1]+=normal[1];
				normalsSum[2]+=normal[2];

				p1[0]=x;		p1[1]=heights[z][x];			p1[2]=z;
				p2[0]=(x-1);	p2[1]=heights[(z-1)][(x-1)];	p2[2]=(z-1);
				p3[0]=(x-1);	p3[1]=heights[z][(x-1)];		p3[2]=z; 

				CalcNormal(p1,p2,p3,normal);
				normalsSum[0]+=normal[0];
				normalsSum[1]+=normal[1];
				normalsSum[2]+=normal[2];
			}
			if (x > 0 && z < height - 1) {
			// <----*
			//	    |
			//      |
			//     \/

				p1[0]=x;		p1[1]=heights[z][x];			p1[2]=z;
				p2[0]=(x-1);	p2[1]=heights[z][(x-1)];		p2[2]=z;
				p3[0]=x;		p3[1]=heights[(z+1)][x];		p3[2]=(z+1); 

				CalcNormal(p1,p2,p3,normal);
				normalsSum[0]+=normal[0];
				normalsSum[1]+=normal[1];
				normalsSum[2]+=normal[2];
			}
			if (x < width - 1 && z < height - 1) {
			// *---->
			// | \
			// |  \
			// \/  \

				p1[0]=x;		p1[1]=heights[z][x];			p1[2]=z;
				p2[0]=x;		p2[1]=heights[(z+1)][x];		p2[2]=(z+1); 
				p3[0]=(x+1);	p3[1]=heights[(z+1)][(x+1)];	p3[2]=(z+1); 	

				CalcNormal(p1,p2,p3,normal);
				normalsSum[0]+=normal[0];
				normalsSum[1]+=normal[1]; 
				normalsSum[2]+=normal[2];

				p1[0]=x;		p1[1]=heights[z][x];			p1[2]=z;
				p2[0]=(x+1);	p2[1]=heights[(z+1)][(x+1)];	p2[2]=(z+1); 	
				p3[0]=(x+1);	p3[1]=heights[z][(x+1)];		p3[2]=z; 

				CalcNormal(p1,p2,p3,normal);
				normalsSum[0]+=normal[0];
				normalsSum[1]+=normal[1];
				normalsSum[2]+=normal[2];

			}
			if (x < width - 1 && z > 0) {
			// ^
			// | 
			// |  
			// *---->
				
				p1[0]=x;		p1[1]=heights[z][x];			p1[2]=z;
				p2[0]=(x+1);	p2[1]=heights[z][(x+1)];		p2[2]=z; 
				p3[0]=x;		p3[1]=heights[(z-1)][x];	p3[2]=(z-1); 	
				
				CalcNormal(p1,p2,p3,normal);
				normalsSum[0]+=normal[0];
				normalsSum[1]+=normal[1];
				normalsSum[2]+=normal[2];
			}
			
			normals[z][x][0]=normalsSum[0];
			normals[z][x][1]=normalsSum[1];
			normals[z][x][2]=normalsSum[2];
		}
	}

	for(int z = 0; z < height; z++) {
		for(int x = 0; x < width; x++) {
			if (x > 0) {
				normals[z][x][0]= normals[z][x-1][0] * SMOOTH_FALLOUT;
				normals[z][x][1]= normals[z][x-1][1] * SMOOTH_FALLOUT;
				normals[z][x][2]= normals[z][x-1][2] * SMOOTH_FALLOUT;
			}
			if (x < width - 1) {
				normals[z][x][0]= normals[z][x+1][0] * SMOOTH_FALLOUT;
				normals[z][x][1]= normals[z][x+1][1] * SMOOTH_FALLOUT;
				normals[z][x][2]= normals[z][x+1][2] * SMOOTH_FALLOUT;
			}
			if (z > 0) {
				normals[z][x][0]= normals[z-1][x][0] * SMOOTH_FALLOUT;
				normals[z][x][1]= normals[z-1][x][1] * SMOOTH_FALLOUT;
				normals[z][x][2]= normals[z-1][x][2] * SMOOTH_FALLOUT;
			}
			if (z < height - 1) {
				normals[z][x][0]= normals[z+1][x][0] * SMOOTH_FALLOUT;
				normals[z][x][1]= normals[z+1][x][1] * SMOOTH_FALLOUT;
				normals[z][x][2]= normals[z+1][x][2] * SMOOTH_FALLOUT;
			}
			if (sqrt(normals[z][x][0]*normals[z][x][0] +
				normals[z][x][1]*normals[z][x][1] +
				normals[z][x][2]*normals[z][x][2]) == 0) //normal has no length
			{
				normals[z][x][0]=0; normals[z][x][1]=1; normals[z][x][2]=0;
			}

			normalize(normals[z][x]);
		}
	}

	// Setup Textures.
	if (BMPNOERROR != BMPLoad(texture,bmp)){
		cout << "an error occured while trying to open Terrain/Texture.bmp" << endl;
		exit(-1);
	}

	/// Generates a free texture handler.
	glGenTextures(1,g_nTextures);
	
	glBindTexture(GL_TEXTURE_2D,g_nTextures[0]);
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D,0,3,bmp.width,bmp.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.bytes);

	//now we have enough information to fill the texture, vertices and elements arrays
	int index = 0;
	for(int z = 0; z < height - 1; z++) {
		for(int x = 0; x < width; x++) {
			texCoords[z+1][x][0] = ((float)x)*(1.0f/width);
			texCoords[z+1][x][1] = ((float)z+1)*(1.0f/height);
			vertices[z+1][x][0] = x;
			vertices[z+1][x][1] = heights[z+1][x];
			vertices[z+1][x][2] = z+1;

			elements[index] = (z+1)*MAX_N + x;
			index++;

			texCoords[z][x][0] = ((float)x)*(1.0f/width);
			texCoords[z][x][1] = ((float)z)*(1.0f/height);
			vertices[z][x][0] = x;
			vertices[z][x][1] = heights[z][x];
			vertices[z][x][2] = z;

			elements[index] = z*MAX_N + x;
			index++;
		}
	}

	//delete the temp heights arrays
	for(int i = 0; i < MAX_N; i++) 
		delete[] heights[i];
	delete[] heights;

	//init VBO
	if (g_useVBO){
		//handle terrain VBO
		glGenBuffersARB(1,&g_nTerrainVerticesVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nTerrainVerticesVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,
						MAX_N*MAX_N*3*sizeof(GLfloat),
						vertices,
						GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nTerrainNormalsVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nTerrainNormalsVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,
						MAX_N*MAX_N*3*sizeof(GLfloat),
						normals,
						GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nTerrainTexVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nTerrainTexVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,
						MAX_N*MAX_N*2*sizeof(GLfloat),
						texCoords,
						GL_STATIC_DRAW_ARB);


		glGenBuffersARB((height-1),g_nTerrainFacesVBO);
		for(int z = 0; z < height - 1; z++) {
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nTerrainFacesVBO[z]);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
						width*2*sizeof(GLuint),
						elements+z*width*2,
						GL_STATIC_DRAW_ARB);
		}

		//handle house VBO
		glGenBuffersARB(1,&g_nHouseVerticesVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nHouseVerticesVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_house.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_house.get_Vertex_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nHouseNormalsVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nHouseNormalsVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_house.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_house.get_Normal_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nHouseFacesVBO);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nHouseFacesVBO);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_house.get_Number_Of_Faces()*3*sizeof(GLuint),g_house.get_Faces_Array(),GL_STATIC_DRAW_ARB);

		//handle lightpost VBO
		glGenBuffersARB(1,&g_nLampVerticesVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nLampVerticesVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_lightPost.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_lightPost.get_Vertex_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nLampNormalsVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nLampNormalsVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_lightPost.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_lightPost.get_Normal_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nLampFacesVBO);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nLampFacesVBO);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_lightPost.get_Number_Of_Faces()*3*sizeof(GLuint),g_lightPost.get_Faces_Array(),GL_STATIC_DRAW_ARB);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
}

void Terrain::drawTerrain(){
	glEnable(GL_TEXTURE_2D);
 
	glColor3f(1.0f, 1.0f, 1.0f);
	for(int z = 0; z < height - 1; z++) {
		glBegin(GL_TRIANGLE_STRIP);
		glBindTexture(GL_TEXTURE_2D, g_nTextures[0]);
		for(int x = 0; x < width; x++) {
			glNormal3f(normals[z][x][0], normals[z][x][1], normals[z][x][2]);
			glTexCoord2f(texCoords[z][x][0],texCoords[z][x][1]);
			glVertex3f(vertices[z][x][0],vertices[z][x][1],vertices[z][x][2]);

			glNormal3f(normals[z+1][x][0], normals[z+1][x][1], normals[z+1][x][2]);
			glTexCoord2f(texCoords[z+1][x][0],texCoords[z+1][x][1]);
			glVertex3f(vertices[z+1][x][0],vertices[z+1][x][1],vertices[z+1][x][2]);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}

void Terrain::drawEnviroment(){

	//draw lightposts
	drawLightPost(LIGHT_POST1_X,LIGHT_POST1_Z,90,GL_LIGHT0);
	drawLightPost(LIGHT_POST2_X,LIGHT_POST2_Z,50,GL_LIGHT1);
	drawLightPost(LIGHT_POST3_X,LIGHT_POST3_Z,0,GL_LIGHT2);
	//draw houses
	drawHouse(HOUSE1_X,HOUSE1_Z,79);
	drawHouse(HOUSE2_X,HOUSE2_Z,11.4);

	glColor3f(1.0f, 1.0f, 1.0f);

	//draw the terrain


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, g_nTextures[0]);

	if (g_useVBO){
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nTerrainVerticesVBO);
		glVertexPointer(3,GL_FLOAT,0,(char*)NULL);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nTerrainNormalsVBO);
		glNormalPointer(GL_FLOAT,0,(char*)NULL);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nTerrainTexVBO);
		glTexCoordPointer(2,GL_FLOAT,0,(char*)NULL);

		for(int z = 0; z < height - 1; z++) {
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nTerrainFacesVBO[z]);
			glDrawElements(GL_TRIANGLE_STRIP,width*2,GL_UNSIGNED_INT,(char*) NULL);
		}

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
	else{

		glVertexPointer(3,GL_FLOAT,0,vertices);
		glNormalPointer(GL_FLOAT,0,normals);
		glTexCoordPointer(2,GL_FLOAT,0,texCoords);

		for(int z = 0; z < height - 1; z++) {
			glDrawElements(GL_TRIANGLE_STRIP,width*2,GL_UNSIGNED_INT,elements+z*width*2);
		}

	}

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);




	//draw some background
	glBegin(GL_QUADS);
	{
		glColor3f(0.5f, 0.0f, 0.0f);

		glVertex3f(-2000,-100,256);
		glVertex3f(2000,-100,256);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(2000,-100,-1000);
		glVertex3f(-2000,-100,-1000);
	}
	glEnd();
}

GLfloat Terrain::getHeight(float x, float z){
	int m = x;
	int n = z;
	if (m>255 || m<0 || n>255 || n<0)
		return 0;
	return vertices[n][m][1];
}

void Terrain::getNormal(float x,float z,float Normal[3]){
	int m = x;
	int n = z;
	if (m>255 || m<0 || n>255 || n<0)
		return;

	Normal[0] =  - normals[n][m][2];
	Normal[1] =  0 ;
	Normal[2] = normals[n][m][0];
}

float Terrain::getXAngle(float x,float z){
	int m = x;
	int n = z;
	if (m>255 || m<0 || n>255 || n<0)
		return 0;
	float t = sqrt(normals[n][m][2]*normals[n][m][2]+normals[n][m][1]*normals[n][m][1]);
	t=normals[n][m][1]/t;
	t= acos(t);
	return normals[n][m][2]>=0?t:-t;
}

float Terrain::getZAngle(float x,float z){
	int m = x;
	int n = z;
	if (m>255 || m<0 || n>255 || n<0)
		return 0;
	float t = sqrt(normals[n][m][1]*normals[n][m][1]+normals[n][m][0]*normals[n][m][0]);
	t=normals[n][m][1]/t;
	t= acos(t);
	return normals[n][m][0]>=0?-t:t;
}

//centers the terrain
void Terrain::Transformations(){
	float scale = 5.0f / max(width - 1,height - 1);

	glScalef(scale, scale, scale);
	glTranslatef(-(float)(width - 1) / 2,0.0f, -(float)(height - 1) / 2);
}

bool Terrain::isLegalPos(float x, float y){
	if ((x>=9 && x<=245 && y>=9 && y<=245) //on terrain
		&& 
		(sqrt(pow((HOUSE1_X-1-x),2)+pow((HOUSE1_Z+7-y),2))>=28) //not inside house 1
		&&
		(sqrt(pow((HOUSE2_X-x),2)+pow((HOUSE2_Z+7-y),2))>=27) //not inside house 2
	    &&
		(sqrt(pow((LIGHT_POST1_X+13.3-x),2)+pow((LIGHT_POST1_Z+1-y),2))>=13) //not inside light 1
		&&
		(sqrt(pow((LIGHT_POST2_X+8-x),2)+pow((LIGHT_POST2_Z+12-y),2))>=14) //not inside light 2
		&&
		(sqrt(pow((LIGHT_POST3_X-1-x),2)+pow((LIGHT_POST3_Z+15-y),2))>=15)) //not inside light 2

	{
		return true;
	}
	return false;
}