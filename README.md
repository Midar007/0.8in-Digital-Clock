# 0.8 inch Digital Clock

There is a 0.8inch clock, designed with ESP8266 on Aliexpress 
- [Aliexpress link1](https://www.aliexpress.com/item/1005003163124952.html)
- [Aliexpress link2](https://www.aliexpress.com/item/1005003167288799.html)
![0.8inch clock](https://github.com/Midar007/0.8in-Digital-Clock/blob/main/images/clock_front.jpg)

Hardware:
- 0.8inch white 7 segment display with clock semi dots
- TM1650 led driver
- ESP8266 with 1M
- three buttons titled SET, UP, DOWN
- ds1302 RTC clock

Software:
- wifiManager [https://github.com/tzapu/WiFiManager](https://github.com/tzapu/WiFiManager)
- TM1650 display driver [https://github.com/maxint-rd/TM16xx](https://github.com/maxint-rd/TM16xx)
- ezTime library [https://github.com/ropg/ezTime](https://github.com/ropg/ezTime)

# Idea
The module have ESP8266 wifi chip, TM1650 driver for LED. For precise clock there is no need more. Connect to wifi, use NTP to get precise clock. 
Three buttons to be used to control brightness and to reset/restart

Functionality:
- time sync with NTP server
- setting for NTP server address
- timezone setup [Olson format](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones), ex. **Europe/Prague** 
- save timezone,ntp server in json to SPIFS
- SET button to reset all settings and restart

# Module circuit wiring
- TM1650 : SCL go to GPIO12, SDA to GPIO13. Read datasheet as this is not standard I2C
- DS1302 : CE to GPIO5, I/O to GPIO14, SCLK to GPIO16 - will not use
- button SET - go to GPIO0 with pull-up resistor
- button UP - go to GPI4, there is no resistor, will use internal (*dont ask me why each button have different wiring*)
- button DOWN - go to GPIO15 with pull down resistor
- red LED connected to GPIO2

# How to install
1. Use ESP flasher and bin file from repository. Restart then module will display "AP" and look for "Wifi Clock" network, connect (192.168.4.1) and follow wifi setup with NTP server and timezome ![wifi manager](https://github.com/Midar007/0.8in-Digital-Clock/blob/main/images/clock_wifimanager.jpg)
2. Create new project in Arduino, download libraries, update TM16xx font file, select generic ESP8266 with 1M and compile

# toDo
- bug fixies, if any
- design 3D printed case

# special thanks
Special thanks to all who created libraries necessary for my project.


