# 0.8 inch Digital Clock
---
There is a 0.8inch clock, designed with ESP8266 on Aliexpress 
- [Aliexpress link1](https://www.aliexpress.com/item/1005003163124952.html)
- [Aliexpress link2](https://www.aliexpress.com/item/1005003167288799.html)

Hardware:
- 0.8inch white 7 segment display with clock semi dots
- TM1650 led driver
- ESP8266 with 1M
- three buttons titled SET, UP, DOWN
- ds1302 RTC clock

Software:
- wifiManager [https://github.com/tzapu/WiFiManager](https://github.com/tzapu/WiFiManager)
- TM1650 display driver [https://github.com/arkhipenko/TM1650](https://github.com/arkhipenko/TM1650)

# Idea
The module have ESP8266 wifi chip, TM1650 driver for LED. For precise clock there is no need more. Connect to wifi, use NTP to get precise clock. 
Three buttons to be used to control brightness and to reset/restart



