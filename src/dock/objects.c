// C Source File
// Created 8/28/2013; 10:22:53 AM

#include <tigcclib.h>
#include "dock.h"

//#define SAT_ARR_HW 8
#define GRID_BOX_HW 3
#define DOCK_BASE_HW 4

/*
void drawSatelliteV(short x, short y) {
	DrawLine(x-1,y-2,x+1,y-2,A_NORMAL);
	DrawLine(x-1,y+2,x+1,y+2,A_NORMAL);
	DrawLine(x-2,y-1,x-2,y+1,A_NORMAL);
	DrawLine(x+2,y-1,x+2,y+1,A_NORMAL);
	short i;
	for (i=0; i<3; i++) {
		short offset = 3+i*4;
		DrawPix(x,y+offset,A_NORMAL);
		DrawPix(x,y-offset,A_NORMAL);
		DrawClipRect(&(WIN_RECT){x-SAT_ARR_HW,y-offset-1,x+SAT_ARR_HW,y-offset-3}, scrRect, A_NORMAL);
		DrawClipRect(&(WIN_RECT){x-SAT_ARR_HW,y+offset+1,x+SAT_ARR_HW,y+offset+3}, scrRect, A_NORMAL);
	}
}

void drawSatelliteH(short x, short y) {
	DrawLine(x-1,y-2,x+1,y-2,A_NORMAL);
	DrawLine(x-1,y+2,x+1,y+2,A_NORMAL);
	DrawLine(x-2,y-1,x-2,y+1,A_NORMAL);
	DrawLine(x+2,y-1,x+2,y+1,A_NORMAL);
	short i;
	for (i=0; i<3; i++) {
		short offset = 3+i*4;
		DrawPix(x+offset,y,A_NORMAL);
		DrawPix(x-offset,y,A_NORMAL);
		DrawClipRect(&(WIN_RECT){x-offset-1,y-SAT_ARR_HW,x-offset-3,y+SAT_ARR_HW}, scrRect, A_NORMAL);
		DrawClipRect(&(WIN_RECT){x+offset+1,y-SAT_ARR_HW,x+offset+3,y+SAT_ARR_HW}, scrRect, A_NORMAL);
	}
}
*/

void drawGridBox(short x, short y) {
	DrawClipRect(&(WIN_RECT){x-GRID_BOX_HW,y-GRID_BOX_HW,x+GRID_BOX_HW,y+GRID_BOX_HW}, scrRect, A_NORMAL);
	DrawLine(x-GRID_BOX_HW+1,y-GRID_BOX_HW+1,x+GRID_BOX_HW-1,y+GRID_BOX_HW-1,A_NORMAL);
	DrawLine(x-GRID_BOX_HW+1,y+GRID_BOX_HW-1,x+GRID_BOX_HW-1,y-GRID_BOX_HW+1,A_NORMAL);
}

void drawDish(short x, short y, short right) {
	short dir = right ? 1 : -1;
	DrawPix(x,y,A_NORMAL);
	x+=dir; DrawLine(x,y-2,x,y+2,A_NORMAL);
	x+=dir; DrawPix(x,y-3,A_NORMAL); DrawPix(x,y+3,A_NORMAL);
	x+=dir; DrawPix(x,y-4,A_NORMAL); DrawPix(x,y+4,A_NORMAL);
	x+=dir; DrawLine(x,y-5,x,y+5,A_NORMAL);
	x+=dir; DrawLine(x,y-1,x+dir,y-1,A_NORMAL); DrawLine(x,y+1,x+dir,y+1,A_NORMAL);
	x+=2*dir; DrawLine(x,y,x+2*dir,y,A_NORMAL);
}

void drawGauntry(short x, short y, short height, short leftT, short rightT, short leftDish, short rightDish, short down) {
	short yy,gridCount,i;
	if (down) {
		yy = y+GRID_BOX_HW;
	} else {
		yy = y-GRID_BOX_HW;
	}
	gridCount = height/(2*GRID_BOX_HW+1);
	for (i=0; i<gridCount; i++) {
		drawGridBox(x,yy);
		if (i<gridCount-1) {
			yy += down ? 2*GRID_BOX_HW+1 : -2*GRID_BOX_HW-1;
		}
	}
	for (i=1; i<=leftT; i++) {
		drawGridBox(x-i*(2*GRID_BOX_HW+1),yy);
	}
	for (i=1; i<=rightT; i++) {
		drawGridBox(x+i*(2*GRID_BOX_HW+1),yy);
	}
	if (leftDish) {
		drawDish(x-leftT*(2*GRID_BOX_HW+1)-GRID_BOX_HW-1,yy,0);
	}
	if (rightDish) {
		drawDish(x+rightT*(2*GRID_BOX_HW+1)+GRID_BOX_HW+1,yy,1);
	}
}

/*
void drawSatellite(short x, short y, short horizontal) {
	if (horizontal) {
		drawSatelliteH(x,y);
	} else {
		drawSatelliteV(x,y);
	}
}
*/

void drawDock(short x, short y, short height, short clampHeight, short down /*, short number*/) {
	short yy = y;
	short odd = (height)%2;
	short dir = down ? 1 : -1;
	short i;
	for (i=0; i<height-3; i++) {
		if (i%2==odd) {
			DrawLine(x-DOCK_BASE_HW,yy,x+DOCK_BASE_HW,yy,A_NORMAL);
		} else {
			DrawPix(x-DOCK_BASE_HW-1,yy,A_NORMAL);
			DrawPix(x+DOCK_BASE_HW+1,yy,A_NORMAL);
		}
		yy += dir;
	}
	DrawLine(x-DOCK_BASE_HW-1,yy,x+DOCK_BASE_HW+1,yy,A_NORMAL);
	yy += dir;
	DrawLine(x-DOCK_BASE_HW-2,yy,x+DOCK_BASE_HW+2,yy,A_NORMAL);
	yy += dir;
	DrawLine(x-DOCK_BASE_HW-3,yy,x-DOCK_BASE_HW-1,yy,A_NORMAL);
	DrawLine(x+DOCK_BASE_HW+3,yy,x+DOCK_BASE_HW+1,yy,A_NORMAL);
	for (i=0; i<clampHeight; i++) {
		yy += dir;
		DrawLine(x-DOCK_BASE_HW-3,yy,x-DOCK_BASE_HW-2,yy,A_NORMAL);
		DrawLine(x+DOCK_BASE_HW+3,yy,x+DOCK_BASE_HW+2,yy,A_NORMAL);
	}
}