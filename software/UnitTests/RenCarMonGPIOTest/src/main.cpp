#include <Arduino.h>
#include "pinout_usage.h"
#include <stdarg.h>
#include <ESP8266WiFi.h>
#include <user_interface.h>
#include <time.h>
void set_io(int type, int num, ...) {
  va_list args;
    va_start(args, num);

  for(int i=0; i< num; i++){
    pinMode(va_arg(args, int), type);
  }
  va_end(args);
}

void dig_write(int state, int num, ...){
  va_list args;
  va_start(args, num);
  for(int i=0; i< num; i++){
    digitalWrite(va_arg(args, int), state);
  }
  va_end(args);
}

void test_gpio() {
  // Test Relays
  /*dig_write(HIGH, 3, (int)REL_STRT_PIN, (int)REL_FUEL_PUMP_PIN, (int)REL_IGN_PIN);
  
   dig_write(HIGH, 2, (int)DQ_COMM_PWR_PIN, (int)DQ_COMM_SW_PIN);
   
  delay(3000);
   // Test Relays
  dig_write(LOW, 3, (int)REL_STRT_PIN, (int)REL_FUEL_PUMP_PIN, (int)REL_IGN_PIN);
  
   dig_write(LOW, 2, (int)DQ_COMM_PWR_PIN, (int)DQ_COMM_SW_PIN);
   
  delay(3000);
*/

/*if(!digitalRead(DI_COMM_MOD_STATE_PIN)){
  for(int i=0; i< 3; i++){
    digitalWrite(REL_STRT_PIN, HIGH);
    delay(100);
    digitalWrite(REL_STRT_PIN, LOW);
    delay(100);
  }
}*/
digitalWrite(REL_IGN_PIN, HIGH);
delay(500);
  if(!digitalRead(DI_IGN_STATE_PIN)){
  for(int i=0; i< 3; i++){
    digitalWrite(REL_IGN_PIN, HIGH);
    delay(100);
    digitalWrite(REL_IGN_PIN, LOW);
    delay(100);
  }
  
}
delay(2000);
//digitalWrite(REL_IGN_PIN, LOW);

}

void init_io(){
set_io(OUTPUT, 3, (int)REL_STRT_PIN, (int)REL_FUEL_PUMP_PIN, (int)REL_IGN_PIN);
  set_io(OUTPUT, 2, (int)DQ_COMM_PWR_PIN, (int)DQ_COMM_SW_PIN);
  set_io(INPUT_PULLUP, 3, (int)DI_COMM_MOD_STATE_PIN, (int)DI_PANIC_BTN_PIN, (int)DI_IGN_STATE_PIN);
}

timeval tme= {1597564178 ,0};
uint32_t tme_s, addr;
void ds_test() 
{
   delay(1000);
  
  Serial.println(ESP.getResetInfo());
  ESP.rtcUserMemoryRead(0, &addr, 4);
  ESP.rtcUserMemoryRead(1, &tme_s, 4);

  if(addr != 0x00FA9907) {
    uint32_t data=0x00FA9907;
    ESP.rtcUserMemoryWrite(0, &data, 4);
    data=1000000;
    ESP.rtcUserMemoryWrite(1, &data, 4);
  }
  else {
    tme_s -= 100;
    ESP.rtcUserMemoryWrite(1, &tme_s, 4);
    Serial.println(tme_s);
  }
  ESP.deepSleep(1000000, RF_DISABLED);
 }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  init_io();
}

void loop() {
  
  test_gpio();
}