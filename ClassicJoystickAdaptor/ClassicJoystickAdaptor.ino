// Ultimate Classic Game Console Joystick USB Adaptor
// Supports ColecoVision / ADAM Joysticks and Atari 2600 Joysticks.
// For the Arduino Leonardo or Arduino Micro
// 2015-05-23
//----------------------------------------------------------------------------------

#include <ClassicController.h>

#define JOYSTICK_COUNT 3

// Emulator Modes
const int gcModeAdamEm = 0;
const int gcModeMame = 1;
const int gcModeJoystickOnly = 2;
const int gcModeMax = 2;
int gCurrentMode;

// Emulator Mode Toggle Pin
const int gcModePin = 9;

// Joystick Pins
const int gcFirePin = 6;
const int gcUpPin = 2;
const int gcDownPin = 3;
const int gcLeftPin = 4;
const int gcRightPin = 5;
const int gcSpinnerAPin = 7;
const int gcSpinnerBPin = 8;
const int gcModeAPin[JOYSTICK_COUNT] = { A0, A2, A4 };
const int gcModeBPin[JOYSTICK_COUNT] = { A1, A3, A5 };

// Monitor Pins
const int gcFireMonitorPin = 13;
const int gcJoystickOnlyModePin = 12;
const int gcMameModePin = 11;
const int gcAdamEmModePin = 10;

// Configure Classic Controller
ClassicController gController[JOYSTICK_COUNT] = {
  ClassicController(0,
    gcUpPin,
    gcDownPin,
    gcLeftPin,
    gcRightPin,
    gcModeAPin[0],
    gcFirePin,
    gcSpinnerAPin,
    gcModeBPin[0],
    gcSpinnerBPin),
  ClassicController(1,
    gcUpPin,
    gcDownPin,
    gcLeftPin,
    gcRightPin,
    gcModeAPin[1],
    gcFirePin,
    gcSpinnerAPin,
    gcModeBPin[1],
    gcSpinnerBPin),
  ClassicController(2,
    gcUpPin,
    gcDownPin,
    gcLeftPin,
    gcRightPin,
    gcModeAPin[2],
    gcFirePin,
    gcSpinnerAPin,
    gcModeBPin[2],
    gcSpinnerBPin)
};

// Frame Variables
const int gcFrameLength = 10;
unsigned long gNextFrameStart = 0;
unsigned int gLoopsPerFrame = 0;

// Shows the status of the joystick.
void ShowJoystickStatus()
{

    // Debug Information
    //Serial.print("Loops per Frame: ");
    //Serial.print(gLoopsPerFrame);  
    //Serial.println();
    //Serial.print("Keypad #3: ");
    //Serial.println(gController[2].getKeypad());

    digitalWrite(gcFireMonitorPin, gController[0].getLeftFire());
}

void DisplayEmulatorMode(int modeToDisplay)
{
    gCurrentMode = modeToDisplay;
    digitalWrite(gcJoystickOnlyModePin, (modeToDisplay == gcModeJoystickOnly));
    digitalWrite(gcAdamEmModePin, (modeToDisplay == gcModeAdamEm));
    digitalWrite(gcMameModePin, (modeToDisplay == gcModeMame));
}

void SetupJoystickOnlyMode()
{
    for (int index = 0; index < JOYSTICK_COUNT; index++)
    {
        gController[index].setDirectionJoystickMode();
        gController[index].setFireJoystickMode();
        gController[index].setExtraFireJoystickMode();
        gController[index].setKeypadJoystickMode();
    }
    DisplayEmulatorMode(gcModeJoystickOnly);
}

void SetupAdamEmMode()
{
    // Joystick 1 Uses Joystick
    gController[0].setDirectionJoystickMode();
    gController[0].setFireJoystickMode();
    gController[0].setExtraFireKeyboardMode();
    gController[0].setKeypadKeyboardMode();

    // Joystick 2 Uses Keyboard
    gController[1].setDirectionKeyboardMode();
    gController[1].setFireKeyboardMode();
    gController[1].setExtraFireKeyboardMode(KEY_LEFT_SHIFT, 'z');
    gController[1].setKeypadKeyboardMode('1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '0', '=');

    // Joystick 3 Not Used
    gController[2].setDirectionJoystickMode();
    gController[2].setFireJoystickMode();
    gController[2].setExtraFireJoystickMode();
    gController[2].setKeypadJoystickMode();

    DisplayEmulatorMode(gcModeAdamEm);
}

void SetupMameMode()
{
    // Joystick 1 (* = Pause, # = Quit)
    gController[0].setDirectionJoystickMode();
    gController[0].setFireJoystickMode();
    gController[0].setExtraFireJoystickMode();
    gController[0].setKeypadKeyboardMode('1', '2', '3', '4', '5', '6', '7', '8', '9', 'P', '0', KEY_ESC);

    // Joystick 2 (* = Screen Capture, # = Mame Menu)
    gController[1].setDirectionJoystickMode();
    gController[1].setFireJoystickMode();
    gController[1].setExtraFireJoystickMode();
    gController[1].setKeypadKeyboardMode('1', '2', '3', '4', '5', '6', '7', '8', '9', KEY_F12, '0', KEY_TAB);

    // Joystick 3 (* = Volume Control, # = Game Reset)
    gController[2].setDirectionJoystickMode();
    gController[2].setFireJoystickMode();
    gController[2].setExtraFireJoystickMode();
    gController[2].setKeypadKeyboardMode('1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#');

    DisplayEmulatorMode(gcModeMame);
}

void ReadController(int joystickIndex)
{
    const int cLineDelay = 50;

    // Put Joystick in Direction Mode
    digitalWrite(gcModeAPin[joystickIndex], HIGH);
    digitalWrite(gcModeBPin[joystickIndex], LOW);
    delayMicroseconds(cLineDelay);
    gController[joystickIndex].readControllerDirection();

    // Put Joystick in Keypad Mode
    digitalWrite(gcModeAPin[joystickIndex], LOW);
    digitalWrite(gcModeBPin[joystickIndex], HIGH);
    delayMicroseconds(cLineDelay);
    gController[joystickIndex].readControllerKeypad();

    // Turn off Joystick
    digitalWrite(gcModeAPin[joystickIndex], HIGH);
}

void CheckForModeChange()
{
    // Change Mode Button is not down, so just return.
    if (digitalRead(gcModePin)) return;

    digitalWrite(gcJoystickOnlyModePin, LOW);
    digitalWrite(gcAdamEmModePin, LOW);
    digitalWrite(gcMameModePin, LOW);

    do
    {
        delay(100);
    } while (!digitalRead(gcModePin));

    // Reset Output
    Keyboard.releaseAll();

    // Go to next mode
    gCurrentMode++;
    if (gCurrentMode > gcModeMax)
    {
        gCurrentMode = 0;
    }

    switch (gCurrentMode)
    {
    case gcModeAdamEm:
        SetupAdamEmMode();
        break;
    case gcModeJoystickOnly:
        SetupJoystickOnlyMode();
        break;
    case gcModeMame:
        SetupMameMode();
        break;
    }
}

void setup() {
    // Setup Serial Monitor
    //Serial.begin(19200);

    // Setup Keyboard
    Keyboard.begin();

    // Setup Joystick Pins
    pinMode(gcFirePin, INPUT_PULLUP);
    pinMode(gcUpPin, INPUT_PULLUP);
    pinMode(gcDownPin, INPUT_PULLUP);
    pinMode(gcLeftPin, INPUT_PULLUP);
    pinMode(gcRightPin, INPUT_PULLUP);
    pinMode(gcSpinnerAPin, INPUT_PULLUP);
    pinMode(gcSpinnerBPin, INPUT_PULLUP);

    for (int index = 0; index < JOYSTICK_COUNT; index++)
    {
        pinMode(gcModeAPin[index], OUTPUT);
        pinMode(gcModeBPin[index], OUTPUT);
    }

    // Setup Mode Pin
    pinMode(gcModePin, INPUT_PULLUP);

    // Setup Output Monitor Pins
    pinMode(gcFireMonitorPin, OUTPUT);
    pinMode(gcAdamEmModePin, OUTPUT);
    pinMode(gcMameModePin, OUTPUT);
    pinMode(gcJoystickOnlyModePin, OUTPUT);

    // Default Mode
    SetupJoystickOnlyMode();
    //SetupAdamEmMode();
    //SetupMameMode();
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime >= gNextFrameStart)
    {
        // Send Values to Host
        for (int index = 0; index < JOYSTICK_COUNT; index++)
        {
            gController[index].writeToHost();
        }

        // Send Values to Monitor
        ShowJoystickStatus();

        // Check to see if user has requested mode change
        CheckForModeChange();

        // Reset Frame Variables
        for (int index = 0; index < JOYSTICK_COUNT; index++)
        {
            gController[index].startReadController();
        }
        gLoopsPerFrame = 0;

        // Time to start next frame
        gNextFrameStart = currentTime + gcFrameLength;
    }
    else
    {
        // Check and store the value of the classic controller.
        for (int index = 0; index < JOYSTICK_COUNT; index++)
        {
            ReadController(index);
        }
        gLoopsPerFrame++;
    }
}
