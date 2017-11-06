#include <Servo.h>
#include <DS3231.h>
#include <Wire.h>

#define SERVO_PIN 9       // PIN used for servo
#define SERVO_DELAY 500   // Delay used for waiting for servo

#define LOOP_DELAY 200

#define ONE_DAY 86400L          // Length of day in seconds (i.e. time between reset of periods)
#define NUM_PERIODS 5           // Number of periods to announce
#define PERIOD_LENGTH 60 * 5    // Length of period in seconds

Servo sg90;
RTClib RTC;
// DS3231 Clock; // High level Clock functions

unsigned long bootTimestamp = 0;
int previousPeriod = -1;

void setup() {
  sg90.attach(SERVO_PIN);
  Wire.begin();
}

void loop() {
  DateTime now = RTC.now();
  unsigned long timestamp = now.unixtime();
  if (!bootTimestamp) {
    bootTimestamp = timestamp;
  }
  
  evaluateTimestamp(timestamp, bootTimestamp);
  delay(LOOP_DELAY);
}

void moveServoAndWait(byte pos) {
  delay(moveServo(pos));
}

short moveServo(byte pos) {
  int nextPos = constrain((int)pos, 0, 179);
  sg90.write(nextPos);
  return SERVO_DELAY;
}

void evaluateTimestamp(unsigned long now, unsigned long start) {
  unsigned long sinceStart = now - start;
  unsigned long sinceStartOfDay = sinceStart % ONE_DAY;
  
  int currentPeriod = sinceStartOfDay / PERIOD_LENGTH;
  
  if (currentPeriod != previousPeriod && currentPeriod < NUM_PERIODS) {
    previousPeriod = currentPeriod;
    
    moveServoAndWait(0);
    moveServoAndWait(179);
  }
}
