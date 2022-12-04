/*
    Buttons class for momentary buttons (not switches).
    Created by Timo Raab, April 3, 2018.
    v0.1
*/

#include "Arduino.h"
#include "Button.h"


// Constructors
Button::Button(unsigned char pin, bool pullUp, bool execAtRelease, 
            unsigned int debounceTime, void (*bFunction)(), bool await) {
                _pin = pin;
                _pullUp = pullUp;
                _execAtRelease = execAtRelease;
                _debounceTime = debounceTime;
                _bFunc = bFunction;
				
				if (_pullUp) {
					pinMode(_pin, INPUT_PULLUP);
				} else {
					pinMode(_pin, INPUT);
				}
				
				_await = await;
            }

Button::Button(unsigned char pin, bool pullUp, bool execAtRelease, 
            unsigned int debounceTime, void (*bFunction)()) {
                _pin = pin;
                _pullUp = pullUp;
                _execAtRelease = execAtRelease;
                _debounceTime = debounceTime;
                _bFunc = bFunction;
				_await = true;
				
				if (_pullUp) {
					pinMode(_pin, INPUT_PULLUP);
				} else {
					pinMode(_pin, INPUT);
				}
				_await = true;
            }


Button::Button(unsigned char pin, bool pullUp) {
    _pin = pin;
	_pullUp = pullUp;
	_execAtRelease = true;
	_debounceTime = 20;
	_bFunc = nullFunction;
	
	if (_pullUp) {
		pinMode(_pin, INPUT_PULLUP);
	} else {
		pinMode(_pin, INPUT);
	}
	_await = true;
}

Button::Button(unsigned char pin) {
	_pin = pin;
	_pullUp = true;
	_execAtRelease = true;
	_debounceTime = 20;
	_bFunc = nullFunction;
	
	if (_pullUp) {
		pinMode(_pin, INPUT_PULLUP);
	} else {
		pinMode(_pin, INPUT);
	}
	_await = true;
}

// set Methods
bool Button::setPullUp(bool pullUp) {
    _pullUp = pullUp;
    return true;
}

bool Button::setExecAtRelease(bool execAtRelease) {
    _execAtRelease = execAtRelease;
    return true;
}

bool Button::setDebounceTime(unsigned int debounceTime) {
    _debounceTime = debounceTime;
    return true;
}

bool Button::setFunction(void (*bFunction)()) {
    _bFunc = bFunction;
    return true;
}

// Methods
bool Button::isPressed(bool execute) {
    if (_pullUp) {
        if (digitalRead(_pin) == LOW) {
			delay(_debounceTime);
			if (_execAtRelease) {
				if (_await) {
					while (digitalRead(_pin) == LOW); //wait for button release	
				}
				if (execute) {
					_bFunc();
				}
			} else {
				if (execute) {
					_bFunc();
				}
				if (_await) {
					while (digitalRead(_pin) == LOW); // wait for button release
				}
			}
            return true;
        }
    } else {
        if (digitalRead(_pin) == HIGH) {
			delay(_debounceTime);
			if (_execAtRelease) {
				if (_await) {
					while (digitalRead(_pin) == HIGH); //wait for button release
				}
				if (execute) {
					_bFunc();
				}
			} else {
				if (execute) {
					_bFunc();
				}
				if (_await) {
					while (digitalRead(_pin) == HIGH); // wait for button release
				}
			}
		}
		return true;
    }
    return false;
}

// other methods
void nullFunction(){
	//Do Nothing
};


//EOF















