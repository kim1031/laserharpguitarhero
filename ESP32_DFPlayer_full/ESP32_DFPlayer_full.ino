/***************************************************
 DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>
 
 ***************************************************
 This example shows the all the function of library for DFPlayer.
 
 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
<https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on ESP32 boards (changes by pcbreflux).

 //modified by jodalyst for 
 ****************************************************/

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup()
{
  mySoftwareSerial.begin(9600, SERIAL_8N1, 32, 33);  // speed, type, RX, TX
  Serial.begin(115200);
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(10);  //Set volume value (0~30).
  myDFPlayer.volumeUp(); //Volume Up
  myDFPlayer.volumeDown(); //Volume Down
  
  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  
  //----Set device we use SD as default----
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  
  //----Mp3 control----
//  myDFPlayer.sleep();     //sleep
//  myDFPlayer.reset();     //Reset the module
//  myDFPlayer.enableDAC();  //Enable On-chip DAC
//  myDFPlayer.disableDAC();  //Disable On-chip DAC
//  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15

  int delayms=100;
  //----Mp3 play----
  /*
  Serial.println(F("myDFPlayer.next()"));
  myDFPlayer.next();  //Play next mp3
  delay(delayms);
  Serial.println(F("myDFPlayer.previous()"));
  myDFPlayer.previous();  //Play previous mp3
  delay(delayms);
  Serial.println(F("myDFPlayer.play(1)"));
  myDFPlayer.play(1);  //Play the first mp3
  delay(delayms);
  Serial.println(F("myDFPlayer.loop(1)"));
  myDFPlayer.loop(1);  //Loop the first mp3
  delay(delayms);
  Serial.println(F("myDFPlayer.pause()"));
  myDFPlayer.pause();  //pause the mp3
  delay(delayms);
  Serial.println(F("myDFPlayer.start()"));
  myDFPlayer.start();  //start the mp3 from the pause
  delay(delayms);
  Serial.println(F("myDFPlayer.playFolder(15, 4)"));
  myDFPlayer.playFolder(15, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
  delay(delayms);
  Serial.println(F("myDFPlayer.enableLoopAll()"));
  myDFPlayer.enableLoopAll(); //loop all mp3 files.
  delay(delayms);
  Serial.println(F("myDFPlayer.disableLoopAll()"));
  myDFPlayer.disableLoopAll(); //stop loop all mp3 files.
  delay(delayms);
  Serial.println(F("myDFPlayer.playMp3Folder(4)"));
  myDFPlayer.playMp3Folder(4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  delay(delayms);
  Serial.println(F("myDFPlayer.advertise(3)"));
  myDFPlayer.advertise(3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
  delay(delayms);
  Serial.println(F("myDFPlayer.stopAdvertise()"));
  myDFPlayer.stopAdvertise(); //stop advertise
  delay(delayms);
  Serial.println(F("myDFPlayer.playLargeFolder(2,999)"));
  myDFPlayer.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
  delay(delayms);
  Serial.println(F("myDFPlayer.loopFolder(5)"));
  myDFPlayer.loopFolder(5); //loop all mp3 files in folder SD:/05.
  delay(delayms);
  Serial.println(F("myDFPlayer.randomAll()"));
  myDFPlayer.randomAll(); //Random play all the mp3.
  delay(delayms);
  Serial.println(F("myDFPlayer.enableLoop()"));
  myDFPlayer.enableLoop(); //enable loop.
  delay(delayms);
  Serial.println(F("myDFPlayer.disableLoop()"));
  myDFPlayer.disableLoop(); //disable loop.
  delay(delayms);
  */

  //----Read imformation----
  Serial.println(F("readState--------------------"));
  Serial.println(myDFPlayer.readState()); //read mp3 state
  Serial.println(F("readVolume--------------------"));
  Serial.println(myDFPlayer.readVolume()); //read current volume
  //Serial.println(F("readEQ--------------------"));
  //Serial.println(myDFPlayer.readEQ()); //read EQ setting
  Serial.println(F("readFileCounts--------------------"));
  Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  Serial.println(F("readCurrentFileNumber--------------------"));
  Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
  Serial.println(F("readFileCountsInFolder--------------------"));
  Serial.println(myDFPlayer.readFileCountsInFolder(3)); //read fill counts in folder SD:/03
  Serial.println(F("--------------------"));

  Serial.println(F("myDFPlayer.play(2)"));
  myDFPlayer.play(2);  //Play the first mp3
}

void loop()
{
  // static unsigned long timer = millis();
  
 if (Serial.available()) {
    String inData = "";
    inData = Serial.readStringUntil('\n');
    if (inData.startsWith("n")) {
      Serial.println(F("next--------------------"));
      myDFPlayer.next();
      Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    } else if (inData.startsWith("p")) {
      Serial.println(F("previous--------------------"));
      myDFPlayer.previous();
      Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    } else if (inData.startsWith("+")) {
      Serial.println(F("up--------------------"));
      myDFPlayer.volumeUp();
      Serial.println(myDFPlayer.readVolume()); //read current volume
    } else if (inData.startsWith("-")) {
      Serial.println(F("down--------------------"));
      myDFPlayer.volumeDown();
      Serial.println(myDFPlayer.readVolume()); //read current volume
    } else if (inData.startsWith("*")) {
      Serial.println(F("pause--------------------"));
      myDFPlayer.pause();
    } else if (inData.startsWith(">")) {
      Serial.println(F("start--------------------"));
      myDFPlayer.start();
    }
 }

 if (myDFPlayer.available()) {
  if (myDFPlayer.readType()==DFPlayerPlayFinished) {
    Serial.println(myDFPlayer.read());
    Serial.println(F("next--------------------"));
     myDFPlayer.next();  //Play next mp3 every 3 second.
    Serial.println(F("readCurrentFileNumber--------------------"));
    Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
    delay(500);
  }
 }  
}
