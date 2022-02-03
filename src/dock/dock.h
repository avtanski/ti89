#define DIRECTION_HORIZONTAL 0
#define DIRECTION_VERTICAL   1
#define ROUND_COLLISION			 1
#define ROUND_LOST           2
#define ROUND_DOCKED         3

SCR_RECT *scrRect;

void drawSatellite(short x, short y, short direction);
void drawGauntry(short x, short y, short leftT, short rightT, short leftDish, short rightDish, short height, short down);
void drawDock(short x, short y, short height, short clampHeight, short down /*, short number*/);
void rebuildEnvironment(short logoMode);

void setDockPosition(short x, short y, short down);
