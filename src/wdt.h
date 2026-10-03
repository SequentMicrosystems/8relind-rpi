#ifndef WDT_H
#define WDT_H

#include "relay.h"

extern const CliCmdType CMD_WDT_RELOAD;
extern const CliCmdType CMD_WDT_SET_PERIOD;
extern const CliCmdType CMD_WDT_GET_PERIOD;
extern const CliCmdType CMD_WDT_SET_INIT_PERIOD;
extern const CliCmdType CMD_WDT_GET_INIT_PERIOD;
extern const CliCmdType CMD_WDT_SET_OFF_PERIOD;
extern const CliCmdType CMD_WDT_GET_OFF_PERIOD;
extern const CliCmdType CMD_WDT_GET_RESET_COUNT;
extern const CliCmdType CMD_WDT_CLR_RESET_COUNT;
extern const CliCmdType CMD_RASP_VOLT_READ;
extern const CliCmdType CMD_TEMP_READ;


int doWdtReload(int argc, char *argv[]);
int doWdtSetPeriod(int argc, char *argv[]);
int doWdtGetPeriod(int argc, char *argv[]);
int doWdtSetInitPeriod(int argc, char *argv[]);
int doWdtGetInitPeriod(int argc, char *argv[]);
int doWdtSetOffPeriod(int argc, char *argv[]);
int doWdtGetOffPeriod(int argc, char *argv[]);
int doWdtGetResetCount(int argc, char *argv[]);
int doWdtClearResetCount(int argc, char *argv[]);
int doRaspVoltRead(int argc, char *argv[]);
int doTempRead(int argc, char *argv[]);

#endif /* WDT_H */