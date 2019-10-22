#include <Servo.h>
#include <DS3231.h>
#include <Wire.h>

// #define DEBUG // This enables debug logging, uncomment to enable debug logging

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
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINT_DEC(x)
  #define DEBUG_PRINT_LN(x)

  // Settings
  #define LOOP_DELAY 200
#endif


Servo sg90;
RTClib RTC;
DS3231 Clock; // High level Clock functions

struct belltime {
  byte hour;
  byte minute;
  byte second;
};


bool is_equal_belltime(struct belltime t1, struct belltime t2) {
  return t1.hour == t2.hour && t1.minute == t2.minute && t1.second == t2.second;
}

struct belltime bell_schedule[] = {
  {9, 45, 0},
  {10, 0, 0}
 };

struct belltime lastTrigger;

// const int ClockPowerPin = 6; // Activates voltage regulator to power the RTC (otherwise is on backup power from VCC or batt)
unsigned short numCyclesPressed = 0;

void setup() {
 #ifdef DEBUG
  Serial.begin(9600);
 #endif

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  sg90.attach(SERVO_PIN);

  Wire.begin();

  moveServoAndWait(180);

  // pinMode(ClockPowerPin, OUTPUT);
  // digitalWrite(ClockPowerPin, HIGH);

  Clock.setClockMode(false);  // set to 24h
  //setClockMode(true); // set to 12h

  /*
  Clock.setYear(19);
  Clock.setMonth(10);
  Clock.setDate(21);
  Clock.setHour(20);
  Clock.setMinute(27);
  Clock.setSecond(0);
  Clock.setDoW(2);
  */
}

void loop() {
  DateTime now = RTC.now();
  unsigned long timestamp = now.unixtime();

  int buttonState = digitalRead(BUTTON_PIN);
  bool manualOverride = false;

  // DEBUG_PRINT("BUTTON STATE (DIGITAL): ");
  // DEBUG_PRINT_LN(buttonState);

  if (buttonState == LOW) {
    DEBUG_PRINT_LN("BUTTON IS PRESSED");
    numCyclesPressed += 1;
  }

  if (numCyclesPressed >= MANUAL_RING_CYCLES_TARGET) {
    manualOverride = true;
    numCyclesPressed = 0;
  }

  evaluateBell(manualOverride);
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

void evaluateBell(bool manualOverride) {
  bool h12, pm;
  byte h = Clock.getHour(h12, pm);
  byte m = Clock.getMinute();
  byte s = Clock.getSecond();
  struct belltime now = {h, m, s};

  bool triggerBell = false;

  DEBUG_PRINT("evaluateBell:");
  DEBUG_PRINT(h);
  DEBUG_PRINT(":");
  DEBUG_PRINT(m);
  DEBUG_PRINT(":");
  DEBUG_PRINT_LN(s);

  size_t schedule_size = sizeof(bell_schedule) / sizeof(bell_schedule[0]);
  for (int i = 0; i < schedule_size; i++) {
    struct belltime t = bell_schedule[i];
    triggerBell |= is_equal_belltime(now, t);
  }

  // triggerBell = s % 10 == 0;
  bool alreadyTriggered = is_equal_belltime(now, lastTrigger);

  if ((!alreadyTriggered && triggerBell) || manualOverride) {
    lastTrigger = {h, m, s};
    moveServoAndWait(0);
    moveServoAndWait(180);
  }
}
