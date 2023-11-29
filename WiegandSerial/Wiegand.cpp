#include "Wiegand.h"

unsigned long WIEGAND::_sysTick = 0;
unsigned long WIEGAND::_lastWiegand = 0;

unsigned long WIEGAND::_cardTempHighA = 0;
unsigned long WIEGAND::_cardTempA = 0;
unsigned long WIEGAND::_codeA = 0;
int 		  WIEGAND::_bitCountA = 0;
int			  WIEGAND::_wiegandTypeA = 0;


WIEGAND::WIEGAND()
{

}


unsigned long WIEGAND::getCode()
{
  return _codeA;
}

int WIEGAND::getWiegandType()
{
  return _wiegandTypeA;
}

bool WIEGAND::available()
{
  return DoWiegandConversion();
}

void WIEGAND::begin()
{
  _sysTick = millis();
  _lastWiegand = 0;

  _cardTempHighA = 0;
  _cardTempA = 0;
  _codeA = 0;
  _wiegandTypeA = 0;
  _bitCountA = 0;

  pinMode(D0PinA, INPUT);					// Set D0 pin as input
  pinMode(D1PinA, INPUT);					// Set D1 pin as input
  attachInterrupt(digitalPinToInterrupt(D0PinA), ReadD0A, FALLING);	// Hardware interrupt - high to low pulse
  attachInterrupt(digitalPinToInterrupt(D1PinA), ReadD1A, FALLING);	// Hardware interrupt - high to low pulse
//  Serial.println("Start 2");
}

void WIEGAND::ReadD0A ()
{
  _bitCountA++;				// Increament bit count for Interrupt connected to D0
  if (_bitCountA > 31)			// If bit count more than 31, process high bits
  {
    _cardTempHighA |= ((0x80000000 & _cardTempA) >> 31);	//	shift value to high bits
    _cardTempHighA <<= 1;
    _cardTempA <<= 1;
  }
  else
  {
    _cardTempA <<= 1;		// D0 represent binary 0, so just left shift card data
  }
  _lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}

void WIEGAND::ReadD1A()
{
  _bitCountA ++;				// Increment bit count for Interrupt connected to D1
  if (_bitCountA > 31)			// If bit count more than 31, process high bits
  {
    _cardTempHighA |= ((0x80000000 & _cardTempA) >> 31);	// shift value to high bits
    _cardTempHighA <<= 1;
    _cardTempA |= 1;
    _cardTempA <<= 1;
  }
  else
  {
    _cardTempA |= 1;			// D1 represent binary 1, so OR card data with 1 then
    _cardTempA <<= 1;		// left shift card data
  }
  _lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}


unsigned long WIEGAND::GetCardId (unsigned long *codehigh, unsigned long *codelow, char bitlength)
{
  unsigned long cardID = 0;

  if (bitlength == 26)								// EM tag
    cardID = (*codelow & 0x1FFFFFE) >> 1;

  if (bitlength == 34)								// Mifare
  {
    *codehigh = *codehigh & 0x03;				// only need the 2 LSB of the codehigh
    *codehigh <<= 30;							// shift 2 LSB to MSB
    *codelow >>= 1;
    cardID = *codehigh | *codelow;
  }
  return cardID;
}

bool WIEGAND::DoWiegandConversion ()
{
  unsigned long cardIDA;

  _sysTick = millis();
  if ((_sysTick - _lastWiegand) > 25)								// if no more signal coming through after 25ms
  {

    if ((_bitCountA == 26) || (_bitCountA == 34) || (_bitCountA == 8)) 	// bitCount for keypress=8, Wiegand 26=26, Wiegand 34=34
    {
      _cardTempA >>= 1;			// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
      if (_bitCountA > 32)			// bit count more than 32 bits, shift high bits right to make adjustment
        _cardTempHighA >>= 1;

      if ((_bitCountA == 26) || (_bitCountA == 34))		// wiegand 26 or wiegand 34
      {
        cardIDA = GetCardId (&_cardTempHighA, &_cardTempA, _bitCountA);
        _wiegandTypeA = _bitCountA;
        _bitCountA = 0;
        _cardTempA = 0;
        _cardTempHighA = 0;
        _codeA = cardIDA;
        return true;
      }
      else if (_bitCountA == 8)		// keypress wiegand
      {
        // 8-bit Wiegand keyboard data, high nibble is the "NOT" of low nibble
        // eg if key 1 pressed, data=E1 in binary 11100001 , high nibble=1110 , low nibble = 0001
        char highNibble = (_cardTempA & 0xf0) >> 4;
        char lowNibble = (_cardTempA & 0x0f);
        _wiegandTypeA = _bitCountA;
        _bitCountA = 0;
        _cardTempA = 0;
        _cardTempHighA = 0;

        if (lowNibble == (~highNibble & 0x0f))		// check if low nibble matches the "NOT" of high nibble.
        {
          if (lowNibble == 0x0b)					// ENT pressed
          {
            _codeA = 0x0d;
          }
          else if (lowNibble == 0x0a)				// ESC pressed
          {
            _codeA = 0x1b;
          }
          else
          {
            _codeA = (int)lowNibble;				// 0 - 9 keys
          }
          return true;
        }
      }
    }
    else
    {
      // well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
      _lastWiegand = _sysTick;
      _bitCountA = 0;
      _cardTempA = 0;
      _cardTempHighA = 0;
    }
    // fine controllo accesso A

    return false;
  }
  else
    return false;
}
