#include config.h
  
String cmd;

    String
parseString()
{
    return cmd;
}

    void
setupParse(String s)
{
    cmd = s;
    cmd.toUpperCase();
    Serial.println(cmd);
}

    boolean
parseIncomplete()
{
    return cmd.length() > 0;
}

    char
currentChar()
{
    return cmd.charAt(0);
}
    
    boolean 
currentIs(char c)
{
    //String s;
    //s = "Next " + String(cmd.charAt(0)) + " Seeking " + c;
    //Serial.println(s);
    
    if (cmd.length()<=0 || cmd.charAt(0) != c) {
        return false;
    }
    cmd.remove(0, 1);
    return true;
}

    boolean 
currentIsHex(char* c)
{
    *c = cmd.charAt(0);
    
    if (isHexadecimalDigit(*c)) {
        cmd.remove(0, 1);
        return true;
    } else {
        return false;
    }
}

    boolean 
currentIsDigit(char* c)
{
    *c = cmd.charAt(0);
    
    if (isHexadecimalDigit(*c)) {
        cmd.remove(0, 1);
        return true;
    } else {
        return false;
    }
}

    boolean
parseDelimiter()
{
    char c;
    boolean ret = false;
    
    c = cmd.charAt(0);
    
    while (parseIncomplete() && !isHexadecimalDigit(c)) {
        cmd.remove(0, 1);
        c = cmd.charAt(0);
        ret = true;
    }
    return ret;
}

    boolean 
parseHex(
    byte*         data,
    unsigned int* dataBits,
    unsigned int  dataLen)
{
    // Assumes upper case in CMD
    char c;
    byte bits;
    boolean shift = true; // Next bits go in upper part of byte
    unsigned int dataIndex = 0;
    
    *dataBits = 0;
    while (   dataIndex < dataLen 
           && currentIsHex(&c)) 
    {
        *dataBits += 4;
        if (isDigit(c)) {
            // 0-9
            bits = c - '0';
        } else {
          // A-F
          bits = c - 'A' + 10;
        }
        if (shift) {
            data[dataIndex] = bits << 4;
            shift = false;
        } else {
            data[dataIndex] = data[dataIndex] + bits;
            dataIndex += 1;
            shift = true;
        }
    }
    
    return *dataBits>0;
}

    boolean 
parseDec(
    unsigned int* decimal)
{
    char c;
    unsigned int d = 0;
    boolean ret = false;
    
    while (currentIsDigit(&c)) {
        // 0-9
        d = d * 10 + (c - '0');
        ret = true;
    }

    *decimal = d;
    return ret;
}
