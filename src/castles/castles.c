#include <tigcclib.h>
#include <stdio.h>


#define H_RANGE           60
#define H_OFFSET          5
#define TERRAIN_STEPS     8
#define TERRAIN_ROUGHNESS 2000
#define BASE_WIDTH        30
#define MIN_TERRAIN       21
#define WIND_RANGE        30
#define WIND_ARROW_Y      15
#define WIND_ARROW_HEIGHT 5
#define CANNON_LENGTH     10.0
#define CANNON_OFFSET     10
#define CANNON_BASE       4
#define CANNON_HEIGHT     4
#define SELECTOR_R        15
#define ENTRY_X           0
#define ENTRY_Y           0
#define MAX_SPEED					999
#define MUZZLE_DIST       3
#define SPEED_FACTOR      0.005f
#define WIND_FACTOR       0.00005f
#define WIND_SCALE        0.02f
#define GRAVITY           0.005f
#define EXPL_PARTICLES    20
#define EXPL_SPEED        2.0f
#define EXPL_GRAVITY      0.1f
#define EXPL_LENGTH       40
#define HIT_DISTANCE      4

short player;
short done;
short cy1,cy2,cx1,cx2,a1,a2,s1,s2;
short wind;
short terrain[TERRAIN_STEPS];
short terrainWidth;
short roundEnd = 0;

SCR_RECT *scrRect;

void rebuildTerrain(short i1, short i2) {
  short distance = i2-i1;
  if (distance<2) return;
  short mid = (i2+i1)/2;
  terrain[mid]=(terrain[i1]+terrain[i2])/2+distance*(random(TERRAIN_ROUGHNESS)-TERRAIN_ROUGHNESS/2)/100;
  rebuildTerrain(i1,mid);
  rebuildTerrain(mid,i2);
}

void initVars() {
  randomize();
	scrRect = &(SCR_RECT){{0, 0, 159, 99}};
  terrainWidth = LCD_WIDTH-2*BASE_WIDTH;
  cx1=CANNON_OFFSET;
  cx2=LCD_WIDTH-CANNON_OFFSET-1;
}

short terrainValid() {
  int i;
  for (i=0; i<TERRAIN_STEPS; i++) {
    if (terrain[i]<MIN_TERRAIN) return 0;    
  }
  return 1;
}

void setupTerrain() {
  do {
    cy1=random(H_RANGE);
    cy2=random(H_RANGE);
    int offset = LCD_HEIGHT-H_OFFSET-max(cy1,cy2);
    cy1+=offset;
    cy2+=offset;
    terrain[0]=cy1;
    terrain[TERRAIN_STEPS-1]=cy2;
    rebuildTerrain(0,TERRAIN_STEPS-1);
  } while (!terrainValid());
}

void setup() {
  done = 0;
  SetCurClip (scrRect);
  setupTerrain();
  wind=random(WIND_RANGE*2)-WIND_RANGE;
  a1=45;
  a2=45;
}

void drawTerrainLine(int offset) {
  short i;
  MoveTo(0,terrain[0]+offset);
  for (i=0; i<TERRAIN_STEPS; i++) {
    int x = BASE_WIDTH+terrainWidth*i/(TERRAIN_STEPS-1);
    LineTo(x,terrain[i]+offset);
  }
  LineTo(LCD_WIDTH-1,terrain[TERRAIN_STEPS-1]+offset);
}

void drawTerrain() {
  drawTerrainLine(0);
  drawTerrainLine(1);
  drawTerrainLine(2);
  //drawTerrainLine(5);
}

void drawCannon(int x, int y, int angle) {
  int xe,ye;
  ye=y-CANNON_LENGTH*sin(angle*3.14159f/180);
  xe=x+CANNON_LENGTH*cos(angle*3.14156f/180);
  DrawLine(x,y,xe,ye,A_NORMAL);
  int i;
  for (i=1; i<=CANNON_HEIGHT; i++) {
    DrawLine(x-CANNON_BASE+i/3,y-i,x+CANNON_BASE-i/3,y-i,A_NORMAL);
  }
}

void drawCannons() {
  drawCannon(cx1,cy1,a1);
  drawCannon(cx2,cy2,180-a2);  
}

void drawWind() {
  MoveTo((LCD_WIDTH+wind)/2,WIND_ARROW_Y);
  LineTo((LCD_WIDTH-wind)/2,WIND_ARROW_Y-WIND_ARROW_HEIGHT);
  LineTo((LCD_WIDTH-wind)/2,WIND_ARROW_Y+WIND_ARROW_HEIGHT);
  LineTo((LCD_WIDTH+wind)/2,WIND_ARROW_Y);
}

void drawScreen() {
  drawTerrain();
  drawWind();
  drawCannons();
}

void inputStr(char *buffer, unsigned short maxlen)
{
  SCR_STATE ss;
  short key;
  unsigned short i = 0;
  buffer[0] = 0;
  SaveScrState (&ss);
  ss.CurX = ENTRY_X;
  ss.CurY = ENTRY_Y;
  do {
      MoveTo (ss.CurX, ss.CurY);
      printf("Speed, Angle: %s_  ", buffer);
        // Note thatwo spaces are required only if the F_4x6 font is used
      key = ngetchx ();
      if (key >= ' ' && key <= '~' && i < maxlen)
        buffer[i++] = key;
      else if (key == KEY_BACKSPACE && i)
        i--;
      buffer[i] = 0;
  } while (key != KEY_ENTER && key != KEY_ESC && key != KEY_CLEAR);
  if (key==KEY_ESC) {
  	done = 1;
  } else if (key==KEY_CLEAR) {
  	roundEnd = 1;
  }
}

void loadParameters(short *a,short *s) {
	char x[6];
	inputStr(x,6);
	int aInt=0,sInt=0;
	sscanf(x,"%d,%d",&sInt,&aInt);
	if (sInt<1) {sInt=1;} else if (sInt>MAX_SPEED) {sInt=MAX_SPEED;}
	*s=(short)sInt;
	if (aInt>90) {aInt=90;}
	if (aInt>0) *a=(short)aInt;
}

void dataEntry(short player) {
  float sx,sy;	
	if (player==0) {
		sx=cx1;
		sy=cy1;
	} else {
		sx=cx2;
		sy=cy2;
	}
	sy-=3;
	DrawClipEllipse(sx,sy,SELECTOR_R,SELECTOR_R,scrRect,A_NORMAL);
	if (player==0) {
    loadParameters(&a1,&s1);
  } else {
    loadParameters(&a2,&s2);
  }
}

short aboveGround(float x, float y) {
	short rtn = 0;
	short found = 0;
	if (x<=BASE_WIDTH) {
		rtn = y<terrain[0];
		found = 1;
	}
	float tx1=BASE_WIDTH;
	float ty1=terrain[0];
	short i;
  for (i=1; i<TERRAIN_STEPS; i++) {
    float tx2 = BASE_WIDTH+terrainWidth*i/(TERRAIN_STEPS-1);
    float ty2=terrain[i];
    if (tx1<x && x<=tx2) {
    	x+=0.00001; // just in case, for division
    	rtn = (y-ty1)/(x-tx1) < (ty2-ty1)/(tx2-tx1);
    	found = 1;
    }
    tx1=tx2;
    ty1=ty2;
  }
  if (found) return rtn;
  return y<terrain[TERRAIN_STEPS-1];
}

short hit(float x, float y, float cx, float cy) {
	return(abs(x-cx)<HIT_DISTANCE && abs(y-cy)<HIT_DISTANCE);
}

short fire(short player) {
	float x,y,vx,vy;
	if (player==0) {
    x=cx1+(CANNON_LENGTH+MUZZLE_DIST)*cos(a1*3.14156f/180);
		y=cy1-(CANNON_LENGTH+MUZZLE_DIST)*sin(a1*3.14159f/180);
    vx=SPEED_FACTOR*s1*cos(a1*3.14156f/180);
    vy=-SPEED_FACTOR*s1*sin(a1*3.14156f/180);
	} else {
    x=cx2-(CANNON_LENGTH+MUZZLE_DIST)*cos(a2*3.14156f/180);
		y=cy2-(CANNON_LENGTH+MUZZLE_DIST)*sin(a2*3.14159f/180);
    vx=-SPEED_FACTOR*s2*cos(a2*3.14156f/180);
    vy=-SPEED_FACTOR*s2*sin(a2*3.14156f/180);
	}
	short i;
	short shellHit = 0;
  for (i=0; i<1000 && !shellHit; i++) {
    if (x>0&&x<160&&y>0&&y<100) DrawPix(x,y,A_NORMAL);
    shellHit = !aboveGround(x,y);
    if (hit(x,y,cx1,cy1)) return 0;
    if (hit(x,y,cx2,cy2)) return 1;
  	x+=vx;
  	y+=vy;
  	vx+=(wind-vx/WIND_SCALE)*WIND_FACTOR;
  	vy+=GRAVITY;
  }	
	//DrawClipEllipse(x,y,3,3,scrRect,A_NORMAL);
	return -1;
}

void showExplosion(int player) {
	int xx,yy;
	if (player==0) {
		xx=cx1;
		yy=cy1;
	} else {
		xx=cx2;
		yy=cy2;
	}
	float x[EXPL_PARTICLES],y[EXPL_PARTICLES],vx[EXPL_PARTICLES],vy[EXPL_PARTICLES];
	int t[EXPL_PARTICLES];
	short i,j;
	for (i=0; i<EXPL_PARTICLES; i++) {
		float a = random(180)*3.14159f/180.0f;
		float v = random(100)*EXPL_SPEED/100.0f;
		x[i]=xx; y[i]=yy-1; vx[i]=v*cos(a); vy[i]=-v*sin(a);
		t[i]=EXPL_LENGTH;
	}
	for (j=0; j<EXPL_LENGTH; j++) {
  	for (i=0; i<EXPL_PARTICLES; i++) {
  		if (y[i]>0 && y[i]<yy && x[i]>0 && x[i]<LCD_WIDTH) {
  			DrawPix(x[i],y[i],A_REVERSE);
  		}
  		x[i]+=vx[i];
  		y[i]+=vy[i];
  		vy[i]+=EXPL_GRAVITY;
  		t[i]--;
  		if (t[i]>0 && y[i]>0 && y[i]<yy-1 && x[i]>0 && x[i]<LCD_WIDTH) {
  			DrawPix(x[i],y[i],A_REPLACE);
  		}
		}
	}
}

void playRound() {
  setup();
  // wind=30;player=0;	
  roundEnd = 0;
  do {
    ClrScr();
    drawScreen();
    dataEntry(player);
    if (!done && !roundEnd) {
      ClrScr();
      drawScreen();
      int hit = fire(player);
      if (hit>=0) {
    	  showExplosion(hit);
  	    roundEnd=1;
      }
      player=!player;
    }
  } while (!roundEnd && !done);
}

void _main(void) {
	initVars();
  ClrScr();
  done = 0;
  randomize();
  player = random(2);
  while (!done) {
    // printf("Hi\n"); ngetchx();
    playRound();
  }
  // if (kbhit())
  // ngetchx();
}
