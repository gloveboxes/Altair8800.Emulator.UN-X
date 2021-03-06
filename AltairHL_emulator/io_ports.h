/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once

#include "dx_timer.h"
#include "dx_utilities.h"
#include "environment_types.h"
#include "iotc_manager.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#ifdef AZURE_SPHERE
#include "onboard_sensors.h"
#else
#include "graphics.h"
#endif

#ifdef AVNET_LIGHT_SENSOR
#include "light_sensor.h"
#endif // AVNET_LIGHT_SENSOR

#ifdef ALTAIR_FRONT_PANEL_RETRO_CLICK
#include "front_panel_retro_click.h"
#endif

DX_DECLARE_ASYNC_HANDLER(async_accelerometer_start_handler);
DX_DECLARE_ASYNC_HANDLER(async_accelerometer_stop_handler);
DX_DECLARE_ASYNC_HANDLER(async_copyx_request_handler);
DX_DECLARE_ASYNC_HANDLER(async_publish_json_handler);
DX_DECLARE_ASYNC_HANDLER(async_publish_weather_handler);
DX_DECLARE_ASYNC_HANDLER(async_set_timer_millisecond_handler);
DX_DECLARE_ASYNC_HANDLER(async_set_timer_seconds_handler);
DX_DECLARE_TIMER_HANDLER(read_accelerometer_handler);
DX_DECLARE_TIMER_HANDLER(tick_count_handler);
DX_DECLARE_TIMER_HANDLER(timer_millisecond_expired_handler);
DX_DECLARE_TIMER_HANDLER(timer_seconds_expired_handler);

#ifdef AZURE_SPHERE
extern DX_GPIO_BINDING gpioRed;
extern DX_GPIO_BINDING gpioGreen;
extern DX_GPIO_BINDING gpioBlue;
extern INTERCORE_ML_CLASSIFY_BLOCK_T intercore_ml_classify_block;
extern DX_INTERCORE_BINDING intercore_ml_classify_ctx;
void intercore_classify_response_handler(void *data_block, ssize_t message_length);
#endif

extern const char ALTAIR_EMULATOR_VERSION[];
extern ALTAIR_CONFIG_T altair_config;
extern DX_TIMER_BINDING tmr_read_accelerometer;
extern DX_TIMER_BINDING tmr_timer_millisecond_expired;
extern DX_TIMER_BINDING tmr_timer_seconds_expired;

extern DX_ASYNC_BINDING async_accelerometer_start;
extern DX_ASYNC_BINDING async_accelerometer_stop;
extern DX_ASYNC_BINDING async_copyx_request;
extern DX_ASYNC_BINDING async_publish_json;
extern DX_ASYNC_BINDING async_publish_weather;
extern DX_ASYNC_BINDING async_set_millisecond_timer;
extern DX_ASYNC_BINDING async_set_seconds_timer;

enum PANEL_MODE_T
{
	PANEL_BUS_MODE,
	PANEL_FONT_MODE,
	PANEL_BITMAP_MODE
};

extern enum PANEL_MODE_T panel_mode;

uint8_t io_port_in(uint8_t port);
void io_port_out(uint8_t port, uint8_t data);
