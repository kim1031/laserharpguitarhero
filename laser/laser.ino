#include <SPI.h>
#include <TFT_eSPI.h>

//TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200); // Set up serial port
  /*tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_WHITE); //fill background
  tft.setTextColor(TFT_BLACK, TFT_WHITE); //set color for font
  */
}

void loop() {
  int bins = analogRead(A11);
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
