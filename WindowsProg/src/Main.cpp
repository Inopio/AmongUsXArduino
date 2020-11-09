#include <stdio.h>
#include "Role.hpp"
#include "SerialPort.hpp"

int main (void){
    Serial_Port sp("\\\\.\\COM3");
    COLORREF pixel = getPixelColor();
    bool role = isImpostor(pixel);
    while(1){
        if(role){
            printf("Imposteur \n");
            sp.write({IMPOSTOR});
        }else{
            printf("Crewmate \n");
            sp.write({CREWMATE});    
        }
        
        auto buff = sp.read(1024);
        //.*s to display a certain number of char (unknown)
        printf("message: '%.*s'\n", (int)buff.size(), buff.data());
        Sleep(1000);
    }
}