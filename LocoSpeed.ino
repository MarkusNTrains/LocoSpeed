

// Digital sensor input Left and Right
#define SENSOR_L 7
#define SENSOR_R 6

////// DECLARATIONS
#include <SPI.h> // Not needed when using I2C
#include <Wire.h> //I2C library
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>  // use this library for a 1.3 inch OLED https://github.com/wonho-maker/Adafruit_SH1106
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET); // 1.3 INCH OLED DISPLAY INIT

//Distance from sensor Left to sensor Right - your distance (mm)
const double distance = 130.0;

//Your model scale 1/xx
int scale = 160;

//Waiting time to display results (ms)
int waitingtime = 8000;

//Name of variables for time and speed measuring
long int deltatime, starttime, freetime;


void setup()  {
  //Sensor Input
  pinMode(SENSOR_L, INPUT_PULLUP);
  pinMode(SENSOR_R, INPUT_PULLUP);

  //Interface baud rate
  Serial.begin(9600);

  //Display
  initialize_display();
}

void loop()  {
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
    
    state_ready_display();
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
    
    state_ready_display();
  }

} //End of loop


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
  display.drawFastHLine(2, 20, 120, WHITE);
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

  delay(waitingtime);


}

void state_ready_display() {

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30, 45);
  display.print("BEREIT");
  display.drawFastHLine(2, 24, 120, WHITE);
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

void initialize_display() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); // initialize 1.3" OLED display (use adresse 0x3D for original Adafruit product)
  display.display();
  display.clearDisplay();
  display.display();
  display.setTextColor(INVERSE);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(6, 15);
  display.println("Siebi likes Trains");
  display.setTextSize(2);
  display.setCursor(2, 38);
  display.println("SiebiSpeed");
  display.display();

  delay(20000);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30, 45);
  display.print("BEREIT");
  display.drawFastHLine(2, 25, 120, WHITE);
  display.setTextSize(2);
  display.setCursor(37, 0);
  display.print("1:");
  display.setCursor(60, 0);
  display.print(scale);
  display.display();

  Serial.println("<<< Messbereit >>>");
}
