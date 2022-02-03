#include <tigcclib.h>
#include <stdio.h>
#include "dock.h"

#define FRONT_R	8
#define BACK_R	3
#define TWOTHIRDSPI	3.14159*0.6666667
#define VR_STEP	0.04
#define V_STEP	0.1
#define MAX_V   50
#define MAX_A   100
#define LOST_BOUNDARY 15
#define DOCK_DISTANCE 2
#define AUTODOCK_CYCLE_LENGTH 20

float x,y,a,vx,vy,vr;
float xx0,yy0,xx1,yy1,xx2,yy2;
float xxo0,yyo0,xxo1,yyo1,xxo2,yyo2;
short done;
short roundEnd;
short dockX,dockY,dockDown;
const char *STATUS_COLLISION = "CRASHED"; 
const char *STATUS_LOST = "GOT LOST"; 
const char *STATUS_DOCKED = "DOCKED"; 

void positionShip() {
	x=15; y=LCD_HEIGHT/2; a=PI*0.5+(random(2*MAX_A)-MAX_A)*0.01;
	a=PI*0.5+(random(2*MAX_A)-MAX_A)*0.01;
	float v = (random(MAX_V))*0.01;
	vx=v*sin(a); vy=-v*cos(a); vr=0.0f;
  xxo0=yyo0=xxo1=yyo1=xxo2=yyo2=-1;
  xx0=yy0=xx1=yy1=xx2=yy2=-1;
}

void setDockPosition(short x, short y, short down) {
	dockX = x;
	dockY = y;
	dockDown = down;
}

void initVars() {
  randomize();
  positionShip();
  scrRect = &(SCR_RECT){{0, 0, 159, 99}};
	SetCurClip(scrRect);
}

void recalculateShipPosition() {
	xxo0=xx0;	yyo0=yy0;
	xxo1=xx1;	yyo1=yy1;
	xxo2=xx2;	yyo2=yy2;
	a+=vr;
	x+=vx;
	y+=vy;
	xx0 = x+FRONT_R*sin(a);
	yy0 = y-FRONT_R*cos(a);
	xx1 = x+BACK_R*sin(a+TWOTHIRDSPI);
	yy1 = y-BACK_R*cos(a+TWOTHIRDSPI);
	xx2 = x+BACK_R*sin(a-TWOTHIRDSPI);
	yy2 = y-BACK_R*cos(a-TWOTHIRDSPI);
}
	
void eraseShip() {
	SetCurAttr(A_REVERSE);
  MoveTo(xxo2,yyo2);
  LineTo(xxo0,yyo0);
  LineTo(xxo1,yyo1);
  LineTo(xxo2,yyo2);
}

void drawShip() {
	SetCurAttr(A_NORMAL);
  MoveTo(xx2,yy2);
  LineTo(xx0,yy0);
  LineTo(xx1,yy1);
  LineTo(xx2,yy2);
}

void handleInput() {
	if (kbhit()) {
		switch (ngetchx()) {
			case KEY_ESC:
			  done = 1;
			  break;
		  case 338: // left
		    vr -= VR_STEP;
		    break;
      case 344: // right
        vr += VR_STEP;
        break;
      case 337: // up
        vx += V_STEP*sin(a);
        vy -= V_STEP*cos(a);
        break;
      case 340: // down
        vx += V_STEP*sin(a+PI);
        vy -= V_STEP*cos(a+PI);
        break;
      case 267: // on = pause
        ngetchx();
        break;
		}
	}
}

short checkForCollision() {
	return GetPix(xx0,yy0) || GetPix(xx1,yy1) || GetPix(xx2,yy2);
}

short playRound(void) {
	roundEnd = 0;
	ClrScr();
	positionShip();
	rebuildEnvironment(0);
	
	// x = dockX; y = dockY+15; a = 0; vx = vy = vr = 0; // debug
	
	while(!roundEnd && !done) {
  	handleInput();
	  recalculateShipPosition();
	  eraseShip();
	  short collision = checkForCollision();
	  drawShip();
	  if (collision) {
	  	roundEnd = ROUND_COLLISION;
	  } else if (x<-LOST_BOUNDARY || x>LCD_WIDTH-1+LOST_BOUNDARY) {
	  	roundEnd = ROUND_LOST;
	  } else if (abs(x-dockX)<=DOCK_DISTANCE && abs(y-dockY)<=DOCK_DISTANCE) {
	  	roundEnd = ROUND_DOCKED;
	  }
	}
	
	// AUTODOCK
	if (roundEnd==ROUND_DOCKED) {
		short i;
		vx = (float)(dockX-x)/(float)AUTODOCK_CYCLE_LENGTH;
		vy = (float)(dockY-y)/(float)AUTODOCK_CYCLE_LENGTH;
		float da = (dockDown ? PI : 0.0f) - a;
		while (da>PI) da-=2.0f*PI;
		while (da<-PI) da+=2.0f*PI;
		// printf("%f\n",a);
		vr = da/(float)AUTODOCK_CYCLE_LENGTH;
		for (i=0; i<AUTODOCK_CYCLE_LENGTH; i++) {
			recalculateShipPosition();
			eraseShip();
			drawShip();
		}
	}
	
	return roundEnd;
}

void showOpeningScreen() {
	ClrScr();
	initVars();
	rebuildEnvironment(1);
	x=dockX;
	y=dockY;
	if (dockDown) {
		a=PI;
	} else {
		a=0;
	}
	vx=vy=vr=0.0f;
	recalculateShipPosition();
	drawShip();
	short i;
	for (i=25; i<44; i++) {
    DrawLine(0,i,160-i,i,A_NORMAL);
  }
  DrawStr(6,31,"DOCK THE SHUTTLE",A_REVERSE);
	ngetchx();
}

void _main(void) {
	showOpeningScreen();
	ClrScr();
	initVars();
  done = 0;
  while (!done) {
    short roundStatus = playRound();
    if (roundStatus>0) {
    	char *str;
    	switch (roundStatus) {
    	  case ROUND_COLLISION:
    	       str=(char *)STATUS_COLLISION;
    	       break;
    	  case ROUND_LOST:
    	       str=(char *)STATUS_LOST;
    	       break;
    	  default:
    	       str=(char *)STATUS_DOCKED;
    	       break;
    	}
      short i;
      for (i=0; i<19; i++) {
      	short v = i+LCD_HEIGHT/2-10;
      	DrawLine(0,v,100-i,v,A_NORMAL);
      }
      DrawStr(10,LCD_HEIGHT/2-4,str,A_REVERSE);
      while(kbhit()) ngetchx();
      if (ngetchx()==KEY_ESC) {
      	done = 1;
      }
    }
  }
}
