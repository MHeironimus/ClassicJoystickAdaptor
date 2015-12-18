/*
ClassicController.h - Library for reading a classic controller
(e.g. ColecoVision, Coleco ADAM, Atari, etc.) and emulating a keyboard,
USB joystict, etc.
Created by Matthew Heironimus, 04/25/2015.
*/

#include "ClassicController.h"

ClassicController::ClassicController(
	int joystickIndex,
	int pin1Up,
	int pin2Down,
	int pin3Left,
	int pin4Right,
	int pin5KeypadMode,
	int pin6FireButton,
	int pin7SpinnerA,
	int pin8DirectionMode,
	int pin9SpinnerB)
{
	// Setup Pins
	_firePin = pin6FireButton;
	_upPin = pin1Up;
	_downPin = pin2Down;
	_leftPin = pin3Left;
	_rightPin = pin4Right;
	_keypadModePin = pin5KeypadMode;
	_directionModePin = pin8DirectionMode;
	_bit0Pin = pin1Up;
	_bit1Pin = pin4Right;
	_bit2Pin = pin2Down;
	_bit3Pin = pin3Left;
	_spinnerAPin = pin7SpinnerA;
	_spinnerBPin = pin9SpinnerB;

	// Initalize USB Joystick
	_joystickIndex = joystickIndex;
	Joystick[_joystickIndex].begin(false);

	// Reset the Previous and Current State
	startReadController();
	startReadController();

    // Set default Modes
    setDirectionJoystickMode();
    setFireJoystickMode();
    setExtraFireJoystickMode();
    setKeypadJoystickMode();
}

void ClassicController::startReadController()
{
	// Copy Current State to Last State
	_lastLeftFire = _leftFire;
	_lastRightFire = _rightFire;
	_lastUp = _up;
	_lastDown = _down;
	_lastLeft = _left;
	_lastRight = _right;
	_lastPurpleFire = _purpleFire;
	_lastBlueFire = _blueFire;
	_lastKeypadValue = _keypadValue;
	_lastKeypadCode = _keypadCode;

	// Reset Frame Loop Counter
	_readCount = 0;

	// Reset Pin Read Value Counters
	_leftFireCount = 0;
	_rightFireCount = 0;
	_upCount = 0;
	_downCount = 0;
	_leftCount = 0;
	_rightCount = 0;
	_bit0Count = 0;
	_bit1Count = 0;
	_bit2Count = 0;
	_bit3Count = 0;

	// Reset Current State
	_leftFire = 0;
	_rightFire = 0;
	_up = 0;
	_down = 0;
	_left = 0;
	_right = 0;
	_keypadValue = 0;
	_keypadCode = 0;
	_purpleFire = 0;
	_blueFire = 0;
	_bit0 = 0;
	_bit1 = 0;
	_bit2 = 0;
	_bit3 = 0;
}

void ClassicController::readControllerDirection()
{
	_leftFireCount += readControllerPin(_firePin);
	_upCount += readControllerPin(_upPin);
	_downCount += readControllerPin(_downPin);
	_leftCount += readControllerPin(_leftPin);
	_rightCount += readControllerPin(_rightPin);
}

void ClassicController::readControllerKeypad()
{
    _readCount++;
    _rightFireCount += readControllerPin(_firePin);
    _bit0Count += readControllerPin(_bit0Pin);
    _bit1Count += readControllerPin(_bit1Pin);
    _bit2Count += readControllerPin(_bit2Pin);
    _bit3Count += readControllerPin(_bit3Pin);
}

void ClassicController::writeToHost()
{
    bool joystickUpdateNeeded = false;

	determineAllPinValues();
	determineSpecialValues();

    // Send Joystick State To Host
    if (_directionJoystickMode)
    {
        joystickUpdateNeeded |= setDirectionJoystickState();
    }
    else
    {
        sendDirectionKeyboardStateToHost();
    }

    if (_fireJoystickMode)
    {
        joystickUpdateNeeded |= setFireJoystickState();
    }
    else
    {
        sendFireKeyboardStateToHost();
    }

    if (_extraFireJoystickMode)
    {
        joystickUpdateNeeded |= setExtraFireJoystickState();
    }
    else
    {
        sendExtraFireKeyboardStateToHost();
    }

    if (_keypadJoystickMode)
    {
        joystickUpdateNeeded |= setKeypadJoystickState();
    }
    else
    {
        sendKeypadKeyboardStateToHost();
    }

    if (joystickUpdateNeeded)
    {
        Joystick[_joystickIndex].sendState();
    }
}

void ClassicController::determineAllPinValues()
{
	_leftFire = determinePinValue(_leftFireCount);
	_rightFire = determinePinValue(_rightFireCount);

	_up = determinePinValue(_upCount);
	_down = determinePinValue(_downCount);
	_left = determinePinValue(_leftCount);
	_right = determinePinValue(_rightCount);

	_bit0 = determinePinValue(_bit0Count);
	_bit1 = determinePinValue(_bit1Count);
	_bit2 = determinePinValue(_bit2Count);
	_bit3 = determinePinValue(_bit3Count);
}

void ClassicController::determineSpecialValues()
{
	_keypadCode = (_bit3 << 3) + (_bit2 << 2) + (_bit1 << 1) + _bit0;

	// Check for SuperAction Controller Buttons.
	if (_keypadCode == 7)
	{
		_purpleFire = 1;
	}
	if (_keypadCode == 11)
	{
		_blueFire = 1;
	}
}

void ClassicController::sendKeypadKeyboardStateToHost()
{
    _keypadValue = _keypadKeys[_keypadCode];

	if ((_lastKeypadValue != 0) && (_lastKeypadValue != _keypadValue))
	{
		Keyboard.release(_lastKeypadValue);
	}
	if ((_keypadValue != 0) && (_lastKeypadValue != _keypadValue))
	{
		Keyboard.press(_keypadValue);
	}
}

bool ClassicController::setDirectionJoystickState()
{
	const byte UP = -127;
	const byte DOWN = 127;
	const byte LEFT = -127;
	const byte RIGHT = 127;

	// Check to see if state needs to be updated
    if ((_up == _lastUp)
		&& (_down == _lastDown)
		&& (_left == _lastLeft)
		&& (_right == _lastRight))
	{
		return false;
	}

	if (_up)
	{
		Joystick[_joystickIndex].setYAxis(UP);
	}
	else if (_down)
	{
		Joystick[_joystickIndex].setYAxis(DOWN);
	}
	else
	{
		Joystick[_joystickIndex].setYAxis(0);
	}

	if (_left)
	{
		Joystick[_joystickIndex].setXAxis(LEFT);
	}
	else if (_right)
	{
		Joystick[_joystickIndex].setXAxis(RIGHT);
	}
	else
	{
		Joystick[_joystickIndex].setXAxis(0);
	}

    return true;
}

void ClassicController::sendDirectionKeyboardStateToHost()
{
    sendLineStateToHost(_lastUp, _up, _upKey);
    sendLineStateToHost(_lastDown, _down, _downKey);
    sendLineStateToHost(_lastLeft, _left, _leftKey);
    sendLineStateToHost(_lastRight, _right, _rightKey);
}

bool ClassicController::setFireJoystickState()
{
    // Check to see if state needs to be updated
    if ((_leftFire == _lastLeftFire)
        && (_rightFire == _lastRightFire))
    {
        return false;
    }

    Joystick[_joystickIndex].setButton(_leftFireButton, _leftFire);
    Joystick[_joystickIndex].setButton(_rightFireButton, _rightFire);

    return true;
}

void ClassicController::sendFireKeyboardStateToHost()
{
    sendLineStateToHost(_lastLeftFire, _leftFire, _leftFireKey);
    sendLineStateToHost(_lastRightFire, _rightFire, _rightFireKey);
}

bool ClassicController::setExtraFireJoystickState()
{
    // Check to see if state needs to be updated
    if ((_purpleFire == _lastPurpleFire)
        && (_blueFire == _lastBlueFire))
    {
        return false;
    }

    Joystick[_joystickIndex].setButton(_purpleFireButton, _purpleFire);
    Joystick[_joystickIndex].setButton(_blueFireButton, _blueFire);

    return true;
}

void ClassicController::sendExtraFireKeyboardStateToHost()
{
    sendLineStateToHost(_lastPurpleFire, _purpleFire, _purpleFireKey);
    sendLineStateToHost(_lastBlueFire, _blueFire, _blueFireKey);
}

bool ClassicController::setKeypadJoystickState()
{
    // Check to see if state needs to be updated
    if (_keypadCode == _lastKeypadCode)
    {
        return false;
    }

    int lastButtonPressed = _keypadButtons[_lastKeypadCode];

    if (lastButtonPressed != NOT_USED)
    {
        Joystick[_joystickIndex].setButton(lastButtonPressed, LOW);
    }

    int currentButtonPressed = _keypadButtons[_keypadCode];
    if (currentButtonPressed != NOT_USED)
    {
        Joystick[_joystickIndex].setButton(currentButtonPressed, HIGH);
    }

    return true;
}

void ClassicController::setDirectionJoystickMode()
{
    _directionJoystickMode = true;
}

void ClassicController::setDirectionKeyboardMode(char upKey, char downKey, char leftKey, char rightKey)
{
    _directionJoystickMode = false;
    _upKey = upKey;
    _downKey = downKey;
    _leftKey = leftKey;
    _rightKey = rightKey;
}

void ClassicController::setFireJoystickMode(int leftFireButton, int rightFireButton)
{
    _fireJoystickMode = true;
    _leftFireButton = leftFireButton;
    _rightFireButton = rightFireButton;
}

void ClassicController::setExtraFireJoystickMode(int purpleFireButton, int blueFireButton)
{
    _extraFireJoystickMode = true;
    _purpleFireButton = purpleFireButton;
    _blueFireButton = blueFireButton;
}

void ClassicController::setFireKeyboardMode(char leftFireKey, char rightFireKey)
{
    _fireJoystickMode = false;
    _leftFireKey = leftFireKey;
    _rightFireKey = rightFireKey;
}

void ClassicController::setExtraFireKeyboardMode(char purpleFireKey, char blueFireKey)
{
    _extraFireJoystickMode = false;
    _purpleFireKey = purpleFireKey;
    _blueFireKey = blueFireKey;
}

void ClassicController::setKeypadJoystickMode(int oneButton, int twoButton, int threeButton,
    int fourButton, int fiveButton, int sixButton,
    int sevenButton, int eightButton, int nineButton,
    int asteriskButton, int zeroButton, int poundButton)
{
    _keypadJoystickMode = true;

    _keypadButtons[1] = sixButton;
    _keypadButtons[2] = oneButton;
    _keypadButtons[3] = threeButton;
    _keypadButtons[4] = nineButton;
    _keypadButtons[5] = zeroButton;
    _keypadButtons[6] = asteriskButton;
    _keypadButtons[8] = twoButton;
    _keypadButtons[9] = poundButton;
    _keypadButtons[10] = sevenButton;
    _keypadButtons[12] = fiveButton;
    _keypadButtons[13] = fourButton;
    _keypadButtons[14] = eightButton;
}

void ClassicController::setKeypadKeyboardMode(char oneKey, char twoKey, char threeKey,
    char fourKey, char fiveKey, char sixKey,
    char sevenKey, char eightKey, char nineKey,
    char asteriskKey, char zeroKey, char poundKey)
{
    _keypadJoystickMode = false;

    _keypadKeys[1] = sixKey;
    _keypadKeys[2] = oneKey;
    _keypadKeys[3] = threeKey;
    _keypadKeys[4] = nineKey;
    _keypadKeys[5] = zeroKey;
    _keypadKeys[6] = asteriskKey;
    _keypadKeys[8] = twoKey;
    _keypadKeys[9] = poundKey;
    _keypadKeys[10] = sevenKey;
    _keypadKeys[12] = fiveKey;
    _keypadKeys[13] = fourKey;
    _keypadKeys[14] = eightKey;
}

void ClassicController::sendLineStateToHost(bool lastState, bool currentState, char keyUsedForLine)
{
    if ((lastState == true) && (currentState == false))
    {
        Keyboard.release(keyUsedForLine);
    }
    if ((lastState == false) && (currentState == true))
    {
        Keyboard.press(keyUsedForLine);
    }
}
