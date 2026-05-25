/*
 * ============================================================
 *  AUTONOMOUS COLOUR SORTING MACHINE
 *  Capstone Research Project -- B.Eng. Computer Engineering
 *  Bayero University, Kano . 2019-2020
 *
 *  Author : Abubakar Rabiu Maihadisi
 *  Email  : maihadisiabubakar@gmail.com
 *  GitHub : https://github.com/Maihadisiabu
 *
 *  Hardware:
 *    - Arduino UNO (ATmega328P)
 *    - TCS3200 RGB Colour Sensor
 *    - SG90 / MG995 Servo Motor
 *
 *  Description:
 *    Implements a 5-state finite state machine (FSM) that
 *    continuously reads RGB frequency values from a TCS3200
 *    sensor, classifies the dominant colour, and drives a
 *    servo motor to route the object to the correct bin.
 *
 *  Pin assignments -- see README.md for full wiring diagram.
 * ============================================================
 */

#include <Servo.h>

/* -- PIN DEFINITIONS ----------------------------------------- */
#define S0_PIN   4    // TCS3200 frequency scaling bit 0
#define S1_PIN   5    // TCS3200 frequency scaling bit 1
#define S2_PIN   6    // TCS3200 colour filter select bit 0
#define S3_PIN   7    // TCS3200 colour filter select bit 1
#define OUT_PIN  8    // TCS3200 frequency output (pulse)
#define SERVO_PIN 9   // Servo PWM signal

/* -- SERVO ANGLE POSITIONS (degrees) ------------------------- */
#define SERVO_HOME    90    // Centre / neutral position
#define SERVO_RED     45    // Left bin
#define SERVO_GREEN   90    // Centre bin
#define SERVO_BLUE   135    // Right bin

/* -- COLOUR CLASSIFICATION THRESHOLDS ------------------------ */
/*
 * These values were calibrated empirically under controlled
 * indoor lighting. Recalibrate using calibration_helper.ino
 * if your ambient lighting conditions differ significantly.
 *
 * Lower pulse count = stronger response for that colour filter.
 */
#define RED_THRESHOLD    80    // pulse count below which red dominates
#define GREEN_THRESHOLD  80    // pulse count below which green dominates
#define BLUE_THRESHOLD   80    // pulse count below which blue dominates

/* -- TIMING CONSTANTS (milliseconds) ------------------------ */
#define SENSE_DURATION     100   // how long to count pulses per channel
#define ACTUATE_HOLD_TIME  600   // time to hold servo at bin position
#define RESET_DELAY        400   // delay before returning to home

/* -- FSM STATE DEFINITIONS ----------------------------------- */
typedef enum {
  STATE_IDLE,
  STATE_SENSE,
  STATE_CLASSIFY,
  STATE_ACTUATE,
  STATE_RESET
} SystemState;

/* -- COLOUR LABELS -------------------------------------------- */
typedef enum {
  COLOUR_NONE,
  COLOUR_RED,
  COLOUR_GREEN,
  COLOUR_BLUE
} ColourLabel;

/* -- GLOBAL OBJECTS & VARIABLES ------------------------------ */
Servo sortingServo;

SystemState currentState = STATE_IDLE;
ColourLabel detectedColour = COLOUR_NONE;

unsigned int redCount   = 0;
unsigned int greenCount = 0;
unsigned int blueCount  = 0;

/* -- FUNCTION PROTOTYPES -------------------------------------- */
unsigned int readColourChannel(uint8_t s2, uint8_t s3);
ColourLabel  classifyColour(unsigned int r, unsigned int g, unsigned int b);
void         setServoPosition(ColourLabel colour);
const char*  colourName(ColourLabel colour);

/* ============================================================
 *  SETUP
 * ============================================================ */
void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Autonomous Colour Sorting Machine ==="));
  Serial.println(F("Initialising..."));

  /* Configure TCS3200 control pins as outputs */
  pinMode(S0_PIN, OUTPUT);
  pinMode(S1_PIN, OUTPUT);
  pinMode(S2_PIN, OUTPUT);
  pinMode(S3_PIN, OUTPUT);

  /* Configure TCS3200 output pin as input */
  pinMode(OUT_PIN, INPUT);

  /*
   * Set frequency scaling to 20%:
   * S0=HIGH, S1=LOW -> 20% scaling
   * Good balance between speed and resolution indoors.
   */
  digitalWrite(S0_PIN, HIGH);
  digitalWrite(S1_PIN, LOW);

  /* Attach servo and move to home position */
  sortingServo.attach(SERVO_PIN);
  sortingServo.write(SERVO_HOME);
  delay(500);   // allow servo to reach home

  Serial.println(F("System ready. Entering IDLE state."));
  Serial.println(F("-----------------------------------------"));
}

/* ============================================================
 *  MAIN LOOP -- FSM EXECUTION
 * ============================================================ */
void loop() {
  switch (currentState) {

    /* -- STATE: IDLE ---------------------------------------- */
    case STATE_IDLE:
      /*
       * In a full hardware build, object detection would use
       * an IR proximity sensor or limit switch. Here we poll
       * on a fixed cycle for demonstration purposes.
       *
       * Replace this delay with: if (digitalRead(IR_PIN) == LOW)
       * when integrating an IR object detection sensor.
       */
      delay(500);
      Serial.println(F("[IDLE] Waiting for object..."));
      currentState = STATE_SENSE;
      break;

    /* -- STATE: SENSE --------------------------------------- */
    case STATE_SENSE:
      Serial.println(F("[SENSE] Reading colour channels..."));

      redCount   = readColourChannel(LOW,  LOW);   // Red filter:   S2=LOW,  S3=LOW
      greenCount = readColourChannel(HIGH, HIGH);   // Green filter: S2=HIGH, S3=HIGH
      blueCount  = readColourChannel(LOW,  HIGH);   // Blue filter:  S2=LOW,  S3=HIGH

      Serial.print(F("  R="));  Serial.print(redCount);
      Serial.print(F("  G="));  Serial.print(greenCount);
      Serial.print(F("  B="));  Serial.println(blueCount);

      currentState = STATE_CLASSIFY;
      break;

    /* -- STATE: CLASSIFY ------------------------------------ */
    case STATE_CLASSIFY:
      detectedColour = classifyColour(redCount, greenCount, blueCount);

      Serial.print(F("[CLASSIFY] Detected colour: "));
      Serial.println(colourName(detectedColour));

      currentState = STATE_ACTUATE;
      break;

    /* -- STATE: ACTUATE ------------------------------------- */
    case STATE_ACTUATE:
      Serial.print(F("[ACTUATE] Routing to bin: "));
      Serial.println(colourName(detectedColour));

      setServoPosition(detectedColour);
      delay(ACTUATE_HOLD_TIME);   // hold at bin position while object falls

      currentState = STATE_RESET;
      break;

    /* -- STATE: RESET --------------------------------------- */
    case STATE_RESET:
      Serial.println(F("[RESET] Returning to home position."));
      Serial.println(F("-----------------------------------------"));

      sortingServo.write(SERVO_HOME);
      delay(RESET_DELAY);

      detectedColour = COLOUR_NONE;
      redCount = greenCount = blueCount = 0;

      currentState = STATE_IDLE;
      break;

    default:
      currentState = STATE_IDLE;
      break;
  }
}

/* ============================================================
 *  FUNCTION: readColourChannel
 *
 *  Selects a colour filter on the TCS3200 by setting S2/S3,
 *  then counts output pulses for SENSE_DURATION milliseconds.
 *  Lower count = higher light intensity for that colour.
 *
 *  Parameters:
 *    s2, s3 -- filter select pin states (LOW/HIGH)
 *  Returns:
 *    pulse count over SENSE_DURATION ms
 * ============================================================ */
unsigned int readColourChannel(uint8_t s2, uint8_t s3) {
  digitalWrite(S2_PIN, s2);
  digitalWrite(S3_PIN, s3);
  delay(10);   // allow filter to stabilise
  return (unsigned int) pulseIn(OUT_PIN, LOW, 100000UL);
  /*
   * pulseIn returns pulse width in microseconds.
   * For a frequency-based sensor, counting pulses over a fixed
   * window is more precise. For a production build, replace
   * pulseIn with a hardware timer interrupt counter.
   */
}

/* ============================================================
 *  FUNCTION: classifyColour
 *
 *  Compares R, G, B pulse counts to identify dominant colour.
 *  The channel with the LOWEST count has the highest intensity
 *  (inverse relationship in TCS3200 pulse-width output).
 *
 *  Returns:
 *    ColourLabel enum value
 * ============================================================ */
ColourLabel classifyColour(unsigned int r, unsigned int g, unsigned int b) {
  /* Guard: if all readings are very high, no object detected */
  if (r > 200 && g > 200 && b > 200) {
    return COLOUR_NONE;
  }

  if (r <= g && r <= b) {
    return COLOUR_RED;
  } else if (g <= r && g <= b) {
    return COLOUR_GREEN;
  } else {
    return COLOUR_BLUE;
  }
}

/* ============================================================
 *  FUNCTION: setServoPosition
 *
 *  Writes the servo to the bin angle corresponding to the
 *  detected colour.
 * ============================================================ */
void setServoPosition(ColourLabel colour) {
  switch (colour) {
    case COLOUR_RED:
      sortingServo.write(SERVO_RED);
      break;
    case COLOUR_GREEN:
      sortingServo.write(SERVO_GREEN);
      break;
    case COLOUR_BLUE:
      sortingServo.write(SERVO_BLUE);
      break;
    default:
      sortingServo.write(SERVO_HOME);
      break;
  }
}

/* ============================================================
 *  FUNCTION: colourName
 *
 *  Returns a human-readable string for a ColourLabel.
 *  Uses PROGMEM-friendly F() macro to save SRAM.
 * ============================================================ */
const char* colourName(ColourLabel colour) {
  switch (colour) {
    case COLOUR_RED:   return "RED";
    case COLOUR_GREEN: return "GREEN";
    case COLOUR_BLUE:  return "BLUE";
    default:           return "UNKNOWN";
  }
}
