#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include "defaults.h"
#include "racm_wifi.h"
#include "racm_mqtt.h"
#include "logging.h"
#include "Ticker.h"

typedef struct {
  const char **sub_topics;
  uint8_t initd;
  uint8_t connctd;
  AsyncMqttClient client;
 Ticker reconnect_timer;

} mqtt_mngr_t;

mqtt_mngr_t MQTT_mngr; 
const char *MQTT_topics[]= {"cmd/stateArm", "cmd/stateTimerMode", "cmd/stateEngineRun", "cmd/systemCmd"};
void mqtt_connect_CB() {
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "Connecting to MQTT...");
  MQTT_mngr.client.connect();
}

void mqtt_subscribe_topics(){
  for(int i=0; i< 4; i++){
    if(! MQTT_mngr.client.subscribe(MQTT_topics[i], 2)){
      log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT, "Failed to subscribe to MQTT topic: ");
      log_msg_append(MQTT_topics[i]);
    }
  }
}
void onMqttConnect(bool sessionPresent) {
 log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT,"Connected to MQTT.");
  mqtt_subscribe_topics();
  MQTT_mngr.connctd=1;
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  MQTT_mngr.connctd=false;
  log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT, "Disconnected from MQTT. Reason: ");
  char buf[5];
  snprintf(buf, 5, "%d", reason);
  log_msg_append(buf);

  if (wifi_conn_status())
  {
     MQTT_mngr.reconnect_timer.once(MQTT_RECONNECT_PRD_s, mqtt_connect_CB);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "Subscribe acknowledged. Packet ID:");
  log_msg_append_i(packetId);
  log_msg_append("  QoS: ");
  log_msg_append_i(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT,"Unsubscribe acknowledged. PAcket ID:");
  log_msg_append_i((int)packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT,"Message Rx. Topic:");
  log_msg_append(topic);
  log_msg_append("Msg:");
  log_msg_append(payload);

}

void onMqttPublish(uint16_t packetId) {
 log_msg(LOG_LVL_INFO, SYS_COMM_MQTT,"Publish acknowledged. Pkt ID: ");
 log_msg_append_i(packetId);
}

void MqttWiFiConnectCB() {
  mqtt_connect_CB();
}

void MqttWiFiDisconnectCB(){
  MQTT_mngr.reconnect_timer.detach();
}
void mqtt_init(const char* mqtt_host=MQTT_HOST, uint16_t mqtt_port = MQTT_PORT,const char* mqtt_client_id=MQTT_CLIENT_ID, const char* mqtt_usrnm=MQTT_USRNM, const char* mqtt_passwd=MQTT_PASSWD) 
{
  MQTT_mngr.connctd=0;
  MQTT_mngr.sub_topics=MQTT_topics;
  wifi_add_connect_CB(MqttWiFiConnectCB);
  wifi_add_disconnect_CB(MqttWiFiConnectCB);

  MQTT_mngr.client.onConnect(onMqttConnect);
  MQTT_mngr.client.onDisconnect(onMqttDisconnect);
  MQTT_mngr.client.onSubscribe(onMqttSubscribe);
  MQTT_mngr.client.onUnsubscribe(onMqttUnsubscribe);
  MQTT_mngr.client.onMessage(onMqttMessage);
  MQTT_mngr.client.onPublish(onMqttPublish);
  MQTT_mngr.client.setServer(mqtt_host,mqtt_port);
  MQTT_mngr.initd=1;
}
