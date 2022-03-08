#pragma once

#include "dx_utilities.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mqtt.h>

#include <stdio.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

#define MQTT_BROKER_URL "127.0.0.1"
#define MQTT_BROKER_PORT "1883"

extern volatile bool dirty_buffer;

typedef enum { TOPIC_UNKNOWN, TOPIC_DATA_SUB, TOPIC_CONTROL_SUB } TOPIC_TYPE;

TOPIC_TYPE topic_type(const void *topic_name, uint16_t topic_name_length);
void init_mqtt(void (*publish_callback)(void **unused, struct mqtt_response_publish *published), void (*mqtt_connected_cb)(void));
void publish_character(char character);
void publish_message(const void *application_message, size_t application_message_length);
void send_partial_message(void);