#include "Role.h"


//https://stackoverflow.com/questions/2659932/how-to-read-the-screen-pixels
COLORREF getPixelColor(){
    HDC hdcScreen = ::GetDC(NULL);
    COLORREF pixel = ::GetPixel(hdcScreen, 48, 134);
    ReleaseDC(NULL, hdcScreen);
    return pixel;
}


/* 
* Return true if your role is impostor, false otherwise.
* Based on a pixel place in the tasks list
* If it's red means that you are impostor
*/
bool isImpostor(COLORREF pixel){
    int red = GetRValue(pixel);
    int green = GetGValue(pixel);
    int blue = GetBValue(pixel);
    printf("r: %d, g: %d, b: %d \n",red,green,blue);
    if(red == 255 && green == 0 && blue == 0){
        return true;
    }else{
        return false;
    }
}

