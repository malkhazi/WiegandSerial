#include "Wiegand.h"

#define FALSE 0
#define TRUE  1

WIEGAND wg;

// => rs232 connection
unsigned long rs232_cr_milis = 0;
String rs232txt = "";
boolean rs232_pak_start = false;    // Packet Receive Flag
char rs232_st_ch = '<';             // Packet start marker
char rs232_cl_ch = '>';             // end-of-packet marker
// <= rs232 connection


void setup() {

  Serial.begin(9600);

  // PIN assigment and declaration for Arduino Mega
  //GATE A
  wg.D0PinA = 2;
  wg.D1PinA = 3;
  // Reader enable
  wg.begin();  // wg.begin(GateA , GateB, GateC)
  //  Serial.println("c");
}

void loop() {
  if (wg.available())
  {
    Serial.print("<");
    Serial.print(wg.getCode());
    Serial.println(">");
  }
  Read_srl();
}

void Read_srl()
{
  if ((unsigned long) (millis() - rs232_cr_milis) > 500) {
    rs232_pak_start = false;
    rs232txt = "";
  }
  while (Serial.available()) {          // As long as there is something to read in the buffer
    int bufferChar = Serial.read();     // Reads the current byte
    if (!rs232_pak_start) {             // If no start marker is detected
      if (rs232_st_ch == bufferChar) {  // We check the packet start marker
        rs232_pak_start = true;
        rs232_cr_milis = millis();
      }
    } else {
      // Packet start marker received
      if (rs232_cl_ch == bufferChar) {  // We check the packet completion marker
        paket_damush();
        rs232_pak_start = false;
        rs232txt = "";
      } else {
        rs232txt += (char)bufferChar;
      }
    }
  }
}

void paket_damush() {
  if (rs232txt.length() >= 4) {
    String cmd_in = rs232txt.substring(0, 4);
    String vl_in = "";
    if (rs232txt.length() > 4) {
      vl_in = rs232txt.substring(4);
    }
    if (cmd_in == "chek") {
      Serial.println("<I_AmRFIDScanner>");
    }  else {
      Serial.println("<UCmd" + cmd_in + ">");
    }
  }
}
