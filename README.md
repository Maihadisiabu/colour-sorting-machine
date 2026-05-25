# Autonomous Colour Sorting Machine

> **Capstone Research Project â€” B.Eng. Computer Engineering**  
> Bayero University, Kano Â· 2019â€“2020

An embedded real-time system that autonomously detects, classifies, and physically sorts coloured objects using a TCS3200 RGB colour sensor and a servo-motor actuator network â€” all controlled by a finite state machine implemented in C/C++ on an Arduino microcontroller.

---

## Table of contents

- [Overview](#overview)
- [System architecture](#system-architecture)
- [Hardware components](#hardware-components)
- [Circuit connections](#circuit-connections)
- [Software design](#software-design)
- [How to run](#how-to-run)
- [Results & validation](#results--validation)
- [Research relevance](#research-relevance)
- [Author](#author)

---

## Overview

The goal of this project was to design and build a fully autonomous object-sorting system without human intervention. The system continuously monitors incoming objects via the TCS3200 colour sensor, classifies them as **Red**, **Green**, or **Blue**, and routes them to the corresponding bin using a servo motor â€” all in real time.

This project demonstrates:
- Closed-loop embedded system design (sense â†’ decide â†’ actuate)
- Finite state machine (FSM) architecture in firmware
- Real-time interrupt-driven I/O
- Quantitative hardware validation and testing

---

## System architecture

```
â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚                   Arduino UNO                        â”‚
â”‚                                                      â”‚
â”‚  TCS3200 Colour Sensor â”€â”€â–º Colour Classification    â”‚
â”‚                             FSM Logic               â”‚
â”‚                                â–¼                    â”‚
â”‚                        Servo Motor Control           â”‚
â”‚                        (PWM signal output)          â”‚
â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜

Flow:
Object detected â†’ Sensor reads RGB values â†’ FSM classifies colour
    â†’ Servo rotates to correct bin position â†’ System resets for next object
```

---

## Hardware components

| Component | Model | Quantity | Function |
|---|---|---|---|
| Microcontroller | Arduino UNO (ATmega328P) | 1 | Central processing & control |
| Colour sensor | TCS3200 | 1 | RGB light-to-frequency conversion |
| Servo motor | SG90 / MG995 | 1 | Mechanical sorting actuator |
| Conveyor/chute | Custom-built | 1 | Object delivery mechanism |
| Power supply | 5V DC | 1 | System power |
| Connecting wires | Jumper wires | â€” | Circuit interconnections |
| Breadboard | Full-size | 1 | Prototyping |

---

## Circuit connections

### TCS3200 â†’ Arduino UNO

| TCS3200 Pin | Arduino Pin | Description |
|---|---|---|
| VCC | 5V | Power supply |
| GND | GND | Ground |
| S0 | Pin 4 | Frequency scaling bit 0 |
| S1 | Pin 5 | Frequency scaling bit 1 |
| S2 | Pin 6 | Colour filter select bit 0 |
| S3 | Pin 7 | Colour filter select bit 1 |
| OUT | Pin 8 | Frequency output |
| OE | GND | Output enable (active LOW) |

### Servo Motor â†’ Arduino UNO

| Servo Wire | Arduino Pin | Description |
|---|---|---|
| Red (VCC) | 5V | Power |
| Brown (GND) | GND | Ground |
| Orange (Signal) | Pin 9 | PWM control signal |

> **Wiring note:** Frequency scaling is set to 20% (S0=HIGH, S1=LOW) for optimal indoor ambient light performance.

---

## Software design

### Finite State machine

The firmware is structured as a 5-state FSM:

```
[IDLE] â”€â”€â–º [SENSE] â”€â”€â–º [CLASSIFY] â”€â”€â–º [ACTUATE] â”€â”€â–º [RESET]
  â–²                                                      â”‚
  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
```

| State | Action |
|---|---|
| IDLE | Wait for object to enter sensor field |
| SENSE | Read R, G, B frequency values from TCS3200 |
| CLASSIFY | Compare values to calibrated thresholds â†’ assign colour label |
| ACTUATE | Send PWM signal to servo â†’ rotate to target bin angle |
| RESET | Return servo to home position, clear variables, return to IDLE |

### Colour classification logic

The TCS3200 outputs a square wave whose frequency is proportional to light intensity for the selected colour filter. By cycling through R, G, B filters and reading pulse counts, the dominant colour is identified:

```
If   R_count < G_count AND R_count < B_count  â†’  RED object
Elif G_count < R_count AND G_count < B_count  â†’  GREEN object
Else                                          â†’  BLUE object
```

Thresholds were calibrated empirically under controlled indoor lighting conditions.

### Servo positions

| Colour | Servo Angle | Bin |
|---|---|---|
| Red | 45Â° | Left bin |
| Green | 90Â° | Centre bin |
| Blue | 135Â° | Right bin |

---

## How to run

### Requirements

- Arduino IDE 1.8+ or Arduino IDE 2.x
- `Servo.h` library (included with Arduino IDE)
- No additional libraries required

### Steps

1. Clone this repository:
   ```bash
   git clone https://github.com/Maihadisiabu/colour-sorting-machine.git
   ```

2. Open `colour_sorting_machine.ino` in the Arduino IDE.

3. Wire the circuit as described in the [Circuit connections](#circuit-connections) section.

4. **Calibrate the sensor** â€” run the `calibration_helper.ino` sketch first to obtain your specific R, G, B frequency counts under your ambient lighting conditions. Update the threshold values in the main sketch accordingly.

5. Select **Arduino UNO** as the board and the correct COM port.

6. Upload the sketch and open the Serial Monitor (9600 baud) to observe real-time classification output.

---

## Results & validation

The system was validated through repeated testing across 60 sorting cycles (20 per colour):

| Colour | Correct Sorts | Errors | Accuracy |
|---|---|---|---|
| Red | 19/20 | 1 | 95% |
| Green | 20/20 | 0 | 100% |
| Blue | 18/20 | 2 | 90% |
| **Overall** | **57/60** | **3** | **95%** |

**Key observations:**
- Classification errors occurred primarily under high-intensity ambient light, causing sensor saturation
- Actuator response time (sensor trigger â†’ servo at target angle): < 300 ms consistently
- System operated continuously for 2+ hours without fault in final demonstration

---

## Research relevance

This project directly applies principles foundational to:
- **Robotic pick-and-place automation** â€” sensor-guided sorting with closed-loop feedback
- **Industrial conveyor systems** â€” real-time embedded decision-making under timing constraints
- **Agricultural robotics** â€” automated crop/fruit grading by colour (ripeness detection)
- **Embedded real-time control** â€” FSM architecture applicable to any reactive autonomous system

---

## Author

**Abubakar Rabiu Maihadisi**  
R.Engr. (COREN) Â· AMICDFA Â· B.Eng. Computer Engineering  
Bayero University, Kano â€” Capstone Project, 2019â€“2020  
ðŸ“§ maihadisiabubakar@gmail.com Â· ðŸŒ [GitHub Profile](https://github.com/Maihadisiabu)
