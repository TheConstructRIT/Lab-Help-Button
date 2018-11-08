/* CONSTRUCT BUTTON CODE
 *  WRITTEN BY A. KOWALCZYK AND Z. COOK
 *  
 *  
 * ---------------------------------------
 * Pinout:
 * 0: TX, goes to RX on radio
 * 1: RX, goes to TX on radio
 * 13: Light Output (both)
 * A0: Configure - 0V = button side, 5V = alarm side
 * 11: Button Input (both)
 * A4: Buzzer Out (alarm side)
 */

// CONFIG
// Pins
const int light = 13;
const int sideInd = 14;
const int button = 11;
const int buzzer = 18;

// Light cycles before buzzer
const int alarmButtonCycles = 5;
// Time between alarm light changing state in milliseconds
const int alarmLightCycleTimeMil = 500;
// Amount of time the buzzer will be on
const int buzzerOnTime = 100;

// Time in milliseconds before button can register as down (time between clicks acknoledged)
const unsigned long buttonCooldownTime = 250;

// Characters to read
char helpChar = 'A';
char acknowledgeChar = 'B';
char clearChar = 'C';





// VARIABLES
int side;
int systemState = 0;
// System States:
// 0 - Off
// 1 - Alarm
// 2 - Acknowledge

unsigned long buttonLastDownMil = 0;
int handleButtonClicked = 0;
int buttonWasDown = 0;

// Start time of alarm (set when button pressed)
unsigned long alarmStartTime = 0;





/*
 * Updates main alarm. Does not handle user input.
 * Set up so it can be terminated at any point.
 */
 void updateAlarmSideLight() {
  if (systemState == 0) {
    // If alarm is off, turn light and speaker off
    digitalWrite(light, LOW);
    digitalWrite(buzzer, LOW);
    alarmStartTime = 0;
  } else if (systemState == 1) {
    // If alarm is flashing, flash alarm
    if (alarmStartTime == 0) {
      alarmStartTime = millis();
    }

    // Localize time to 0 - (2 * alarmLightCycleTimeMil * alarmButtonCycles) milliseconds
    int totalAlarmTime = 2 * alarmLightCycleTimeMil * alarmButtonCycles;
    int alarmStateTimeMil = millis() % totalAlarmTime;

    // If alarm is in first half of a second, turn on light. Turn off otherwise.
    if (alarmStateTimeMil % (2 * alarmLightCycleTimeMil) <= alarmLightCycleTimeMil) {
      digitalWrite(light, HIGH);
    } else {
      digitalWrite(light, LOW);
    }

    // If alarm is within last buzzerOnTime milliseconds, turn on speaker. Turn off otherwise.
    if (alarmStateTimeMil > totalAlarmTime - buzzerOnTime) {
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
    }
  } else if (systemState == 2) {
    // If alarm is acknowledged, stay on
    digitalWrite(light, HIGH);
    digitalWrite(buzzer, LOW);
    alarmStartTime = 0;
  }
 }

/*
 * Updates input for main alarm
 */
 void updateAlarmSideInput() {
  // Handle alarm state
  if (systemState == 0) {
    // State 0: Off - Move to 1 if help character sent
    if (Serial.available()) {
      if (Serial.read() == helpChar) {
        systemState = 1;
      }
    }
  } else if (systemState == 1) {
    // State 1: Alarm - Move to 2 if clicked and send acknowledge signal. Move to 0 if clear sent
    if (handleButtonClicked == 1) {
      systemState = 2;
      Serial.print(acknowledgeChar);
    }

    if (Serial.available()) {
      if (Serial.read() == clearChar) {
        systemState = 0;
      }
    }
  } else if (systemState == 2) {
    // State 2: Ack - Move to 0 if clicked and send clear signal. Move to 0 if clear sent
    if (handleButtonClicked == 1) {
      systemState = 0;
      Serial.print(clearChar);
    }

    if (Serial.available()) {
      if (Serial.read() == clearChar) {
        systemState = 0;
      }
    }
  }

  
  // Update alarm light
  updateAlarmSideLight();
 }



/*
 * Updates help button. Does not handle user input.
 * Set up so it can be terminated at any point.
 */
 void updateHelpSideLight() {
  if (systemState == 0) {
    // If help button is off, turn light off
    digitalWrite(light, LOW);
    alarmStartTime = 0;
  } else if (systemState == 1) {
    // If alarm is flashing, flash alarm
    if (alarmStartTime == 0) {
      alarmStartTime = millis();
    }

    // Localize time to 0 - (2 * alarmLightCycleTimeMil) milliseconds
    int totalButtonTime = 2 * alarmLightCycleTimeMil;
    int alarmStateTimeMil = millis() % totalButtonTime;

    // If alarm is in first half of a second, turn on light. Turn off otherwise.
    if (alarmStateTimeMil <= alarmLightCycleTimeMil) {
      digitalWrite(light, HIGH);
    } else {
      digitalWrite(light, LOW);
    }
  } else if (systemState == 2) {
    // If alarm is acknowledged, stay on
    digitalWrite(light, HIGH);
    alarmStartTime = 0;
  }
 }
 
/*
 * Updates input for help button
 */
void updateHelpSideInput() {
  // Handle button state
  if (systemState == 0) {
    // State 0: Off - Move to 1 and send help character is pressed
    if (handleButtonClicked == 1) {
      systemState = 1;
      Serial.print(helpChar);
    }
  } else if (systemState == 1) {
    // State 1: Alarm - Move to 2 if sent acknowledge signal. Move to 0 and send clear if button pressed
    if (handleButtonClicked == 1) {
      systemState = 0;
      Serial.print(clearChar);
    }

    if (Serial.available()) {
      if (Serial.read() == acknowledgeChar) {
        systemState = 2;
      }
    }
  } else if (systemState == 2) {
    // State 2: Ack - Move to 0 if clicked and send clear signal. Move to 0 if clear sent
    if (handleButtonClicked == 1) {
      systemState = 0;
      Serial.print(clearChar);
    }

    if (Serial.available()) {
      if (Serial.read() == clearChar) {
        systemState = 0;
      }
    }
  }

  
  // Update help light
  updateHelpSideLight();
}



/*
 * Main setup function when intitialized
 */
void setup() {
  // Create pin setup
  Serial.begin(9600);
  pinMode(light, OUTPUT);
  pinMode(sideInd, INPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  // Configuration detect (LOW = help side, HIGH = alarm side)
  side = digitalRead(sideInd);
}

/*
 * Main loop function after initialized
 */
void loop() {
  // Get if button should be pressed
  int buttonState = digitalRead(button);
  if (buttonState == LOW) {
    // Button is currently down
    unsigned long cooldownTimeEnd = buttonLastDownMil + buttonCooldownTime;
    
    if (buttonWasDown == 0 && cooldownTimeEnd < millis()) {
      buttonLastDownMil = millis();
      handleButtonClicked = 1;
    } else {
      handleButtonClicked = 0;
    }
    buttonWasDown = 1;
  } else {
    // Button is currently up
    handleButtonClicked = 0;
    buttonWasDown = 0;
  }
  
  
  // If side is 5V, update alarm input. Handle help input if it is 0V.
  if (side == HIGH) {
    updateAlarmSideInput();
  } else {
    updateHelpSideInput();
  }
}
