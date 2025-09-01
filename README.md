# ThreeChannelDroneJammer
Final Project Electrical Engineer Three Channel Drone Jammer - Afeka Academic College
# Three Channel Drone Jammer - Final Project

## Executive Summary

This project provides a defensive solution against the growing misuse of drones in areas such as espionage, sabotage, data theft, and privacy violations.  
The system neutralizes drones using a **Soft Kill** technique – disrupting communication and navigation signals rather than physically destroying the drone.  

---

## 📂 Repository Structure

- **Main Program/**  
  The main software, written in C++ with Qt.  
  This program includes the **GUI** and manages all communication with the **LimeSDR Mini 2.0**.

- **usb2any_helper/**  
  A helper process compiled in **32-bit** mode.  
  This is required to enable communication with the **TI LMX2492EVM** PLL + VCO board.

- **Project Report.pdf**  
  Full project documentation (design, methods, results, and conclusions).

- **Hardware Setup.jpg**  
  Internal view of the jammer hardware.

- **Full System.jpg**  
  Field test setup with the laptop, jammer, antennas, and target drone.

---

## ⚙️ Requirements

To build and run the project you will need:

- **Visual Studio 2022** (or later) with C++ support  
- **Qt Framework** (for the UI files)  
- **LimeSuite / LimeSDR drivers** (for using the LimeSDR Mini 2.0 hardware)  
- **USB2ANY drivers and SDK** (for communication with the TI LMX2492EVM board)  

---
