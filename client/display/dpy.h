#pragma once

typedef struct micoCDDisplay micoCDDisplay;

micoCDDisplay *micoCDCreateDisplay(int width, int height);
void micoCDDestroyDisplay(micoCDDisplay *display);

void micoCDGetDimensions(micoCDDisplay *dpy, int *pwidth, int *pheight);
