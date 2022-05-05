#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
// Sensor Pins
const int sensorRead = 0;

// Pin for MOSFET activation
const int gatePin = 2;  

// Distance Value
int distValue = 0; 

/*Final System Value -- This gets has the output incremented onto itself, then is written onto the electromagnet through PWM.  
 *This is just like how on the Simulink system, you'd see the PID values being summed together to go into the plant and watching the plant's results from a scope.
 *Byte, which is an Arduino 8-bit unsigned data type, was used as a "limiter" to limit the values being incremented onto it. 
 *As it is being used as PWM to write onto the electromagnet, byte was taken advantage of as it only an 8 bit unsigned number, meaning it at any time it only exists on the positive end(unsigned) and is limited to 255 (8 bits)
*/
byte outputValue = 0;   

// Setpoint, this is set through calibration on the distance sensors to set a distance where the electromagnet is able to be "felt" on the object, but at the same time not be too close to the electroagnet. Thus finding a middle ground to levitate freely.
int setpoint = 120; 

// Delay Value
int delayValue = 10;  

// Time Counter
float timeCounter = 0; 

// Incrementing Variable
byte counter = 0;

// PID Values, each variable below is responsible for tuning the entire electromagnetic levitation system. The proportional gain, integral and derivative all have different gain values received from Simulink.
float output = 0;
float integral = 0;
float derivative = 0;
float error = 0;
float previous_error = 0;
float dt = 0.1;
float Kp = 100.00;
float Ki = 23.00;
float Kd = 0.01;

// Misc Print Character
char out[50];

void setup()
{
  // Serial initialisation to read sensory data on Serial Monitor
  Serial.begin(74880);
  // Wire Initialisation to allow Arduino to read Sensor Data from Lidar/VL53L0X
  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("No sensor found!");
    while (1) {}
  }
  sensor.startContinuous();
  pinMode(gatePin, OUTPUT);

  // Set to millis, intended to count total time of program run time
  timeCounter = millis();
}

void timeRelease() {
  // This function is for future purposes, as it would be combined with an LCD screen to put timeCounter's total runtime on it and show it on the LCD. 
  // Setting a time limit on it would have been good, as after, e.g. 5 minutes, the LCD would warn the user of heat damage.
  if (counter == 254) {
    //write timeCounter onto LCD
    counter = 0;
  }
}

void loop() 
{
  counter++;
  // Distance Sensor Read: The distance sensor's values get printed onto Serial Monitor to see the object's distance.
  distValue = sensor.readRangeContinuousMillimeters();
  //distValue = analogRead(sensorRead);
  // Preprocessor directive, this is just to test if the potentiometer can be used to add and subtract from the setpoint without having to restart the program.
  // Setting it as preprocessor means that it naturally gets ignored by the IDE unless defined.
  #ifdef DIST_MANIPULATOR
  setpointAlter = analogRead(potentiometer);
  distValue += setpointAlter;
  #endif
  Serial.print("Sensor Read: ");
  Serial.println(distValue);
  /* PID calculations, dealing with the the current, rate of change and the steady state error of the system.
  Firstly, the error variable is set equal to the setpoint subtracted by the current sensor value. 
  The integral, which is purposed to built error over time is set equal to itself added with the error multiplied by time dt.
  Finally, the derivative is calculated. As it is based on the rate of change of the error, it subtracts the previous error from the current error and divides it by time dt.
  All of this, just like in any PID system, gets summed up in an output named "output". This output features the gains, which are negative due to the values as seen on Simulink, multiplied by each of their respective terms and summed into an output.
  After this entire PID process is complete, it passes the current error as a previous error, which will be used in the next loop.
  */
  error = setpoint - distValue;
  integral = integral + error * dt;
  derivative = (error - previous_error) / dt;
  output = (-Kp * error) + (-Ki * integral) + (-Kd * derivative);
  previous_error = error;

  /*
  As you would see between a controller and a plant in a Control System, the above processes feature the calculations done by a controller. Thus, the only thing left to do now is to have a system that increments these outputs into itself to then write
  onto the electromagnet, and perfectly enough there is a variable that can do that. As mentioned in the definitions section, the byte "outputValue" is limited from 0 to 255 for PWM, thus it deals with the above values and contains them within this short rnage.
  This then gets written through "analogWrite", which uses PWM(Pulse Width Modulation) to write the results of the outputs onto the "gatePin" MOSFET. This is then to be sent to the MOSFET to activate the electromagnet based on the values received here,
  making this a full-fledged closed loop system.
  */
  outputValue += output;
  
  // This Increases/Decreases the value for Electromagnet depending on outputValue's value.
  analogWrite(gatePin, outputValue);
  
  // Time between electromagnet state changes
  delayMicroseconds(delayValue);
}
