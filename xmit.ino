#include "config.h"
  
int dataPin = 12;
int high = HIGH;
int low = LOW;

boolean configured = false;
unsigned int zeroOn;
unsigned int zeroOff;
unsigned int oneOn;
unsigned int oneOff;

unsigned int startOfRowOn;
unsigned int startOfRowOff;

unsigned int endOfRowOff;

unsigned int endOfMsgOff;

    void
setupXmitDefaults()
{
    // Set defaults
    dataPin = 12;
    
    zeroOn = 700;
    zeroOff = 340;
    oneOn = 340;
    oneOff = 700;

    startOfRowOn = 4650;
    startOfRowOff = 1480;

    endOfRowOff = 9000;

    endOfMsgOff = 6000;

    pinMode(dataPin, OUTPUT);
    configured = true;
}
   
    void
setupXmitConfig(
    unsigned int zOn, 
    unsigned int zOff, 
    unsigned int oOn, 
    unsigned int oOff, 
    unsigned int sOn, 
    unsigned int sOff, 
    unsigned int rOff, 
    unsigned int mOff,
    unsigned int pin)
{
    if (pin) dataPin = pin;
    if (zOn) zeroOn = zOn;
    if (zOff) zeroOff = zOff;
    if (oOn) oneOn = oOn;
    if (oOff) oneOff = oOff;
    if (sOn) startOfRowOn = sOn;
    if (sOff) startOfRowOff = sOff;
    if (rOff) endOfRowOff = rOff;
    if (mOff) endOfMsgOff = mOff;

    pinMode(dataPin, OUTPUT);
    configured = true;
}

    void 
showConfig()
{
    String s;
    s = "Data Pin " + String(dataPin);
    Serial.println(s);
    s = "0 (" + String(zeroOn) + "," + String(zeroOff) + ")  1 (" + String(oneOn) + "," + String(oneOff) + ")";
    Serial.println(s);
    s = "Start Row (" + String(startOfRowOn) + "," + String(startOfRowOff) + ")";
    Serial.println(s);
    s = "End Row/Msg (" + String(endOfRowOff) + "," + String(endOfMsgOff) + ")";
    Serial.println(s);
}

// xmitRow
/*
 * xmitRww
 * 
 * Repeat:
 *    Send Start of row pulse
 *    Send Data
 *    Delay for endo of row gap
 */

    void
xmitRow(
    byte* data, 
    unsigned int dataBits, 
    unsigned int repeats)
{
    unsigned int dataIndex;
    int bits;
    String s;

    while (repeats > 0) {
        s = "Repeat " + String(repeats);
        
        // xmit a start of row
        xmitPulse(startOfRowOn, startOfRowOff);
        
        dataIndex = 0;
        bits = dataBits;
        while (bits > 0) {
            if (dataBits<8) {
               xmitByte(data[dataIndex], bits);
            } else {
                xmitByte(data[dataIndex], 8);
            }
            bits -= 8;
            dataIndex += 1;
        }
        xmitGap(endOfRowOff);
        
        repeats -= 1;
    }
}

    void 
xmitByte(
    byte b,
    unsigned int dataBits)
{
    while (dataBits>0) {
        if (b & 0x80) {
            // 1 Bit
            xmitPulse(oneOn, oneOff);  
        } else {
            // 0 bit
            xmitPulse(zeroOn, zeroOff);  
        }
        b = b << 1;
        dataBits -= 1;
    }
}

    void
xmitEndOfMsg()
{
    xmitGap(endOfMsgOff);
}

  void 
xmitGap(
  int offTime)
{
  digitalWrite(dataPin, low);
  delayMicroseconds(offTime);
}

  void 
xmitPulse(
  int onTime, 
  int offTime)
{
  digitalWrite(dataPin, HIGH);
  // Delay for on time
  delayMicroseconds(onTime);
  
  digitalWrite(dataPin, LOW);
  // Delay for off time
  delayMicroseconds(offTime);
}
