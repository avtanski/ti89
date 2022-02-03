#include <tigcclib.h>
#include <stdio.h>
#include <args.h>

#define HMARGIN 2
#define HALFBARSIZE 7
#define MV_STEP 10
#define XSPEED 0.2f
#define YSPEED 0.2f
#define YSPEED_MAX 0.4f
#define BSIZE 2

short sa,sb,posa,posb,posa_changed,posb_changed;
float bx,by,sx,sy,obx,oby;
float xspeed,yspeed,yspeedMax;
short shouldExit;
char scoreString[20];

void reset(short); 
    
void drawScreen() {
  // BAR A
  if (posa_changed) {
	  DrawLine(HMARGIN,0,HMARGIN,LCD_HEIGHT, A_REVERSE);
	  DrawLine(HMARGIN+1,0,HMARGIN+1,LCD_HEIGHT, A_REVERSE);
	  posa_changed = 0;
	}
	DrawLine(HMARGIN,posa-HALFBARSIZE+1,HMARGIN,posa+HALFBARSIZE-1, A_NORMAL);
	DrawLine(HMARGIN+1,posa-HALFBARSIZE+1,HMARGIN+1,posa+HALFBARSIZE-1, A_NORMAL);
	// BAR B
	if (posb_changed) {
		DrawLine(LCD_WIDTH-HMARGIN-1,0,LCD_WIDTH-HMARGIN-1,LCD_HEIGHT, A_REVERSE);
		DrawLine(LCD_WIDTH-HMARGIN-2,0,LCD_WIDTH-HMARGIN-2,LCD_HEIGHT, A_REVERSE);
		posb_changed = 0;
	}
	DrawLine(LCD_WIDTH-HMARGIN-1,posb-HALFBARSIZE+1,LCD_WIDTH-HMARGIN-1,posb+HALFBARSIZE-1, A_NORMAL);
	DrawLine(LCD_WIDTH-HMARGIN-2,posb-HALFBARSIZE+1,LCD_WIDTH-HMARGIN-2,posb+HALFBARSIZE-1, A_NORMAL);
	DrawClipEllipse((short)obx,(short)oby,BSIZE,BSIZE,&(SCR_RECT){{0, 0, LCD_WIDTH, LCD_HEIGHT}},A_REVERSE);
	DrawClipEllipse((short)bx,(short)by,BSIZE,BSIZE,&(SCR_RECT){{0, 0, LCD_WIDTH, LCD_HEIGHT}},A_NORMAL);
}

void handleInput() {
	if (!kbhit()) return;
	short c = ngetchx();
	//printf("%d",c); ngetchx();
	switch (c) {
		case 120:  // X
		  if (posa>0) {
		  	posa-=MV_STEP;
		  	posa_changed = 1;
		  }
		  break;
		case 61:  // =
		case 97:  // x
		  if (posa<LCD_HEIGHT) {
		  	posa+=MV_STEP;
		  	posa_changed = 1;
		  }
		  break;
		case 94:  // ^
		  if (posb>0) {
		  	posb-=MV_STEP;
		  	posb_changed = 1;
		  }
		  break;
		case 47:  // /
		case 101: // e
		  if (posb<LCD_HEIGHT) {
		  	posb+=MV_STEP;
		  	posb_changed = 1;
		  }
		  break;
		case KEY_ESC:
		  shouldExit = 1;
		  break;
	}
}

float getNewSY(float currentSY) {
	float newSY = currentSY + (2.0f*(float)rand()/(float)RAND_MAX-1.0f)*yspeed;
	if (newSY>yspeedMax) {
		newSY = yspeedMax;
	} else if (newSY<-yspeedMax) {
		newSY = -yspeedMax;
	}
	return newSY;
}

void score(short *s) {
	(*s)++;
	ClrScr();	
	sprintf(scoreString, "Score: %02d : %02d",sa,sb);
	DrawStr(35,45,scoreString,A_NORMAL);
	switch (ngetchx()) {
		case KEY_ESC:
		  shouldExit = 1;
		  break;
		case KEY_CLEAR:
		  reset(1);
		  break;
		default:
		  reset(0);
		  break;
	}
}

void moveBall() {
	obx = bx;
	oby = by;
	bx += sx;
	by += sy;
	// printf("%f %f\n%f %f\n",bx,sx,by,sy); ngetchx();
	if (by<BSIZE) {
		by=BSIZE; sy=abs(sy);
	} else if(by>=LCD_HEIGHT-BSIZE) {
		by=LCD_HEIGHT-BSIZE; sy=-abs(sy);
	}
	if (bx<BSIZE+HMARGIN+1) {
		if (abs(posa-by)<HALFBARSIZE) {
			bx=BSIZE+HMARGIN+1; sx=abs(sx);
			sy = getNewSY(sy);
		} else {
			score(&sb);
		}
	} else if (bx>LCD_WIDTH-BSIZE-HMARGIN-1) {
		if (abs(posb-by)<HALFBARSIZE) {
			bx=LCD_WIDTH-BSIZE-HMARGIN-1; sx=-abs(sx);
			sy = getNewSY(sy);
		} else {
			score(&sa);
		}
	}
}

void reset(short resetScore) {
  ClrScr();
  if (resetScore) {
	  sa = 0;
	  sb = 0;
	}
	posa = LCD_HEIGHT/2;
	posb = LCD_HEIGHT/2;
	posa_changed = 0;
	posb_changed = 0;
	// shouldExit = 0;
	bx = LCD_WIDTH*0.5f;
	by = LCD_HEIGHT*0.5f;
	sx = (float)rand()/(float)RAND_MAX<0.5f ? -xspeed : xspeed;
	sy = getNewSY(0.0f);
}

void _main(void) {
    ESI argptr;
    long difficulty=5;
    InitArgPtr(argptr);
    int argtype = GetArgType(argptr);
    // printf_xy (0, 30, "%ld", argtype); ngetchx();
    if (argtype == POSINT_TAG)
    {
      difficulty = GetIntArg (argptr);
      //printf_xy (0, 60, "%ld", num); ngetchx();
    }
    float dif = 1.0f;
    if (difficulty>0) {
      dif = (float)difficulty/5.0f;
    }
    // printf_xy (0, 60, "%f", dif); ngetchx();
    xspeed=XSPEED*dif;
    yspeed=YSPEED*dif;
    yspeedMax=YSPEED_MAX*dif;
    
    shouldExit = 0;
    reset(1);
    ClrScr();
    while (!shouldExit) {
    	drawScreen();
    	handleInput();
    	moveBall();
    }
    // y=random(LCD_HEIGHT);
    // DrawPix(x,y,A_NORMAL);
    ClrScr();
}
