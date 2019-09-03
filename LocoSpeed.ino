/*******************************************************************************
Project   LocoSpeed

  This is an OpenSource Project.
  You can use, share or improve this project. If you improve this source code
  please share with the comunity or at least with the author of the original 
  source code
  
  Created 03. September 2019 by MarkusNTrains
================================================================================
$HeadURL:  $
$Id:  $
*******************************************************************************/

// used displays
#define OLED_DISPLAY_SSD1306_I2C
//#define OLED_DISPLAY_SSD1306_SPI
//#define OLED_DISPLAY_SH1106_I2C

// Digital sensor input Left and Right
#define SENSOR_L 7
#define SENSOR_R 6

////// DECLARATIONS
#include <SPI.h> // Not needed when using I2C
#include <Wire.h> //I2C library
#include <Adafruit_GFX.h>
#ifdef OLED_DISPLAY_SH1106_I2C
  #include <Adafruit_SH1106.h>  // use this library for a 1.3 inch OLED https://github.com/wonho-maker/Adafruit_SH1106
#endif
#if defined(OLED_DISPLAY_SSD1306_SPI) || defined(OLED_DISPLAY_SSD1306_I2C)
  #include <Adafruit_SSD1306.h>  // use this library for a 0.96" OLED
#endif
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4


#ifdef OLED_DISPLAY_SH1106_I2C
  Adafruit_SH1106 display(OLED_RESET); // 1.3 INCH OLED DISPLAY INIT
#endif

#ifdef OLED_DISPLAY_SSD1306_I2C
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  
#endif

#ifdef OLED_DISPLAY_SSD1306_SPI 
  // Declaration for SSD1306 display connected using software SPI (default case):
  #define OLED_MOSI   9
  #define OLED_CLK   10
  #define OLED_DC    11
  #define OLED_CS    12
  #define OLED_RESET 13
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
    OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#endif


//Distance from sensor Left to sensor Right - your distance (mm)
const double distance = 200.0;

//Your model scale 1/xx
int scale = 160;

//Waiting time to display results (ms)
int waitingtime = 1000;

//Name of variables for time and speed measuring
long int deltatime, starttime, freetime;


bool wait_for_sensor = false;


void setup()  {
  //Sensor Input
  pinMode(SENSOR_L, INPUT_PULLUP);
  pinMode(SENSOR_R, INPUT_PULLUP);

  //Interface baud rate
  Serial.begin(9600);

  //Display
  initialize_display();
}

void loop()  
{
  //waiting for signal, sensor Left
  if (digitalRead(SENSOR_L) == LOW)
  {
    //Sensor Left was activated, start time = system time (ms)
    starttime = millis();
    underprogress_display();
    //waiting for signal sensor Right
    while (digitalRead(SENSOR_R) == HIGH) {}

    //Measured time in ms
    deltatime = (millis() - starttime);

    refresh_display();
    freetime = 0;
    if (digitalRead(SENSOR_R) == LOW)
    {
      
      freetime = 0;  
      while (freetime < 20)
      {
        freetime++;
   
        if (digitalRead(SENSOR_R) == LOW)
        {
          freetime = 0;  //reset time with new occupation
        }
        delay (100);
     
      }
      delay (100);
    }
    
    //state_ready_display();
    wait_till_sensor_idle();
  }


  //waiting for signal, sensor Right
  if (digitalRead(SENSOR_R) == LOW)
  {
    //Sensor Right was activated, start time = system time (ms)
    starttime = millis();
    underprogress_display();
    //waiting for signal sensor Left
    while (digitalRead(SENSOR_L) == HIGH) {}

    //Measured time in ms
    deltatime = (millis() - starttime);

    refresh_display();
    freetime = 0;
    if (digitalRead(SENSOR_L) == LOW)
    {
      
      freetime = 0; 
      while (freetime < 20)
      {
        freetime++;
        
        if (digitalRead(SENSOR_L) == LOW)
        {
          freetime = 0;  //reset time with new occupation
        }
        delay (100);
     
      }
      delay (100);
    }
    
    //state_ready_display();
    wait_till_sensor_idle();
  }

} //End of loop


// wait till sensor values are idle
void wait_till_sensor_idle(void)
{
  int cnt = 0;
  int debounce = 10;
  
  display.drawFastHLine(0, 18, 128, WHITE);
  display.display();

  Serial.println("ready");
  /*while (1)
  {
    Serial.print("Left ");
    Serial.print(digitalRead(SENSOR_L));
    Serial.print("Right ");
    Serial.println(digitalRead(SENSOR_R));
    if ((digitalRead(SENSOR_L) == HIGH) && (digitalRead(SENSOR_R) == HIGH))
    {
      cnt++;
      delay(waitingtime/debounce);
      
      if (cnt >= debounce)
      {
        return;
      }
    }
    else
    {
      cnt = 0;
    }
  }*/
}


//Output on display
void refresh_display() {

  //Output on serial monitor
  Serial.println("Messung läuft..");
  Serial.println("");
  Serial.println("Deltatime:");
  Serial.print(deltatime);
  Serial.println(" ms");
  Serial.println("---------");
  Serial.println("Geschwindigkeit:");
  Serial.print(distance / deltatime);
  Serial.println(" m/s (real)");
  Serial.print(scale * distance / deltatime * 3.6, 0);
  Serial.print(" km/h (scale 1/");
  Serial.print(scale);
  Serial.println(")");

  //Output on OLED
  display.clearDisplay();
  display.setTextSize(5);
  display.setCursor(0, 28);
  display.print(scale * distance / deltatime * 3.6, 0); //calculating speed in km/h
  display.setTextSize(1);
  display.setCursor(97, 50);
  display.print("km/h");
  display.setCursor(91, 31);
  display.print("1:");
  display.setCursor(105, 31);
  display.print(scale);
  display.drawFastHLine(0, 20, 128, WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(deltatime * 0.001);
  display.setTextSize(1);
  display.setCursor(50, 7);
  display.print("s");

  display.setTextSize(2);
  display.setCursor(71, 00);
  display.print(distance / deltatime);
  display.setTextSize(1);
  display.setCursor(121, -2);
  display.print("m");
 
  display.setCursor(121, 8);
  display.print("s");
  display.display();

//  delay(waitingtime);

  //display.drawFastHLine(0, 22, 128, WHITE);

}

void state_ready_display() {

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30, 45);
  display.print("BEREIT");
  display.drawFastHLine(0, 24, 128, WHITE);
  display.setTextSize(2);
  display.setCursor(37, 0);
  display.print("1:");
  display.setCursor(60, 0);
  display.print(scale);
  display.display();

  Serial.println("");
  Serial.println("<<< Bereit >>>");
}

void underprogress_display() {

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println("");
 
  display.setTextSize(2);
  display.setCursor(17,28);
  display.display();
  display.print("MESSUNG!");
  display.display();
}

void initialize_display() 
{
  #ifdef OLED_DISPLAY_SSD1306_I2C
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
  #elif defined(OLED_DISPLAY_SSD1306_SPI)
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
  #elif defined(OLED_DISPLAY_SH1106_I2C)
    display.begin(SH1106_SWITCHCAPVCC, 0x3C); // initialize 1.3" OLED display (use adresse 0x3D for original Adafruit product)
  #endif
  
  display.display();
  display.clearDisplay();
  display.display();
  display.setTextColor(INVERSE);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(25, 4);
  display.println("MarkusNTrains");
  display.drawFastHLine(0, 25, 128, WHITE);
  display.setTextSize(2);
  display.setCursor(4, 38);
  display.println("Loco Speed");
  display.display();

  delay(3000);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30, 45);
  display.print("BEREIT");
  display.drawFastHLine(0, 25, 128, WHITE);
  display.setTextSize(2);
  display.setCursor(37, 0);
  display.print("1:");
  display.setCursor(60, 0);
  display.print(scale);
  display.display();

  Serial.println("<<< Messbereit >>>");
}
