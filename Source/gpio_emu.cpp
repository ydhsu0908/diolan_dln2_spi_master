#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <bitset>
#include <iostream>
#include <iomanip>
#include "iniparser.h"
#include "CDln.h"
#include "dln_gpio.h"
#include "dln_spi_master.h"
#include "dln_generic.h"
#include "gpio_emu.h"

#define  _CRT_SECURE_NO_WARNINGS 

extern HDLN     device;

int gpio_emu_spi_command_prompt()
{
	uint8_t spi_status = 0;

	while (1)
	{
		printf("====================================\n");
		printf(" Diolan DLN-2 SPI Command mode  \n");
		printf("====================================\n");
		printf("Command:\n");
		printf("1.  Test 1 \n");
		printf("2.  Test 2 \n");
		printf("3.  Test 3 \n");
		printf("10. Exit \n");
		int scan_in;
		scanf("%d", &scan_in);

		switch (scan_in) {
		case 1: break;
		case 2: break;
		case 3: break;
		case 10:  
				while (DlnSpiMasterDisable(device, 0, spi_status) != 0x00);
				return 0;
		default:  printf("Invailid selection...\n");
		}
		printf("\n\n\n");
	}
}

/*
	DlnGpioPinSetDirection(device, 27, 1);   //RESET
	DlnGpioPinEnable(device, 27);
	DlnGpioPinSetOutVal(device, 27, 1);

	DlnGpioPinSetDirection(device, 28, 1);   //SCLK
	DlnGpioPinEnable(device, 28);
	DlnGpioPinSetOutVal(device, 28, 0);

	DlnGpioPinSetDirection(device, 29, 1);   //SDA out
	DlnGpioPinEnable(device, 29);
	DlnGpioPinSetOutVal(device, 29, 1);

	DlnGpioPinSetDirection(device, 30, 1);   //SRDY
	DlnGpioPinEnable(device, 30);
	DlnGpioPinSetOutVal(device, 30, 1);

	DlnGpioPinPulldownEnable(device, 31);
	DlnGpioPinSetDirection(device, 31, 0);   //SDA in
	DlnGpioPinEnable(device, 31);
*/
