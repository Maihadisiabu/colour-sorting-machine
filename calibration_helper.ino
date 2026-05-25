/*
 * ============================================================
 *  COLOUR SORTING MACHINE — CALIBRATION HELPER
 *
 *  Run this sketch BEFORE the main programme to obtain
 *  the correct R, G, B pulse count readings for your
 *  specific ambient lighting conditions.
 *
 *  Instructions:
 *    1. Upload this sketch.
 *    2. Open Serial Monitor at 9600 baud.
 *    3. Hold a RED object in front of the sensor.
 *       Note the R, G, B values printed.
 *    4. Repeat with a GREEN object, then a BLUE object.
 *    5. Update RED_THRESHOLD, GREEN_THRESHOLD, BLUE_THRESHOLD
 *       in colour_sorting_machine.ino with your observed values.
 * ============================================================
 */

#define S0_PIN  4
#define S1_PIN  5
#define S2_PIN  6
#define S3_PIN  7
#define OUT_PIN 8

unsigned int readChannel(uint8_t s2, uint8_t s3);

void setup() {
  Serial.begin(9600);
  pinMode(S0_PIN, OUTPUT); pinMode(S1_PIN, OUTPUT);
  pinMode(S2_PIN, OUTPUT); pinMode(S3_PIN, OUTPUT);
  pinMode(OUT_PIN, INPUT);
  digitalWrite(S0_PIN, HIGH); digitalWrite(S1_PIN, LOW);  // 20% scaling
  Serial.println(F("=== Calibration Helper ==="));
  Serial.println(F("Hold object in front of sensor. Reading every 1 second."));
  Serial.println(F("----------------------------------------------------------"));
}

void loop() {
  unsigned int r = readChannel(LOW,  LOW);
  unsigned int g = readChannel(HIGH, HIGH);
  unsigned int b = readChannel(LOW,  HIGH);

  Serial.print(F("R = ")); Serial.print(r);
  Serial.print(F("  G = ")); Serial.print(g);
  Serial.print(F("  B = ")); Serial.println(b);

  delay(1000);
}

unsigned int readChannel(uint8_t s2, uint8_t s3) {
  digitalWrite(S2_PIN, s2); digitalWrite(S3_PIN, s3);
  delay(10);
  return (unsigned int) pulseIn(OUT_PIN, LOW, 100000UL);
}
