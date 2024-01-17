/********************************************************************/
/* SMART_WATER_TANK.ino
 * Author: Gabriel Christopher
 * Date: 2023/12/28
 * Function: Monitors water level of the tank and controls the pump operation accordingly.
 * An automated water tank management system that regulates pump operation based on water levels, 
 * detects faults, and handles them by triggering an alarm.
 * This code is designed to work on ATtiny85 chip. It can be adapted for other microcontrollers.
 * Version: 1.1 */
/********************************************************************/

const int BUZZER_PIN      = 0;  // Buzzer
const int RELAY_PIN       = 1;  // Pump_relay
const int TDC_SENSOR_PIN  = 2;  // BDC_sensor
const int PUSH_BUTTON_PIN = 3;  // Push button
const int BDC_SENSOR_PIN  = 4;  // TDC_sensor

const int  FAULT_TIMEOUT = 10000;       // Timeout for sensor fault detection in milliseconds
const long INTERVAL      = 1000;        // Interval between beeps (in milliseconds)
long       beepDuration  = 0;           // Duration of each beep (in milliseconds)

unsigned long lastTimeTDCSensorWasHigh, // 49.71 days till overflow.
              lastTimeBDCSensorWasHigh,
              lastTimeTDCSensorWasLow,
              lastTimeBDCSensorWasLow,
              
              currentMillis,            // currentMillis declared globally for multiuse
              previousMillis,
              beepStartTime = 0;        // Stores the start time of the beep
              
int           TDC_sensorState,
              BDC_sensorState;
              
boolean       TDC_State,
              BDC_State,
              TDC_Fault,             
              BDC_Fault;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);     // BUZZER
  pinMode(RELAY_PIN, OUTPUT);      // RELAY & LED
  pinMode(TDC_SENSOR_PIN, INPUT);  // SENSOR TDC
  pinMode(BDC_SENSOR_PIN, INPUT);  // SENSOR BDC
  digitalWrite(RELAY_PIN, LOW);    // PUMP IS TURNED OFF INITIALLY
}
//------------------------------------------------------DATA FILTERING------------------------------------------------------
void sensorState_from_deviation() {
   currentMillis = millis();
   TDC_sensorState = digitalRead(TDC_SENSOR_PIN);
   BDC_sensorState = digitalRead(BDC_SENSOR_PIN);
   
   // Check for millis overflow and reset variables
   if (currentMillis < lastTimeTDCSensorWasHigh) {
       lastTimeTDCSensorWasHigh = currentMillis;
   }
   if (currentMillis < lastTimeTDCSensorWasLow) {
       lastTimeTDCSensorWasLow = currentMillis;
   }
   if (currentMillis < lastTimeBDCSensorWasHigh) {
       lastTimeBDCSensorWasHigh = currentMillis;
   }
   if (currentMillis < lastTimeBDCSensorWasLow) {
       lastTimeBDCSensorWasLow = currentMillis;
   }

   // Sensor deviation at TDC
   if (TDC_sensorState == HIGH) {
       lastTimeTDCSensorWasHigh = currentMillis;
   } else {
       lastTimeTDCSensorWasLow = currentMillis;
   }

   // Sensor deviation at BDC
   if (BDC_sensorState == HIGH) {
       lastTimeBDCSensorWasHigh = currentMillis;
   } else {
       lastTimeBDCSensorWasLow = currentMillis;
   }

   // Determine TDC_State based on deviation duration
   TDC_State = (currentMillis - lastTimeTDCSensorWasHigh <= 3000) && (currentMillis - lastTimeTDCSensorWasLow > 3000);

   // Determine BDC_State based on deviation duration
   BDC_State = (currentMillis - lastTimeBDCSensorWasHigh <= 3000) && (currentMillis - lastTimeBDCSensorWasLow > 3000);
}  
//------------------------------------------------------PUMP FUNCTION-------------------------------------------------------
void relayFunc_from_sensorState(){
   digitalWrite(RELAY_PIN, (TDC_sensorState == true && BDC_sensorState == true && !TDC_Fault && !BDC_Fault) ? HIGH : LOW);
}
//------------------------------------------------FAULT DETECTION & HANDLING------------------------------------------------
void detectAndHandleFaults(){
  // Fault detection for TDC sensor
  if (currentMillis - lastTimeTDCSensorWasHigh > FAULT_TIMEOUT && currentMillis - lastTimeTDCSensorWasLow > FAULT_TIMEOUT) {
    // TDC sensor fault detected, handle accordingly
    TDC_Fault = true;
    buzzerAlarm();
  }else{
    TDC_Fault = false;
  }
  // Fault detection for BDC sensor
  if (currentMillis - lastTimeBDCSensorWasHigh > FAULT_TIMEOUT && currentMillis - lastTimeBDCSensorWasLow > FAULT_TIMEOUT) {
    // BDC sensor fault detected, handle accordingly
    BDC_Fault = true;
    buzzerAlarm();
  }else{
    BDC_Fault = false;
  }
}
//---------------------------------------------------BUZZER ALARM SEQUENCE--------------------------------------------------
void buzzerAlarm(){
  // Buzzer alarm sequence for TDC & BDC fault
  beepDuration = TDC_Fault ? 50 : (BDC_Fault ? 500 : 0);
              
  if (TDC_Fault || BDC_Fault){
    // Check for interval between beeps
    if (currentMillis - previousMillis >= INTERVAL) {
      previousMillis = currentMillis;  // Update the previous time
      beepStartTime = currentMillis;   // Start the beep
      digitalWrite(BUZZER_PIN, HIGH);          // Turn the buzzer on
      }
    // Check for beep duration
    if (currentMillis - beepStartTime >= beepDuration) {
      digitalWrite(BUZZER_PIN, LOW);           // Turn the buzzer off
    }  
  }
}
//--------------------------------------------------------------------------------------------------------------------------
void loop() {
  sensorState_from_deviation();
  relayFunc_from_sensorState();
  detectAndHandleFaults();
}
