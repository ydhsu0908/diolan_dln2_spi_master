#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <bitset>
#include "iniparser.h"
#include "CDln.h"
#include "dln_gpio.h"
#include "dln_generic.h"
#include "dln_spi_master.h"
#include "gpio_emu.h"
#include "spi_emu.h"

using namespace std;
HDLN     device;

vector<string> CollectArgs(int argc, char* argv[])
{
	vector<string> args;

	for (int i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}

	return args;
}

void GetDlnParamsFromIni(char* iniPath, DLN_INIT_PARAMS* pParams)
{
	DLN_INIT_PARAMS	params = { 0 };

	dictionary* iniDict = iniparser_load(iniPath);
	if (iniDict)
	{
		params.devNum = iniparser_getint(iniDict, "SPI:DEV_NUM", 0);
		params.cpol = iniparser_getint(iniDict, "SPI:CPOL", 1);
		params.cpha = iniparser_getint(iniDict, "SPI:CPHA", 1);
		params.frameSize = iniparser_getint(iniDict, "SPI:FRAMES", 1);
		params.delayBetweenFrame = iniparser_getint(iniDict, "SPI:DELAY_BETWEEN_FRAMES", 1);
		params.delayAfterSS = iniparser_getint(iniDict, "SPI:DELAY_AFTER_SS", 1);
		params.clock = iniparser_getint(iniDict, "SPI:CLOCK", 1000000);
		iniparser_freedict(iniDict);

		memcpy(pParams, &params, sizeof(DLN_INIT_PARAMS));
	}
	else
	{
		pParams->devNum = 0;
		pParams->cpol = 1;
		pParams->cpha = 0;
		pParams->frameSize = 16;
		pParams->delayBetweenFrame = 1;
		pParams->delayAfterSS = 1;
		pParams->clock = 1000000;
	}
}

int main(int argc, char* argv[])
{
	CDln dlnInstance;

	// Read INI
	DLN_INIT_PARAMS dlnParams = { 0 };
	GetDlnParamsFromIni("../spitool.ini", &dlnParams);
	if (dlnInstance.init(&dlnParams) == false)
	{
		cout << "\nERROR!! Initial Diolan DLN-2 error, check USB cable...\n" ;
		return 0;
	}
	
	//HDLN local_device;
	DlnOpenUsbDevice(&device);

	cout << "====================================\n";
	cout << " Test Initialization  \n";
	cout << "====================================\n";

	if (DLN2_SPI_MODE)
	{
		cout << " [Diolan GPIO and SPI mode starts..]\n";
		spi_emu_spi_businit();
		gpio_emu_spi_command_prompt();
	}
	else
	{
		cout << " [Diolan GPIO mode starts..]\n";
		//spi_emu_spi_businit();
		gpio_emu_spi_command_prompt();
	}
	return 0;
}

