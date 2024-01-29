# water_tank_management
An automated water tank filling system utilizes dual XKC-Y25 NPN non-contact liquid-level sensors for precise water level detection, with Attiny85 controlling the pump's relay to ensure efficient and reliable water management operations.

**Hardware Setup:**

1. **Sensor Placement:** Securely mount two XKC-Y25 sensors inside your water tank:
    - One at the Top Dead Center (TDC).
    - One at the Bottom Dead Center (BDC).
2. **Pump Connection:** Connect your water pump to a 5V SPDT relay.
3. **Power Supply for Relay:**
    - Power the relay using the 5V output from your microcontroller board.
    - Connect the relay's signal pin to the specified pin on your Attiny85 microcontroller according to the provided program.
4. **Power Supply for Attiny85:** Provide a constant external power source to your microcontroller, such as an 5v AC-DC adapter.

**Software Configuration:**

1. **Threshold Calibration:** The water flow in the tank may cause fluctuations in sensor readings due to water movement. To improve reliability, we're creating a time-based threshold for the deviation. The acceptable deviation duration needs to be adjusted based on your pump's discharge rate and the size of your tank.
2. **Calculate Discharge Rate:**
    - If you know your pump's discharge rate, skip this step.
    - Otherwise, follow these steps:
        - **Measure Filling Time:** Use a stopwatch to measure the time it takes to completely fill your water tank from empty.
        - **Calculate Discharge Rate:** Use the formula `Q = V/t`, where:
            - `Q` is the discharge rate (volume per unit time).
            - `V` is the volume of your tank (e.g., 1000 liters).
            - `t` is the measured filling time (seconds).
        - For example, if it takes 300 seconds (5 mins) to fill your 500-liter tank, the discharge rate (Q) would be:
        - Q = 500 liters/300 seconds ≈ 1.67 liters/seconds
    - This calculation provides an average flow rate in the tank and not the pump and also may not account for variations in pump's efficiency or changes in flow rate over time.
3. **Determine Deviation Allowance:** Based on the calculated discharge rate and tank size, estimate the time it takes for water to flow from BDC to TDC. Adjust the threshold allowances (currently set to 3 seconds) in the program code to accommodate this estimated time, ensuring reliable detection.

   
