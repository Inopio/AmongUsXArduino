#include <IRremote.h>
#include <string.h>
#include <Arduino.h>

#define IMPOSTOR 1
#define CREWMATE 2
#define RECEIVER_PIN  8
#define IR 3
#define CarrierFreqInterval 11

int byteToRead;
int relayPin = 4;
IRsend irsend;
IRrecv receiver(RECEIVER_PIN);
uint8_t buff;


//code taken on https://forum.arduino.cc/index.php?topic=626792.0

void pulseHIGH() {
  // Pulse 38KHz good for a LOGIC '1'
  
  for (int i = 0; i < 21; i++) {
    digitalWrite(IR, HIGH);
    delayMicroseconds(CarrierFreqInterval);
    digitalWrite(IR, LOW);
    delayMicroseconds(CarrierFreqInterval);
  }
  delay(1);
  delayMicroseconds(687.5);
}

void pulseLOW() {
  // Pulse 38KHz good for a LOGIC '0'

  for (int i = 0; i < 21; i++) {
    digitalWrite(IR, HIGH);
    delayMicroseconds(CarrierFreqInterval);
    digitalWrite(IR, LOW);
    delayMicroseconds(CarrierFreqInterval);
  }
  delayMicroseconds(562.5);

}

void transmit(uint32_t data) {
  //Function for transmiting the data

  uint32_t bitcount = 0x80000000;

  // 9ms pulse burst
  for (int i = 0; i < 355; i++) {
    digitalWrite(IR, HIGH);
    delayMicroseconds(CarrierFreqInterval);
    digitalWrite(IR, LOW);
    delayMicroseconds(CarrierFreqInterval);
  }


  // 4.5ms space
  delayMicroseconds(4500);



  //8bit address,adress inverse,command,command inverse
  while ( bitcount != 0b0) {
    if ((data & bitcount) == bitcount) {
      pulseHIGH();
    }
    else {
      pulseLOW();
    }

    bitcount = bitcount >> 1;
  }


  //final pulse burst
  for (int i = 0; i < 21; i++) {
    digitalWrite(IR, HIGH);
    delayMicroseconds(CarrierFreqInterval);
    digitalWrite(IR, LOW);
    delayMicroseconds(CarrierFreqInterval);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(IR, OUTPUT);
  pinMode(relayPin,INPUT_PULLUP);
  pinMode(relayPin,OUTPUT);
  digitalWrite(IR, LOW);
}


void loop(){
  
  if (Serial.available()>0){
    byteToRead = Serial.read();
    if(byteToRead == IMPOSTOR){
      Serial.write("Debug : send red light !");
      transmit(0xff6897);
      delay(500);
    }else if(byteToRead == CREWMATE){
      Serial.write("Debug : send green light !");
      transmit(0xff9867);
      delay(500);
    }else{
      Serial.write("ELSE...");
    }
  }
}