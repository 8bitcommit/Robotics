# Peacekeeper Bot

An autonomous, Nerf-firing UGV designed to detect, track, and engage red balloon targets using basic sensors, OpenCV, and a Raspberry Pi + Arduino combo.

---

## 📌 Project Overview

Peacekeeper Bot is a low-cost, multisensor robotic turret platform built for autonomous object detection and engagement. The bot uses computer vision to detect red balloons, aims using servo-driven control, and fires Nerf darts with precision. Its core system is powered by a Raspberry Pi (for vision and high-level control) and an Arduino (for actuation and motor control).

---

## 🎯 Features

- Autonomous roaming and obstacle avoidance using a VL53L0X ToF sensor
- Object recognition and target tracking via OpenCV
- Dynamic state management (roaming, aiming, shooting, pausing)
- Serial communication between Raspberry Pi and Arduino
- Shooting mechanism driven by Hitec servo motors and a Nerf blaster
- Command interface supporting:
  - `a`: roam
  - `s`: stop
  - `FIRE`: fire
  - `SLx` / `SRx`: slow left/right turn for x milliseconds

---

## 🧠 Architecture

### Hardware Components

- **Raspberry Pi 3**: Vision processing and control logic
- **Arduino Uno**: Motor and servo actuation
- **VL53L0X Distance Sensor**: ToF-based obstacle detection via I²C
- **DFRobot FIT0701 Camera**: USB camera for vision tracking
- **Hitec HSS-422 Servos**: Control flywheels and reloading arm
- **Sabertooth 12A Motor Driver**: Dual-channel motor control
- **Chassis**: 4-wheel drive base + 3D-printed turret mount

### Software Stack

- **Python (OpenCV, NumPy)**: For image processing and robot state management
- **C++ (Arduino)**: For servo control, driving, and sensor feedback
- **Serial Communication**: 115200 baud for command/control between Pi and Arduino

---

## 🧪 Test Summary

| Test | Description | Status |
|------|-------------|--------|
| Drive & rotate | Forward and CCW motion | ✅ (Theoretical due to hardware failure) |
| Obstacle detection | Detect object within 120–330mm | ✅ |
| Obstacle avoidance | Turn and continue | ✅ (Theoretical) |
| Object classification | Detect balloon vs. human | ✅ |
| Color detection | Red vs. blue balloons | ✅ |
| Aim | Align crosshair with red balloon | ✅ (Theoretical) |
| Fire | Hit red balloon | ✅ |

> ⚠️ One motor burnt out during testing, so movement was simulated.

---

## 📈 Future Improvements

- Add vertical aiming for aerial targets
- Integrate power monitoring to avoid sudden shutdowns
- Improve chassis durability and traction
- Refine targeting algorithm for better precision
- Add ROS integration for advanced autonomy

---

## 🧾 References

This project references work on SLAM, sensor fusion, UAV-UGV coordination, and targeting systems. For full citations, see the [`Final_Report.pdf`](./Final_Report.pdf).

---


