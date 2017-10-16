#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <bitset>
#include <iostream>
#include <iomanip>
#include "iniparser.h"
#include "CDln.h"
#include "dln_spi_master.h"
#include "dln_generic.h"
#include "spi_emu.h"
#include "gpio_emu.h"

#define _CRT_SECURE_NO_WARNINGS 

extern HDLN     device;

void spi_emu_spi_businit()
{
	cout << "   [SPI bus init by DLN-2 SPI interface] \n";

	uint8_t spi_status = 0;
	while (DlnSpiMasterDisable(device, 0, spi_status) != 0x00);

	//set SPI frequency
	uint32_t frequency;
	//DlnSpiMasterSetFrequency(device, 0, 2000, &frequency);     //2KHz
    DlnSpiMasterSetFrequency(device, 0, 4000, &frequency);     //4KHz
	//DlnSpiMasterSetFrequency(device, 0, 1000000, &frequency);  //1MHz
	//DlnSpiMasterSetFrequency  (device, 0, 2000000, &frequency);  //2MHz
	//DlnSpiMasterSetFrequency(device, 0, 3250000, &frequency);  //3.25MHz	
	//DlnSpiMasterSetFrequency(device, 0, 4000000, &frequency);  //4MHz	
	//DlnSpiMasterSetFrequency(device, 0,   8000000, &frequency);  //8MHz	
	//DlnSpiMasterSetFrequency(device, 0, 10000000, &frequency); //10MHz
	//DlnSpiMasterSetFrequency(device, 0, 12000000, &frequency); //12MHz
	//DlnSpiMasterSetFrequency(device, 0, 18000000, &frequency);   //18MHz

	if (frequency > 1000000)
	{
		printf("     * DLN-2 SPI clock frequency = %d MHz \n", frequency/1000000);
	}
	else if (frequency > 1000)
	{
		printf("     * DLN-2 SPI clock frequency = %d KHz \n", frequency/1000);
	}
	else
	{
		printf("     * DLN-2 SPI clock frequency = %d Hz \n", frequency);
	}

	//set SPI mode
	DlnSpiMasterSetMode(device, DLN_SPI_MASTER_CPOL_0, DLN_SPI_MASTER_CPHA_0); //CPOL=0; CPHA=0
	printf("     * DLN-2 SPI CPOL = 0 \n");
	printf("     * DLN-2 SPI CPHA = 0 \n");


	if (DLN2_SPI_8bit)
	{
		DlnSpiMasterSetFrameSize(device, 0, 8);
		printf("     * DLN-2 SPI 8-bit mode\n");
	}
	else
	{
		DlnSpiMasterSetFrameSize(device, 0, 16);
		printf("     * DLN-2 SPI 16-bit mode\n");
	}

	/*
	uint32_t interval;
	DlnSpiMasterGetDelayBetweenFrames(device, 0, &interval);
	printf("Interval: %d.\n", interval);

	DLN_RESULT result;
	result = DlnSpiMasterSetDelayBetweenFrames(device, 0, 0, &interval);
	printf("Result: %x \n", result);

	DlnSpiMasterGetDelayBetweenFrames(device, 0, &interval);
	printf("Interval: %d \n",interval);
	*/

    //Enable SPI master
	uint16_t conflict;
	if (DlnSpiMasterEnable(device, 0, &conflict) == 0x00)
	{
		//cout << "  SPI master is enabled...\n";
	}
	else
	{
		cout << "  ERROR!! DLN-2 SPI master is not enabled...";
		exit(1);
	}

	//wait idle
	Sleep(1);
}
