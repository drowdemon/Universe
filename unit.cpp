#include "unit.h"
#include "globals.h"

unit::unit(int p, int i, short s, short h, short str, bool g, short intel, unsigned char a, short d, short td, short px, short py, short pspeed, short los)
{
    player=p;
    index=i;
    sleep=s;
    hunger=h;
    strength=str;
    gender=g;
    intelligence=intel;
    age=a;
    diseased=d;
    timeDiseased=td;
    moveToX=x=px;
    moveToY=y=py;   
    speed=pspeed;
    lineOfSight=los;
}
void unit::nextFrame()
{
}
void unit::move()
{
    if(moveToX==x && moveToY==y)
        return;
    if(frames%speed==0)
    {
        if(moveToX>x)
        {
            if(moveToY>y)
            {
                if(map[y+1][x+1].walkable(map[y][x].height,x,y))
                {   
                    x++;
                    y++;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            else if(moveToY==y)
            {
                if(map[y][x+1].walkable(map[y][x].height,x,y))
                {   
                    x++;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            else if(moveToY<y)
            {
                if(map[y-1][x+1].walkable(map[y][x].height,x,y))
                {   
                    x++;
                    y--;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            
    }
}