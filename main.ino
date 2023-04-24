#include <GyverTM1637.h> 
#include <Servo.h> 
 
#define SERV_PIN 7 
#define Left_BTN 12 
#define Right_BTN 11 
 
int ThermistorPin = A1; 
float log_temp, temp; 
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //коэффициенты Штейнхарта-Харта для термистора 
float min_temp = 26.50; 
float max_temp = 30.00; 
 
unsigned long left_btn_press = 0; 
unsigned long right_btn_press = 0; 
bool left_btn_edit = false; 
bool right_btn_edit = false; 
bool left_btn_val = false; 
bool right_btn_val = false; 
bool left_btn_was_pressed = false; 
bool right_btn_was_pressed = false; 
 
int press_duration = 2000; 
float temp_step = 0.1; 
 
GyverTM1637 disp(5, 6); 
Servo servo; 
 
void setup() { 
  Serial.begin(9600); 
 
  pinMode(Left_BTN, INPUT_PULLUP); 
  pinMode(Right_BTN, INPUT_PULLUP); 
 
  servo.attach(SERV_PIN); 
   
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум) 
  disp.point(true); 
  disp.clear(); 
} 
 
void display_float(float number) { 
  String str_number = String(number); 
  String fraction = str_number.substring(str_number.indexOf(".") + 1, str_number.length()); 
  String new_n = str_number.substring(0, str_number.indexOf(".")) + fraction; 
  if (fraction.length() == 1) { 
    new_n += "0"; 
  } 
  disp.displayInt(new_n.toInt()); 
} 
 
void check_temp() { 
  log_temp = log(10000 * (1023.0 / (float)(1023 - analogRead(ThermistorPin)) - 1.0)); 
  temp = (1.0 / (c1 + c2 * log_temp + c3 * log_temp * log_temp * log_temp)) - 273.15; 
 
  if (temp > max_temp) { 
    servo.write(0); 
  } else if (temp < min_temp) { 
    servo.write(180); 
  } 
} 
 
void loop() { 
  check_temp(); 
   
  left_btn_val = digitalRead(Left_BTN); 
  right_btn_val = digitalRead(Right_BTN); 
  if (!left_btn_edit && !right_btn_edit) { 
    display_float(temp); 
 
    if (left_btn_val && (left_btn_press == 0)) { 
      left_btn_press = millis(); 
    } 
    if (right_btn_val && (right_btn_press == 0)) { right_btn_press = millis(); } 
    if ((!left_btn_val) && (left_btn_press != 0)) { 
      if (millis() - left_btn_press >= press_duration) { 
 
        left_btn_edit = true; 
      } 
      left_btn_press = 0; 
    } 
    if ((!right_btn_val) && (right_btn_press != 0)) { 
      if (millis() - right_btn_press >= press_duration) { 
        right_btn_edit = true; 
      } 
      right_btn_press = 0; 
    } 
  } else if (left_btn_edit) { 
    display_float(min_temp); 
 
    if (left_btn_val && !left_btn_was_pressed) { 
      left_btn_was_pressed = true; 
      left_btn_press = millis(); 
    } 
    if (!left_btn_val && left_btn_was_pressed) { 
      left_btn_was_pressed = false; 
      if (millis() - left_btn_press >= press_duration) { 
        left_btn_edit = false; 
      } else { 
        min_temp -= temp_step; 
      } 
      left_btn_press = 0; 
    } 
    if (right_btn_val) { right_btn_was_pressed = true; } 
    if (!right_btn_val && right_btn_was_pressed) { 
      right_btn_was_pressed = false; 
      min_temp += temp_step; 
    } 
 
  } else if (right_btn_edit) { 
    display_float(max_temp); 
 
    if (left_btn_val) { left_btn_was_pressed = true; } 
    if (!left_btn_val && left_btn_was_pressed) { 
      left_btn_was_pressed = false; 
      max_temp -= temp_step; 
    } 
    if (right_btn_val && !right_btn_was_pressed) { 
      right_btn_was_pressed = true; 
      right_btn_press = millis(); 
    } 
    if (!right_btn_val && right_btn_was_pressed) { 
      right_btn_was_pressed = false; 
      if (millis() - right_btn_press >= press_duration) { 
        right_btn_edit = false; 
      } else { 
        max_temp += temp_step; 
      } 
      right_btn_press = 0; 
    } 
  } 
  Serial.print("Value: " + String(left_btn_val) + " " + String(right_btn_val) + " Mode: " + String(left_btn_edit) + " " + String(right_btn_edit)); 
  Serial.println(" temp: " + String(min_temp) + " " + String(max_temp)); 
}
