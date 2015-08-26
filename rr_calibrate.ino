
#include <Servo.h>
#include <EEPROM.h>

const unsigned short idxLeft = 0;
const unsigned short idxRight = 1;
const unsigned short idxForward = 2;
const unsigned short idxBack = 3;
const unsigned short idxUp = 4;
const unsigned short idxDown = 5;
const unsigned short idxGrab = 6;
const unsigned short idxLetGo = 7;

const unsigned short ledPin = 13;

const unsigned short clawPin = 8;
const unsigned short swivelPin = 9;
const unsigned short fwdbackPin = 10;
const unsigned short updownPin = 11;

Servo clawServo;
Servo swivelServo;
Servo updownServo;
Servo fwdbackServo;

String inputString = "";
boolean stringComplete = false;

short lastClawValue = 0;
short lastSwivelValue = 0;
short lastUpDownValue = 0;
short lastFwdBackValue = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  ledOn();

  Serial.begin(9600);
  inputString.reserve(32);

  ledOff();
}

void ledOn() {
  digitalWrite(ledPin, HIGH);
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void loop() {
  handleSerial();
  if (stringComplete) {
    ledOn();

    if (inputString.startsWith("clear")) {
      String confirm = inputString.substring(6);
      if (confirm.equals("yes")) {
        Serial.println("clearing EEPROM");
        ledOn();
        for (int i = 0; i < EEPROM.length(); i++)
          EEPROM.write(i, 0);
        ledOff();
        Serial.println("EEPROM clear");
      } else {
        Serial.println("clear yes - to clear EEPROM");
      }
    } else if (inputString.startsWith("cl")) {
      String command = inputString.substring(2);
      if (command.startsWith("open")) {
        EEPROM.put(idxLetGo * sizeof(short), lastClawValue);
        Serial.println("claw open value stored");
      } else if (command.startsWith("close")) {
        EEPROM.put(idxGrab * sizeof(short), lastClawValue);
        Serial.println("claw close value stored");
      } else {
        testClawValue((short)command.toInt());
      }
    } else if (inputString.startsWith("sw")) {
      String command = inputString.substring(2);
      if (command.startsWith("min")) {
        EEPROM.put(idxLeft * sizeof(short), lastSwivelValue);
        Serial.println("swivel min value stored");
      } else if (command.startsWith("max")) {
        EEPROM.put(idxRight * sizeof(short), lastSwivelValue);
        Serial.println("swivel max value stored");
      } else {
        testSwivelValue((short)command.toInt());
      }
    } else if (inputString.startsWith("ud")) {
      String command = inputString.substring(2);
      if (command.startsWith("min")) {
        EEPROM.put(idxUp * sizeof(short), lastUpDownValue);
        Serial.println("up/down min value stored");
      } else if (command.startsWith("max")) {
        EEPROM.put(idxDown * sizeof(short), lastUpDownValue);
        Serial.println("up/down max value stored");
      } else {
        testUpDownValue((short)command.toInt());
      }
    } else if (inputString.startsWith("fb")) {
      String command = inputString.substring(2);
      if (command.startsWith("min")) {
        EEPROM.put(idxForward * sizeof(short), lastFwdBackValue);
        Serial.println("forward/back min value stored");
      } else if (command.startsWith("max")) {
        EEPROM.put(idxBack * sizeof(short), lastFwdBackValue);
        Serial.println("forward/back max value stored");
      } else {
        testFwdBackValue((short)command.toInt());
      }
    } else if (inputString.startsWith("?") || inputString.startsWith("help")) {
      Serial.println("cl[open|close|num]");
      Serial.println("sw[min|max|num]");
      Serial.println("ud[min|max|num]");
      Serial.println("fb[min|max|num]");
      Serial.println("clear <yes>");
      Serial.println("check");
      Serial.println("dump");
    } else if (inputString.startsWith("check")) {
      runCheck();
    } else if (inputString.startsWith("dump")) {
      Serial.println("Stored data dump:");
      printStoredValue("Left", idxLeft);
      printStoredValue("Right", idxRight);
      printStoredValue("Up", idxUp);
      printStoredValue("Down", idxDown);
      printStoredValue("Forward", idxForward);
      printStoredValue("Back", idxBack);
      printStoredValue("Grab", idxGrab);
      printStoredValue("LetGo", idxLetGo);
    } else if (inputString.startsWith("ver")) {
      Serial.println("rr_calibrate v0.1");
    }

    inputString="";
    stringComplete = false;
    ledOff();
  }
}

void handleSerial() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar > 31 && inChar < 128) {
      inputString += inChar;
    }
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void printStoredValue(String name, unsigned short index) {
  short value;
  EEPROM.get(index * sizeof(short), value);
  Serial.print(name);
  Serial.print(": ");
  Serial.println(value);
}

void testClawValue(short value) {
  if (value < 1)
    return;

  clawServo.attach(clawPin);
  clawServo.write(value);
  delay(max(abs(lastClawValue - value) * 3, 100));
  clawServo.detach();

  lastClawValue = value;
}

void testSwivelValue(short value) {
  if (value < 1)
    return;

  swivelServo.attach(swivelPin);
  swivelServo.write(value);
  delay(max(abs(lastSwivelValue - value) * 3, 100));
  swivelServo.detach();

  lastSwivelValue = value;
}

void testUpDownValue(short value) {
  if (value < 1)
    return;

  updownServo.attach(updownPin);
  updownServo.write(value);
  delay(max(abs(lastUpDownValue - value) * 3, 100));
  updownServo.detach();

  lastUpDownValue = value;
}

void testFwdBackValue(short value) {
  if (value < 1)
    return;

  fwdbackServo.attach(fwdbackPin);
  fwdbackServo.write(value);
  delay(max(abs(lastFwdBackValue - value) * 3, 100));
  fwdbackServo.detach();

  lastFwdBackValue = value;
}

void runCheck() {
  short minSwivel;
  short maxSwivel;

  short minUpDown;
  short maxUpDown;

  short minFwdBack;
  short maxFwdBack;

  short clawOpen;
  short clawClose;

  short tmpMin;
  short tmpMax;

  EEPROM.get(idxLeft * sizeof(short), tmpMin);
  EEPROM.get(idxRight * sizeof(short), tmpMax);
  minSwivel = min(tmpMin, tmpMax);
  maxSwivel = max(tmpMin, tmpMax);

  EEPROM.get(idxUp * sizeof(short), tmpMin);
  EEPROM.get(idxDown * sizeof(short), tmpMax);
  minUpDown = min(tmpMin, tmpMax);
  maxUpDown = max(tmpMin, tmpMax);

  EEPROM.get(idxForward * sizeof(short), tmpMin);
  EEPROM.get(idxBack * sizeof(short), tmpMax);
  minFwdBack = min(tmpMin, tmpMax);
  maxFwdBack = max(tmpMin, tmpMax);

  EEPROM.get(idxGrab * sizeof(short), clawClose);
  EEPROM.get(idxLetGo * sizeof(short), clawOpen);

  Serial.print("minSwivel: "); Serial.println(minSwivel);
  Serial.print("maxSwivel: "); Serial.println(maxSwivel);
  Serial.print("minUpDown: "); Serial.println(minUpDown);
  Serial.print("maxUpDown: "); Serial.println(maxUpDown);
  Serial.print("minFwdBack: "); Serial.println(minFwdBack);
  Serial.print("maxFwdBack: "); Serial.println(maxFwdBack);
  Serial.print("clawOpen: "); Serial.println(clawOpen);
  Serial.print("clawClose: "); Serial.println(clawClose);

  testClawValue(clawOpen);
  testSwivelValue((minSwivel + maxSwivel) / 2);
  testUpDownValue((minUpDown + maxUpDown) / 2);
  testFwdBackValue((minFwdBack + maxFwdBack) / 2);
}

