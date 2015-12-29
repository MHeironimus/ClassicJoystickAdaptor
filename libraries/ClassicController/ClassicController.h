/*
ClassicController.h - Library for reading a classic controller 
(e.g. ColecoVision, Coleco ADAM, Atari, etc.) and emulating a keyboard,
USB joystict, etc.
Created by Matthew Heironimus, 04/27/2015.
*/
#ifndef ClassicController_h
#define ClassicController_h

#include "Arduino.h"

// Define special keyboard keys not defined in Keyboard library.
#define KEY_NUMPAD_DIVIDE   0xDC
#define KEY_NUMPAD_MULTIPLY 0xDD
#define KEY_NUMPAD_MINUS    0xDE
#define KEY_NUMPAD_PLUS     0xDF
#define KEY_NUMPAD_ENTER    0xE0
#define KEY_NUMPAD_1        0xE1
#define KEY_NUMPAD_2        0xE2
#define KEY_NUMPAD_3        0xE3
#define KEY_NUMPAD_4        0xE4
#define KEY_NUMPAD_5        0xE5
#define KEY_NUMPAD_6        0xE6
#define KEY_NUMPAD_7        0xE7
#define KEY_NUMPAD_8        0xE8
#define KEY_NUMPAD_9        0xE9
#define KEY_NUMPAD_0        0xEA
#define KEY_NUMPAD_DEL      0xEB

// Constants
const int NOT_USED = -1;

class ClassicController
{
private:
	// Controller Pins
	int _firePin;
	int _upPin;
	int _downPin;
	int _leftPin;
	int _rightPin;
	int _keypadModePin;
	int _directionModePin;
	int _bit0Pin;
	int _bit1Pin;
	int _bit2Pin;
	int _bit3Pin;
	int _spinnerAPin;
	int _spinnerBPin;

	// Pin Read Value Counters
	unsigned int _leftFireCount;
	unsigned int _rightFireCount;
	unsigned int _upCount;
	unsigned int _downCount;
	unsigned int _leftCount;
	unsigned int _rightCount;
	unsigned int _bit0Count;
	unsigned int _bit1Count;
	unsigned int _bit2Count;
	unsigned int _bit3Count;

	// Pin Values
	bool _leftFire;
	bool _rightFire;
	bool _up;
	bool _down;
	bool _left;
	bool _right;
	bool _bit0;
	bool _bit1;
	bool _bit2;
	bool _bit3;

	// Special Values
	int _keypadCode;
	char _keypadValue;
	bool _purpleFire;
	bool _blueFire;

	// Previous Pin Values
	bool _lastLeftFire;
	bool _lastRightFire;
	bool _lastUp;
	bool _lastDown;
	bool _lastLeft;
	bool _lastRight;
	bool _lastBit0;
	bool _lastBit1;
	bool _lastBit2;
	bool _lastBit3;

	// Previous Special Values
	int _lastKeypadCode;
	char _lastKeypadValue;
	bool _lastPurpleFire;
	bool _lastBlueFire;

    // Joystick Button Mappings
    int _leftFireButton;
    int _rightFireButton;
    int _purpleFireButton;
    int _blueFireButton;

    // Joystick Button Keyboard Mappings
    char _leftFireKey;
    char _rightFireKey;
    char _purpleFireKey;
    char _blueFireKey;

    // Direction Keyboard Mappings
    char _upKey;
    char _downKey;
    char _leftKey;
    char _rightKey;

    // Keypad Button Mappings
    int _keypadButtons[16] = {
        NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED,
        NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED, NOT_USED
    };

    // Keypad Keyboard Mappings
    char _keypadKeys[16] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

	int _joystickIndex;
	unsigned int _readCount;

    // Mode Variables
    bool _directionJoystickMode;
    bool _fireJoystickMode;
    bool _extraFireJoystickMode;
    bool _keypadJoystickMode;

	// Private: Constants
	const int _cModeDelayMicroseconds = 50;
	const unsigned int _cPinLowThreashold = 5;
	const char _cKeypadCodeTable[16] = {
		0, '6', '1', '3', '9', '0', '*', 0,
		'2', '#', '7', 0, '5', '4', '8', 0 };

	// Private: Methods
	void determineAllPinValues();
	void determineSpecialValues();
    bool setDirectionJoystickState();
    void sendDirectionKeyboardStateToHost();
    bool setFireJoystickState();
    void sendFireKeyboardStateToHost();
    bool setExtraFireJoystickState();
    void sendExtraFireKeyboardStateToHost();
    bool setKeypadJoystickState();
	void sendKeypadKeyboardStateToHost();
    void sendLineStateToHost(bool lastState, bool currentState, char keyUsedForLine);

	inline unsigned int readControllerPin(int pin) { return (digitalRead(pin) == LOW); }
	inline bool determinePinValue(unsigned int pinLowCount) { return (pinLowCount >= _cPinLowThreashold); }

public:
	ClassicController(
		int joystickIndex,
		int pin1Up,
		int pin2Down,
		int pin3Left,
		int pin4Right,
		int pin5KeypadMode,
		int pin6FireButton,
		int pin7SpinnerA,
		int pin8DirectionMode,
		int pin9SpinnerB);

	void startReadController();
	void readControllerKeypad();
    void readControllerDirection();
    void writeToHost();

    // The classic controller's direction values are emulated using the USB Joystick.
    void setDirectionJoystickMode();

    // The classic controller's direction values are emulated using the specified keyboard keys.
    void setDirectionKeyboardMode(char upKey = KEY_UP_ARROW, char downKey = KEY_DOWN_ARROW, char leftKey = KEY_LEFT_ARROW, char rightKey = KEY_RIGHT_ARROW);

    void setFireJoystickMode(int leftFireButton = 0, int rightfireButton = 1);
    void setFireKeyboardMode(char leftFireKey = KEY_LEFT_ALT, char rightFireKey = KEY_LEFT_CTRL);
    void setExtraFireJoystickMode(int purpleFireButton = 2, int blueFireButton = 3);
    void setExtraFireKeyboardMode(char purpleFireKey = KEY_NUMPAD_MINUS, char blueFireKey = KEY_NUMPAD_PLUS);
    void setKeypadJoystickMode(int oneButton = 4, int twoButton = 5, int threeButton = 6,
        int fourButton = 7, int fiveButton = 8, int sixButton = 9,
        int sevenButton = 10, int eightButton = 11, int nineButton = 12,
        int asteriskButton = 13, int zeroButton = 14, int poundButton = 15);
    void setKeypadKeyboardMode(char oneKey = KEY_NUMPAD_1, char twoKey = KEY_NUMPAD_2, char threeKey = KEY_NUMPAD_3,
        char fourKey = KEY_NUMPAD_4, char fiveKey = KEY_NUMPAD_5, char sixKey = KEY_NUMPAD_6,
        char sevenKey = KEY_NUMPAD_7, char eightKey = KEY_NUMPAD_8, char nineKey = KEY_NUMPAD_9,
        char asteriskKey = KEY_NUMPAD_DEL, char zeroKey = KEY_NUMPAD_0, char poundKey = KEY_NUMPAD_ENTER);

	inline bool getUp() { return _lastUp; }
	inline bool getDown() { return _lastDown; }
	inline bool getLeft() { return _lastLeft; }
	inline bool getRight() { return _lastRight; }
	inline bool getLeftFire() { return _lastLeftFire; }
	inline bool getRightFire() { return _lastRightFire; }
	inline bool getPurpleFire() { return _lastPurpleFire; }
	inline bool getBlueFire() { return _lastBlueFire; }
	inline char getKeypad() { return _cKeypadCodeTable[_lastKeypadCode]; }
};

#endif
