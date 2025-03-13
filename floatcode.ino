

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MS5837.h>
#include <Stepper.h>
#include <Wire.h>

const String companyNum = "RN16"; 

MS5837 sensor;
const int stepsPerRevolution = 5000;  
const int dirPin = 2;  // Example: GPIO14 instead of D5
const int stepPin = 0;  // Example: GPIO12 instead of D4
const int enablePin = 13; // Example: GPIO13 instead of D3
const int stepReps = 6; //stepper motor num of rotations

Stepper myStepper(stepsPerRevolution, dirPin, stepPin);

const char* ssid = "TP-LINK_643A";     
const char* password = "78845558";
float pressure = 0.0;
float depth = 0.0;
const float pressureThreshold = 1307.55;  // estimated
bool mounted = false;

WiFiServer server(80); // TCP server on port 80


struct DataPoint {
  long time;
  float depth;
  float pressure;
};


DataPoint dataPoints[240];
int dataPointIndex = 0;
unsigned long lastDataPointTime = 0;
unsigned long startingTime = 0;


void setup() {
  Serial.begin(9600);
  delay(100);

  pinMode(enablePin, OUTPUT); 
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  digitalWrite(enablePin, HIGH);

  // push out
  /*
  digitalWrite(dirPin, LOW);
  // make it three if it dont work
  for (int i =0 ; i < 2; i++) {
  digitalWrite(enablePin, LOW);
    // Spin motor quickly
    for (int i = 0; i < stepReps/2; i++) {
      for(int x = 0; x < stepsPerRevolution; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
      }
      delay(500);
    }
    delay(5000); // Replace delay(20000) with Stall(20000), adjust time as needed
  }
  digitalWrite(enablePin, HIGH);
  */

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin(); // Start the server

  lastDataPointTime = 0; // Initialize the timer for data collection


}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return; // No client connected, skip the rest of the loop
  }

  // Wait for data from client
  while(!client.available()){
    delay(1);
  }

  String request = client.readStringUntil('\n');
  request.trim(); // Remove any newline characters


  if (request.equals("mount")) {
      // mounted = true;
      //  Wire.begin(4, 5);

      // if (!sensor.init()) {
      //   Serial.println("Sensor init failed!");
      //   while(1);  
      // }
      // sensor.setModel(MS5837::MS5837_30BA);
      // sensor.setFluidDensity(992.7); // density for chlorine
      Serial.println("Kaito = Clown");

  }

  // if (request.equals("in")) {  
  //      digitalWrite(dirPin, HIGH);
  //     for (int i =0; i < 2; i++) {
  //       digitalWrite(enablePin, LOW);


  //       // Spin motor quickly
  //       for (int i = 0; i < stepReps/2; i++) {
  //         for(int x = 0; x < stepsPerRevolution; x++)
  //         {
  //           digitalWrite(stepPin, HIGH);
  //           delayMicroseconds(1000); // Short delays for stepping are okay
  //           digitalWrite(stepPin, LOW);
  //           delayMicroseconds(1000); // Short delays for stepping are okay
  //         }
  //         delay(500);
  //       }
  //       digitalWrite(enablePin, HIGH);


      
  //       delay(5000); // Replace delay(20000) with Stall(20000), adjust time as needed
  //     }
  // }
  // if (request.equals("plot")) {
  //       client.println(logData());
  // }
  // if (request.equals("float")) {
    

  //   if (!mounted) {
  //     Wire.begin(4, 5);

  //     if (!sensor.init()) {
  //       Serial.println("Sensor init failed!");
  //       while(1);  
  //     }
  //     sensor.setModel(MS5837::MS5837_30BA);
  //     sensor.setFluidDensity(992.7); // density for chlorine

  //     // floating set up
  //     digitalWrite(enablePin, LOW);

  //     digitalWrite(dirPin, LOW);
  //     for (int i =0 ; i < 2 ; i++) {
  //         // Spin motor quickly
  //         for (int i = 0; i < stepReps/2; i++) {
  //           for(int x = 0; x < stepsPerRevolution; x++)
  //           {
  //             digitalWrite(stepPin, HIGH);
  //             delayMicroseconds(1000);
  //             digitalWrite(stepPin, LOW);
  //             delayMicroseconds(1000);
  //           }
  //           delay(500);
  //         }
  //     }
  //     mounted = true;
      
  //     digitalWrite(enablePin, HIGH);

  //   }
  //   startingTime = millis();
  //   lastDataPointTime = startingTime;
  //   updateSensors();
  //   client.println("Company Number: " + companyNum + ", Time: 0, Pressure: " + String(pressure) + ", Depth: " + String(depth));
        
  //   dataPoints[dataPointIndex++] = {0, depth, pressure};

  //   Stall(8000);
  //   // Execute the float operation

  //   FloatDown();
  //   digitalWrite(enablePin, HIGH);
  //   reconnectToWiFi();


  //   // Stall(10000);
  //   // Send data back to client
  // }
  
}

void reconnectToWiFi() {
    int fails = 0;
    int runs = 0;
    bool pushed = true;
    WiFi.disconnect();
    Stall(5000);
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting to WiFi...");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            Stall(500);
            Serial.print(".");
            fails++;
            if (fails > 30) {
              if (runs < 1) {
                failSafe();
                runs++;
              } else {
                pushed = false;
                break;
              }
              fails = 0;              
            }
        }

        
        if (!pushed) {
          while (WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnecting to WiFi...");
            WiFi.begin(ssid, password);
            Stall(5000);
            FloatUp();
          }
        }
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }
    Serial.println("DONE!");
}

void FloatDown() {
  // initial push out first

  digitalWrite(dirPin, HIGH);

  for (int i =0; i < 2; i++) {
    digitalWrite(enablePin, LOW);


    // Spin motor quickly
    for (int i = 0; i < stepReps/2; i++) {
      for(int x = 0; x < stepsPerRevolution; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000); // Short delays for stepping are okay
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000); // Short delays for stepping are okay
      }
      delay(500);
    }
    digitalWrite(enablePin, HIGH);


   
    Stall(5000); // Replace delay(20000) with Stall(20000), adjust time as needed
  }
  Stall(30000); // Replace delay(20000) with Stall(20000), adjust time as needed

  FloatUp();  
}


void Stall(int stallAmt) {
  unsigned long start = millis()-startingTime;
  while(millis()-startingTime - start < stallAmt) {
    logDataPoint(); // Check if it's time to log a data point
    delay(50);
  }
}


void FloatUp() {

   digitalWrite(dirPin, LOW);
   // make it three if it dont work
   for (int i =0 ; i < 2 ; i++) {
    digitalWrite(enablePin, LOW);
      // Spin motor quickly
      for (int i = 0; i < stepReps/2; i++) {
        for(int x = 0; x < stepsPerRevolution; x++)
        {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(1000);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(1000);
        }
        delay(500);
      }
      digitalWrite(enablePin, HIGH);


      Stall(5000); // Replace delay(20000) with Stall(20000), adjust time as needed
   }
}

String logData() {

  // Append each data point, formatted as time:depth, separated by semicolons
  String dataPacket = companyNum + "|"; // Start with the company number, then a delimiter

  bool isFirstPoint = true; // Flag to manage separators without leading semicolon

  for (int i = 0; i < dataPointIndex; i++) {
      // Skip appending the current data point if it has the same time as the next one (and is not the last point)
      if (i != dataPointIndex - 1 && dataPoints[i + 1].time == dataPoints[i].time) {
          continue;
      }

      // Add a semicolon before appending data except for the first valid data point
      if (!isFirstPoint) {
          dataPacket += ";"; 
      } else {
          isFirstPoint = false; // Update the flag after the first valid data point is appended
      }

      // Append the data point, formatted as time:depth:pressure
      dataPacket += String(dataPoints[i].time) + ":" + String(dataPoints[i].depth) + ":" + String(dataPoints[i].pressure);
  }


  // Reset all data points to zero
  for (int i = 0; i < 240; i++) {
    dataPoints[i].time = 0;
    dataPoints[i].depth = 0.0;
    dataPoints[i].pressure = 0.0;
  }

  // Reset the index to start logging new data points at the beginning of the array
  dataPointIndex = 0;

  return dataPacket;
}

void logDataPoint() {
  unsigned long currentTime = millis();  // Time since start
  unsigned long nextDataPointTime = lastDataPointTime + 1000;  // Schedule the next data point time


  if (dataPointIndex < 240 && currentTime >= nextDataPointTime) {
    updateSensors(); // Update sensors to get the latest readings

    // Update lastDataPointTime for the next call (keeps it at 5-second intervals)
    lastDataPointTime = nextDataPointTime;

    // Calculate output time in seconds and adjust to be a multiple of 5
    unsigned long output = (currentTime / 1000)-startingTime/1000;

    // Store the new data point
    dataPoints[dataPointIndex++] = {output, depth, pressure};
    Serial.print("Logged at: ");
    Serial.print(output);
  }
}



void updateSensors() {
  sensor.read(); 
  pressure = sensor.pressure();  
  depth = sensor.depth(); 
  pressure *= 0.1;
}

float pressureToDepth(float pressure) {
  return (pressure - 1013.25) / 10.0;
}

void failSafe() {
  digitalWrite(dirPin, HIGH);

  for (int i =0; i < 1; i++) {
    digitalWrite(enablePin, LOW);


    // Spin motor quickly
    for (int i = 0; i < stepReps/2; i++) {
      for(int x = 0; x < stepsPerRevolution; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000); // Short delays for stepping are okay
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000); // Short delays for stepping are okay
      }
      delay(500);
    }
    digitalWrite(enablePin, HIGH);


   
    Stall(5000); // Replace delay(20000) with Stall(20000), adjust time as needed
