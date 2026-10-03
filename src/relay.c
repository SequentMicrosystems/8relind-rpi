/*
 * relay.c:
 *	Command-line interface to the Raspberry
 *	Pi's 8-Relay Industrial board.
 *	Copyright (c) 2016-2021 Sequent Microsystem
 *	<http://www.sequentmicrosystem.com>
 ***********************************************************************
 *	Author: Alexandru Burcea
 ***********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "relay.h"
#include "comm.h"
#include "thread.h"
#include "wdt.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)2
#define VERSION_MINOR	(int)0

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define CMD_ARRAY_SIZE	64

const u8 relayMaskRemap[8] =
{
	0x01,
	0x04,
	0x40,
	0x10,
	0x20,
	0x80,
	0x08,
	0x02};
const int relayChRemap[8] =
{
	0,
	2,
	6,
	4,
	5,
	7,
	3,
	1};

int relayChSet(int dev, u8 channel, OutStateEnumType state);
int relayChGet(int dev, u8 channel, OutStateEnumType* state);
u8 relayToIO(u8 relay);
u8 IOToRelay(u8 io);
int cfg485Set(int dev, u8 mode, u32 baud, u8 stopB, u8 parity, u8 add);
int cfg485Get(int dev);
static int doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
	{
		"-h",
		1,
		&doHelp,
		"  -h           Display the list of command options or one command option details\n",
		"  Usage:       8relind -h    Display command options list\n",
		"  Usage:       8relind -h <param>   Display help for <param> command option\n",
		"  Example:     8relind -h write    Display help for \"write\" command option\n"};

static int doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION =
	{
		"-v",
		1,
		&doVersion,
		"  -v            Display the version number\n",
		"  Usage:       8relind -v\n",
		"",
		" Example:      8relind -v  Display the version number\n"};

static int doWarranty(int argc, char* argv[]);
const CliCmdType CMD_WAR =
	{
		"-warranty",
		1,
		&doWarranty,
		"  -warranty    Display the warranty\n",
		"  Usage:       8relind -warranty\n",
		"",
		"  Example:     8relind -warranty  Display the warranty text\n"};

static int doList(int argc, char *argv[]);
const CliCmdType CMD_LIST =
	{
		"-list",
		1,
		&doList,
		"  -list:      	List all 8relind boards connected,\n return       nr of boards and stack level for every board\n",
		"  Usage:       8relind -list\n",
		"",
		"  Example:     8relind -list display: 1,0 \n"};

static int doRelayWrite(int argc, char *argv[]);
const CliCmdType CMD_WRITE =
	{
		"write",
		2,
		&doRelayWrite,
		"  write:       Set relays On/Off\n",
		"  Usage:       8relind <id> write <channel> <on/off>\n",
		"  Usage:       8relind <id> write <value>\n",
		"  Example:     8relind 0 write 2 On; Set Relay #2 on Board #0 On\n"};

static int doRelayRead(int argc, char *argv[]);
const CliCmdType CMD_READ =
	{
		"read",
		2,
		&doRelayRead,
		"  read:        Read relays status\n",
		"  Usage:       8relind <id> read <channel>\n",
		"  Usage:       8relind <id> read\n",
		"  Example:     8relind 0 read 2; Read Status of Relay #2 on Board #0\n"};

static int doTest(int argc, char* argv[]);
const CliCmdType CMD_TEST =
	{
		"test",
		2,
		&doTest,
		"  test:        Turn ON and OFF the relays until press a key\n",
		"",
		"  Usage:       8relind <id> test\n",
		"  Example:     8relind 0 test\n"};


static int doBoard1(int argc, char* argv[]);
const CliCmdType CMD_BOARD =
	{
		"board", 
		2, 
		&doBoard1,
		"  board:       Display board firmware version and status\n", 
		"  Usage:       8relind <id> board\n",
		"",
		"  Example:     8relind 0 board\n"};

int doRs485Write(int argc, char *argv[]);
const CliCmdType CMD_RS485_WRITE =
	{
		"cfg485wr", 2, &doRs485Write,
		"  cfg485wr:    Write the RS485 communication settings\n",
		"  Usage:       8relind <id> cfg485wr <mode> <baudrate> <stopBits> <parity> <slaveAddr>\n",
		"",
		"  Example:		 8relind 0 cfg485wr 1 9600 1 0 1; Write the RS485 settings on Board #0 \n   (mode = Modbus RTU; baudrate = 9600 bps; stop bits one; parity none; modbus slave address = 1)\n"};

int doRs485Read(int argc, char *argv[]);
const CliCmdType CMD_RS485_READ =
	{"cfg485rd", 2, &doRs485Read,
		"  cfg485rd:    Read the RS485 communication settings\n",
		"  Usage:       8relind <id> cfg485rd\n", "",
		"  Example:		8relind 0 cfg485rd; Read the RS485 settings on Board #0\n"};

CliCmdType gCmdArray[CMD_ARRAY_SIZE];

void doUsage(void)
{
	int i = 0;
	for (i = 0; i < CMD_ARRAY_SIZE; i++)
	{
		if (gCmdArray[i].usage1 != NULL)
		{
			printf("%s", gCmdArray[i].usage1);
		}
	}
}

char *usage = "Usage:	 8relind -h <command>\n"
	"         8relind -v\n"
	"         8relind -warranty\n"
	"         8relind -list\n"
	"         8relind <id> write <channel> <on/off>\n"
	"         8relind <id> write <value>\n"
	"         8relind <id> read <channel>\n"
	"         8relind <id> read\n"
	"         8relind <id> test\n"
	"Where: <id> = Board level id = 0..7\n"
	"Type 8relind -h <command> for more help"; // No trailing newline needed here.

char *warranty =
	"	       Copyright (c) 2016-2020 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";
u8 relayToIO(u8 relay)
{
	u8 i;
	u8 val = 0;
	for (i = 0; i < 8; i++)
	{
		if ( (relay & (1 << i)) != 0)
			val += relayMaskRemap[i];
	}
	return val;
}

u8 IOToRelay(u8 io)
{
	u8 i;
	u8 val = 0;
	for (i = 0; i < 8; i++)
	{
		if ( (io & relayMaskRemap[i]) != 0)
		{
			val += 1 << i;
		}
	}
	return val;
}

int relayChSet(int dev, u8 channel, OutStateEnumType state)
{
	int resp;
	u8 buff[2];

	if ( (channel < CHANNEL_NR_MIN) || (channel > RELAY_CH_NR_MAX))
	{
		printf("Invalid relay nr!\n");
		return ERROR;
	}
	if (FAIL == i2cMem8Read(dev, RELAY8_INPORT_REG_ADD, buff, 1))
	{
		return FAIL;
	}

	switch (state)
	{
	case OFF:
		buff[0] &= ~ (1 << relayChRemap[channel - 1]);
		resp = i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1);
		break;
	case ON:
		buff[0] |= 1 << relayChRemap[channel - 1];
		resp = i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1);
		break;
	default:
		printf("Invalid relay state!\n");
		return ERROR;
		break;
	}
	return resp;
}

int relayChGet(int dev, u8 channel, OutStateEnumType* state)
{
	u8 buff[2];

	if (NULL == state)
	{
		return ERROR;
	}

	if ( (channel < CHANNEL_NR_MIN) || (channel > RELAY_CH_NR_MAX))
	{
		printf("Invalid relay nr!\n");
		return ERROR;
	}

	if (FAIL == i2cMem8Read(dev, RELAY8_INPORT_REG_ADD, buff, 1))
	{
		return ERROR;
	}

	if (buff[0] & (1 << relayChRemap[channel - 1]))
	{
		*state = ON;
	}
	else
	{
		*state = OFF;
	}
	return OK;
}

int relaySet(int dev, int val)
{
	u8 buff[2];

	buff[0] = relayToIO(0xff & val);

	return i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1);
}

int relayGet(int dev, int* val)
{
	u8 buff[2];

	if (NULL == val)
	{
		return ERROR;
	}
	if (FAIL == i2cMem8Read(dev, RELAY8_INPORT_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	*val = IOToRelay(buff[0]);
	return OK;
}
int cfg485Set(int dev, u8 mode, u32 baud, u8 stopB, u8 parity, u8 add)
{
	ModbusSetingsType settings;
	u8 buff[5];

	if (baud > 921600 || baud < 1200)
	{
		printf("Invalid RS485 Baudrate [1200, 921600]!\n");
		return ERROR;
	}
	if (mode > 1)
	{
		printf("Invalid RS485 mode : 0 = disable, 1= Modbus RTU (Slave)!\n");
		return ERROR;
	}
	if (stopB < 1 || stopB > 2)
	{
		printf("Invalid RS485 stop bits [1, 2]!\n");
		return ERROR;
	}
	if (parity > 2)
	{
		printf("Invalid RS485 parity 0 = none; 1 = even; 2 = odd! \n");
		return ERROR;
	}
	if (add < 1)
	{
		printf("Invalid MODBUS device address: [1, 255]!\n");
	}
	settings.mbBaud = baud;
	settings.mbType = mode;
	settings.mbParity = parity;
	settings.mbStopB = stopB;
	settings.add = add;

	memcpy(buff, &settings, sizeof(ModbusSetingsType));
	if (OK != i2cMem8Write(dev, I2C_MODBUS_SETINGS_ADD, buff, 5))
	{
		printf("Fail to write RS485 settings!\n");
		return ERROR;
	}
	return OK;
}

int cfg485Get(int dev)
{
	ModbusSetingsType settings;
	u8 buff[5];

	if (OK != i2cMem8Read(dev, I2C_MODBUS_SETINGS_ADD, buff, 5))
	{
		printf("Fail to read RS485 settings!\n");
		return ERROR;
	}
	memcpy(&settings, buff, sizeof(ModbusSetingsType));
	printf("<mode> <baudrate> <stopbits> <parity> <add> %d %d %d %d %d\n",
		(int)settings.mbType, (int)settings.mbBaud, (int)settings.mbStopB,
		(int)settings.mbParity, (int)settings.add);
	return OK;
}

int doBoardInit(int stack)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	if ( (stack < 0) || (stack > 7))
	{
		printf("Invalid stack level [0..7]!");
		return ERROR;
	}
	add = (stack + RELAY8_HW_I2C_BASE_ADD) ^ 0x07;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;

	}
	if (ERROR == i2cMem8Read(dev, RELAY8_CFG_REG_ADD, buff, 1))
	{
		add = (stack + RELAY8_HW_I2C_ALTERNATE_BASE_ADD) ^ 0x07;
		dev = i2cSetup(add);
		if (dev == -1)
		{
			return ERROR;
		}
		if (ERROR == i2cMem8Read(dev, RELAY8_CFG_REG_ADD, buff, 1))
		{
			printf("8relind board id %d not detected\n", stack);
			return ERROR;
		}
	}
	if (buff[0] != 0) //non initialized I/O Expander
	{
		// put all pins in 0-logic state
		buff[0] = 0;
		if (0 > i2cMem8Write(dev, RELAY8_OUTPORT_REG_ADD, buff, 1))
		{
			return ERROR;
		}
		// make all I/O pins output
		buff[0] = 0;
		if (0 > i2cMem8Write(dev, RELAY8_CFG_REG_ADD, buff, 1))
		{
			return ERROR;
		}

	}

	return dev;
}

int boardCheck(int hwAdd)
{
	int dev = 0;
	uint8_t buff[8];

	hwAdd ^= 0x07;
	dev = i2cSetup(hwAdd);
	if (dev == -1)
	{
		return FAIL;
	}
	if (ERROR == i2cMem8Read(dev, RELAY8_CFG_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	return OK;
}

/*
 * doRelayWrite:
 *	Write coresponding relay channel
 **************************************************************************************
 */
static int doRelayWrite(int argc, char *argv[])
{
	int pin = 0;
	OutStateEnumType state = STATE_COUNT;
	int val = 0;
	int dev = 0;
	OutStateEnumType stateR = STATE_COUNT;
	int valR = 0;
	int retry = 0;

	if ( (argc != 5) && (argc != 4))
	{
		printf("Usage: 8relind <id> write <relay number> <on/off> \n");
		printf("Usage: 8relind <id> write <relay reg value> \n");
		return ERROR;
	}

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}
	if (argc == 5)
	{
		pin = atoi(argv[3]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > RELAY_CH_NR_MAX))
		{
			printf("Relay number value out of range\n");
			return ERROR;
		}

		/**/if ( (strcasecmp(argv[4], "up") == 0)
			|| (strcasecmp(argv[4], "on") == 0))
			state = ON;
		else if ( (strcasecmp(argv[4], "down") == 0)
			|| (strcasecmp(argv[4], "off") == 0))
			state = OFF;
		else
		{
			if ( (atoi(argv[4]) >= STATE_COUNT) || (atoi(argv[4]) < 0))
			{
				printf("Invalid relay state!\n");
				return ERROR;
			}
			state = (OutStateEnumType)atoi(argv[4]);
		}

		retry = RETRY_TIMES;

		while ( (retry > 0) && (stateR != state))
		{
			if (OK != relayChSet(dev, pin, state))
			{
				printf("Fail to write relay\n");
				return ERROR;
			}
			if (OK != relayChGet(dev, pin, &stateR))
			{
				printf("Fail to read relay\n");
				return ERROR;
			}
			retry--;
		}
#ifdef DEBUG_I
		if(retry < RETRY_TIMES)
		{
			printf("retry %d times\n", 3-retry);
		}
#endif
		if (retry == 0)
		{
			printf("Fail to write relay\n");
			return ERROR;
		}
	}
	else
	{
		val = atoi(argv[3]);
		if (val < 0 || val > 255)
		{
			printf("Invalid relay value\n");
			return ERROR;
		}

		retry = RETRY_TIMES;
		valR = -1;
		while ( (retry > 0) && (valR != val))
		{

			if (OK != relaySet(dev, val))
			{
				printf("Fail to write relay!\n");
				return ERROR;
			}
			if (OK != relayGet(dev, &valR))
			{
				printf("Fail to read relay!\n");
				return ERROR;
			}
			retry--;
		}
		if (retry == 0)
		{
			printf("Fail to write relay!\n");
			return ERROR;
		}
	}
	return OK;
}

/*
 * doRelayRead:
 *	Read relay state
 ******************************************************************************************
 */
static int doRelayRead(int argc, char *argv[])
{
	int pin = 0;
	int val = 0;
	int dev = 0;
	OutStateEnumType state = STATE_COUNT;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 4)
	{
		pin = atoi(argv[3]);
		if ( (pin < CHANNEL_NR_MIN) || (pin > RELAY_CH_NR_MAX))
		{
			printf("Relay number value out of range!\n");
			return ERROR;
		}

		if (OK != relayChGet(dev, pin, &state))
		{
			printf("Fail to read!\n");
			return ERROR;
		}
		if (state != 0)
		{
			printf("1\n");
		}
		else
		{
			printf("0\n");
		}
	}
	else if (argc == 3)
	{
		if (OK != relayGet(dev, &val))
		{
			printf("Fail to read!\n");
			return ERROR;
		}
		printf("%d\n", val);
	}
	else
	{
		printf("Usage: %s read relay value\n", argv[0]);
		return ERROR;
	}
	return OK;
}

static int doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		for (i = 0; i < CMD_ARRAY_SIZE; i++)
		{
			if ( (gCmdArray[i].name != NULL))
			{
				if (strcasecmp(argv[2], gCmdArray[i].name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i].help, gCmdArray[i].usage1,
						gCmdArray[i].usage2, gCmdArray[i].example);
					break;
				}
			}
		}
		if (CMD_ARRAY_SIZE == i)
		{
			printf("Option \"%s\" not found\n", argv[2]);
			
			doUsage();
		}
	}
	else
	{
		
		doUsage();
	}
	return OK;
}

static int doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("8relind v%d.%d.%d Copyright (c) 2016 - 2020 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: 8relind -warranty\n");
	return OK;	
}

static int doList(int argc, char *argv[])
{
	int ids[8];
	int i;
	int cnt = 0;

	UNUSED(argc);
	UNUSED(argv);

	for (i = 0; i < 8; i++)
	{
		if (boardCheck(RELAY8_HW_I2C_BASE_ADD + i) == OK)
		{
			ids[cnt] = i;
			cnt++;
		}
		else
		{
			if (boardCheck(RELAY8_HW_I2C_ALTERNATE_BASE_ADD + i) == OK)
			{
				ids[cnt] = i;
				cnt++;
			}
		}
	}
	printf("%d board(s) detected\n", cnt);
	if (cnt > 0)
	{
		printf("Id:");
	}
	while (cnt > 0)
	{
		cnt--;
		printf(" %d", ids[cnt]);
	}
	printf("\n");
	return OK;
}

/* 
 * Self test for production
 */
static int doTest(int argc, char* argv[])
{
	int dev = 0;
	int i = 0;
	int retry = 0;
	int relVal;
	int valR;
	int relayResult = 0;
	FILE* file = NULL;
	const u8 relayOrder[8] =
	{
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		8};

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return -1;
	}
	if (argc == 4)
	{
		file = fopen(argv[3], "w");
		if (!file)
		{
			printf("Fail to open result file\n");
			return -1;
		}
	}
//relay test****************************
	if (strcasecmp(argv[2], "test") == 0)
	{
		relVal = 0;
		printf(
			"Are all relays and LEDs turning on and off in sequence?\nPress y for Yes or any key for No....");
		startThread();
		while (relayResult == 0)
		{
			for (i = 0; i < 8; i++)
			{
				relayResult = checkThreadResult();
				if (relayResult != 0)
				{
					break;
				}
				valR = 0;
				relVal = (u8)1 << (relayOrder[i] - 1);

				retry = RETRY_TIMES;
				while ( (retry > 0) && ( (valR & relVal) == 0))
				{
					if (OK != relayChSet(dev, relayOrder[i], ON))
					{
						retry = 0;
						break;
					}

					if (OK != relayGet(dev, &valR))
					{
						retry = 0;
					}
				}
				if (retry == 0)
				{
					printf("Fail to write relay\n");
					if (file)
						fclose(file);
					return -1;
				}
				busyWait(150);
			}

			for (i = 0; i < 8; i++)
			{
				relayResult = checkThreadResult();
				if (relayResult != 0)
				{
					break;
				}
				valR = 0xff;
				relVal = (u8)1 << (relayOrder[i] - 1);
				retry = RETRY_TIMES;
				while ( (retry > 0) && ( (valR & relVal) != 0))
				{
					if (OK != relayChSet(dev, relayOrder[i], OFF))
					{
						retry = 0;
					}
					if (OK != relayGet(dev, &valR))
					{
						retry = 0;
					}
				}
				if (retry == 0)
				{
					printf("Fail to write relay!\n");
					if (file)
						fclose(file);
					return -1	;
				}
				busyWait(150);
			}
		}
	}
	if (relayResult == YES)
	{
		if (file)
		{
			fprintf(file, "Relay Test ............................ PASS\n");
		}
		else
		{
			printf("Relay Test ............................ PASS\n");
		}
	}
	else
	{
		if (file)
		{
			fprintf(file, "Relay Test ............................ FAIL!\n");
		}
		else
		{
			printf("Relay Test ............................ FAIL!\n");
		}
	}
	if (file)
	{
		fclose(file);
	}
	relaySet(dev, 0);
	return OK;
}

static int doWarranty(int argc UNU, char* argv[] UNU)
{
	printf("%s\n", warranty);
	return OK;
}
int doRs485Read(int argc, char *argv[])
{
	int dev = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}

	if (argc == 3)
	{
		if (OK != cfg485Get(dev))
		{
			return ERROR;
		}
	}
	else
	{
		return ARG_CNT_ERR;
	}
	return OK;
}
static int doBoard1(int argc, char *argv[])
{
	int dev = 0;
	u8 buff[3];
	u8 revMin = 0;
	u8 revMaj = 0;
	UNUSED(argc);


	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return (FAIL);
	}
	if (OK != i2cMem8Read(dev, I2C_MEM_REVISION_MAJOR_ADD, buff, 2))
	{
		printf("Fail to read!\n");
		return ERROR;
	}
	revMaj = buff[0];
	revMin = buff[1];
	if (OK != i2cMem8Read(dev, I2C_MEM_DIAG_3V3_MV_ADD, buff, 3))
	{
		printf("Fail to read!\n");
		return ERROR;
	}
	printf(
		"8-RELAY card found firmware version %d.%02d CPU voltage %0.3fV temperature %d'C\n",
		(int)revMaj, (int)revMin,
		(int) ((int) ( (buff[1] << 8) | buff[0])) / 1000.0, (int)buff[2]);
	return OK;
}


int doRs485Write(int argc, char *argv[])
{
	int dev = 0;
	u8 mode = 0;
	u32 baud = 1200;
	u8 stopB = 1;
	u8 parity = 0;
	u8 add = 0;

	dev = doBoardInit(atoi(argv[1]));
	if (dev <= 0)
	{
		return ERROR;
	}
	if (argc == 8)
	{
		mode = 0xff & atoi(argv[3]);
		baud = atoi(argv[4]);
		stopB = 0xff & atoi(argv[5]);
		parity = 0xff & atoi(argv[6]);
		add = 0xff & atoi(argv[7]);
		if (OK != cfg485Set(dev, mode, baud, stopB, parity, add))
		{
			return ERROR;
		}
		printf("done\n");
	}
	else
	{
		return ARG_CNT_ERR;
	}
	return OK;
}

static void cliInit(void)
{
	int i = 0;

	memset(gCmdArray, 0, sizeof(CliCmdType) * CMD_ARRAY_SIZE);

	memcpy(&gCmdArray[i], &CMD_HELP, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_VERSION, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WAR, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_LIST, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WRITE, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_READ, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_TEST, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_RS485_READ, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_RS485_WRITE, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_BOARD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_RELOAD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_SET_PERIOD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_GET_PERIOD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_SET_INIT_PERIOD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_GET_INIT_PERIOD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_SET_OFF_PERIOD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_GET_OFF_PERIOD, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_GET_RESET_COUNT, sizeof(CliCmdType));
	i++;
	memcpy(&gCmdArray[i], &CMD_WDT_CLR_RESET_COUNT, sizeof(CliCmdType));
	i++;
}

int main(int argc, char *argv[])
{
	int i = 0;
	int ret = 0;

	cliInit();

	if (argc == 1)
	{
		doUsage();
		return ERROR;
	}
	for (i = 0; i < CMD_ARRAY_SIZE; i++)
	{
		if ( (gCmdArray[i].name != NULL) && (gCmdArray[i].namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i].namePos], gCmdArray[i].name) == 0)
			{
				ret = gCmdArray[i].pFunc(argc, argv);
				return ret;
			}
		}
	}
	printf("Invalid command option\n");
	doUsage();

	return -1;
}
