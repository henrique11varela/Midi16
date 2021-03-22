#include <Arduino.h>
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <frequencyToNote.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

//Multiplexer pins
const int sig = 9;
const int s[4] = {15, 14, 16, 10};
//Screen pins
const int sck = 3;
const int sda = 2;
//Switch pins
const int right = 5;
const int left = 4;
//Other pins
const int side = 8;
const int jack = 18;

const int btnTranslate[16] = {12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3};
const char binary[16][4] = {{0, 0, 0, 0},
                            {1, 0, 0, 0},
                            {0, 1, 0, 0},
                            {1, 1, 0, 0},
                            {0, 0, 1, 0},
                            {1, 0, 1, 0},
                            {0, 1, 1, 0},
                            {1, 1, 1, 0},
                            {0, 0, 0, 1},
                            {1, 0, 0, 1},
                            {0, 1, 0, 1},
                            {1, 1, 0, 1},
                            {0, 0, 1, 1},
                            {1, 0, 1, 1},
                            {0, 1, 1, 1},
                            {1, 1, 1, 1}};

//Variables
bool oldButtonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void controlChange(byte channel, byte control, byte value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

bool ButtonIsPressed(int i)
{
  for (int j = 0; j < 4; j++)
  {
    digitalWrite(s[j], binary[btnTranslate[i]][j]);
  }
  return digitalRead(sig) == LOW ? true : false;
}

void PlayMode()
{
  for (int i = 0; i < 16; i++)
  {
    if (ButtonIsPressed(i) != oldButtonState[i]{
      if (ButtonIsPressed(i))
      {
        noteOn(0, i + 36, 127);
        Serial.print("Play: ");
        Serial.print("ch");
        Serial.print(" ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println("vel");
        MidiUSB.flush();
      }
      else
      {
        noteOff(0, i + 36, 0);
        Serial.print("Stop: ");
        Serial.print("ch");
        Serial.print(" ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println("vel");
        MidiUSB.flush();
      }
      oldButtonState[i] = ButtonIsPressed(i);
    }
  }
}

void setup()
{
  pinMode(sig, INPUT_PULLUP);
  pinMode(s[0], OUTPUT);
  pinMode(s[1], OUTPUT);
  pinMode(s[2], OUTPUT);
  pinMode(s[3], OUTPUT);
  pinMode(right, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(side, INPUT_PULLUP);
  pinMode(jack, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop()
{
  PlayMode();
}