// C Source File
// Created 8/29/2013; 9:38:59 PM

#include <tigcclib.h>
#include "dock.h"

#define MAX_HULL_HEIGHT 20
#define MIN_HULL_HEIGHT 5
#define HULL_STEPS 5
#define HULL_SLOPE_WIDTH 10
#define MAX_OBJECT_LENGTH 67  // was 47
#define MIN_OBJECT_LENGTH 25  // must be > MAX_HULL_HEIGHT
#define NO_ANT_MAX_LENGTH 45
#define NO_ANT_MIN_LENGTH 15
#define MAX_CLAMP_HEIGHT 10
#define MIN_CLAMP_HEIGHT 4

short upperHull[HULL_STEPS];
short lowerHull[HULL_STEPS];

void generateHullShape() {
	short i;
	for (i=0; i<HULL_STEPS; i++) {
		if (i%2) {
		  upperHull[i]=lowerHull[i]=0;
		} else {
		  upperHull[i]=lowerHull[i]=random(MAX_HULL_HEIGHT-MIN_HULL_HEIGHT)+MIN_HULL_HEIGHT;
		  //lowerHull[i]=random(MAX_HULL_HEIGHT-MIN_HULL_HEIGHT)+MIN_HULL_HEIGHT;
		}
	}
}

void rebuildEnvironment(short logoMode) {
	generateHullShape();
	float sectionLength = (float)(LCD_WIDTH-(HULL_STEPS-1)*HULL_SLOPE_WIDTH)/(float)(HULL_STEPS);
	short a,b,oldB=0,i;
	for (i=0; i<HULL_STEPS; i++) {
		a=i*(sectionLength+HULL_SLOPE_WIDTH);
		b=a+sectionLength;
		if (i>0) {
			DrawLine(oldB,upperHull[i-1],a,upperHull[i],A_NORMAL);
			DrawLine(oldB,LCD_HEIGHT-1-lowerHull[i-1],a,LCD_HEIGHT-1-lowerHull[i],A_NORMAL);
			DrawLine(oldB,upperHull[i-1]-1,a,upperHull[i]-1,A_NORMAL);
			DrawLine(oldB,LCD_HEIGHT-1-lowerHull[i-1]+1,a,LCD_HEIGHT-1-lowerHull[i]+1,A_NORMAL);
			DrawLine(oldB,0,oldB,upperHull[i-1],A_NORMAL);
			DrawLine(a,0,a,upperHull[i],A_NORMAL);
			DrawLine(oldB,LCD_HEIGHT-1,oldB,LCD_HEIGHT-lowerHull[i-1],A_NORMAL);
			DrawLine(a,LCD_HEIGHT-1,a,LCD_HEIGHT-lowerHull[i],A_NORMAL);
		}
		DrawLine(a,upperHull[i],b,upperHull[i],A_NORMAL);
		DrawLine(a,LCD_HEIGHT-1-lowerHull[i],b,LCD_HEIGHT-1-lowerHull[i],A_NORMAL);
		DrawLine(a,upperHull[i]-1,b,upperHull[i]-1,A_NORMAL);
		DrawLine(a,LCD_HEIGHT-1-lowerHull[i]+1,b,LCD_HEIGHT-1-lowerHull[i]+1,A_NORMAL);
		oldB = b;
	}
	short dockStep = logoMode ? HULL_STEPS-1 : HULL_STEPS-1-random(2);
	short clampHeight = random(MAX_CLAMP_HEIGHT-MIN_CLAMP_HEIGHT)+MIN_CLAMP_HEIGHT;
	for (i=1; i<HULL_STEPS; i++) {
		short side = logoMode ? 1 : random(2);
	  short length = random(MAX_OBJECT_LENGTH-MIN_OBJECT_LENGTH)+MIN_OBJECT_LENGTH;
	  if (logoMode) {
	  	length = 45;
	  }
	  // length = MAX_OBJECT_LENGTH;  // debugging
		a = i*(sectionLength+HULL_SLOPE_WIDTH)+sectionLength*0.5f;
		b = side ? LCD_HEIGHT-lowerHull[i]-1 : lowerHull[i];
		length-=side ? lowerHull[i] : upperHull[i];
		if (i==dockStep) {
			drawDock(a,b,length,clampHeight,!side /*,0*/);
			setDockPosition(a,side ? b-length-2 : b+length+2, !side);
		} else {
			short antL = random(2);
			short antR = random(2);
			if (length>NO_ANT_MAX_LENGTH || length<NO_ANT_MIN_LENGTH) {
				antL=0;
				antR=0;
			}
			drawGauntry(a,b,length,random(2-antL),random(2-antR),antL,antR,!side);
		}
	}
}