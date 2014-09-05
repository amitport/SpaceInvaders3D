
#include <iostream>
#include <math.h>
#include <limits>
#include "Terrain.h"
#include "Windows.h"
#include "DREK.h"
#include "AlienHive.h"
#include <fmod.h>
#include <fmod_errors.h>
#include <gl/glew.h> // NEW - Must be included in order to use extensions!
#include <gl/glut.h>

// automatically link to fmod library
#pragma comment(lib,"lib/fmod.lib")
#pragma comment(lib,"lib/glew32.lib")
#pragma comment(lib,"lib/glut32.lib")


#define PI 3.1415926538f

using std::cout;
using std::endl;
using std::max;
using std::numeric_limits;
using std::vector;

//globals
Terrain terrain;
DREK drek;
AlienHive alienHive;

// Mouse button globals
int g_nButton;
int g_nMousePrevX = -1;
int g_nMousePrevY = -1;

// Transformation variables (debug
float g_nScaleFactor = 1.0f;
float g_nRotateDX = 0.0f;
float g_nRotateDY = 0.0f;
float g_nTranslateDX = 0.0f;
float g_nTranslateDY = 0.0f;

//user position
float user_posX = 128.0f;
float user_posY = 206.0f;
float user_angle = 0.0f;

/// this is a pointer to the mp3 stream we will be reading from the disk.
FSOUND_STREAM* g_mp3_stream = NULL;

//game mode
bool g_useVBO = true;
bool g_debug = false;
bool g_fullScreen = false;
bool lostGame = false;
bool wonGame = false;
bool g_newRecord = false;
bool g_gotMusic = false;

//delta time (for animations)
unsigned long deltaTime;
unsigned long g_elapsedTime = 0;
unsigned long g_record = numeric_limits<DWORD>::max();

// key press helper
bool leftDown = false,rightDown = false,upDown = false, downDown = false, spaceDown = false, mDown = false, kDown = false;

// game logic functions
void handleKeysDown() {
	float tempYDelta,tempXDelta;
	float stride = mDown?10.f:40.4;//2.5f:10.0f;
	float angleStep = mDown?30.f:120.f;//7.5f:30.0f;

	stride *=((double)deltaTime)/1000; //set the progress relative to the frame rate
	angleStep *=((double)deltaTime)/1000;

	if (!upDown && downDown){
		angleStep = -angleStep;
	}

	if (spaceDown){
		drek.addLaser(user_posX,user_posY,terrain.getHeight(user_posX,user_posY),GetTickCount());
	}
	if (leftDown){
		user_angle += angleStep;
	}else
	if (rightDown){
		user_angle -= angleStep;
	}
	if (upDown){
		tempYDelta = cos(user_angle*2*PI/360.f)*stride;
		tempXDelta = sin(user_angle*2*PI/360.f)*stride;
		if (terrain.isLegalPos(user_posX + tempXDelta,user_posY + tempYDelta))
		{
			user_posX+= tempXDelta;
			user_posY+= tempYDelta;
		}else {
			stride= 10.f*((double)deltaTime)/1000;
			tempYDelta = cos(user_angle*2*PI/360.f)*stride;
			tempXDelta = sin(user_angle*2*PI/360.f)*stride;
			if (terrain.isLegalPos(user_posX + tempXDelta,user_posY + tempYDelta))
			{
				user_posX+= tempXDelta;
				user_posY+= tempYDelta;
			}
			else 
			if (terrain.isLegalPos(user_posX + tempXDelta,user_posY))
			{
				user_posX+= tempXDelta;
			}else 
			if (terrain.isLegalPos(user_posX ,user_posY + tempYDelta))
			{
				user_posY+= tempYDelta;
			}
		}
	}else
	if (downDown){
		tempYDelta = cos(user_angle*2*PI/360.f)*stride;
		tempXDelta = sin(user_angle*2*PI/360.f)*stride;
		if (terrain.isLegalPos(user_posX - tempXDelta,user_posY - tempYDelta)){
			user_posX-= tempXDelta;
			user_posY-= tempYDelta;
		}else {
			stride= 10.f*((double)deltaTime)/1000;
			tempYDelta = cos(user_angle*2*PI/360.f)*stride;
			tempXDelta = sin(user_angle*2*PI/360.f)*stride;
			if (terrain.isLegalPos(user_posX - tempXDelta,user_posY - tempYDelta)){
				user_posX-= tempXDelta;
				user_posY-= tempYDelta;
			}else
			if (terrain.isLegalPos(user_posX - tempXDelta,user_posY)){
				user_posX-= tempXDelta;
			} else 
			if (terrain.isLegalPos(user_posX ,user_posY - tempYDelta)){
				user_posY-= tempYDelta;
			}
		}
	}
}

void gameCycleInit(){
	//Handle deltaTime used for animations
	static DWORD lastUpdate = GetTickCount();
	DWORD currentTime = GetTickCount();

	if (currentTime < lastUpdate){
		//long int overflow case
		deltaTime = numeric_limits<DWORD>::max()-lastUpdate+currentTime;
	}else{
		deltaTime = currentTime - lastUpdate;
	}
	lastUpdate = currentTime;
	g_elapsedTime += deltaTime;

	//update variable with pressed keys
	handleKeysDown();
}

void gameCycleEnd(){
	const int NumOfExtraFrames = 1;

	static int extraFramesCount = 0;
	//handle game is done
	if ((lostGame) && (++extraFramesCount>NumOfExtraFrames)){
		if (g_fullScreen){
			if (g_gotMusic){
				FSOUND_Stream_Stop( g_mp3_stream );
				FSOUND_Stream_Close( g_mp3_stream );
				// kill off fmod
				FSOUND_Close();
			}
			glutLeaveGameMode();
			MessageBox(NULL,L"YOU HAVE LOST !!!", L"Space Invaders 3D",MB_OK);
			exit(0);
		}else{
			if (g_gotMusic){
				FSOUND_Stream_Stop( g_mp3_stream );
			}
			int msgID = MessageBox(NULL,L"YOU HAVE LOST !!!\n\n Try Again?", L"Space Invaders 3D", MB_YESNO|MB_DEFBUTTON2);
			if (msgID == IDYES){
				alienHive.init(&terrain);
				g_elapsedTime = 0;
				leftDown = false;
				rightDown = false;
				upDown = false;
				downDown = false;
				spaceDown = false;
				mDown = false;
				kDown = false;
				lostGame = false;
				if (g_gotMusic){
					FSOUND_Stream_SetPosition(g_mp3_stream,0);
					FSOUND_Stream_Play(0,g_mp3_stream );
				}
		}else{
			if (g_gotMusic){
				FSOUND_Stream_Stop( g_mp3_stream );
				FSOUND_Stream_Close( g_mp3_stream );
				// kill off fmod
				FSOUND_Close();
			}
			exit(0);
		}
	}
}

	if (wonGame){
		glutLeaveGameMode();
		if (g_gotMusic){
			FSOUND_Stream_Stop( g_mp3_stream );
			FSOUND_Stream_Close( g_mp3_stream );
			// kill off fmod
			FSOUND_Close();
		}
		if (g_newRecord)
			MessageBox(NULL,L"YOU HAVE WON !!!\nNEW RECORD !!!", L"Space Invaders 3D", MB_OK);
		else
			MessageBox(NULL,L"YOU HAVE WON !!!", L"Space Invaders 3D", MB_OK);
		exit(0);
	}
	if (!g_debug && alienHive.aliensLeft==0 && alienHive.aliens.size()==0){
		if ((g_elapsedTime/1000) < (g_record/1000)){
			FILE * pFile;
			pFile = fopen ("record.txt","w");
			if (pFile!=NULL)
			{
				fprintf(pFile, "%lu", g_elapsedTime);
				fclose (pFile);
			}
			g_newRecord = true;
		}
		wonGame = true;
	}
}

bool startUpGame(){
	FILE * pFile;
	pFile = fopen ("record.txt","r");
	if (pFile!=NULL)
	{
		fscanf(pFile, "%lu", &g_record);
		fclose (pFile);
	}

	int msgboxID;
	
	cout << "GOALS:\n\n\
You've got 24 alien ships to destroy,\n\
if one of them lands on the terrain, you lose,\n\
if one of their lasers hit you, you lose.\n\
\n\
CONTROLS:\n\n\
<w> - Forward\n\
<s> - Backward\n\
<a> - Turn Left\n\
<d> - Turn Right\n\n\
<space> - Fire\n\
<m> - Percision Mode\n\
<k> - Targeting Mode\n\n\
<Esc> - exit\n\
\n\
TIPS:\n\n\
* Use <m> when you're near an alien (more accurate motion)\n\
* Use <k> when an alien is low (draws a red circle on the terrain underneath the alien)\n\
* When you see red squares around an alien it means you're in each other line of fire\n\
* Not all ships are descending at the same speed, aim at the faster ones\n\
\n\
Good Luck!" << endl;

	//display information about the game
	MessageBox(NULL,
L"GOALS:\n\n\
You've got 24 alien ships to destroy,\n\
if one of them lands on the terrain, you lose,\n\
if one of their lasers hit you, you lose.\n\
\n\
CONTROLS:\n\n\
<w> - Forward\n\
<s> - Backward\n\
<a> - Turn Left\n\
<d> - Turn Right\n\n\
<space> - Fire\n\
<m> - Percision Mode\n\
<k> - Targeting Mode\n\n\
<Esc> - exit\n\
\n\
TIPS:\n\n\
* Use <m> when you're near an alien (more accurate motion)\n\
* Use <k> when an alien is low (draws a red circle on the terrain underneath the alien)\n\
* When you see red squares around an alien it means you're in each other line of fire\n\
* Not all ships are descending at the same speed, aim at the faster ones\n\
\n\
Good Luck!"
		, L"Space Invaders 3D", MB_OK);
	
	//debug mode?
	msgboxID = MessageBox(NULL,
				L"Start in Debug Mode?\n\n* No aliens\n* Can use mouse to rotate (left button) pan (right button) and zoom (wheel)"
				, L"Space Invaders 3D", MB_YESNO|MB_DEFBUTTON2);

	if (msgboxID == IDYES){
		g_debug = true;
	}

	//full screen?
	msgboxID = MessageBox(NULL,
				L"Start in Full Screen?"
				, L"Space Invaders 3D",MB_YESNO|MB_DEFBUTTON2);

	// initialise fmod, 44000 Hz, 64 channels
	if( FSOUND_Init(44000,64,0) == FALSE )
	{
		cout << "[ERROR] Could not initialise fmod\n";
	} else	{

		// attempt to open the mp3 file as a stream
		g_mp3_stream = FSOUND_Stream_Open( "Sound/BTW.mp3" , FSOUND_2D , 0 , 0 );

		// make sure mp3 opened OK
		if(!g_mp3_stream) {
			cout << "[ERROR] could not open file\n";
		} else {
			FSOUND_Stream_Play(0,g_mp3_stream);
			g_gotMusic = true;
		}
	}

	return (msgboxID == IDYES);
}


//opengl functions
void handleKeypress(unsigned char key, int x, int y) {
	switch (key)
	{
		case 27:
			glutLeaveGameMode();
			exit(0);
		case ' ':
			spaceDown = true;
			break;
		case 'a':
		case 'A':		
			leftDown = true;
			break;
		case 'd':
		case 'D':		
			rightDown = true;
			break;
		case 'w':
		case 'W':
			upDown = true;
			break;
		case 's':
		case 'S':
			downDown = true;
			break;
		case 'm':
		case 'M':
			mDown = true;
			break;
		case 'k':
		case 'K':
			kDown = true;
			break;
	}
}

void handlekeyboardUp(unsigned char key, int x, int y) {
switch (key) {
		case 'a': //left
		case 'A': //left
			leftDown = false;
			break;
		case 'w' : //up
		case 'W' : //up
			upDown = false;
			break;
		case 'd' : //right
		case 'D' : //right
			rightDown = false;
			break;
		case 's': //down
		case 'S': //down
			downDown = false;
			break;
		case ' ':
			spaceDown = false;
			break;
		case 'm':
		case 'M':
			mDown = false;
			break;
		case 'k':
		case 'K':
			kDown = false;
			break;
	}
}

void HandleMouseClick(int button, int state, int x, int y)
{
	g_nButton = button;

	switch(button)
	{
	case GLUT_LEFT_BUTTON:
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			g_nMousePrevX = x;
			g_nMousePrevY = y;
		}
		break;
	case GLUT_WHEEL_UP:
		g_nScaleFactor += 0.3f;
		break;
	case GLUT_WHEEL_DOWN:
		g_nScaleFactor -= 0.3f;
		break;
	}
}

void HandleMouseMove(int x,int y)
{
	int nDX = x - g_nMousePrevX;
	int nDY = y - g_nMousePrevY;
	g_nMousePrevX = x;
	g_nMousePrevY = y;
	
	switch (g_nButton)
	{
	case GLUT_LEFT_BUTTON:
		
		g_nRotateDX += ((float)nDX);
		g_nRotateDY += ((float)nDY);
		break;
	case GLUT_RIGHT_BUTTON:
		g_nTranslateDX += (((float)nDX) / 50.0f);
		g_nTranslateDY += (((float)nDY) / 50.0f);
		break;
	}
}

void renderBitmapString(
		float x, 
		float y, 
		float z, 
		void *font, 
		char *string) {  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

//open gl main functions
void InitOpenGL() {
	if ((GLEW_OK != glewInit()) || !GLEW_ARB_vertex_buffer_object)
	{
		cout << endl << "Error with GLEW: disable VBO" << endl << endl;
		g_useVBO = false;
	} else
	{
		cout << endl << "Using GLEW Version: " << glewGetString(GLEW_VERSION) << endl << endl;;
		g_useVBO = true;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	//light posts
	glEnable(GL_LIGHT0); 
	glEnable(GL_LIGHT1);	
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3); 
	//directional
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);

	terrain.load(	"Terrain/heightmap.bmp",
					"Terrain/Texture.bmp",
					"Models/Static Objects/house.off",
					"Models/Static Objects/lightpost.off",
					44,
					g_useVBO);

	drek.load("Models/Player/jeep.off",g_useVBO);

	if (!g_debug){
		alienHive.init(&terrain);
		alienHive.load("Models/Aliens/alien2.off",g_useVBO);
	}
}

void UserTransformations()
{
	glScalef(g_nScaleFactor,g_nScaleFactor,g_nScaleFactor);
	glTranslatef(g_nTranslateDX,0.0f,g_nTranslateDY);
	glRotatef(g_nRotateDX,0.0f,1.0f,0.0f);
	glRotatef(g_nRotateDY,1.0f,0.0f,0.0f);
}

void preDraw(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Global transformations
	glTranslatef(0.0f, -2.2f, -10.0f);
	glRotatef(15.0f, 1.0f, 0.0f, 0.0f);
	glScalef(0.79f, 1.0f, 1.0f);
	if (g_debug){
		UserTransformations();
	}
	terrain.Transformations();

	//lights
	GLfloat ambientColorLost[] = {0.8f, 0.0f, 0.0f, 1.0f};
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	if (lostGame){
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColorLost);
		glColor3f(1.f,1.f,1.f);
		renderBitmapString(105,200,258,GLUT_BITMAP_TIMES_ROMAN_24,"Game Over");
	}
	else		
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor0[] = {0.4f, 0.4f, 0.4f, 1.0f};
	GLfloat lightPos0[] = {1.f, 1.f, -1.f, 0.0f};
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT3, GL_POSITION, lightPos0);
}

void drawScene() {

	gameCycleInit();

	preDraw();

	//draw terrain (and static objects and background)
	terrain.drawEnviroment();

	//draw the player
	drek.draw(	//position
				user_posX,
				user_posY,
				//angle
				terrain.getXAngle(user_posX,user_posY),
				user_angle,
				terrain.getZAngle(user_posX,user_posY),
				//height
				terrain.getHeight(user_posX,user_posY),
				deltaTime);


	if (!g_debug && !lostGame && !wonGame){
		//draw the aliens
		lostGame = alienHive.draw(deltaTime,user_posX,user_posY,kDown);
	}

	//writing stuff to the screen
	if (wonGame){
		glColor3f(1.f,1.f,1.f);
		renderBitmapString(81,200,258,GLUT_BITMAP_TIMES_ROMAN_24,"You Saved the Planet!");
		if (g_newRecord){
			glColor3f(1.f,1.f,0.f);
			renderBitmapString(100,150,258,GLUT_BITMAP_TIMES_ROMAN_24,"New Record!");
		}
	}
	if (!g_debug){
		glColor3f(1.f,1.f,1.f);
		char str[80];

		sprintf(str,"Aliens Left: %d",alienHive.aliensLeft+alienHive.aliens.size());
		if (g_fullScreen)
			renderBitmapString(-35,272,258,GLUT_BITMAP_HELVETICA_18,str);
		else
			renderBitmapString(40,272,258,GLUT_BITMAP_HELVETICA_12,str);

		sprintf(str,"Elapsed Time: %02d:%02d",(g_elapsedTime/1000)/60,(g_elapsedTime/1000)%60);
		if (g_fullScreen)
			renderBitmapString(-36,262,258,GLUT_BITMAP_HELVETICA_18,str);
		else
			renderBitmapString(40,267,258,GLUT_BITMAP_HELVETICA_12,str);

		glColor3f(1.f,1.f,0.f);
		if (g_record == numeric_limits<DWORD>::max())
			sprintf(str,"Record Time: --:--");
		else
			sprintf(str,"Record Time: %02d:%02d",(g_record/1000)/60,(g_record/1000)%60);
		if (g_fullScreen)
			renderBitmapString(-37,252,258,GLUT_BITMAP_HELVETICA_18,str);
		else
			renderBitmapString(40,262,258,GLUT_BITMAP_HELVETICA_12,str);
	}

	glutSwapBuffers();

	gameCycleEnd();
}

int main(int argc, char** argv) {
	g_fullScreen = startUpGame();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	if (g_fullScreen){
		glutGameModeString( "1600x1200:24@60" );//using low color depth in full screen
		glutEnterGameMode();
	}
	else{	
		glutInitWindowSize(500, 800);
		glutCreateWindow("Space Invaders 3D - Amit Portnoy");
	}

	if (g_debug){//no need for those in normal mode
		glutMouseFunc(HandleMouseClick);	
		glutMotionFunc(HandleMouseMove);
	}

	glutDisplayFunc(drawScene);
	glutIdleFunc(drawScene);	
	glutKeyboardFunc(handleKeypress);
	glutKeyboardUpFunc(handlekeyboardUp);
	glutReshapeFunc(handleResize);	

	InitOpenGL();

	glutMainLoop();

	return 0;
}

