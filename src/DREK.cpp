#include "DREK.h"
#include <gl/glew.h> 
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <Windows.h>
using namespace std;

#define PI 3.1415926538f

DREK::DREK(void)
{
}

DREK::~DREK(void)
{
}

void drawCannon(float xangle,float yangle){
	float x = 0,y = 0,h = 0;
	float radius,angle;
	float xt,yt,zt;
	float xt2,yt2,zt2;
	const float 
		CANNON_P1_START=12.0f+h,
		CANNON_P1_END = 22.5f+h,
		CANNON_P2_END = 22.4f+h,
		CANNON_RADIUS = 2.0f,
		CANNON_INNER_RADIUS = 1.9f;

	glColor3f(0.7f, 0.7f, 1.0f);
	radius = CANNON_RADIUS;
	glBegin(GL_TRIANGLE_STRIP);
	for(int i = 0; i <= 30; i++) { 
        angle = i*2*PI/30; 

		//base point
		xt = x + (cos(angle) * radius);
		yt = CANNON_P1_END;
		zt = y + (sin(angle) * radius);

		//go to rotation base
		yt -= CANNON_P1_START;
		//Z-axis transformation
		xt2 = xt*cos(yangle)-yt*sin(yangle); yt2 = xt*sin(yangle)+yt*cos(yangle); zt2 = zt;
		//X-axis transformation
		xt = xt2; yt = yt2*cos(xangle)-zt2*sin(xangle); zt = yt2*sin(xangle)+zt2*cos(xangle);
		//go back from rotation base
		yt+=CANNON_P1_START;

		//draw the CANNON_P1_END point
		glNormal3f(xt,0,zt);
        glVertex3f(xt,yt,zt);

		//draw the CANNON_P1_START point
		glNormal3f(x + (cos(angle) * radius),0,y + (sin(angle) * radius));
		glVertex3f(x + (cos(angle) * radius),CANNON_P1_START,y + (sin(angle) * radius)); 
    } 
	glEnd();

	radius = CANNON_INNER_RADIUS;
	glColor3f(0.4f,0.1f,0.2f);
	glBegin(GL_POLYGON);
	for(int i = 0; i < 30; i++) { 
        angle = i*2*PI/30; 

        //base point
		xt = x + (cos(angle) * radius);
		yt = CANNON_P2_END;
		zt = y + (sin(angle) * radius);

		//go to rotation base
		yt -= CANNON_P1_START;
		//Z-axis transformation
		xt2 = xt*cos(yangle)-yt*sin(yangle); yt2 = xt*sin(yangle)+yt*cos(yangle); zt2 = zt;
		//X-axis transformation
		xt = xt2; yt = yt2*cos(xangle)-zt2*sin(xangle); zt = yt2*sin(xangle)+zt2*cos(xangle);
		//go back from rotation base
		yt+=CANNON_P1_START;

		//draw the CANNON_P2_END point
		glNormal3f(xt,0,zt);
        glVertex3f(xt,yt,zt); 
    } 
    glEnd(); 

	//draw the cannon base
	radius = 2.7;
	glColor3f(0.6f, 0.6f, 1.0f);
	glBegin(GL_POLYGON);
	for(int i = 0; i < 30; i++) { 
        angle = i*2*PI/30; 
		xt = x + (cos(angle) * radius);	yt = 13.0f+h; zt = y + (sin(angle) * radius);
		glNormal3f(xt,0,zt);
        glVertex3f(xt,yt,zt);
    } 
    glEnd();
	
	glColor3f(0.3f, 0.6f, 0.8f);
	glBegin(GL_TRIANGLE_STRIP);
	for(int i = 0; i <= 30; i++) { 
        angle = i*2*PI/30; 
		radius = 2.7;	
		xt = x + (cos(angle) * radius);	yt = 13.0f+h; zt = y + (sin(angle) * radius);
		glNormal3f(xt,0,zt);
        glVertex3f(xt,yt,zt);
		radius = 3.2;
		xt = x + (cos(angle) * radius);	yt = 10.0f+h; zt = y + (sin(angle) * radius);
		glNormal3f(xt,0,zt);
		glVertex3f(xt,yt,zt);
    } 
	glEnd();
}

void DREK::draw(float Xpos,float Zpos, float Xangle, float Yangle, float Zangle, float height,unsigned long deltaTime)
{
	glPushMatrix();
		
	//go to starting position (+12.5 cause get_center don't work well)
	const Vertex* pCenter = g_objModel.get_center();
	glTranslatef(-(pCenter->m_arrPosition[0]),-(pCenter->m_arrPosition[1]),-(pCenter->m_arrPosition[2])+12.5);

	//go to current position
	glTranslatef(Xpos,height+0.35,Zpos);
	
	//rotate with terrain angle
	glRotatef(Zangle*360.f/(2.f*PI),0.0f,0.0f,1.0f);
	glRotatef(Xangle*360.f/(2.f*PI),1.0f,0.0f,0.0f);
	
	//draw the drek's cannon
	drawCannon(-Xangle,-Zangle);

	//user's rotation
	glRotatef(Yangle,0.0f,1.0f,0.0f);

	//repair model
	glScalef(0.43f,0.43f,0.43f);
	glRotatef(-90,1.0f,0.0f,0.0f);

	//draw the Drek
	glColor3f(0.47f, 0.47f, 0.7f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	unsigned int nNumberOfFaces = g_objModel.get_Number_Of_Faces();

	if(useVBO){
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nDREKVerticesVBO);
		glVertexPointer(3,GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nDREKNormalsVBO);
		glNormalPointer(GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nDREKFacesVBO);
		glDrawElements(GL_TRIANGLES,nNumberOfFaces*3,GL_UNSIGNED_INT,(char*)NULL);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	} else {
		float* pVertexArray = g_objModel.get_Vertex_Array();
		float* pNormalArray = g_objModel.get_Normal_Array();
		unsigned int* pFacesArray	= g_objModel.get_Faces_Array();

		glVertexPointer(3,GL_FLOAT,0,pVertexArray);
		glNormalPointer(GL_FLOAT,0,pNormalArray);
		glDrawElements(GL_TRIANGLES,nNumberOfFaces*3,GL_UNSIGNED_INT,pFacesArray);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glPopMatrix();

	//draw the DREK's lasers
	bool del = false;
	std::vector<Laser>::iterator delIter;
	for(std::vector<Laser>::iterator iter = lasers.begin();iter != lasers.end(); iter++){
		iter->h += 30*((double)deltaTime)/1000;
		if (iter->h > 300){
			if (!del){
				del = true;
				delIter = iter;
			}
		}else{
			iter->draw();
		}
	}
	if (del){
		lasers.erase(delIter);
	}

}

void DREK::load(std::string model,bool useVBOt){
	g_objModel.load_OFF(model);
	useVBO = useVBOt;
	if (useVBO){
		//handle DREK VBO
		glGenBuffersARB(1,&g_nDREKVerticesVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nDREKVerticesVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_objModel.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_objModel.get_Vertex_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nDREKNormalsVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nDREKNormalsVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_objModel.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_objModel.get_Normal_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nDREKFacesVBO);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nDREKFacesVBO);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_objModel.get_Number_Of_Faces()*3*sizeof(GLuint),g_objModel.get_Faces_Array(),GL_STATIC_DRAW_ARB);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
}

void DREK::addLaser(float x,float z,float h,unsigned long currentRequestTime){

	const Vertex* pCenter = g_objModel.get_center();
	static unsigned long lastRequestTime = currentRequestTime;

	if ((currentRequestTime <= lastRequestTime) || (currentRequestTime - lastRequestTime)>=1000)
	{
		PlaySound(TEXT("Sound/canon.WAV"), NULL, SND_FILENAME|SND_ASYNC);
		lastRequestTime = currentRequestTime;
		lasers.push_back(Laser(
						-(pCenter->m_arrPosition[0])+x,
						-(pCenter->m_arrPosition[2])+12.5+z,
						h+20 ,
						false));
	}
}

