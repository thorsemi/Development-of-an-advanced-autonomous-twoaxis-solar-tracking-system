# Development-of-an-advanced-autonomous-twoaxis-solar-tracking-system
Autonomous dual-axis solar tracker using actuator &amp; stepper control, GPS/compass input, and SunPy-based sun positioning. Features Raspberry Pi–Arduino communication, MQTT/HTTP protocols, and Node-RED dashboard for monitoring &amp; control.

![1739203162932](https://github.com/user-attachments/assets/18fc3c99-fcf5-427c-ac71-d5c0f5a99bb2)

## Files 
MotorController__-_Stepper_motor
- This module contains the control logic for the stepper motor. It includes initialization of pins, encoder feedback for precise angular positioning, and safety routines to ensure stable operation under varying loads.

MotorController_-_Akuator
- This module handles the control of the LA36 actuator. It converts desired tilt angles into voltage signals, reads feedback voltages, and applies calibration functions to ensure accurate linear positioning.


Navigation_Module_1
- This module integrates GPS and compass data to determine the current position and orientation of the solar tracker. It parses NMEA sentences, calibrates heading, and provides latitude/longitude values for further processing.


SolarPy Algoritme
- This Python module implements the solar position algorithm. Based on geographic coordinates and time, it calculates azimuth and altitude angles, which are then published via MQTT to the motor controllers for real-time tracking.
  
Userinterface
- This module provides the Node-RED based user interface. It visualizes system status, environmental data, manual/automatic control options, and modes such as WindSafe and SnowClear for protective operation.


## CAD 
All mechanical components of the solar tracker were designed and modeled in SolidWorks, including the frame, base, and moving platform. The CAD work ensured correct integration of actuators and stepper motors, while load-bearing structures were dimensioned according to wind and snow load requirements. SolidWorks simulations were used to evaluate stresses, deflections, and safety factors, providing a reliable digital twin before prototyping. In addition, the software enabled the creation of assembly drawings and a complete Bill of Materials (BOM), which guided both sourcing and manufacturing.
![High](https://github.com/user-attachments/assets/0838fa04-7584-4766-96a2-39260b3518dc)

## Userinterface
The user interface was developed in Node-RED to provide both monitoring and control of the solar tracker. It includes a control panel with multiple operating modes, such as WindSafe and SnowClear, as well as manual and autonomous tracking modes. Real-time data, including environmental conditions and system status, is automatically updated and visualized through dashboards. The interface also integrates mapping functionality to show location and orientation, while Node-RED manages communication between all modules via MQTT and serial protocols, ensuring seamless interaction across devices.

![Oversigt overview med trun on](https://github.com/user-attachments/assets/ffadf97d-d8f7-41ee-bcbc-125c76575e87)


### Sponsors
This project was made possible through the generous support of both national and international companies. Their contributions enabled the development of a full-scale prototype by providing essential components, funding, and expertise. I am sincerely grateful for their involvement and commitment, which played a crucial role in the successful completion of my bachelor thesis.

Project Sponsors:
![Skærmbillede 2025-08-20 194859](https://github.com/user-attachments/assets/230a9b36-540c-43ec-a875-e37b6a219dcf)

