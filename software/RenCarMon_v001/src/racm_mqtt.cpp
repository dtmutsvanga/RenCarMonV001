#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include "defaults.h"
#include "racm_wifi.h"
#include "racm_mqtt.h"
#include "logging.h"
#include "Ticker.h"
#include "cmd_queue.h"
#include "fsm_states.h"
#include "comms_defs.h"

#define DELAY_ms(x) delay(x)

// Commands
typedef struct
{
  const char **sub_topics;
  uint8_t initd;
  uint8_t connctd;
  AsyncMqttClient client;
  Ticker reconnect_timer;

} mqtt_mngr_t;

mqtt_mngr_t MQTT_mngr;
#define NUM_SUB_TOPICS 7
const char *MQTT_topics[NUM_SUB_TOPICS] = {"0_cmd/0_Idle", "0_cmd/1_Arm", "0_cmd/2_TMode" ,"0_cmd/3_EngineRun", "0_cmd/4_Admin", "1_inf/0_log", "1_inf/2_sysData"};

uint8_t parse_message(char *topic, char *payload, int len)
{

  if(len>0 && payload[0] == '-') return 1;  // Command already processed
  switch (topic[0] - '0')
  {
  case 0:
    switch (topic[6] - '0')
    {
    case EV_CMD_IDLE:
    if(len>0){
      log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "EV_CMD_IDLE received");
        cmd_q_push(EV_CMD_IDLE, NULL, 0);
        MQTT_mngr.client.publish(topic, 2, 1, "-",2);
         MQTT_mngr.client.publish("Feedback", 2, 1, "IDLE command received",2);
      
    }
      break;
    case EV_CMD_ARM:
      log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "EV_CMD_ARM received");
      cmd_q_push(EV_CMD_ARM, NULL, 0);
      MQTT_mngr.client.publish(topic, 2, 1, "-",2);
      MQTT_mngr.client.publish("Feedback", 2, 1, "ARM command received",2);
      break;
    case EV_CMD_TIM_MODE:
    {
      if (len < 5)
      {
        log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT, "Invalid MQTT command EV_CMD_TIM_MODE received: Length < 5, Len = ");
        log_msg_append_i(len);
        return 0;
      }
      uint16_t tme = 10000 * (payload[0] - '0'); // 100s
      tme += 1000 * (payload[1] - '0');          // 10s
      tme += 100 * (payload[2] - '0');           // 10s
      tme += 10 * (payload[3] - '0');            // 10s
      tme += payload[4] - '0';                   // units
      if (tme > MAX_TIMER_MODE_TIME_MIN)
        tme = MAX_TIMER_MODE_TIME_MIN;
      log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "EV_CMD_TIM_MODE received. Time = ");
      log_msg_append_i(tme);
      cmd_q_push(EV_CMD_TIM_MODE, (uint8_t *)&tme, sizeof(uint16_t));
       MQTT_mngr.client.publish(topic, 2, 1, "-",2);
      MQTT_mngr.client.publish("Feedback", 2, 1, "Timer command received",2);
    }
    break;
    case EV_CMD_ENG_RUN:

    {
      if (len < 2)
      {
        log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT, "Invalid MQTT command EV_CMD_ENG_RUN. Length < 2, Len = ");
        log_msg_append_i(len);
        return 0;
      }
      uint8_t tme = 10 * (payload[0] - '0');
      tme += (payload[1] - '0');
      if (tme > MAX_ENG_RUN_TIME_MIN)
        tme = MAX_ENG_RUN_TIME_MIN;
      log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "EV_CMD_ENG_RUN received. Time = ");
      log_msg_append_i(tme);
      cmd_q_push(EV_CMD_ENG_RUN, (uint8_t *)&tme, sizeof(tme));

      MQTT_mngr.client.publish(topic, 2, 1, "-",2);
      MQTT_mngr.client.publish("Feedback", 2, 1, "Engine run command received",2);
    }
    break;
    case EV_CMD_ADMIN:
      log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "EV_CMD_ADMIN received");
      cmd_q_push(EV_CMD_ADMIN, NULL, 0);
      MQTT_mngr.client.publish(topic, 2, 1, "-",2);
      MQTT_mngr.client.publish("Feedback", 2, 1, "Admin command received",2);
      break;
    default:
      log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT, "Unknown command received. ID = ");
      log_msg_append_i(topic[6] - '0');
      break;
    }
    break;
  case 1:
    // Info request messages. Where to post them?
    break;
  default:
    log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT, "Unknown command received. ID = ");
    log_msg_append_i(topic[0] - '0');
    break;
  }

  return 1;
}

void mqtt_subscribe_topics()
{
  for (int i = 0; i < NUM_SUB_TOPICS; i++)
  {
    if (!MQTT_mngr.client.subscribe(MQTT_mngr.sub_topics[i], 2))
    {
      log_msg(LOG_LVL_WARNING, SYS_COMM_MQTT, "Failed to subscribe to MQTT topic: ");
      log_msg_append(MQTT_topics[i]);
      DELAY_ms(25);
    }
  }
}
void onMqttConnect(bool sessionPresent)
{
  log_msg(LOG_LVL_CRITICAL, SYS_COMM_MQTT, "Connected to MQTT.");
  mqtt_subscribe_topics();
  MQTT_mngr.connctd = 1;
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  MQTT_mngr.connctd = false;
  log_msg(LOG_LVL_CRITICAL, SYS_COMM_MQTT, "Disconnected from MQTT. Reason: ");
  char buf[5];
  snprintf(buf, 5, "%d", reason);
  log_msg_append(buf);

  if (wifi_conn_status())
  {
    MQTT_mngr.reconnect_timer.once(MQTT_RECONNECT_PRD_s, MqttWiFiConnectCB);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "Subscribe acknowledged. Packet ID:");
  log_msg_append_i(packetId);
  log_msg_append("  QoS: ");
  log_msg_append_i(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "Unsubscribe acknowledged. PAcket ID:");
  log_msg_append_i((int)packetId);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "Message Rx. Topic:");
  log_msg_append(topic);
  log_msg_append(" Retain:");
  log_msg_append_i(properties.retain);
  log_msg_append(" Payload:");
  if (len)
    log_msg_append(payload);
  // Parse message
  parse_message(topic, payload, len);
}

void onMqttPublish(uint16_t packetId)
{
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "Publish acknowledged. Pkt ID: ");
  log_msg_append_i(packetId);
}

void MqttWiFiConnectCB()
{
  log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "Connecting to MQTT...");

  MQTT_mngr.client.connect();
}

void MqttWiFiDisconnectCB()
{
  MQTT_mngr.reconnect_timer.detach();
}
void mqtt_init(const char *mqtt_host = MQTT_HOST, uint16_t mqtt_port = MQTT_PORT, const char *mqtt_client_id = MQTT_CLIENT_ID, const char *mqtt_usrnm = MQTT_USRNM, const char *mqtt_passwd = MQTT_PASSWD)
{

  MQTT_mngr.connctd = 0;
  MQTT_mngr.sub_topics = MQTT_topics;
  MQTT_mngr.client.onConnect(onMqttConnect);
  MQTT_mngr.client.onDisconnect(onMqttDisconnect);
  MQTT_mngr.client.onSubscribe(onMqttSubscribe);
  MQTT_mngr.client.onUnsubscribe(onMqttUnsubscribe);
  MQTT_mngr.client.onMessage(onMqttMessage);
  MQTT_mngr.client.onPublish(onMqttPublish);
  MQTT_mngr.client.setServer(mqtt_host, mqtt_port);
  MQTT_mngr.client.setCredentials(mqtt_usrnm, mqtt_passwd);
  MQTT_mngr.client.setClientId(mqtt_client_id);
  MQTT_mngr.initd = 1;
}

uint8_t mqtt_read(uint8_t *data)
{
  /* Do nothing */
  return 1;
}

uint8_t mqtt_write(uint8_t *data, int len)
{
  if (!MQTT_mngr.connctd)
    return 0;

  // All messages are either response, info or log messages
  switch (data[0])
  {
  case MSG_INFO_SYS_DATA:
    MQTT_mngr.client.publish("resp/sysData", 2, 0, (const char *)(data + 1), len);
    break;
  case MSG_INFO_LOG:
    MQTT_mngr.client.publish("resp/Log", 2, 0, (const char *)(data + 1), len - 1);
    break;
  default:
    return 0;
    break;
  }
  return 1;
}
