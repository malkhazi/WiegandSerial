#ifndef _WIEGAND_H
#define _WIEGAND_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif



class WIEGAND {

public:
	WIEGAND();
	void begin();
	
	bool available();
	unsigned long getCode();
	int getWiegandType();
	
	int D0PinA ;
	int D1PinA ;

	
	
private:
	static void ReadD0A();
	static void ReadD1A();

	static bool DoWiegandConversion ();
	static unsigned long GetCardId (unsigned long *codehigh, unsigned long *codelow, char bitlength);
	
	static unsigned long 	_sysTick;
	static unsigned long 	_lastWiegand;
	
	static unsigned long 	_cardTempHighA;
	static unsigned long 	_cardTempA;
	static int				_bitCountA;	
	static int				_wiegandTypeA;
	static unsigned long	_codeA;

};

#endif
