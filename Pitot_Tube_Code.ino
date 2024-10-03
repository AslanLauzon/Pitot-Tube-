#include <Arduino.h> 

#include <SD.h> 

#include <SPI.h> 

#include "Adafruit_SHT4x.h" 

 

#define sensorPin A0 // The pin where the sensor output is connected 

 

// Constants for NPA-500B-030D sensor 

const float Out_min = 0.5; // Sensor's minimum output voltage in Volts 

const float Out_max = 4.5; // Sensor's maximum output voltage in Volts 

const float P_min = 0;     // Sensor's minimum pressure in PSI 

const float P_max = 30.0;  // Sensor's maximum pressure in PSI 

const float manual_offset = -21.7; // Manual offset to zero the sensor 

 

// Initialize the SHT4x sensor 

Adafruit_SHT4x sht4 = Adafruit_SHT4x(); 

File myFile; 

 

unsigned long lastWriteTime = 0; // Time since last SD write 

const unsigned int writeInterval = 25; // Time interval to write data (in milliseconds) 

 

void setup() { 

  Serial.begin(9600); 

  Serial.print("Initializing SD card..."); 

 

  if (!SD.begin(10)) { 

    Serial.println("Initialization failed!"); 

    while (1); 

  } 

  Serial.println("Initialization done."); 

 

  if (!sht4.begin()) { 

    Serial.println("Sensor failed to initialize!"); 

    while (1); 

  } 

  Serial.println("Sensor initialized."); 

 

  // Open the file for writing 

  myFile = SD.open("Datalog.txt", FILE_WRITE); 

  if (myFile) { 

    myFile.println("TIME,TEMP,HUMIDITY,PRESSURE"); 

    Serial.println("File header written."); 

  } else { 

    Serial.println("Error opening Datalog.txt"); 

  } 

} 

 

void loop() { 

  sensors_event_t humidity, temp; 

  sht4.getEvent(&humidity, &temp); // Get data 

 

  // Read the sensor value 

  int sensorValue = analogRead(sensorPin); 

  // Convert the sensor reading to voltage 

  float voltage = sensorValue * (5.0 / 1023.0); 

  // Convert the voltage to pressure in PSI 

  float pressure_PSI = P_min + ((voltage - Out_min) / (Out_max - Out_min)) * (P_max - P_min); 

  // Adjust the pressure by the manual offset 

  pressure_PSI += manual_offset; 

 

  unsigned long now = millis(); 

 

  if ((now - lastWriteTime) >= writeInterval) { 

    if (myFile) { 

      myFile.print(now); 

      myFile.print(","); 

      myFile.print(temp.temperature); 

      myFile.print(","); 

      myFile.print(humidity.relative_humidity); 

      myFile.print(","); 

      myFile.println(pressure_PSI); 

 

      myFile.flush(); // Ensure data is written to the SD card 

      lastWriteTime = now; // Update last write time 

    } 

    Serial.print("Temp: "); Serial.print(temp.temperature); 

    Serial.print(" Humidity: "); Serial.print(humidity.relative_humidity); 

    Serial.print(" Pressure: "); Serial.println(pressure_PSI); 

  } 

 

  delay(7); // Short delay to manage sampling rate 

} 

 
