/*
  0.8inch wifi clock
  March 2022
  
*/
#include <FS.h>                             // this needs to be first, or it all crashes and burns...
#include <TM1650.h>
#undef max
#undef min
#include <ezTime.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 

// definition of pins
#define LED_BUILTIN 2
#define BUTT_UP 4
#define BUTT_DOWN 15
#define BUTT_SET 0

// define display
TM1650 module(13, 12);                      // data, clock, 4 digits

int brightness=4; 
unsigned long myTime;
char set_ntp[40] = "pool.ntp.org";
char my_timezone[40] = "Europe/Prague";     //corresponding to GMT+1
bool tiktak = false;
bool format12h = false;
Timezone tz;
WiFiManager wifiManager;

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    // ezTime debug information
    setDebug(INFO);
    
    pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (it is inverse)  
    // initiate buttons 
    pinMode(BUTT_UP, INPUT_PULLUP);   // must to activate internal PULLUP as switch connected to ground and not pullup on PCB
    pinMode(BUTT_DOWN, INPUT);        // PCB have pull UP connection
    pinMode(BUTT_SET, INPUT);         // PCB have pull DOWN, it is GPIO0
    
    module.clearDisplay();
    module.setDisplayToString("AP  "); // display AP string 

    // init file system
    bool result = SPIFFS.begin();
    Serial.println("SPIFFS opened: " + result);    
    //clean FS, for testing
    //wifiManager.resetSettings();
    //SPIFFS.format();
    //ESP.eraseConfig();

    WiFiManagerParameter custom_html("<p style=\"color:black;font-weight:Bold;\">Setting for Clock</p>"); 
    WiFiManagerParameter cust_ntp("ntpClientId", "NTP client", set_ntp, 40);
    WiFiManagerParameter cust_tz("TZClientId", "Timezone name (<a href=\"https://en.wikipedia.org/wiki/List_of_tz_database_time_zones\" target=\"_blank\">Olson format</a>)", my_timezone, 40);
    
    const char _custCheckbox[] = "type=\"checkbox\"";
    WiFiManagerParameter cust_12h("Form12hId", "checkbox_12h", "T", 2, _custCheckbox, WFM_LABEL_AFTER );

    wifiManager.addParameter(&custom_html);
    wifiManager.addParameter(&cust_ntp);
    wifiManager.addParameter(&cust_tz);
    wifiManager.addParameter(&cust_12h);

    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);
     
    wifiManager.autoConnect("Wifi clock");
    
    strcpy(set_ntp, cust_ntp.getValue());  
    strcpy(my_timezone, cust_tz.getValue());  
    format12h = (strncmp(cust_12h.getValue(), "T", 1) == 0);
    
    //save the custom parameters to FS
    if (shouldSaveConfig) {
      Serial.println("saving config");
      DynamicJsonDocument json(1024);
      json["set_ntp"] = set_ntp;
      json["my_timezone"] = my_timezone;
      json["12h_format"] = format12h;

      // init file system
      bool result = SPIFFS.begin();
      Serial.println("SPIFFS opened: " + result);   
    
      File configFile = SPIFFS.open("/config.json", "w");
      if (!configFile) {
        Serial.println("failed to open config file for writing");
      }
  
      serializeJson(json, Serial);
      Serial.println();
      serializeJson(json, configFile);
      configFile.close();
      //end save
    }
    
    module.setDisplayToString("SET  "); // display AP string  

    //read configuration from FS json
    //read always as there should be autoconnect
    Serial.println("mounting FS...");
  
    if (result) {
      Serial.println("mounted file system");
      if (SPIFFS.exists("/config.json")) {
        //file exists, reading and loading
        Serial.println("reading config file");
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile) {
          Serial.println("opened config file");
          size_t size = configFile.size();
          // Allocate a buffer to store contents of the file.
          std::unique_ptr<char[]> buf(new char[size]);
  
          configFile.readBytes(buf.get(), size);
          DynamicJsonDocument json(1024);
          auto deserializeError = deserializeJson(json, buf.get());
          serializeJson(json, Serial);
          if ( ! deserializeError ) {
            Serial.println("\nparsed json");
  
            strcpy(set_ntp, json["set_ntp"]);
            strcpy(my_timezone, json["my_timezone"]);
            format12h = (json["12h_format"] == true );
            
  
          } else Serial.println("failed to load json config");
        } else Serial.println("config not read");
      } else Serial.println("config do not exist");
    } else {
      Serial.println("failed to mount FS");
    }
    //end read
    
    Serial.println("Exit wifi Manager..");
    Serial.println("Sync NTP..");
    Serial.println("UTC:             " + UTC.dateTime());
    if (!tz.setLocation(my_timezone) ) {
      Serial.println("Timezone setting failed, setting default. ");
      Serial.println(errorString());
      tz.setLocation();
    }
    setServer(set_ntp);
    waitForSync();

    delay(2000);
    Serial.println(tz.dateTime());

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.println("WiFi connected.");

    module.clearDisplay();
    module.setupDisplay(true, brightness);   
    
}

void loop() {
    time_t t= tz.now();
    int my_hour;
    
    if (millis() - myTime > 1000*60 ) { //each minute
      events();
      Serial.printf("Time: %d:%d \n" , hour(t), minute(t));
      myTime = millis();
    }

    tiktak = !tiktak;
    my_hour = (format12h) ? hourFormat12(t) : hour(t);
    if( my_hour > 9) { 
      module.setDisplayDigit(my_hour/10, 0);
      module.setDisplayDigit(my_hour%10, 1, tiktak);
    } else {
      module.clearDisplayDigit(0, false);
      module.setDisplayDigit(my_hour%10, 1, tiktak);
    }
    // shouw minutes
    module.setDisplayDigit(minute(t)/10,2);
    module.setDisplayDigit(minute(t)%10,3);

    if (digitalRead(BUTT_DOWN) == HIGH) {
      module.clearDisplay();
      module.setDisplayToString("b   "); 
      while (digitalRead(BUTT_DOWN) == HIGH) {
        brightness = max(brightness-1 , 0);
        module.setupDisplay(true, brightness);  
        module.setDisplayDigit(brightness, 3); 
        Serial.printf("Brightness down %u \n", brightness );
        delay(500);
      }
    }
    if (digitalRead(BUTT_UP) == LOW) {
      module.clearDisplay();
      module.setDisplayToString("b   "); 
      while (digitalRead(BUTT_UP) == LOW) {
        brightness = min(brightness+1 , 7);
        module.setupDisplay(true, brightness);  
        module.setDisplayDigit(brightness, 3); 
        Serial.printf("Brightness down %u \n", brightness );
        delay(500);
      }
    }
    if (digitalRead(BUTT_SET) == LOW) {
      // SET button pressed, reset wifiManager and restart
      module.clearDisplay();
      module.setDisplayToString("    " ); 
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)  
      int i=3;
      while (digitalRead(BUTT_SET) == LOW && i>=0) {
        delay(300);
        module.setDisplayDigit(0, i);
        Serial.printf("Reset setting countdown . \n" );
        i--;
      }
      // if still pressed then reset
      if( digitalRead(BUTT_SET) == LOW && i<0) {
        // reset wifimanager
        Serial.printf("Reset !! \n" );
        wifiManager.resetSettings();
        SPIFFS.format();
        ESP.eraseConfig();
        delay(100);
        ESP.reset();
      }
      digitalWrite(LED_BUILTIN, HIGH);      // turn the LED on (HIGH is the voltage level)  
    }
    delay(500);                             // wait half sec

}
