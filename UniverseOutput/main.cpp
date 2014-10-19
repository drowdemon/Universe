#include <cstdlib>
#include <fstream>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

#define PI 3.141592
#define TILESIZE 4

int mainWindow=0;
int WIDTH=0;
int HEIGHT=0;
int mousex=0;
int mousey=0;
int numFrames;

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

class ARGB
{
public:
    unsigned char a;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    ARGB(unsigned char pa, unsigned char pr, unsigned char pg, unsigned char pb)
    {
        a=pa;
        r=pr;
        g=pg;
        b=pb;
    }
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
    bool uniton;
    short unitplayer;
    short unitindex;
    tile(unsigned char r=0, unsigned short wtr=0, unsigned char wst=0, unsigned char a=0, unsigned char b=0, unsigned char t=0, short h=0, bool uo=false)
    {
        road=r;
        water=wtr;
        waste=wst;
        animal=a;
        bush=b;
        tree=t;
        height=h;
        uniton=uo;
        unitplayer=unitindex=-1;
    }
};

class point
{
public:
    int x;
    int y;
    point(int px, int py)
    {
        x=px;
        y=py;
    }
};

class unit
{
public:
    int x;
    int y;
    int player;
    int index;
    int energy;
    int sleep;
    int hunger;
    int health;
    int pregnant;
    vector<point> *sight;
    unit(int px=0, int py=0, int p=-1, int i=-1, int e=0, int s=0, int hgr=0, int h=0, int prg=0, vector<point> *sght=NULL)
    {
        x=px;
        y=py;
        player=p;
        index=i;
        energy=e;
        sleep=s;
        hunger=hgr;
        health=h;
        pregnant=prg;
        if(sght==NULL)
            sight=NULL;
        else
        {
            sight = new vector<point>;
            for(unsigned int i=0; i<sght->size(); i++)
                sight->push_back(point(((*sght)[i]).x, ((*sght)[i]).y));
        }
    }
};

vector<vector<tile> > map;
vector<vector<unit*> > allunits;
ifstream *inf;
ifstream *unitin;

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
            int temp=0;
            inf->get(); //open
            *inf >> temp;
            map[i][j].road=temp;
            inf->get(); //comma
            *inf >> temp;
            map[i][j].water=temp;
            inf->get(); //comma
            *inf >> temp;
            map[i][j].waste=temp;
            inf->get(); //comma
            *inf >> temp;
            map[i][j].animal=temp;
            inf->get(); //comma
            *inf >> temp;
            map[i][j].bush=temp;
            inf->get(); //comma
            *inf >> temp;
            map[i][j].tree=temp;
            inf->get(); //comma
            *inf >> temp;
            map[i][j].height=temp;
            inf->get(); //comma
            *inf >> temp;
            ((temp==0) ? (map[i][j].uniton=false) : (map[i][j].uniton=true));
            inf->get(); //close
        }
        inf->get(); //newline
    }
}
void readUnit()
{
    int mx;
    int my;
    char buf=0;
    bool change=false;
    while(true)
    {
        unit *u = new unit;
        u->sight = new vector<point>;
        unitin->get(buf); //open
        if(buf=='\n' || buf==0)
        {
            if(change==true)
                numFrames++;
            delete u;
            break; //end. else, its the open bracket
        }
        change=true;
        *unitin >> u->x;
        unitin->get(); //comma
        *unitin >> u->y;
        unitin->get();//comma
        *unitin >> mx;
        unitin->get();//comma
        *unitin >> my;
        unitin->get();//comma
        *unitin >> u->player;
        unitin->get();//comma
        *unitin >> u->index;
        unitin->get();//comma
        *unitin >> u->energy;
        unitin->get();//comma
        *unitin >> u->hunger;
        unitin->get();//comma
        *unitin >> u->sleep;
        unitin->get();//comma
        *unitin >> u->health;
        unitin->get();//comma
        *unitin >> u->pregnant;
        unitin->get();//end bracket
        unitin->get();//open bracket
        int num1=0;
        int num2=0;
        while(true)
        {
            *unitin >> num1;
            if(num1<0)
                break; //store exit code in num1
            unitin->get(); //comma
            *unitin >> num2;
            unitin->get(); //comma
            u->sight->push_back(point(num1,num2));
        }
        if(num1==-1)
        {
            delete u->sight;
            u->sight=NULL;
        }
        unitin->get();//end bracket
        unitin->get();//end line
        if(u->index+1>(int)allunits[u->player].size())
        {
            allunits[u->player].push_back(u);
            map[u->y][u->x].uniton=true;
            map[u->y][u->x].unitplayer=u->player;
            map[u->y][u->x].unitindex=u->index;
            u = new unit;
        }
        else if(u->x==u->energy && u->x==u->hunger && u->x==u->health && u->x==u->pregnant && u->x==u->sleep && u->x==u->y && u->x==u->index && u->x==u->player && u->x==-99999) //reformat code
        {
            vector<vector<int> > indexMappings;
            indexMappings.resize(allunits.size());
            for(unsigned int i=0; i<allunits.size(); i++)
            {
                indexMappings[i].resize(allunits[i].size());
                int currDeleted=0;
                for(unsigned int j=0; j<allunits[i].size(); j++)
                {
                    if(!allunits[i][j]) //invalid pointer
                    {
                        allunits[i].erase(allunits[i].begin()+j); //delete it
                        j--;
                        currDeleted++;
                    }
                    else //valid
                    {
                        allunits[i][j]->index-=currDeleted;
                        map[allunits[i][j]->y][allunits[i][j]->x].unitindex-=currDeleted;
                        indexMappings[i][j]=allunits[i][j]->index;
                    }
                }
            }
        }
        else if(u->x==u->energy && u->x==u->hunger && u->x==u->health && u->x==u->pregnant && u->x==u->sleep && u->x==u->y && u->x==-99999) //death code
        {
            /*for(int i=u.index+1; i<allunits[u.player].size(); i++)
            {
                allunits[u.player][i].index--;
                map[allunits[u.player][i].y][allunits[u.player][i].x].unitindex--;
            }*/
            map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].uniton=false;
            map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].unitplayer=-1;
            map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].unitindex=-1;
            if(allunits[u->player][u->index]->sight)
            {
                delete allunits[u->player][u->index]->sight;
                allunits[u->player][u->index]->sight=NULL;
            }
            delete allunits[u->player][u->index];
            allunits[u->player][u->index]=NULL;
            //allunits[u->player].erase(allunits[u->player].begin()+u->index);
        }
        else
        {
            if(mx!=0 || my!=0)
            {
                map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].uniton=false;
                map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].unitplayer=-1;
                map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].unitindex=-1;
            }
            allunits[u->player][u->index]->x+=mx;
            allunits[u->player][u->index]->y+=my;
            allunits[u->player][u->index]->energy+=u->energy;
            allunits[u->player][u->index]->health+=u->health;
            allunits[u->player][u->index]->hunger+=u->hunger;
            allunits[u->player][u->index]->pregnant+=u->pregnant;
            allunits[u->player][u->index]->sleep+=u->sleep;
            if(u->sight)
            {
                if(allunits[u->player][u->index]->sight)
                    delete allunits[u->player][u->index]->sight;
                allunits[u->player][u->index]->sight = new vector<point>;
                for(unsigned int i=0; i<u->sight->size(); i++)
                    allunits[u->player][u->index]->sight->push_back(point(((*u->sight)[i]).x, ((*u->sight)[i]).y));
            }
            if(mx!=0 || my!=0)
            {
                map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].uniton=true;
                map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].unitplayer=u->player;
                map[allunits[u->player][u->index]->y][allunits[u->player][u->index]->x].unitindex=u->index;
            }
        }
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
    RGB bush(0,150,0);
    RGB offwhite(230,255,255);
    RGB black(0,0,0);
    ARGB grey(100,100,100,100);
    
    readUnit();
    for(unsigned int i=0; i<map.size(); i++)
    {
        for(unsigned int j=0; j<map[i].size(); j++)
        {
            if(map[i][j].water>0)
                makeRect(j*TILESIZE+50, i*TILESIZE+50,TILESIZE,TILESIZE,water);
            else if(map[i][j].road>0)
                makeRect(j*TILESIZE+50, i*TILESIZE+50,TILESIZE,TILESIZE,road);
            else if(map[i][j].bush>0)
                makeRect(j*TILESIZE+50, i*TILESIZE+50,TILESIZE,TILESIZE,bush);
            else
                makeRect(j*TILESIZE+50, i*TILESIZE+50,TILESIZE,TILESIZE,grass);
            if(map[i][j].uniton)
                drawEmptyRect(j*TILESIZE+50, i*TILESIZE+50,TILESIZE,TILESIZE,black);
        }
    }
    for(unsigned int i=0; i<allunits.size(); i++)
    {
        for(unsigned int j=0; j<allunits[i].size(); j++)
        {
            if(allunits[i][j] && allunits[i][j]->sight)
            {
                for(unsigned int k=0; k<allunits[i][j]->sight->size(); k++)
                {
                    makeRect((*(allunits[i][j]->sight))[k].x*TILESIZE+50,(*(allunits[i][j]->sight))[k].y*TILESIZE+50,TILESIZE,TILESIZE,grey);
                }
            }
        }
    }
    int leftRect = 900;
    makeRect(leftRect, 50, 550, 600, offwhite);
    int tx,ty;
    tx=(mousex-50)/TILESIZE;
    ty=(mousey-50)/TILESIZE;
    string text="x: ";
    text+=inttostring(tx);
    text+=", y: ";
    text+=inttostring(ty);
    char *print = new char[text.length()+1];
    for(unsigned int i=0; i<text.length(); i++)
        print[i]=text[i];
    print[text.length()]=0;
    renderBitmapString(leftRect+10,80,0,GLUT_BITMAP_HELVETICA_18,print);
    delete[] print;
    
    text="height: ";
    if(ty>=0 && ty<map.size() && tx>=0 && tx<map.size())
        text+=inttostring(map[ty][tx].height);
    else
        text+="n\\a";
    print = new char[text.length()+1];
    for(unsigned int i=0; i<text.length(); i++)
        print[i]=text[i];
    print[text.length()]=0;
    renderBitmapString(leftRect+10,100,0,GLUT_BITMAP_HELVETICA_18,print);
    delete[] print;
    
    text="water: ";
    if(ty>=0 && ty<map.size() && tx>=0 && tx<map.size())
        text+=inttostring(map[ty][tx].water);
    else
        text+="n\\a";
    print = new char[text.length()+1];
    for(unsigned int i=0; i<text.length(); i++)
        print[i]=text[i];
    print[text.length()]=0;
    renderBitmapString(leftRect+10,120,0,GLUT_BITMAP_HELVETICA_18,print);
    delete[] print;
    
    text="frames: ";
    text+=inttostring(numFrames);
    print = new char[text.length()+1];
    for(unsigned int i=0; i<text.length(); i++)
        print[i]=text[i];
    print[text.length()]=0;
    renderBitmapString(leftRect+10,140,0,GLUT_BITMAP_HELVETICA_18,print);
    delete[] print;
    
    if(ty>=0 && ty<map.size() && tx>=0 && tx<map.size() && map[ty][tx].uniton)
    {
        text="energy: ";
        text+=inttostring(allunits[map[ty][tx].unitplayer][map[ty][tx].unitindex]->energy);
        print = new char[text.length()+1];
        for(unsigned int i=0; i<text.length(); i++)
            print[i]=text[i];
        print[text.length()]=0;
        renderBitmapString(leftRect+140,100,0,GLUT_BITMAP_HELVETICA_18,print);
        delete[] print;
        
        text="health: ";
        text+=inttostring(allunits[map[ty][tx].unitplayer][map[ty][tx].unitindex]->health);
        print = new char[text.length()+1];
        for(unsigned int i=0; i<text.length(); i++)
            print[i]=text[i];
        print[text.length()]=0;
        renderBitmapString(leftRect+140,120,0,GLUT_BITMAP_HELVETICA_18,print);
        delete[] print;
        
        text="hunger: ";
        text+=inttostring(allunits[map[ty][tx].unitplayer][map[ty][tx].unitindex]->hunger);
        print = new char[text.length()+1];
        for(unsigned int i=0; i<text.length(); i++)
            print[i]=text[i];
        print[text.length()]=0;
        renderBitmapString(leftRect+140,140,0,GLUT_BITMAP_HELVETICA_18,print);
        delete[] print;
        
        text="sleep: ";
        text+=inttostring(allunits[map[ty][tx].unitplayer][map[ty][tx].unitindex]->sleep);
        print = new char[text.length()+1];
        for(unsigned int i=0; i<text.length(); i++)
            print[i]=text[i];
        print[text.length()]=0;
        renderBitmapString(leftRect+140,160,0,GLUT_BITMAP_HELVETICA_18,print);
        delete[] print;
        
        text="pregnant: ";
        text+=inttostring(allunits[map[ty][tx].unitplayer][map[ty][tx].unitindex]->pregnant);
        print = new char[text.length()+1];
        for(unsigned int i=0; i<text.length(); i++)
            print[i]=text[i];
        print[text.length()]=0;
        renderBitmapString(leftRect+140,180,0,GLUT_BITMAP_HELVETICA_18,print);
        delete[] print;
    }
    
    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    inf = new ifstream("../Universe/map");
    unitin = new ifstream ("../Universe/unitChanges");
    map.resize(200);
    for(int i=0; i<200; i++)
    {
        map[i].resize(200);
    }
    allunits.resize(2);
    readMap();
    readUnit();
    
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
