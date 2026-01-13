Note for README:

The content of this README was written entirely by me (Zoraiz). Grok AI was used solely to refine the language, eliminate grammatical and spelling errors, and enhance readability.

# Regenerative Braking Testbed (EVPTT)

Physical prototype that demonstrates **real regenerative braking** — converting mechanical braking energy into usable electrical energy instead of wasting it as heat.

First serious attempt at building a complete electromechanical system under real torque, voltage, and current stress.

## What It Does

- Drives a wheel using a **brushless DC (BLDC) motor**  
- Applies mechanical brake → coupled generator produces voltage  
- Regulates regenerated voltage and feeds it back toward the battery  
- Shows real-time values on a 20×4 LCD:
  - Battery voltage + percentage
  - Regenerated (charging) voltage
  - Current
  - Instant power (W)
  - Estimated speed (km/h)
- Ultrasonic sensor cuts power if an object gets too close (basic safety stop)

## Main Features

- True sensorless 6-step BLDC motor control with back-EMF detection  
- Real regenerative charging path with voltage regulation  
- Live power & energy recovery monitoring  
- Custom LCD icons (battery, panel, PWM, flash)  
- Simple obstacle detection → emergency power cut  

## Hardware Components

- 2× Arduino Uno (one for BLDC control, one for monitoring)  
- 36V battery pack  
- BLDC motor (drives the wheel)  
- DC dynamo/generator (used during braking)  
- Custom 3-phase ESC (IRF2104 drivers + parallel MOSFETs)  
- ACS712 current sensor (20A version)  
- 20×4 LCD display  
- HC-SR04 ultrasonic sensor  
- DC-DC buck converters  
- Speed control potentiometer  
- Custom welded steel frame  
- Rickshaw wheel  
- Mechanical brake lever/rod  

## Repository Files

## How to Upload & Run

**You need two separate Arduino boards**

**1. BLDC Control Arduino** (`BLDC_Ardunio_ESC.ino`)
- Connect to 3-phase driver board  
  (PWM outputs: pins 9, 10, 11 | BEMF inputs: pins 2, 3, 4)
- Open file in Arduino IDE
- Board → Arduino Uno
- Select correct COM port
- Upload
- After upload → connect to 36V supply (always use breaker/fuse!)
- Control motor speed with potentiometer connected to A0

**2. Monitoring Arduino** (`RPM_Battery_Ultra.ino`)
- Connect to:
  - ACS712 current sensor → A2
  - Voltage dividers → A0 (battery voltage), A1 (regen voltage)
  - LCD → pins 3,4,5,6,7,8
  - Ultrasonic → trigger pin 11, echo pin 10
  - Relay/output control → pin 9
- Open file in Arduino IDE
- Board → Arduino Uno
- Select correct COM port
- Upload

**After both are uploaded:**

- Power the whole system with 36V (with proper fuse/circuit breaker!)
- Turn the potentiometer → wheel should start spinning
- Watch battery %, voltages, current, power and speed on LCD
- Apply mechanical brake → charging voltage & current should appear on display

## Important Safety Notes

- **High voltage and current** — work with extreme care  
- Wheel must be very securely fixed  
- Always use circuit breaker or fuse on battery side  
- Never touch power stage/MOSFETs while system is powered  
- Test in a safe, open area  


Made to understand how real regenerative braking works.  
— Zoraiz
