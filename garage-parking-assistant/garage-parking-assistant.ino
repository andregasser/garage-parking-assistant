/*
 HC-SR04 Ping distance sensor:
 VCC to arduino 5v 
 GND to arduino GND
 Echo to Arduino pin 7 
 Trig to Arduino pin 8
 
 This sketch originates from Virtualmix: http://goo.gl/kJ8Gl
 Has been modified by Winkle ink here: http://winkleink.blogspot.com.au/2012/05/arduino-hc-sr04-ultrasonic-distance.html
 And modified further by ScottC here: http://arduinobasics.blogspot.com.au/2012/11/arduinobasics-hc-sr04-ultrasonic-sensor.html
 on 10 Nov 2012.

   // we suggest putting delays after each command to make sure the data 
  // is sent and the LCD is updated.

 */
#include <SoftwareSerial.h>
#include "Arduino.h"

#define ECHO_PIN               7    // Echo Pin
#define TRIG_PIN               8    // Trigger Pin
#define MAX_TRACK_RANGE        100 
#define UPPER_OK               100
#define LOWER_OK               51
#define UPPER_WARN             50
#define LOWER_WARN             31
#define UPPER_STOP             30
#define LOWER_STOP             0

// Create a software serial port on digital pin 2
SoftwareSerial lcd = SoftwareSerial(0,2);
long duration, distance; // Duration used to calculate distance

void setup() {
  // Configure serial console / monitor
  Serial.begin(9600);
  
  // Configure LCD
  lcd.begin(9600);  
  lcd_set_display_size();
  lcd_set_contrast(200);
  lcd_set_brightness(255);    
  lcd_set_bg_color(0, 255, 0); // green
  lcd_turn_off_cursors();
  lcd_turn_off_autoscroll();
  lcd_create_block_char();  
  lcd_clear();
  lcd_set_cursor(2, 1);
  lcd_print_bar(100);  

  // Configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() 
{
  // The following TRIG_PIN / ECHO_PIN cycle is used to determine the
  // distance of the nearest object by bouncing soundwaves off of it 
  digitalWrite(TRIG_PIN, LOW); 
  delayMicroseconds(2); 

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10); 
 
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
 
  // Calculate the distance (in cm) based on the speed of sound
  distance = duration / 58.2;

  set_display_color_based_on_distance(distance);
  set_line1_text_based_on_distance(distance);
  set_line2_text_based_on_distance(distance);

  // Delay 50ms before next reading.
  delay(200);
}

/*
 *  Sets background color based on distance.
 */
void set_display_color_based_on_distance(int distance)
{
  
  if (distance >= LOWER_OK) 
  {
    lcd_set_bg_color(0, 255, 0); // green
  } 
  else
  {
    if (distance >= LOWER_WARN)
    {
      lcd_set_bg_color(255, 25, 0); // orange
    }
    else
    {
      lcd_set_bg_color(255, 0, 0); // red
    }
  }
}

void set_line1_text_based_on_distance(int distance)
{
  if (distance > MAX_TRACK_RANGE)
  {
    lcd_set_cursor(1,1);
    lcd.print("Awaiting car...");
  }
  else
  {
    if (distance <= MAX_TRACK_RANGE && distance >= LOWER_WARN)
    {
      lcd_clear_line(1);
      lcd_set_cursor(5,1);
      lcd.print(distance);
      lcd.print(" cm");
    }
    else
    {
      lcd_set_cursor(1,1);
      lcd.print("   !! STOP !!   "); 
    }
  }  
}

void set_line2_text_based_on_distance(int distance) 
{
  Serial.print("distance = ");
  Serial.println(distance);
  
  int percentage = 100.0 / 70 * (distance - 30);

  if (percentage > 100) {
    percentage = 100;
  }

  Serial.print("percentage = ");
  Serial.println(percentage);
  
  if (distance <= MAX_TRACK_RANGE && distance >= LOWER_WARN)
  {
    lcd_clear_line(2);
    lcd_set_cursor(2,1);
    lcd_print_bar(percentage);    
  }
  else
  {
    lcd_clear_line(2);
  }
}

/*
 * Sets the size of the display if it isn't 16x2 
 * (you only have to do this once)
 */
void lcd_set_display_size()
{
  lcd.write(0xFE);
  lcd.write(0xD1);
  lcd.write(16);  // 16 columns
  lcd.write(2);   // 2 rows
  delay(10);       
}

/*
 * Sets the contrast, 200 is a good place to start, adjust as desired
 */
void lcd_set_contrast(int contrast)
{
  lcd.write(0xFE);
  lcd.write(0x50);
  lcd.write(contrast);
  delay(10);  
}

void lcd_clear_line(int lineNo)
{
  lcd_set_cursor(lineNo, 1);
  lcd.print("                ");
}

/*
 * Sets the brightness (255 is max brightness)
 */
void lcd_set_brightness(int brightness)
{
  lcd.write(0xFE);
  lcd.write(0x99);
  lcd.write(brightness);
  delay(10); 
}

/*
 * Sets the background color of the LCD
 */
void lcd_set_bg_color(int red, int green, int blue)
{
  lcd.write(0xFE);
  lcd.write(0xD0);
  lcd.write(red); 
  lcd.write(green);
  lcd.write(blue);
  delay(10);  
}

void lcd_turn_off_autoscroll()
{
  lcd.write(0xFE);
  lcd.write(0x52);
  delay(10);  
}

/*
 * Clears the LCD screen.
 */
void lcd_clear() 
{
  lcd.write(0xFE);
  lcd.write(0x58);
  delay(10);   
}

void lcd_set_cursor(int row, int col)
{
  lcd.write(0xFE);
  lcd.write(0x47);
  lcd.write(col); 
  lcd.write(row);
  delay(10);  
}

/*
 * Creates a custom character
 */
void lcd_create_block_char() 
{
  lcd.write(0xFE);
  lcd.write(0x4E);
  
  // Location #0
  lcd.write((uint8_t)0);     
  
  // 8 bytes of character data
  lcd.write(0x1F);           
  lcd.write(0x1F);
  lcd.write(0x1F);
  lcd.write(0x1F);
  lcd.write(0x1F);
  lcd.write(0x1F);
  lcd.write(0x1F);
  lcd.write(0x1F);
  delay(10);
}

/*
 * Turns off cursors.
 */
void lcd_turn_off_cursors()
{
  lcd.write(0xFE);
  lcd.write(0x4B);
  lcd.write(0xFE);
  lcd.write(0x54);
}

/* 
 * Prints a percentage bar on the second line of the display.
 */
void lcd_print_bar(int percent)
{
  int max_blocks = 16;
  int num_blocks = max_blocks / 100.0 * percent;

  for (int i = 0; i < num_blocks; i++)
  {
    lcd.write((uint8_t)0);  
  }
}

