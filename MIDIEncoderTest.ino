#include <MIDI.h>

//#define WITHOUT_BUTTON 1
#include <ClickEncoder.h>
#include <TimerOne.h>

#define NUM_ROT 6

int buttonPin[8];    // the number of the pushbutton pin
int buttonState[8];

ClickEncoder *encoder[8];
int16_t last[8], value[8];

void timerIsr()
{
  for(int i=0;i<NUM_ROT;i++) encoder[i]->service();
}

elapsedMillis msec = 0;

void setup()
{
  Serial.begin(9600);

  encoder[0] = new ClickEncoder( 26, 25, -1);
  encoder[1] = new ClickEncoder( 23, 22, -1);
  encoder[2] = new ClickEncoder( 3, 4, -1);
  encoder[3] = new ClickEncoder( 7, 8, -1);
  encoder[4] = new ClickEncoder( 10, 11, -1);
  encoder[5] = new ClickEncoder( 13, 14, -1);

  buttonPin[0]= 24;
  buttonPin[1]= 21;
  buttonPin[2]= 5;
  buttonPin[3]= 9;
  buttonPin[4]= 12;
  buttonPin[5]= 15;

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 

  for(int i=0;i<NUM_ROT;i++) 
  {
    pinMode((const int)buttonPin[i], INPUT_PULLUP);
    last[i] = -1;
    buttonState[i]=0;
  }
}

void loop()
{
  for(int i=0;i<NUM_ROT;i++)
  {
  
    buttonState[i] = digitalRead(buttonPin[i]);
    
    if (buttonState[i] == LOW) 
    {
      Serial.print("pressed ");
      Serial.println(i);
    }
   
    value[i] += encoder[i]->getValue();
    if(value[i] < 0) value[i] = 0;
    else if(value[i] >= 127) value[i] = 127;
      
    if (value[i] != last[i])
    {
      last[i] = value[i];
      Serial.print("Encoder Value: ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(value[i]);
      if (msec >= 20)
      {
        // only check the analog inputs 50 times per second,
        // to prevent a flood of MIDI messages
        msec = 0;
        usbMIDI.sendNoteOn(60, value[i], 1);
      }

    }
  }

  while (usbMIDI.read()) {} // ignore incoming messages


}

