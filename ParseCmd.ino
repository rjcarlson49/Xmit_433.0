#include "config.h"
  
/*
 * Cmd ::= + ('?' | Config | Msg | Delay) 
 * 
 * Config ::= "!" [D] * ((' ' | ',') D)
 * 
 * Msg ::= (+ Row) 
 * 
 * Row ::= ["#" N] "+" H ["*" D] // # N indicates data length in bits NYI
 * 
 * Delay ::= ">" N
 * 
 * N ::= decimal digits
 * 
 * H ::= hex digits
 */

    boolean
parseCmd()
{
    boolean ok = false;
    unsigned int delayInSecs = 0;
    
    while (parseIncomplete()) {
        if (parseConfig()) {
            Serial.println("parseConfig done " + parseString());
            ok = true;
        
        } else if (currentIs('?')) {
            showHelp();

        } else if (parseMsg()) {
            Serial.println("parseMsg done " + parseString());
            
        } else if (parseDelay()) {
            Serial.println("parseDelay done" + parseString());
          
        } else {
            endParse();
            Serial.println(parseString() + "end parse");
        }
    }
    Serial.println("!!READY!!");
}

    void 
showHelp()
{
    Serial.println("----------------------------------------");
    Serial.println("Cmd    ::= + (Help | Config | Msg | Delay)");
    Serial.println("Help   ::= '?'                          // Help");
    Serial.println("Config ::= '!' [D] * ((' ' | ',') D)    // Decimals delimited by space or comma");
    Serial.println("Delay  ::= '>' N                        // Delay for N milleseconds");
    Serial.println("Msg    ::= (+ Row)                      // Msg to be sent - One or more rows");
    Serial.println("Row    ::= ['#' N] '+' H ['*' D]        // # N indicates data length in bits - Not yet implemented");
    Serial.println("N      ::= decimal digits               // Series of decimal digits - a number");
    Serial.println("H      ::= hex digits                   // Series of hex digits - the data");
    Serial.println("");
    Serial.println("Row is the data to be sent, a series of hex digits. '*N' indicates sending");
    Serial.println("the row N times. '#N' indicates the number of data bits in the row.");
    Serial.println("Otherwise the hex digits imply the length.");
    Serial.println("----------------------------------------");
    showConfigHelp();
}

    void 
showConfigHelp()
{
    Serial.println("Config: !zeroOn,zeroOff,oneOn,oneOff,startRowOn,startRowOff,endRowOff,endMsgOff,dataPin");
    Serial.println("0, 1 - On pulse len, Off pulse length");
    Serial.println("Start of a row - On pulse len, Off pulse length");
    Serial.println("End of a row off pulse length");
    Serial.println("End of a message off pulse length");
    Serial.println("Data pin number on Arduino");
    Serial.println("----------------------------------------");
}

    boolean 
parseConfig()
{
    unsigned int zeroOn = 0;
    unsigned int zeroOff = 0;
    unsigned int oneOn = 0;
    unsigned int oneOff = 0;
    unsigned int startRowOn = 0;
    unsigned int startRowOff = 0;
    unsigned int endRowOff = 0;
    unsigned int endMsgOff = 0;
    unsigned int dataPin = 0;

    if (currentIs('!')) {
        if (!parseIncomplete()) {
            showConfigHelp();
            return true;
        }
        parseDec(&zeroOn);
        parseDelimiter();

        parseDec(&zeroOff);
        parseDelimiter();
        
        parseDec(&oneOn);
        parseDelimiter();
        
        parseDec(&oneOff);
        parseDelimiter();
        
        parseDec(&startRowOn);
        parseDelimiter();
        
        parseDec(&startRowOff);
        parseDelimiter();
        
        parseDec(&endRowOff);
        parseDelimiter();
        
        parseDec(&endMsgOff);
        parseDelimiter();

        parseDec(&dataPin);

        setupXmitConfig(zeroOn, zeroOff, oneOn, oneOff, startRowOn, startRowOff, endRowOff, endMsgOff, dataPin);

        showConfig();
        return true;
    }
    return false;
}

    boolean 
parseMsg()
{
    boolean ret = false;
    
    while (parseRow()) {
        ret = true;
    }
    xmitEndOfMsg();
    return ret;
}

    boolean 
parseRow()
{
    byte data[25];
    unsigned int dataBits;
    unsigned int repeats;
    String s;
    
    if (!currentIs('+')) return false;
    
    dataBits = 0;
    if (!parseHex(data, &dataBits, 25)) {
        return false;
    }
    
    repeats = 1;
    if (currentIs('*')) {
        if (!parseDec(&repeats)) repeats = 1;
    }
    
    //s = "Row " + String(dataBits) + " bits  " + String(repeats) + " repeats  ";
    //Serial.println(s);
    
    xmitRow(data, dataBits, repeats);
    return true;
}

    boolean 
parseDelay()
{
    unsigned int delayInMSecs;

    if (!currentIs('>')) return false;
    delayInMSecs = 1;
    if (!parseDec(&delayInMSecs)) return false;
    String s = "Delay " + String(delayInMSecs);
    Serial.println(s);
    delay(delayInMSecs);
    Serial.println("End delay");
    return true;
}
