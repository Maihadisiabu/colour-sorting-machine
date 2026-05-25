# Autonomous Colour Sorting Machine

> Capstone Research Project -- B.Eng. Computer Engineering
> Bayero University, Kano -- 2019-2020

An embedded real-time system that autonomously detects, classifies, and physically sorts coloured objects using a TCS3200 RGB colour sensor and a servo-motor actuator network, controlled by a finite state machine (FSM) implemented in C/C++ on an Arduino microcontroller.

---

## Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Circuit Connections](#circuit-connections)
- [Software Design](#software-design)
- [How to Run](#how-to-run)
- [Results and Validation](#results-and-validation)
- [Research Relevance](#research-relevance)
- [Author](#author)

---

## Overview

The goal of this project was to design and build a fully autonomous object-sorting system without human intervention. The system continuously monitors incoming objects via the TCS3200 colour sensor, classifies them as Red, Green, or Blue, and routes them to the corresponding bin using a servo motor in real time.

This project demonstrates:

- Closed-loop embedded system design (sense, decide, actuate)
- Finite state machine (FSM) architecture in firmware
- Real-time interrupt-driven I/O
- Quantitative hardware validation and testing

---

## System Architecture

```
Arduino UNO
  |
  |-- TCS3200 Colour Sensor --> Colour Classification (FSM Logic)
  |                                        |
  |                                        v
  |-- Servo Motor Control <-- PWM signal output
```

Flow: Object detected --> Sensor reads RGB values --> FSM classifies colour --> Servo rotates to correct bin --> System resets for next object

---

## Hardware Components

| Component        | Model                    | Quantity | Function                          |
|------------------|--------------------------|----------|-----------------------------------|
| Microcontroller  | Arduino UNO (ATmega328P) | 1        | Central processing and control    |
| Colour sensor    | TCS3200                  | 1        | RGB light-to-frequency conversion |
| Servo motor      | SG90 / MG995             | 1        | Mechanical sorting actuator       |
| Conveyor/chute   | Custom-built             | 1        | Object delivery mechanism         |
| Power supply     | 5V DC                    | 1        | System power                      |
| Connecting wires | Jumper wires             | --       | Circuit interconnections          |
| Breadboard       | Full-size                | 1        | Prototyping                       |

---

## Circuit Connections

### TCS3200 to Arduino UNO

| TCS3200 Pin | Arduino Pin | Description                |
|-------------|-------------|----------------------------|
| VCC         | 5V          | Power supply               |
| GND         | GND         | Ground                     |
| S0          | Pin 4       | Frequency scaling bit 0    |
| S1          | Pin 5       | Frequency scaling bit 1    |
| S2          | Pin 6       | Colour filter select bit 0 |
| S3          | Pin 7       | Colour filter select bit 1 |
| OUT         | Pin 8       | Frequency output           |
| OE          | GND         | Output enable (active LOW) |

### Servo Motor to Arduino UNO

| Servo Wire      | Arduino Pin | Description        |
|-----------------|-------------|--------------------|
| Red (VCC)       | 5V          | Power              |
| Brown (GND)     | GND         | Ground             |
| Orange (Signal) | Pin 9       | PWM control signal |

Wiring note: Frequency scaling is set to 20% (S0=HIGH, S1=LOW) for optimal indoor ambient light performance.

---

## Software Design

### Finite State Machine

The firmware is structured as a 5-state FSM:

```
[IDLE] --> [SENSE] --> [CLASSIFY] --> [ACTUATE] --> [RESET]
  ^                                                    |
  |____________________________________________________|
```

| State    | Action                                                        |
|----------|---------------------------------------------------------------|
| IDLE     | Wait for object to enter sensor field                         |
| SENSE    | Read R, G, B frequency values from TCS3200                    |
| CLASSIFY | Compare values to calibrated thresholds, assign colour label  |
| ACTUATE  | Send PWM signal to servo, rotate to target bin angle          |
| RESET    | Return servo to home position, clear variables, return to IDLE|

### Colour Classification Logic

The TCS3200 outputs a square wave whose frequency is proportional to light intensity for the selected colour filter. By cycling through R, G, B filters and reading pulse counts, the dominant colour is identified:

```
If   R_count < G_count AND R_count < B_count  -->  RED object
Elif G_count < R_count AND G_count < B_count  -->  GREEN object
Else                                          -->  BLUE object
```

Thresholds were calibrated empirically under controlled indoor lighting conditions.

### Servo Positions

| Colour | Servo Angle | Bin        |
|--------|-------------|------------|
| Red    | 45 degrees  | Left bin   |
| Green  | 90 degrees  | Centre bin |
| Blue   | 135 degrees | Right bin  |

---

## How to Run

### Requirements

- Arduino IDE 1.8+ or Arduino IDE 2.x
- Servo.h library (included with Arduino IDE by default)
- No additional libraries required

### Steps

1. Clone this repository:

```
git clone https://github.com/Maihadisiabu/colour-sorting-machine.git
```

2. Open colour_sorting_machine.ino in the Arduino IDE.

3. Wire the circuit as described in the Circuit Connections section above.

4. Calibrate the sensor: run the calibration_helper.ino sketch first to obtain your specific R, G, B frequency counts under your ambient lighting conditions. Update the threshold values in the main sketch accordingly.

5. Select Arduino UNO as the board and the correct COM port.

6. Upload the sketch and open the Serial Monitor (9600 baud) to observe real-time classification output.

---

## Results and Validation

The system was validated through repeated testing across 60 sorting cycles (20 per colour):

| Colour  | Correct Sorts | Errors | Accuracy |
|---------|---------------|--------|----------|
| Red     | 19/20         | 1      | 95%      |
| Green   | 20/20         | 0      | 100%     |
| Blue    | 18/20         | 2      | 90%      |
| Overall | 57/60         | 3      | 95%      |

Key observations:

- Classification errors occurred primarily under high-intensity ambient light, causing sensor saturation
- Actuator response time (sensor trigger to servo at target angle): less than 300 ms consistently
- System operated continuously for 2+ hours without fault in final demonstration

---

## Research Relevance

This project directly applies principles foundational to:

- Robotic pick-and-place automation: sensor-guided sorting with closed-loop feedback
- Industrial conveyor systems: real-time embedded decision-making under timing constraints
- Agricultural robotics: automated crop and fruit grading by colour (ripeness detection)
- Embedded real-time control: FSM architecture applicable to any reactive autonomous system

---

## Author

Abubakar Rabiu Maihadisi
R.Engr. (COREN) | AMICDFA | B.Eng. Computer Engineering
Bayero University, Kano -- Capstone Project, 2019-2020
Email: maihadisiabubakar@gmail.com
GitHub: https://github.com/Maihadisiabu
