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
int menu = 0;
int oldSwitchState = 0;
bool oldButtonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int midiChannel = 0;
int rootNote = 0;
int noteIndex = 3;

//Functions
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

int SwitchState()
{
  if (digitalRead(left) == LOW)
  {
    return 1;
  }
  else if (digitalRead(right) == LOW)
  {
    return 2;
  }
  else
  {
    return 0;
  }
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
    if (ButtonIsPressed(i) != oldButtonState[i])
    {
      if (ButtonIsPressed(i))
      {
        noteOn(midiChannel, noteIndex * 12 + rootNote + i, 127);
        Serial.print("Play: ch ");
        Serial.print(midiChannel);
        Serial.print(" i ");
        Serial.print(i);
        Serial.print(" rt ");
        Serial.print(rootNote);
        Serial.print(" vel ");
        Serial.print("vel");
        Serial.print(" index ");
        Serial.println(noteIndex);
        MidiUSB.flush();
        delay(2);
      }
      else
      {
        noteOff(midiChannel, noteIndex * 12 + rootNote + i, 0);
        Serial.print("Stop: ch ");
        Serial.print(midiChannel);
        Serial.print(" i ");
        Serial.print(i);
        Serial.print(" rt ");
        Serial.print(rootNote);
        Serial.print(" vel ");
        Serial.print("vel");
        Serial.print(" index ");
        Serial.println(noteIndex);
        MidiUSB.flush();
        delay(2);
      }
      oldButtonState[i] = ButtonIsPressed(i);
    }
  }
}

void MidiChannelMode()
{
  for (int i = 0; i < 16; i++)
  {
    if (ButtonIsPressed(i))
    {
      midiChannel = i;
    }
  }
}

void TranscribeMode()
{

  for (int i = 0; i < 16; i++)
  {
    if (ButtonIsPressed(i) != oldButtonState[i])
    {
      //Root Note Incrementer
      if (ButtonIsPressed(12))
      {
        if (rootNote > 0)
        {
          rootNote--;
        }
        else if (noteIndex > 0)
        {
          rootNote = 11;
          noteIndex--;
        }
      }
      else if (ButtonIsPressed(13))
      {
        if (rootNote < 11)
        {
          rootNote++;
        }
        else if (noteIndex < 7)
        {
          rootNote = 0;
          noteIndex++;
        }
      }
      //Octave change
      else if (ButtonIsPressed(14) && noteIndex > 0)
      {
        noteIndex--;
      }

      else if (ButtonIsPressed(15) && noteIndex < 7)
      {
        noteIndex++;
      }
      //Root Note Changer
      else
      {
        rootNote = i;
      }
      oldButtonState[i] = ButtonIsPressed(i);
      delay(2);
    }
  }
}

void DebugMode()
{
  for (int i = 0; i < 16; i++)
  {
    Serial.print(ButtonIsPressed(i));
    Serial.print(" ");
  }
  Serial.println("");
}

void MainMenu()
{
  if (SwitchState() != oldSwitchState)
  {
    //Left
    if (SwitchState() == 1)
    {
      if (menu > 0)
      {
        menu--;
      }
    }
    //Right
    else if (SwitchState() == 2)
    {
      if (menu < 2)
      {
        menu++;
      }
    }
    oldSwitchState = SwitchState();
  }

  if (menu == 0) DebugMode();
  else if (menu == 1) TranscribeMode();
  else if (menu == 2) MidiChannelMode();
}

//Main

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
  if (digitalRead(side) == HIGH)
  {
    PlayMode();
  }
  else
  {
    MainMenu();
  }
}