# Intelligent-Tunnel-Monitoring-System
The Intelligent Tunnel Monitoring System leverages FreeRTOS to implement multithreading and concurrency, ensuring responsiveness and reliability in a resource-constrained environment therefore enabling real-time multitasking for:
   - Sensor monitoring (vehicle entry/exit, gas/smoke detection).
   - Emergency alerts and traffic control.
   - User feedback via LEDs and the Serial Monitor.

This project implements an intelligent tunnel monitoring system using an Arduino Mega 2560 microcontroller and sensors to detect vehicles, hazardous gases, and smoke. The system enhances traffic safety and efficiency by tracking vehicle movement, enforcing capacity limits, and triggering alerts for emergencies. It uses FreeRTOS for real-time multitasking and provides visual feedback via LEDs and the Serial Monitor.

WOKWI Simulation : https://wokwi.com/projects/420279293544719361
![Screenshot 2025-03-18 134125](https://github.com/user-attachments/assets/00e4a43b-388e-43dc-9955-95168852d82e)

🚀 FEATURES
 - Traffic Monitoring: Detects vehicle entry/exit on two lanes using PIR sensors.
 - Vehicle Counting: Tracks vehicles in the tunnel with a maximum limit of 3 vehicles per lane .
 - Emergency Alerts: Activates a yellow LED for smoke/gas detection or manual panic button activation.
 - Real-Time Feedback: Displays vehicle counts and alerts in the Serial Monitor.
 - Manual Control: A panic button toggles emergency mode, overriding normal operations.


⚒️ HARDWARE COMPONENTS
 - Arduino Mega 2560: Manages all tasks and sensor inputs.
 - PIR Sensors :
   - SENSOR_ENTRY_1/2: Detect vehicle entry on each lane.
   - SENSOR_EXIT_1/2: Detect vehicle exit on each lane.
 - LEDs :
   - White : Indicates vehicles present in each lane.
   - Green : Flashes when a vehicle enters.
   - Red : Flashes when a vehicle exits.
   - Yellow : Activates during alerts (gas/smoke detected or panic mode).
 - Potentiometers :
   - POTENTIOMETER_GAS: Simulates hazardous gas levels.
   - POTENTIOMETER_SMOKE: Simulates smoke levels.
 - Panic Button : Manually triggers emergency alerts.


💻 SOFTWARE COMPONENTS
 - Arduino IDE : For coding and uploading to the microcontroller.
 - FreeRTOS : Manages multitasking via concurrent tasks.
 - Serial Monitor : Displays real-time data (vehicle counts, alerts).


🚗 TEST CASES

The code includes four test cases to validate system functionality. Uncomment the desired test case in the setup() function to run it.

✅Test Case 1: "Two Vehicles Enter Lane 1".
 - Simulates two vehicles entering Lane 1.
 - Expected Result :
   - The vehicle count for Lane 1 increases to 2.
   - Green LED flashes twice, white LED stays on.

✅Test Case 2: "Multi-Lane Traffic with Safe Gas Levels"
 - Simulates four vehicles (two per lane) entering/exiting and gas level set to 650 (below threshold).
 - Expected Result :
   - Vehicle counts update correctly.
   - No gas alert triggers (yellow LED remains off).

✅Test Case 3: "Gas Leak Detection and Block Entry"
 - Simulates two vehicles entering Lane 1 and as level set to 950 (above threshold).Third vehicle entry is blocked.
 - Expected Result :
   - Vehicle count stops at 2.
   - Gas alert triggers (yellow LED on).

✅Test Case 4: "Panic Button Activation"
 - Simulates a vehicle entering Lane 1.Panic button is pressed to trigger emergency mode.The vehicle exits, panic button is pressed again to deactivate.
 - Expected Result :
   - Panic mode toggles, yellow LED activates/deactivates
   - Serial Monitor logs panic events.
