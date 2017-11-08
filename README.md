# Cymbal Bot

Uses a DS3231 Real-Time Clock to trigger a servo min-max cycle on a specific interval one time each day. By default this cycle will be triggered at intervals 0, 5, 10, 15 and 20 minutes, and repeat after 24 hours. This behaviour can be tweaked by redefining the constants `ONE_DAY`, `NUM_PERIODS` and `PERIOD_LENGTH`.

The servo should be connected to PIN 9 (`SERVO_PIN`) and the DS3231 circuit's SDA and SCL should connect to the corresponding dedicated ports on the Arduino (A4 and A5 respectively on Arduino Uno).

## Requirements

The following libraries are requried:

* Servo
* Wire
* DS3231
