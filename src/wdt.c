#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "comm.h"
#include "relay.h"
#include "wdt.h"


const CliCmdType CMD_WDT_RELOAD = {
	"wdtr",
	2,
	&doWdtReload,
	"  wdtr         Reload the watchdog timer and enable the watchdog if is disabled.\n",
	"  Usage:       "PROGRAM_NAME" <id> wdtr\n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtr; Reload the watchdog timer on Board #0 with the period \n"
};
int doWdtReload(int argc, char *argv[]) {
	if(argc != 3) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if(dev < 0) {
		return ERROR;
	}
	uint8_t buf[1] = { WDT_RESET_SIGNATURE };
	if (OK != i2cMem8Write(dev, I2C_MEM_WDT_RESET_ADD, buf, 1)) {
		printf("Fail to write watchdog reset key!\n");
		return ERROR;
	}
	return OK;
}

const CliCmdType CMD_WDT_GET_PERIOD = {
	"wdtprd",
	2,
	&doWdtGetPeriod,
	"  wdtprd       Get the watchdog period in seconds, reload command must\n"
	"               be issue in this interval to prevent Raspberry Pi power off.\n",
	"  Usage:       "PROGRAM_NAME" <id> wdtprd \n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtprd; Get the watchdog timer period on Board #0\n"
};
int doWdtGetPeriod(int argc, char *argv[]) {
	if(argc != 3) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0) {
		return ERROR;
	}
	uint8_t buf[2];
	if (OK != i2cMem8Read(dev, I2C_MEM_WDT_INTERVAL_GET_ADD, buf, 2)) {
		printf("Fail to read watchdog period!\n");
		return ERROR;
	}
	uint16_t period;
	memcpy(&period, buf, 2);
	printf("%d\n", (int)period);
	return OK;
}

const CliCmdType CMD_WDT_SET_PERIOD = {
	"wdtpwr",
	2,
	&doWdtSetPeriod,
	"  wdtpwr       Set the watchdog period in seconds, reload command must\n"
	"               be issue in this interval to prevent Raspberry Pi power off.\n",
	"  Usage:       "PROGRAM_NAME" <id> wdtpwr <val> \n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtpwr 10; Set the watchdog timer period on Board #0 at 10 seconds \n"
};
int doWdtSetPeriod(int argc, char *argv[]) {
	if(argc != 4) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if(dev < 0) {
		return ERROR;
	}
	uint16_t period;
	period = (uint16_t)atoi(argv[3]);
	if(0 == period) {
		printf("Invalid period!\n");
		return ERROR;
	}
	uint8_t buff[2];
	memcpy(buff, &period, 2);
	if(OK != i2cMem8Write(dev, I2C_MEM_WDT_INTERVAL_SET_ADD, buff, 2)) {
		printf("Fail to write watchdog period!\n");
		return ERROR;
	}
	return OK;
}

const CliCmdType CMD_WDT_SET_INIT_PERIOD = {
	"wdtipwr",
	2,
	&doWdtSetInitPeriod,
	"  wdtipwr       Set the watchdog initial period in seconds. This period\n"
	"               is loaded after power cycle, giving Raspberry time to boot.\n",
	"  Usage:       "PROGRAM_NAME" <id> wdtipwr <val> \n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtipwr 10; Set the watchdog timer initial period on Board #0 at 10 seconds \n"
};
int doWdtSetInitPeriod(int argc, char *argv[]) {
	if(argc != 4) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if (dev < 0) {
		return ERROR;
	}
	uint16_t period = atoi(argv[3]);
	if(0 == period) {
		printf("Invalid period!\n");
		return ERROR;
	}
	uint8_t buf[2];
	memcpy(buf, &period, 2);
	if(OK != i2cMem8Write(dev, I2C_MEM_WDT_INIT_INTERVAL_SET_ADD, buf, 2)) {
		printf("Fail to write watchdog period!\n");
		return ERROR;
	}
	return OK;
}

const CliCmdType CMD_WDT_GET_INIT_PERIOD = {
	"wdtiprd",
	2,
	&doWdtGetInitPeriod,
	"  wdtiprd       Get the watchdog initial period in seconds.\n"
	"               This period is loaded after power cycle, giving Raspberry time to boot\n",
	"  Usage:       "PROGRAM_NAME" <id> wdtiprd \n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtiprd; Get the watchdog timer initial period on Board #0\n"
};
int doWdtGetInitPeriod(int argc, char *argv[]) {
	if(argc != 3) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if(dev < 0) {
		return ERROR;
	}
	uint8_t buf[2];
	if(OK != i2cMem8Read(dev, I2C_MEM_WDT_INIT_INTERVAL_GET_ADD, buf, 2)) {
		printf("Fail to read watchdog period!\n");
		return ERROR;
	}
	uint16_t period;
	memcpy(&period, buf, 2);
	printf("%d\n", (int)period);
	return OK;
}

const CliCmdType CMD_WDT_GET_OFF_PERIOD = {
	"wdtoprd",
	2,
	&doWdtGetOffPeriod,
	"  wdtoprd       Get the watchdog off period in seconds (max 48 days).\n"
	"               This is the time that watchdog mantain Raspberry turned off \n",
	"  Usage:       "PROGRAM_NAME" <id> wdtoprd \n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtoprd; Get the watchdog off period on Board #0\n"
};
int doWdtGetOffPeriod(int argc, char *argv[])
{
	if(argc != 3) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if (dev < 0) {
		return ERROR;
	}
	uint8_t buf[4];
	if (OK != i2cMem8Read(dev, I2C_MEM_WDT_POWER_OFF_INTERVAL_GET_ADD, buf, 4)) {
		printf("Fail to read watchdog period!\n");
		return ERROR;
	}
	uint32_t period;
	memcpy(&period, buf, 4);
	printf("%d\n", (int)period);

	return OK;
}

const CliCmdType CMD_WDT_SET_OFF_PERIOD = {
	"wdtopwr",
	2,
	&doWdtSetOffPeriod,
	"  wdtopwr       Set the watchdog off period in seconds (max 48 days).\n"
	"               This is the time that watchdog mantain Raspberry turned off \n",
	"  Usage:       "PROGRAM_NAME" <id> wdtopwr <val> \n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtopwr 10; Set the watchdog off interval on Board #0 at 10 seconds \n"
};
int doWdtSetOffPeriod(int argc, char *argv[]) {
	if(argc != 4) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if (dev < 0) {
		return ERROR;
	}
	uint32_t period = (uint32_t)atoi(argv[3]);
	if ( (0 == period) || (period > 1 << 20 /* TODO: USE WDT_MAX_OFF_INTERVAL_S */)) {
		printf("Invalid period!\n");
		return ARG_RANGE_ERROR;
	}
	uint8_t buf[4];
	memcpy(buf, &period, 4);
	if (OK != i2cMem8Write(dev, I2C_MEM_WDT_POWER_OFF_INTERVAL_SET_ADD, buf, 4)) {
		printf("Fail to write watchdog period!\n");
		return ERROR;
	}
	return OK;
}

const CliCmdType CMD_WDT_GET_RESET_COUNT = {
	"wdtrcrd",
	2,
	&doWdtGetResetCount,
	"  wdtrcrd          Get the watchdog numbers of performed repowers.\n",
	"  Usage:       "PROGRAM_NAME" <id> wdtrcrd \n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtrcrd; Get the watchdog reset count on Board #0\n"
};
int doWdtGetResetCount(int argc, char *argv[]) {
	if(argc != 3) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if (dev < 0) {
		return ERROR;
	}
	uint8_t buf[2];
	if (OK != i2cMem8Read(dev, I2C_MEM_WDT_RESET_COUNT_ADD, buf, 2))
	{
		printf("Fail to read watchdog reset count!\n");
		return ERROR;
	}
	uint16_t period;
	memcpy(&period, buf, 2);
	printf("%d\n", (int)period);
	return OK;
}

const CliCmdType CMD_WDT_CLR_RESET_COUNT = {
	"wdtrcclr",
	2,
	&doWdtClearResetCount,
	"  wdtrcclr       Clear the reset count.\n",
	"  Usage:       "PROGRAM_NAME" <id> wdtrcclr\n",
    "",
	"  Example:     "PROGRAM_NAME" 0 wdtrcclr -> Clear the watchdog resets count on Board #0\n"
};
int doWdtClearResetCount(int argc, char *argv[]) {
	if(argc != 3) {
		return ARG_CNT_ERR;
	}
	int dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0) {
		return ERROR;
	}
	uint8_t buf[1] = { WDT_RESET_COUNT_SIGNATURE };
	if (OK != i2cMem8Write(dev, I2C_MEM_WDT_CLEAR_RESET_COUNT_ADD, buf, 1))
	{
		printf("Fail to clear the reset count!\n");
		return ERROR;
	}
	return OK;
}
