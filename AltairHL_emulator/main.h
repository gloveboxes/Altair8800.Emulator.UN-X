#pragma once

// DevX Libraries
#include "dx_async.h"
#include "dx_device_twins.h"
#include "dx_exit_codes.h"
#include "dx_terminate.h"
#include "dx_timer.h"
#include "dx_version.h"

// System Libraries
#include "applibs_versions.h"
#include <applibs/log.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

// Altair app
#include "altair_config.h"
#include "altair_panel.h"
#include "cpu_monitor.h"
#include "iotc_manager.h"
#include "utils.h"
#include "web_socket_server.h"
#include <curl/curl.h>
#include <curl/easy.h>

// Intel 8080 emulator
#include "88dcdd.h"
#include "intel8080.h"
#include "io_ports.h"
#include "memory.h"

#ifdef ALTAIR_FRONT_PANEL_PI_SENSE
#include "front_panel_pi_sense_hat.h"
#else
#include "front_panel_none.h"
#endif // ALTAIR_FRONT_PANEL_PI_SENSE

const char ALTAIR_EMULATOR_VERSION[] = "4.6.1";
#define Log_Debug(f_, ...)      dx_Log_Debug((f_), ##__VA_ARGS__)
#define DX_LOGGING_ENABLED      FALSE

// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:climatemonitor;1"

#define BASIC_SAMPLES_DIRECTORY "BasicSamples"

static const char *AltairMsg = "\x1b[2J\r\nAltair 8800 Emulator ";

enum PANEL_MODE_T panel_mode = PANEL_BUS_MODE;

char msgBuffer[MSG_BUFFER_BYTES] = {0};

static DX_MESSAGE_PROPERTY *diag_msg_properties[] = {
	&(DX_MESSAGE_PROPERTY){.key = "appid", .value = "altair"},
	&(DX_MESSAGE_PROPERTY){.key = "type", .value = "diagnostics"},
	&(DX_MESSAGE_PROPERTY){.key = "schema", .value = "1"}};

static DX_MESSAGE_CONTENT_PROPERTIES diag_content_properties = {
	.contentEncoding = "utf-8", .contentType = "application/json"};

// CPU CPU_RUNNING STATE (CPU_STOPPED/CPU_RUNNING)
CPU_OPERATING_MODE cpu_operating_mode = CPU_STOPPED;

ALTAIR_CONFIG_T altair_config;
ENVIRONMENT_TELEMETRY environment;

intel8080_t cpu;
uint8_t memory[64 * 1024]; // Altair system memory.

ALTAIR_COMMAND cmd_switches;
uint16_t bus_switches = 0x00;

// basic app load helpers.
static bool haveAppLoad            = false;
static char terminalInputCharacter = 0x00;

static bool haveTerminalInputMessage  = false;
static bool haveTerminalOutputMessage = false;
static int altairInputBufReadIndex    = 0;
static int altairOutputBufReadIndex   = 0;
static int terminalInputMessageLen    = 0;
static int terminalOutputMessageLen   = 0;

static char *input_data = NULL;

bool azure_connected  = false;
bool send_partial_msg = false;
static FILE *app_stream;

static char Log_Debug_Time_buffer[128];

static bool load_application(const char *fileName);
static void send_terminal_character(char character, bool wait);
static void spin_wait(bool *flag);

static DX_DECLARE_TIMER_HANDLER(heart_beat_handler);
// static DX_DECLARE_TIMER_HANDLER(panel_refresh_handler);
static DX_DECLARE_TIMER_HANDLER(report_memory_usage);
static DX_DECLARE_TIMER_HANDLER(update_environment_handler);
static void *altair_thread(void *arg);

const uint8_t reverse_lut[16] = {
	0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf};

// clang-format off
// Common Timers

DX_TIMER_BINDING tmr_partial_message = {.repeat = &(struct timespec){0, 250 * ONE_MS}, .name = "tmr_partial_message", .handler = partial_message_handler};
DX_TIMER_BINDING tmr_timer_seconds_expired = {.name = "tmr_timer_seconds_expired", .handler = timer_seconds_expired_handler};
DX_TIMER_BINDING tmr_timer_millisecond_expired = {.name = "tmr_timer_millisecond_expired", .handler = timer_millisecond_expired_handler};
DX_TIMER_BINDING tmr_ws_ping_pong = {.repeat = &(struct timespec){10, 0}, .name = "tmr_partial_message", .handler = ws_ping_pong_handler};

static DX_TIMER_BINDING tmr_heart_beat = {.repeat = &(struct timespec){60, 0}, .name = "tmr_heart_beat", .handler = heart_beat_handler};
static DX_TIMER_BINDING tmr_report_memory_usage = {.repeat = &(struct timespec){45, 0}, .name = "tmr_report_memory_usage", .handler = report_memory_usage};
static DX_TIMER_BINDING tmr_tick_count = {.repeat = &(struct timespec){1, 0}, .name = "tmr_tick_count", .handler = tick_count_handler};
static DX_TIMER_BINDING tmr_update_environment = {.delay = &(struct timespec){2, 0}, .name = "tmr_update_environment", .handler = update_environment_handler};

DX_ASYNC_BINDING async_copyx_request = {.name = "async_copyx_request", .handler = async_copyx_request_handler};
DX_ASYNC_BINDING async_expire_session = { .name = "async_expire_session", .handler = async_expire_session_handler};
DX_ASYNC_BINDING async_publish_json = {.name = "async_publish_json", .handler = async_publish_json_handler};
DX_ASYNC_BINDING async_publish_weather = {.name = "async_publish_weather", .handler = async_publish_weather_handler};
DX_ASYNC_BINDING async_set_millisecond_timer = {.name = "async_set_millisecond_timer", .handler = async_set_timer_millisecond_handler};
DX_ASYNC_BINDING async_set_seconds_timer = {.name = "async_set_seconds_timer", .handler = async_set_timer_seconds_handler};
DX_ASYNC_BINDING async_terminal = {.name = "async_terminal", .handler = async_terminal_handler};

// Azure IoT Central Properties (Device Twins)

DX_DEVICE_TWIN_BINDING dt_air_quality_index = {.propertyName = "AirQualityIndexUS", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_carbon_monoxide = {.propertyName = "CarbonMonoxide", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_nitrogen_monoxide = {.propertyName = "NitrogenMonoxide", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_nitrogen_dioxide = {.propertyName = "NitrogenDioxide", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_ozone = {.propertyName = "Ozone", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_sulphur_dioxide = {.propertyName = "SulphurDioxide", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_ammonia = {.propertyName = "Ammonia", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_pm2_5 = {.propertyName = "PM2_5", .twinType = DX_DEVICE_TWIN_FLOAT};
DX_DEVICE_TWIN_BINDING dt_pm10 = {.propertyName = "PM10", .twinType = DX_DEVICE_TWIN_FLOAT};

DX_DEVICE_TWIN_BINDING dt_humidity = {.propertyName = "Humidity", .twinType = DX_DEVICE_TWIN_INT};
DX_DEVICE_TWIN_BINDING dt_pressure = {.propertyName = "Pressure", .twinType = DX_DEVICE_TWIN_INT};
DX_DEVICE_TWIN_BINDING dt_temperature = {.propertyName = "Temperature", .twinType = DX_DEVICE_TWIN_INT};
DX_DEVICE_TWIN_BINDING dt_weather = {.propertyName = "Weather", .twinType = DX_DEVICE_TWIN_STRING};
DX_DEVICE_TWIN_BINDING dt_wind_direction = {.propertyName = "WindDirection", .twinType = DX_DEVICE_TWIN_INT};
DX_DEVICE_TWIN_BINDING dt_wind_speed = {.propertyName = "WindSpeed", .twinType = DX_DEVICE_TWIN_FLOAT};

DX_DEVICE_TWIN_BINDING dt_ledBrightness = {.propertyName = "LedBrightness", .twinType = DX_DEVICE_TWIN_INT, .handler = set_led_brightness_handler};

DX_DEVICE_TWIN_BINDING dt_city = {.propertyName = "City", .twinType = DX_DEVICE_TWIN_STRING};
DX_DEVICE_TWIN_BINDING dt_country = {.propertyName = "Country", .twinType = DX_DEVICE_TWIN_STRING};
DX_DEVICE_TWIN_BINDING dt_location = {.propertyName = "Location", .twinType = DX_DEVICE_TWIN_JSON_OBJECT};

DX_DEVICE_TWIN_BINDING dt_filesystem_reads = {.propertyName = "FilesystemReads", .twinType = DX_DEVICE_TWIN_INT};
DX_DEVICE_TWIN_BINDING dt_difference_disk_reads = {.propertyName = "DifferenceDiskReads", .twinType = DX_DEVICE_TWIN_INT};
DX_DEVICE_TWIN_BINDING dt_difference_disk_writes = {.propertyName = "DifferenceDiskWrites", .twinType = DX_DEVICE_TWIN_INT};
DX_DEVICE_TWIN_BINDING dt_new_sessions = {.propertyName = "NewSessions", .twinType = DX_DEVICE_TWIN_INT};

static DX_DEVICE_TWIN_BINDING dt_deviceStartTimeUtc = {.propertyName = "StartTimeUTC", .twinType = DX_DEVICE_TWIN_STRING};
static DX_DEVICE_TWIN_BINDING dt_heartbeatUtc = {.propertyName = "HeartbeatUTC", .twinType = DX_DEVICE_TWIN_STRING};
static DX_DEVICE_TWIN_BINDING dt_softwareVersion = {.propertyName = "SoftwareVersion", .twinType = DX_DEVICE_TWIN_STRING};
// clang-format on

static DX_ASYNC_BINDING *async_bindings[] = {
	&async_copyx_request,
	&async_expire_session,
	&async_publish_json,
	&async_publish_weather,
	&async_set_millisecond_timer,
	&async_set_seconds_timer,
	&async_terminal,
};

// initialize bindings
static DX_TIMER_BINDING *timer_bindings[] = {
	&tmr_heart_beat,
	&tmr_partial_message,
	&tmr_report_memory_usage,
	&tmr_tick_count,
	&tmr_timer_millisecond_expired,
	&tmr_timer_seconds_expired,
	&tmr_update_environment,
	&tmr_ws_ping_pong,
};

static DX_DEVICE_TWIN_BINDING *device_twin_bindings[] = {
	&dt_deviceStartTimeUtc,
	&dt_softwareVersion,
	&dt_ledBrightness,
	&dt_temperature,
	&dt_pressure,
	&dt_humidity,
	&dt_wind_speed,
	&dt_wind_direction,
	&dt_weather,
	&dt_location,
	&dt_country,
	&dt_city,
	&dt_heartbeatUtc,
	&dt_air_quality_index,
	&dt_carbon_monoxide,
	&dt_nitrogen_monoxide,
	&dt_nitrogen_dioxide,
	&dt_ozone,
	&dt_sulphur_dioxide,
	&dt_ammonia,
	&dt_pm2_5,
	&dt_pm10,

	&dt_filesystem_reads,
	&dt_difference_disk_reads,
	&dt_difference_disk_writes,
	&dt_new_sessions,
};
