#ifndef __SS_H__
#define __SS_H__

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#define PIXEL_SIZE 3

void screenshot(const char* file);
void loadIni(const char* file, u32 *buttons);

#endif //__SS_H__
