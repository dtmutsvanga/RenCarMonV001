#include <Arduino.h>
#include "dio.h"
#include  "racm_wifi_mqtt.h"
dio_t dio;

void setup()
{
  Serial.begin(9600);
  dio_setup();
  setup_wifi_mqtt();
}

void loop(){

}
