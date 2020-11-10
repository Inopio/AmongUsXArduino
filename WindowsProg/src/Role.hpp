#include <stdio.h>
#include <windows.h>
#include <cstdlib

#define IMPOSTOR 1
#define CREWMATE 2

extern COLORREF getPixelColor();

extern bool isImpostor(COLORREF pixel);

