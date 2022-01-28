#define PSignal 2
#define IN1 3
#define IN2 4
#define PSignal2 5
#define IN3 6
#define IN4 7

// the setup routine runs once when you press reset:
void setup() {

  pinMode(PSignal, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValueA = analogRead(A0);
  int sensorValueB = analogRead(A1);
  // print out the value you read:
  Serial.print("SensorA: ");
  Serial.print(sensorValueA);
  Serial.println();
  Serial.print("SensorB: ");
  Serial.println(sensorValueB);
  Serial.println();
  delay(1);        // delay in between reads for stability

  if (sensorValueA < 500) {
    // Setting in a rangeof between 500 and 520 allows for constant spin on each opposite range, but setting it just on 520 makes it react instantly to the magnet's polarity
    digitalWrite(PSignal, HIGH);
    digitalWrite(PSignal2, HIGH);
    digitalWrite(IN1, sensorValueA);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    //Serial.print("Value Greater than 520 for Sensor A");
    //Serial.println();
  } else if (sensorValueB < 500) {
   digitalWrite(PSignal, HIGH);
    digitalWrite(PSignal2, HIGH);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    //Serial.print("Value Less than 520 for Sensor A");
    //Serial.println();
  }
}