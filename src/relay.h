#ifndef RELAY8_H_
#define RELAY8_H_

#include <stdint.h>

#define RETRY_TIMES	10
#define RELAY8_INPORT_REG_ADD	0x00
#define RELAY8_OUTPORT_REG_ADD	0x01
#define RELAY8_POLINV_REG_ADD	0x02
#define RELAY8_CFG_REG_ADD		0x03

#define CHANNEL_NR_MIN		1
#define RELAY_CH_NR_MAX		8

#define ERROR	-1
#define OK		0
#define FAIL	-1
#define ARG_CNT_ERR	-2
#define ARG_RANGE_ERROR -3

#define WDT_RESET_SIGNATURE     0xca
#define WDT_RESET_COUNT_SIGNATURE    0xbe

#define RELAY8_HW_I2C_BASE_ADD	0x38
#define RELAY8_HW_I2C_ALTERNATE_BASE_ADD 0x20

#define PROGRAM_NAME "8relind"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef enum
{
	OFF = 0,
	ON,
	STATE_COUNT
} OutStateEnumType;

enum
{
	I2C_INPORT_REG_ADD,
	I2C_OUTPORT_REG_ADD = 1,
	I2C_POLINV_REG_ADD = 2,
	I2C_CFG_REG_ADD = 3,
	I2C_SW_MOM_ADD = 4,
	I2C_SW_INT_ADD,
	I2C_SW_INT_EN_ADD,
	I2C_MEM_DIAG_3V3_MV_ADD,
	I2C_MEM_DIAG_TEMPERATURE_ADD = I2C_MEM_DIAG_3V3_MV_ADD +2,
	I2C_MEM_DIAG_5V_ADD,
	I2C_MEM_WDT_RESET_ADD = I2C_MEM_DIAG_5V_ADD + 2,
	I2C_MEM_WDT_INTERVAL_SET_ADD,
	I2C_MEM_WDT_INTERVAL_GET_ADD = I2C_MEM_WDT_INTERVAL_SET_ADD + 2,
	I2C_MEM_WDT_INIT_INTERVAL_SET_ADD = I2C_MEM_WDT_INTERVAL_GET_ADD + 2,
	I2C_MEM_WDT_INIT_INTERVAL_GET_ADD = I2C_MEM_WDT_INIT_INTERVAL_SET_ADD + 2,
	I2C_MEM_WDT_RESET_COUNT_ADD = I2C_MEM_WDT_INIT_INTERVAL_GET_ADD + 2,
	I2C_MEM_WDT_CLEAR_RESET_COUNT_ADD = I2C_MEM_WDT_RESET_COUNT_ADD + 2,
	I2C_MEM_WDT_POWER_OFF_INTERVAL_SET_ADD,
	I2C_MEM_WDT_POWER_OFF_INTERVAL_GET_ADD = I2C_MEM_WDT_POWER_OFF_INTERVAL_SET_ADD + 4,
	I2C_MODBUS_SETINGS_ADD  = I2C_MEM_WDT_POWER_OFF_INTERVAL_GET_ADD + 4,//5 bytes
	I2C_MEM_RELAY_FAILSAFE_EN_ADD = I2C_MODBUS_SETINGS_ADD + 5,//8 bits
	I2C_MEM_RELAY_FAILSAFE_VAL_ADD = I2C_MEM_RELAY_FAILSAFE_EN_ADD + 1,//8 bits

	I2C_MEM_CPU_RESET = 0xaa,
	I2C_MEM_REVISION_HW_MAJOR_ADD ,
	I2C_MEM_REVISION_HW_MINOR_ADD,
	I2C_MEM_REVISION_MAJOR_ADD,
	I2C_MEM_REVISION_MINOR_ADD,
	I2C_MEM_LED_MODE = 254,
	SLAVE_BUFF_SIZE = 255
};

typedef struct
{
 const char* name;
 const int namePos;
 int(*pFunc)(int, char**);
 const char* help;
 const char* usage1;
 const char* usage2;
 const char* example;
}CliCmdType;

typedef struct
	__attribute__((packed))
	{
		unsigned int mbBaud :24;
		unsigned int mbType :4;
		unsigned int mbParity :2;
		unsigned int mbStopB :2;
		unsigned int add:8;
	} ModbusSetingsType;

	int doBoardInit(int stack);

#endif //RELAY8_H_
