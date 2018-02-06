#include <Servo.h>
#include <DS3231.h>
#include <Wire.h>

// #define DEBUG // This enables debug logging, uncomment to enable debug loggin

#define BUTTON_PIN 2
#define SERVO_PIN 9       // PIN used for servo
#define SERVO_DELAY 500   // Delay used for waiting for servo
#define MANUAL_RING_CYCLES_TARGET 1

#ifdef DEBUG
  #define DEBUG_PRINT(x)       Serial.print (x)
  #define DEBUG_PRINT_DEC(x)   Serial.print (x, DEC)
  #define DEBUG_PRINT_LN(x)    Serial.println (x)

  // Settings
  #define LOOP_DELAY 200

  #define ONE_DAY 86400L          // Length of day in seconds (i.e. time between reset of periods)
  #define NUM_PERIODS 1           // Number of periods to announce
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
unsigned short numCyclesPressed = 0;

void setup() {
 #ifdef DEBUG
  Serial.begin(9600);
 #endif

  pinMode(BUTTON_PIN, INPUT_PULLUP);
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

  int buttonState = digitalRead(BUTTON_PIN);
  bool manualOverride = false;

  DEBUG_PRINT("BUTTON STATE (DIGITAL): ");
  DEBUG_PRINT_LN(buttonState);

  if (buttonState == LOW) {
    DEBUG_PRINT_LN("BUTTON IS PRESSED");
    numCyclesPressed += 1;
  }

  if (numCyclesPressed >= MANUAL_RING_CYCLES_TARGET) {
    manualOverride = true;
    numCyclesPressed = 0;
  }

  evaluateTimestamp(timestamp, bootTimestamp, manualOverride);
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

void evaluateTimestamp(unsigned long now, unsigned long start, bool manualOverride) {
  // DEBUG_PRINT("evaluateTimestamp:");
  // DEBUG_PRINT(now);
  // DEBUG_PRINT(" - ");
  // DEBUG_PRINT_LN(start);

  unsigned long sinceStart = now - start;
  unsigned long sinceStartOfDay = sinceStart % ONE_DAY;

  int currentPeriod = sinceStartOfDay / PERIOD_LENGTH;

  /*
  DEBUG_PRINT("sinceStart:");
  DEBUG_PRINT_LN(sinceStart);
  // DEBUG_PRINT("sinceStartOfDay:");
  // DEBUG_PRINT_LN(sinceStartOfDay);
  DEBUG_PRINT("currentPeriod:");
  DEBUG_PRINT_LN(currentPeriod);
  */

  if (manualOverride || (currentPeriod != previousPeriod && currentPeriod < NUM_PERIODS)) {
    previousPeriod = currentPeriod;

    moveServoAndWait(0);
    moveServoAndWait(180);
  }
}
