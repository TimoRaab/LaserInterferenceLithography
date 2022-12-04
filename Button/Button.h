/*
    Buttons class for momentary buttons (not switches).
    Created by Timo Raab, April 3, 2018.
    v0.1
*/

#ifndef Button_h
#define Button_h

#include "Arduino.h"

class Button {

    private:
        unsigned char _pin;             // Arduino Pin
        bool _pullUp;                   // Use of internal pull up resistor,
											// if not pull down is assumend,
											// standard: true
        bool _execAtRelease;            // define function execution time
											// (at button press or release)
											// standard: true (release)
        unsigned int _debounceTime;     // software debounce 
											// standard: 20 ms
        void (*_bFunc)();          		// function call at button press
											// standard: do nothing
		bool _await;

    public:
		// constructor
        Button(unsigned char pin);
        Button(unsigned char pin, bool pullUp);
        Button(unsigned char pin, bool pullUp, bool execAtRelease, unsigned int debounceTime, void (*bFunction)());
		Button(unsigned char pin, bool pullUp, bool execAtRelease, unsigned int debounceTime, void (*bFunction)(), bool await);

		// set Methods
		bool setPullUp(bool pullUp);
        bool setExecAtRelease(bool execAtRelease);
        bool setDebounceTime(unsigned int debounceTime);
        bool setFunction(void (*bFunction)());
		
		// detect if a button is pressed
		// par: if function call should be executed
		// ret:	if button is pressed 
        bool isPressed(bool execute);

};
// function for internal use only. Standard function call for library
void nullFunction();

#endif

//EOF