#include <stdio.h>
#include <windows.h>
#include <cstdlib>
#include "stb_image.h"

#define COM3 3
#define IMPOSTOR 0
#define CREWMATE 1

extern COLORREF getPixelColor();

extern bool isImpostor(COLORREF pixel);

