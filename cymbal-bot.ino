#include <Servo.h>
#include <DS3231.h>
#include <Wire.h>

// #define DEBUG // This enables debug logging, uncomment to enable debug loggin

#define SERVO_PIN 9       // PIN used for servo
#define SERVO_DELAY 500   // Delay used for waiting for servo

#ifdef DEBUG
  #define DEBUG_PRINT(x)       Serial.print (x)
  #define DEBUG_PRINT_DEC(x)   Serial.print (x, DEC)
  #define DEBUG_PRINT_LN(x)    Serial.println (x)

  // Settings
  #define LOOP_DELAY 200

  #define ONE_DAY 86400L          // Length of day in seconds (i.e. time between reset of periods)
  #define NUM_PERIODS 5           // Number of periods to announce
  #define PERIOD_LENGTH (2 * 5)        // Length of period in seconds
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINT_DEC(x)
  #define DEBUG_PRINT_LN(x)

  // Settings
  #define LOOP_DELAY 200

  #define ONE_DAY 86400L          // Length of day in seconds (i.e. time between reset of periods)
  #define NUM_PERIODS 2           // Number of periods to announce
  #define PERIOD_LENGTH (60 * 10)    // Length of period in seconds
#endif


Servo sg90;
RTClib RTC;
// DS3231 Clock; // High level Clock functions

unsigned long bootTimestamp = 0;
int previousPeriod = -1;

void setup() {
 #ifdef DEBUG
  Serial.begin(9600);
 #endif

  sg90.attach(SERVO_PIN);
  moveServoAndWait(0);

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
  // DEBUG_PRINT("evaluateTimestamp:");
  // DEBUG_PRINT(now);
  // DEBUG_PRINT(" - ");
  // DEBUG_PRINT_LN(start);

  unsigned long sinceStart = now - start;
  unsigned long sinceStartOfDay = sinceStart % ONE_DAY;

  int currentPeriod = sinceStartOfDay / PERIOD_LENGTH;

  DEBUG_PRINT("sinceStart:");
  DEBUG_PRINT_LN(sinceStart);
  // DEBUG_PRINT("sinceStartOfDay:");
  // DEBUG_PRINT_LN(sinceStartOfDay);
  DEBUG_PRINT("currentPeriod:");
  DEBUG_PRINT_LN(currentPeriod);

  if (currentPeriod != previousPeriod && currentPeriod < NUM_PERIODS) {
    previousPeriod = currentPeriod;

    moveServoAndWait(0);
    moveServoAndWait(180);
  }
}
