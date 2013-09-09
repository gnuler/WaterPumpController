

#ifndef __DEVICE_CONFIG_H__
#define __DEVICE_CONFIG_H__


// Intetion is to have here everithing that is specific to a device
// so everithing else is generic


// Baud rate used to communicate with the XBEE
// it _must_ match the xbee setup
#define BAUD_RATE   9600



#define PROXY_ADDRESS_MSB 0x0013a200
#define PROXY_ADDRESS_LSB 0x40a01203


#define PUMP_DIR          DDRC
#define PUMP_PORT         PORTC
#define PUMP_BIT          _BV(5)

#define LED_ALIVE_PORT          PORTC   // red LED is connected to PD2
#define LED_ALIVE_DIR           DDRC
#define LED_ALIVE_BIT           _BV(0)

#define BTN_1_DIR               DDRC
#define BTN_1_PORT              PORTC
#define BTN_1_BIT               _BV(3)


#define ALARM_HIGH_DIR               DDRC
#define ALARM_LOW_DIR                DDRC
#define ALARM_HIGH_PORT              PINC
#define ALARM_LOW_PORT               PINC
#define ALARM_HIGH_BIT               _BV(1)
#define ALARM_LOW_BIT                _BV(2)



// Zone name
#define STR_WATER_TANK "WaterTank"
#define STR_WATER_TANK_LEN strlen(STR_WATER_TANK)

#define ZONE_NAME_MAX_LENGTH    20

#define STR_ZONE_NAME       STR_WATER_TANK
#define STR_ZONE_NAME_LEN   STR_WATER_TANK_LEN

#define DEVICE_NAME "WPC"



// Categories
#define STR_PUMP "Pump"
#define STR_PUMP_LEN strlen(STR_PUMP)

#define STR_SENSOR "Sensor"
#define STR_SENSOR_LEN strlen(STR_SENSOR)

#define STR_CONFIG "Config"
#define STR_CONFIG_LEN strlen(STR_CONFIG)


// Pump devices
#define STR_MAIN "Main"
#define STR_MAIN_LEN strlen(STR_MAIN)


// Sensor devices

#define STR_ALARM_HIGH "AlarmHigh"
#define STR_ALARM_HIGH_LEN strlen(STR_ALARM_HIGH)

#define STR_ALARM_LOW "AlarmLow"
#define STR_ALARM_LOW_LEN strlen(STR_ALARM_LOW)

// Config
#define STR_AUTO_START "AutoStart"
#define STR_AUTO_START_LEN strlen(STR_AUTO_START)

#define STR_AUTO_STOP "AutoStop"
#define STR_AUTO_STOP_LEN strlen(STR_AUTO_STOP)






#endif
