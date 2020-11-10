#include <stdio.h>
#include "Role.hpp"
#include "SerialPort.hpp"

/*
* Get the pixel value of the settings button
* And map button (that are fixed objects)
*/
bool isInGame(){
    int r1,r2,r3,g1,g2,g3,b1,b2,b3;
    //If you have multiples screens, it is by default the 1st one
    //Means that you have to launch the game on the 1st screen
    HDC hdcScreen = ::GetDC(NULL);
    COLORREF pixel1 = ::GetPixel(hdcScreen, 1848, 30);
    COLORREF pixel2 = ::GetPixel(hdcScreen, 1848, 50);
    COLORREF pixel3 = ::GetPixel(hdcScreen, 1848, 179);
    ReleaseDC(NULL, hdcScreen);

    r1 = GetRValue(pixel1);
    r2 = GetRValue(pixel2);
    r3 = GetRValue(pixel3);
    g1 = GetGValue(pixel1);
    g2 = GetGValue(pixel2);
    g3 = GetGValue(pixel3);
    b1 = GetBValue(pixel1);
    b2 = GetBValue(pixel2);
    b3 = GetBValue(pixel3);
    
    if(r1 == 238 && g1 == 238 && b1 == 238
        && r2 == 34 && g2 == 34 && b2 == 34
        && r3 == 238 && g3 == 238 && b3 == 238){

        return true;
    }else{
        return false;
    }
}

int main (void){
    //Port where the arduino is plugged
    Serial_Port sp("\\\\.\\COM3");
    
    while(1){
        if(isInGame()){
            COLORREF pixel = getPixelColor();
            bool role = isImpostor(pixel);
            if(role){
                printf("Imposteur \n");
                sp.write({IMPOSTOR});
            }else{
                printf("Crewmate \n");
                sp.write({CREWMATE});    
            }
        }else{
            printf("Not in game yet \n");

        }
        /* DEBUG */
        //auto buff = sp.read(1024);
        //.*s to display a certain number of char (unknown)
        //printf("message: '%.*s'\n", (int)buff.size(), buff.data());

        Sleep(1000);
    }
}