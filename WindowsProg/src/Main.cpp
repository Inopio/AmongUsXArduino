#include <stdio.h>
#include "Role.h"
#include <windows.h>

int main (void){
   
   COLORREF pixel = getPixelColor();
   bool role = isImpostor(pixel);
   if(role){
       printf("Imposteur \n");
   }else{
       printf("Crewmate \n");
   }
}