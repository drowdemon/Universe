#include <cstdlib>
#include <fstream>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

#define PI 3.141592

int mainWindow=0;
int WIDTH=0;
int HEIGHT=0;
int mousex=0;
int mousey=0;

class RGB
{
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    RGB(unsigned char pr, unsigned char pg, unsigned char pb)
    {
        r=pr;
        g=pg;
        b=pb;
    }
};

struct ARGB
{
    unsigned char a;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class tile
{
public:
    unsigned char road;
    unsigned short water;
    unsigned char waste;
    unsigned char animal;
    unsigned char bush;
    unsigned char tree;
    short height;
    tile(unsigned char r=0, unsigned short wtr=0, unsigned char wst=0, unsigned char a=0, unsigned char b=0, unsigned char t=0, short h=0)
    {
        road=r;
        water=wtr;
        waste=wst;
        animal=a;
        bush=b;
        tree=t;
        height=h;
    }
};

vector<vector<tile> > map;
ifstream *inf;

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
            // Set the camera
    gluLookAt(1.0f, 1.0f, 1.0f,
              1.0f, 1.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glutSwapBuffers();
}
void makeRect(float x, float y, float width, float height, RGB color)
{
    glColor3f((float)color.r/255.0, (float)color.g/255.0, (float)color.b/255.0);
    glBegin(GL_QUADS);
        glVertex3f((float)x*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,0);
        glVertex3f((float)(x+width)*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,0);
        glVertex3f((float)(x+width)*2.0/WIDTH,2.0-(float)(y+height)*2.0/HEIGHT,0);
        glVertex3f((float)x*2.0/WIDTH,2.0-(float)(y+height)*2.0/HEIGHT,0);
    glEnd();
}
void makeEllipse(float x, float y, float radiusx, float radiusy, RGB color)
{
    glColor3f((float)color.r/255.0, (float)color.g/255.0, (float)color.b/255.0);
    glBegin(GL_POLYGON);
        for(double i = 0; i < 2 * PI; i += PI / 24) //The 6 is the smoothness, basically. Higher number makes it smoother.
            glVertex3f((float)(cos(i)*radiusx+x)*2.0/WIDTH, 2.0-(float)(sin(i)*radiusy+y)*2.0/HEIGHT, 0.0);
    glEnd();
}
void makeRect(float x, float y, float width, float height, ARGB color)
{
    glColor4f(color.r/255.0, color.g/255.0, color.b/255.0, color.a/255.0); //rgba
    glBegin(GL_QUADS);
        glVertex3f((float)x*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,0);
        glVertex3f((float)(x+width)*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,0);
        glVertex3f((float)(x+width)*2.0/WIDTH,2.0-(float)(y+height)*2.0/HEIGHT,0);
        glVertex3f((float)x*2.0/WIDTH,2.0-(float)(y+height)*2.0/HEIGHT,0);
    glEnd();
}
void makeEllipse(float x, float y, float radiusx, float radiusy, ARGB color)
{
    glColor4f((float)color.r/255.0, (float)color.g/255.0, (float)color.b/255.0, color.a/255.0);
    glBegin(GL_POLYGON);
        for(double i = 0; i < 2 * PI; i += PI / 24) //The 6 is the smoothness, basically. Higher number makes it smoother.
            glVertex3f((float)(cos(i)*radiusx+x)*2.0/WIDTH, 2.0-(float)(sin(i)*radiusy+y)*2.0/HEIGHT, 0.0);
    glEnd();
}
void makeRect(float x, float y, float width, float height, RGB color, int winwidth, int winheight)
{
    glColor3f((float)color.r/255.0, (float)color.g/255.0, (float)color.b/255.0);
    glBegin(GL_QUADS);
        glVertex3f((float)x*2.0/winwidth,2.0-(float)y*2.0/winheight,0);
        glVertex3f((float)(x+width)*2.0/winwidth,2.0-(float)y*2.0/winheight,0);
        glVertex3f((float)(x+width)*2.0/winwidth,2.0-(float)(y+height)*2.0/winheight,0);
        glVertex3f((float)x*2.0/winwidth,2.0-(float)(y+height)*2.0/winheight,0);
    glEnd();
}
void drawLine(float x, float y, float fx, float fy, RGB color)
{
    glColor3f((float)color.r/255.0, (float)color.g/255.0, (float)color.b/255.0);
    glBegin(GL_LINES);
        glVertex3f((float)x*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,0);
        glVertex3f((float)fx*2.0/WIDTH,2.0-(float)fy*2.0/HEIGHT,0);
    glEnd();
}
void drawEmptyRect(float x, float y, float width, float height, RGB color)
{
    glColor3f(color.r/255.0, color.g/255.0, color.b/255.0);
    glBegin(GL_LINE_LOOP);
        glVertex3f((float)x*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,0);
        glVertex3f((float)(x+width)*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,0);
        glVertex3f((float)(x+width)*2.0/WIDTH,2.0-(float)(y+height)*2.0/HEIGHT,0);
        glVertex3f((float)x*2.0/WIDTH,2.0-(float)(y+height)*2.0/HEIGHT,0);
    glEnd();
}
void drawEmptyRect(float x, float y, float width, float height, RGB color, int winwidth, int winheight)
{
    glColor3f(color.r/255.0, color.g/255.0, color.b/255.0);
    glBegin(GL_LINE_LOOP);
        glVertex3f((float)x*2.0/winwidth,2.0-(float)y*2.0/winheight,0);
        glVertex3f((float)(x+width)*2.0/winwidth,2.0-(float)y*2.0/winheight,0);
        glVertex3f((float)(x+width)*2.0/winwidth,2.0-(float)(y+height)*2.0/winheight,0);
        glVertex3f((float)x*2.0/winwidth,2.0-(float)(y+height)*2.0/winheight,0);
    glEnd();
}

void renderBitmapString(float x,float y,float z,void *font,char *string)
{
    char *c;
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0,0.0,0.0);
    glRasterPos3f((float)x*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,(float)z);
    for (c=string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
    glEnable(GL_TEXTURE_2D);
}
void renderBitmapString(float x,float y,float z,void *font,char *string,RGB color)
{
    char *c;
    glDisable(GL_TEXTURE_2D);
    glColor3f(color.r/255.0, color.g/255.0, color.b/255.0);
    glRasterPos3f((float)x*2.0/WIDTH,2.0-(float)y*2.0/HEIGHT,(float)z);
    for (c=string; *c != '\0'; c++)
    {
            glutBitmapCharacter(font, *c);
    }
    glEnable(GL_TEXTURE_2D);
}
void renderBitmapString(float x,float y,float z,void *font,char *string, int winwidth, int winheight)
{
    char *c;
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0,0.0,0.0);
    glRasterPos3f((float)x*2.0/winwidth,2.0-(float)y*2.0/winheight,(float)z);
    int numnew=0;
    for (c=string; *c != '\0'; c++)
    {
        if(*c=='\n')
        {
            numnew++;
            glRasterPos3f((float)x*2.0/winwidth,2.0-(float)(y+numnew*20)*2/winheight,(float)z);
        }
        glutBitmapCharacter(font, *c);
    }
    glEnable(GL_TEXTURE_2D);
}
void processPassiveMouseMove(int x, int y)
{
    mousex=x;
    cout << x << " " << y << endl;
    mousey=y;
}
void processMouseMove(int x, int y)
{
    mousex=x;
    mousey=y;
}

void readMap()
{
    for(unsigned int i=0; i<map.size(); i++)
    {
        for(unsigned int j=0; j<map.size(); j++)
        {
            inf->get(); //open
            *inf >> map[i][j].road;
            inf->get(); //comma
            *inf >> map[i][j].water;
            inf->get(); //comma
            *inf >> map[i][j].waste;
            inf->get(); //comma
            *inf >> map[i][j].animal;
            inf->get(); //comma
            *inf >> map[i][j].bush;
            inf->get(); //comma
            *inf >> map[i][j].tree;
            inf->get(); //comma
            *inf >> map[i][j].height;
            inf->get(); //close
        }
        inf->get(); //newline
    }
}

string inttostring(int conv)
{
	string ret;
	if(conv==0)
	{
		ret="0";
		return ret;
	}
	while(conv>=1)
	{
		ret.push_back(char((conv%10)+48));
		conv/=10;
	}
	for(unsigned int i=0; i<ret.size()/2; i++)
		swap(ret[i],ret[ret.size()-1-i]);
	return ret;
}

void timerProc(int arg)
{
    glutTimerFunc(50,timerProc,0);
    
    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(1.0f, 1.0f, 1.0f,
              1.0f, 1.0f, 0.0f,
              0.0f, 1.0f, 0.0f);
    
    RGB water(0,0,255);
    RGB road(0,255,255);
    RGB grass(0,255,0);
    RGB offwhite(230,255,255);
    for(int i=0; i<map.size(); i++)
    {
        for(int j=0; j<map[i].size(); j++)
        {
            if(map[i][j].water>0)
                makeRect(j*3+50, i*3+50,3,3,water);
            else if(map[i][j].road>0)
                makeRect(j*3+50, i*3+50,3,3,road);
            else
                makeRect(j*3+50, i*3+50,3,3,grass);
        }
    }
    makeRect(700, 50, 550, 600, offwhite);
    int tx,ty;
    tx=(mousex-50)/3;
    ty=(mousey-50)/3;
    string text="x: ";
    text+=inttostring(tx);
    text+=", y: ";
    text+=inttostring(ty);
    char *print = new char[text.length()+1];
    for(int i=0; i<text.length(); i++)
        print[i]=text[i];
    print[text.length()]=0;
    renderBitmapString(710,80,0,GLUT_BITMAP_HELVETICA_18,print);
    delete[] print;
    
    text="height: ";
    if(ty>=0 && ty<map.size() && tx>=0 && tx<map.size())
        text+=inttostring(map[ty][tx].height);
    else
        text+="n\\a";
    print = new char[text.length()+1];
    for(int i=0; i<text.length(); i++)
        print[i]=text[i];
    print[text.length()]=0;
    renderBitmapString(710,100,0,GLUT_BITMAP_HELVETICA_18,print);
    delete[] print;
    
    text="water: ";
    if(ty>=0 && ty<map.size() && tx>=0 && tx<map.size())
        text+=inttostring(map[ty][tx].water);
    else
        text+="n\\a";
    print = new char[text.length()+1];
    for(int i=0; i<text.length(); i++)
        print[i]=text[i];
    print[text.length()]=0;
    renderBitmapString(710,120,0,GLUT_BITMAP_HELVETICA_18,print);
    delete[] print;
    
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    inf = new ifstream("../Universe/map");
    map.resize(200);
    for(int i=0; i<200; i++)
    {
        map[i].resize(200);
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50,30);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH)-80,glutGet(GLUT_SCREEN_HEIGHT)-60);
    //glutInitWindowSize(700,700);

    mainWindow=glutCreateWindow("Game Engine");

    //glutFullScreen();
    WIDTH=glutGet(GLUT_WINDOW_WIDTH);
    HEIGHT=glutGet(GLUT_WINDOW_HEIGHT);

    glutDisplayFunc(renderScene);
    /*glutMouseFunc(processMouse);
    glutReshapeFunc(changeSize);*/
    glutMotionFunc(processMouseMove); //this is while mouse button pressed
    glutPassiveMotionFunc(processPassiveMouseMove); //this is while mouse button is not pressed
    //glutKeyboardFunc(processKeys);

    //initializeGameEngine();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glutTimerFunc(50,timerProc,0);
    glutMainLoop();
    return 0;
}
