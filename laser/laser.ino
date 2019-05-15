/*
 * This is just code that we used to test our lasers and phototransistors. We used it to determine an appropriate threshold voltage for when a laser should be considered broken vs. unbroken.
 */

#include <SPI.h>
#include <TFT_eSPI.h>

void setup() {
  Serial.begin(115200); // Set up serial port
}

void loop() {
  int bins = analogRead(A0);
  float voltage = (bins/4096.0)*3.3;
  Serial.print("Voltage ");
  Serial.println(voltage);
  if(voltage < 0.9) {
    Serial.println("laser unbroken");   
  }
  else{
    Serial.println("laser broken");
  }

}
