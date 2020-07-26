#include "config.h"
  
  void 
setup()
{
    Serial.begin(115200);
    delay(5000);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println(" ");
    Serial.println(NAME);
    Serial.println(" ");
    setupXmitDefaults();
    showConfig();
}

    void
loop()
{
    String s;
    
    s = Serial.readStringUntil("/n");
    s.remove(s.length()-1, 1); // remove /n
    
    if (s.length()>0) {
        setupParse(s);
        parseCmd();
    }
}
