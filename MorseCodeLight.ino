/*
 * MorseCodeLight: Translate Morse code light signals onto a photoresistor and translate them into text.
 * Wiring: 5V to photo resistor (https://www.adafruit.com/product/161), photoresistor parallel connection to ground through a 1k ohm resistor and to A0.
 * 
 */

#include "MorseCode.h"

int lightPin = 0;
int lightReading = 0;
float ambientLight = 0;
float ambientLightTolerance = 0;
int lightState = 0;
int previousLightState = 0;
int onTiming = 0;
int offTiming = 0;
int timingIndex = 0;
int spaceMilliseconds = 1000;

typedef struct {
  int state;
  int milliseconds;
} stateTiming;

stateTiming timingHistory[15];



void setup() 
{
  pinMode(lightPin, INPUT);
  Serial.begin(9600);

  float ambientLightReadings = 0;

  for (int i = 0; i < 10; i++) {
    ambientLightReadings += analogRead(lightPin) * ANALOG_VOLTAGE_UNITS;
    Serial.println(ambientLightReadings);
    delay(250);
  }

  ambientLight = ambientLightReadings / 10;
  ambientLightTolerance = ambientLight * TOLERANCE_PERCENTAGE;

  Serial.print("Ambient light average: ");
  Serial.println(ambientLight);
}
 
void loop() 
{
  
  lightReading = analogRead(lightPin);
  delay(100);

  if ( (lightReading * ANALOG_VOLTAGE_UNITS) > (ambientLight + ambientLightTolerance) ) {
    lightState = 1;
  } else {
    lightState = 0;
  }

  if (lightState == 0 && previousLightState == 1) {
    Serial.println("Light off");
    onTiming = millis();
    Serial.print("Light was on for ");
    Serial.println(onTiming - offTiming);
    timingHistory[timingIndex].state = 1;
    timingHistory[timingIndex].milliseconds = onTiming - offTiming;
    timingIndex++;

  } else if (lightState == 1 && previousLightState == 0) {
    Serial.println("Light on");
    offTiming = millis();
    Serial.print("Light was off for ");
    Serial.println(offTiming - onTiming);
    timingHistory[timingIndex].state = 0;
    timingHistory[timingIndex].milliseconds = offTiming - onTiming;
    timingIndex++;
  }

  if (timingIndex == 7) {
    decodeTimingHistory(timingHistory);
    timingIndex++;
  }
  
  previousLightState = lightState;
}


void decodeTimingHistory(stateTiming timingHistory[]) {

  String letter = "";

  Serial.println("Timing:");
  for (int i = 0; i < 8; i++) {
    Serial.print(timingHistory[i].state);
    Serial.print(", ");
    Serial.println(timingHistory[i].milliseconds);
  }

  for (int i = 1; i < 8; i++) {

    if (timingHistory[i].state == 1) {
      if (timingHistory[i].milliseconds < 1500) {
        letter += ".";
      } else {
        letter += "-";
      }
    }
  }

  Serial.println(letter);
  Serial.println(decode(letter + " "));
}


String decode(String morse)
{
  String msg = "";
  
  int lastPos = 0;
  int pos = morse.indexOf(' ');
  while( lastPos <= morse.lastIndexOf(' ') )
  {    
    for( int i = 0; i < sizeof MorseMap / sizeof *MorseMap; ++i )
    {
      if( morse.substring(lastPos, pos) == MorseMap[i].code )
      {
        msg += MorseMap[i].letter;
      }
    }

    lastPos = pos+1;
    pos = morse.indexOf(' ', lastPos);
    
    // Handle white-spaces between words (7 spaces)
    while( morse[lastPos] == ' ' && morse[pos+1] == ' ' )
    {
      pos ++;
    }
  }

  return msg;
}
