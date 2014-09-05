#include "Alien.h"
#include <gl/glew.h> 
#include <GL/glut.h>
#include <stdio.h>
#include <Windows.h>
#include <time.h>

using namespace std;

#define PI 3.1415926538f


OFF_File Alien::g_objModel;
bool Alien::useVBO = true;
unsigned int Alien::g_nAlienVerticesVBO;
unsigned int Alien::g_nAlienNormalsVBO;
unsigned int Alien::g_nAlienFacesVBO;

float Alien::currentSpeedFactor=0.5f;

Alien::Alien():laser(0,0,0,true)
{
	height = 300;
	laserReady = true;

	speedFactor = currentSpeedFactor;
	//make next aliens faster
	currentSpeedFactor*=1.1;
}

Alien::~Alien(void)
{
}

void Alien::init(float Xpost, float Zpost, float endHt){
	Xpos = Xpost;
	Zpos = Zpost;
	endH = endHt;
	
	timeToFire = (double)rand() / (RAND_MAX + 1) * 300;
}

void Alien::load(std::string model,bool useVBOt){
	g_objModel.load_OFF(model);
	useVBO = useVBOt;
	if (useVBO){
		//handle DREK VBO
		glGenBuffersARB(1,&g_nAlienVerticesVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nAlienVerticesVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_objModel.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_objModel.get_Vertex_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nAlienNormalsVBO);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nAlienNormalsVBO);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,g_objModel.get_Number_Of_Vertices()*3*sizeof(GLfloat),g_objModel.get_Normal_Array(),GL_STATIC_DRAW_ARB);

		glGenBuffersARB(1,&g_nAlienFacesVBO);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nAlienFacesVBO);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_objModel.get_Number_Of_Faces()*3*sizeof(GLuint),g_objModel.get_Faces_Array(),GL_STATIC_DRAW_ARB);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
}

bool Alien::draw(unsigned long deltaTime,bool playerBelow,bool displayTarget)
{
	glPushMatrix();
	
	//go to current position
	glTranslatef(Xpos,height,Zpos);
	//repair model
	glScalef(0.08f,0.08f,0.08f);

	//go to starting position (+12.5 cause get_center don't work well)
	const Vertex* pCenter = g_objModel.get_center();
	glTranslatef(-(pCenter->m_arrPosition[0]),-(pCenter->m_arrPosition[1]),-(pCenter->m_arrPosition[2])+12.5);

	//draw the Alien
	glColor3f(0.47f, 0.47f, 0.7f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	unsigned int nNumberOfFaces = g_objModel.get_Number_Of_Faces();
	if(useVBO){
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nAlienVerticesVBO);
		glVertexPointer(3,GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,g_nAlienNormalsVBO);
		glNormalPointer(GL_FLOAT,0,(char*)NULL);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,g_nAlienFacesVBO);
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

	if (playerBelow){
		//draw a marker for the player to aim at
		glColor3f(0.97f, 0.17f, 0.1f);
		glLineWidth(1);	
		for (int i =0; i<6;i++){
			glBegin(GL_LINE_LOOP);
			glVertex3f(-200,-70+40*i,-200);
			glVertex3f(-200,-70+40*i,200);
			glVertex3f(200,-70+40*i,200);
			glVertex3f(200,-70+40*i,-200);
			glEnd();
		}
		
	}

	glPopMatrix();

	//start draw the black marker on the ground and shadow	
	
	glLineWidth(2);

	float xt,yt,zt,angle;
	yt = endH+5.4;//15;

	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glTranslatef(Xpos,0,Zpos);
	glRotatef(45,0,1,0);

	//draw the aliens shadow
	float radius = (301-height)*0.07;
	
	if (radius>1){
		float transFactor = ((301-height)/300)*0.5f;

		//draw shadow
		glEnable(GL_BLEND);     // Turn blending On 
		glColor4f(0.0f, 0.0f, 0.0f,transFactor);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_POLYGON);
		for(int i = 0; i < 20; i++) { 
			angle = i*2*PI/20; 
			xt = 0 + (cos(angle) * radius); zt = 0 + (sin(angle) * radius);
			glVertex3f(xt,yt,zt);
		} 
		glEnd();
		
		glDisable(GL_BLEND);
	}

	//targeting mode
	if (displayTarget){
	    radius = height*0.34;
		glDisable(GL_LIGHTING);
		glColor3f(0.97f, 0.17f, 0.1f);

		glBegin(GL_LINE_STRIP);
		for(int i = 0; i <= 50; i++) { 
			angle = i*2*PI/50; 
			xt = 0 + (cos(angle) * radius); zt = 0 + (sin(angle) * radius);
			glVertex3f(xt,yt,zt);
		} 
		glEnd();
		glBegin(GL_LINES);
		angle = 0;
		
		xt = 0 + (cos(angle) * radius*0.3); zt = 0 + (sin(angle) * radius*0.3);
		glVertex3f(xt,yt,zt);
		xt = 0 + (cos(angle) * radius); zt = 0 + (sin(angle) * radius);
		glVertex3f(xt,yt,zt);

		angle = 10*2*PI/40;
		
		xt = 0 + (cos(angle) * radius*0.3); zt = 0 + (sin(angle) * radius*0.3);
		glVertex3f(xt,yt,zt);
		xt = 0 + (cos(angle) * radius); zt = 0 + (sin(angle) * radius);
		glVertex3f(xt,yt,zt);

		angle = 20*2*PI/40;
		
		xt = 0 + (cos(angle) * radius*0.3); zt = 0 + (sin(angle) * radius*0.3);
		glVertex3f(xt,yt,zt);
		xt = 0 + (cos(angle) * radius); zt = 0 + (sin(angle) * radius);
		glVertex3f(xt,yt,zt);

		angle = 30*2*PI/40;
		
		xt = 0 + (cos(angle) * radius*0.3); zt = 0 + (sin(angle) * radius*0.3);
		glVertex3f(xt,yt,zt);
		xt = 0 + (cos(angle) * radius); zt = 0 + (sin(angle) * radius);
		glVertex3f(xt,yt,zt);

		glEnd();
		
		glEnable(GL_LIGHTING);

	}	

	glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	//end of shadow and marker

	//draw the Alien's laser
	if(laserReady){
		if( timeToFire == 0){//fire at random intervals
			laser.x = Xpos;
			laser.z = Zpos;
			laser.h = height+5;
			laserReady = false;
			PlaySound(TEXT("Sound/laser.wav"), NULL, SND_FILENAME|SND_ASYNC);
			laser.draw();
		}else{
			timeToFire--;
		}
	}else{
		laser.h -= 10*((double)deltaTime)/1000;
		laser.draw();
		if (laser.h <= endH+10){
			if (playerBelow)
				return true;
			if (laser.h <= endH-4){
				laserReady = true;
				timeToFire = (double)rand() / (RAND_MAX + 1) * 300;
			}
		}
	}

	//advance down
	height -= speedFactor*((double)deltaTime)/1000;
	if (height <= endH+24){
		return true;
	}
	return false;
}