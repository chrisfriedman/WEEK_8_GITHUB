#include <EEPROM.h>

//Week 6 Digital Lab: Drum Sequencer
//Chris Friedman

boolean stepState[3][4] =
{
  { false, false, false, false },
  { false, false, false, false },
  { false, false, false, false }
};
int buttonPin[4] = { 36, 35, 34, 33 };
boolean lastButtonState[4] = { LOW, LOW, LOW, LOW };
boolean buttonState[4] = { LOW, LOW, LOW, LOW };
int ledPin[4] = { 7, 8, 9, 10 };
int tempo = 17;
int currentStep = 0;
unsigned long lastStepTime = 0;
int i = 0;
int j = 0;
int p = 0;
int note[3][4] =
{
  { 46, 46, 46, 46 },
  { 58, 58, 58, 58 },
  { 66, 66, 66, 66 }
};
int bankLeds[3] = { 15, 14, 13 };
int bankButtonForward = 32;
int bankButtonBackward = 31;
boolean bankButtonForwardState = LOW;
boolean lastBankButtonForwardState = LOW;
boolean bankButtonBackwardState = LOW;
boolean lastBankButtonBackwardState = LOW;
int currentSequence = 0;


void setup()
{
  //Serial.begin(9600);
  for (i = 0; i < 4; i++)
  {
    pinMode(buttonPin[i], INPUT);
    pinMode(ledPin[i], OUTPUT);
  }
  for (i = 0; i < 3; i++)
  {
    pinMode(bankLeds[i], OUTPUT);
  }
  pinMode(bankButtonForward, INPUT);
  pinMode(bankButtonBackward, INPUT);

  loadFromEEPROM();
}

void loop()
{
  checkSequenceButtons();
  checkButtons();
  sequence();
  setLeds();
}

void checkButtons()
{
  for (i = 0; i < 4; i++)
  {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW)
    {
      if (stepState[currentSequence][i] == false)
      {
        stepState[currentSequence][i] = true;
      }
      else if (stepState[currentSequence][i] == true)
      {
        stepState[currentSequence][i] = false;
      }
      saveToEEPROM();
    }
  }
}

void checkSequenceButtons()
{
  digitalWrite(bankLeds[currentSequence], HIGH);

  lastBankButtonForwardState = bankButtonForwardState;
  bankButtonForwardState = digitalRead(bankButtonForward);
  lastBankButtonBackwardState = bankButtonBackwardState;
  bankButtonBackwardState = digitalRead(bankButtonBackward);

  if (bankButtonForwardState == HIGH && lastBankButtonForwardState == LOW)
  {
    sequenceCounterForward();
  }
  else if (bankButtonBackwardState == HIGH && lastBankButtonBackwardState == LOW)
  {
    sequenceCounterBackward();
  }
}

void setLeds()
{
  for (i = 0; i < 4; i++)
  {
    if (i == currentStep)
    {
      analogWrite(ledPin[i], 255);
    }
    else if (stepState[currentSequence][i] == true)
    {
      analogWrite(ledPin[i], 25);
    }
    else
    {
      analogWrite(ledPin[i], 0);
    }
  }
}

void sequence()
{
  tempo = analogRead(A3);

  if (millis() > lastStepTime + tempo)
  {
    currentStep = currentStep + 1;
    if (currentStep > 3)
    {
      currentStep = 0;
    }

    sendMidi();

    lastStepTime = millis();
  }
}

void sendMidi()
{
  for (i = 0; i < 4; i++)
  {
    if (stepState[currentSequence][i] == true && i == currentStep)
    {
      usbMIDI.sendNoteOff(note[currentSequence][i], 0, 1);
      usbMIDI.sendNoteOn(note[currentSequence][i], 127, 1);
    }
  }
}

void sequenceCounterForward()
{
  digitalWrite(bankLeds[currentSequence], LOW);
  currentSequence++;
  if (currentSequence > 2)
  {
    currentSequence = 0;
  }
  digitalWrite(bankLeds[currentSequence], HIGH);
}

void sequenceCounterBackward()
{
  digitalWrite(bankLeds[currentSequence], LOW);
  currentSequence--;
  if (currentSequence < 0)
  {
    currentSequence = 2;
  }
  digitalWrite(bankLeds[currentSequence], HIGH);
}

void saveToEEPROM()
{
  p = 0;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 4; j++)
    {
      EEPROM.write(p, stepState[i][j]);
      p++;
    }
  }

//  Serial.println("New stepState EEPROM Write");
//  for (i = 0; i < 3; i++)
//  {
//    for (j = 0; j < 4; j++)
//    {
//      Serial.println(stepState[i][j]);
//    }
//  }
}

void loadFromEEPROM()
{
  p = 0;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 4; j++)
    {
      stepState[i][j] = EEPROM.read(p);
      p++;
    }
  }
}

