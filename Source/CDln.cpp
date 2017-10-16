#include <Dln.h>
#include <dln_generic.h>
#include <dln_spi_master.h>
#include "CDln.h"
#include <vector>
#include <stdint.h>
using namespace std;

#define CPOL0 (0)
#define CPOL1 (1)
#define CPHA0 (0 << 1)
#define CPHA1 (1 << 1)

#define IF_FAIL(x)	if (DLN_FAILED(x)){ printf("%s[%d]: DLN failed!\n", __FUNCTION__, __LINE__); assert(0); }
#define DLN_VERSION_TO_STRING(x) ((x == DLN_HW_TYPE_DLN5) ? "DLN5" : (x == DLN_HW_TYPE_DLN4M) ? "DLN4M" : (x == DLN_HW_TYPE_DLN4S) ? "DLN4S" : (x == DLN_HW_TYPE_DLN3) ? "DLN3" : (x == DLN_HW_TYPE_DLN2) ? "DLN2" : (x == DLN_HW_TYPE_DLN1) ? "DLN1" : "Unknown")

#define BYTE_SWAP32(x) \
	((((x)& 0xff000000) >> 24) | (((x)& 0x00ff0000) >> 8) | \
	(((x)& 0x0000ff00) << 8) | (((x)& 0x000000ff) << 24))

//#define DLN_SWAP32(x) (((x & 0xFF000000) >> 8) | ((x & 0x00FF0000) << 8) | ((x & 0x0000FF00) >> 8) | ((x & 0x000000FF) << 8))
#define DLN_SWAP32(x) (((x & 0xFF000000) >> 16) | ((x & 0x00FF0000) >> 16) | ((x & 0x0000FF00) << 16) | ((x & 0x000000FF) << 16))
//#define DLN_SWAP32(x) (x)

typedef enum _SPI_CMD
{
	SPI_CMD_NOP = 0x00,
	SPI_CMD_INIT = 0x01,
	SPI_CMD_READ_ALLOCATE = 0x04,
	SPI_CMD_READ_FETCH = 0x05,
	SPI_CMD_PING = 0x06,
	SPI_CMD_WRITE = 0x07,
	SPI_CMD_CPU_RESET = 0x08,
	SPI_CMD_CPU_RESET_RELEASE = 0x09,
}SPI_CMD;

bool IsTimeout(ULONGLONG startTick, ULONGLONG timeLimit)
{
	ULONGLONG	currTick = GetTickCount64();
	bool		result = false;

	if ((currTick - startTick) >= timeLimit)
	{
		result = true;
	}

	return result;
}

CDln::CDln()
{
}

CDln::~CDln()
{
	DLN_RESULT	dlnResult;

	dlnResult = DlnCloseHandle(m_DlnHandle);
	dlnResult = DlnDisconnect("localhost", DLN_DEFAULT_SERVER_PORT);
}

bool CDln::init()
{
	DLN_INIT_PARAMS params = { 0 };

	// Use default value
	params.devNum = 0;
	params.cpol = 1;
	params.cpha = 0;
	params.frameSize = 16;
	params.delayBetweenFrame = 5;
	params.delayAfterSS = 5;

	return init(&params);
}

bool CDln::init(DLN_INIT_PARAMS* params)
{
	bool		result = true;
	HDLN		dlnHandle = 0;
	DLN_RESULT	dlnResult;
	uint32_t	dlnDeviceSn = 0;
	DLN_VERSION	dlnVersion = { 0 };
	uint32_t	actualDelayAfterSS = 0;
	uint32_t	actualDelayBetweenFrames = 0;
	uint32_t	actualFrequency = 0;
	uint32_t	currFrequency = 0;
	uint16_t	masterEnableconflict;

	do
	{
		uint8_t		dev_curr_mode = 0;
		uint8_t		dev_curr_frameSize = 0;
		uint32_t	cpValue = 0;

		dlnResult = DlnConnect("localhost", DLN_DEFAULT_SERVER_PORT);
		if (DLN_FAILED(dlnResult))
		//if (!DLN_SUCCEEDED(dlnResult))
		{
			printf("DlnConnect() failed. dlnResult = %d\n", dlnResult);
			result = false;
			break;
		}

		dlnResult = DlnOpenDevice(params->devNum, &dlnHandle);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnOpenDevice() failed.\n");
			result = false;
			break;
		}

		dlnResult = DlnGetDeviceSn(dlnHandle, &dlnDeviceSn);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnGetDeviceSn() failed.\n");
		}

		dlnResult = DlnGetVersion(dlnHandle, &dlnVersion);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnConnect() failed.\n");
		}

		cpValue = (params->cpol == 1) ? CPOL1 : CPOL0;
		cpValue |= (params->cpha == 1) ? CPHA1 : CPHA0;
		dlnResult = DlnSpiMasterGetMode(dlnHandle, params->devNum, &dev_curr_mode);
		if (DLN_SUCCEEDED(dlnResult) && (dev_curr_mode != cpValue))
		{
			dlnResult = DlnSpiMasterSetMode(dlnHandle, params->devNum, cpValue);
			if (DLN_FAILED(dlnResult))
			{
				printf("DlnSpiMasterSetMode() failed.\n");
				//result = false;
				break;
			}
		}

		dlnResult = DlnSpiMasterGetFrameSize(dlnHandle, params->devNum, &dev_curr_frameSize);
		if (DLN_SUCCEEDED(dlnResult) && (dev_curr_frameSize != params->frameSize))
		{
			dlnResult = DlnSpiMasterSetFrameSize(dlnHandle, params->devNum, params->frameSize);
			if (DLN_FAILED(dlnResult))
			{
				printf("DlnSpiMasterSetFrameSize() failed.\n");
				result = false;
				break;
			}
		}

		dlnResult = DlnSpiMasterGetFrequency(dlnHandle, params->devNum, &currFrequency);
		if (DLN_SUCCEEDED(dlnResult) && (currFrequency != params->clock))
		{
			dlnResult = DlnSpiMasterSetFrequency(dlnHandle, params->devNum, params->clock, &actualFrequency);
			if (DLN_FAILED(dlnResult))
			{
				printf("DlnSpiMasterSetFrequency() failed.\n");
				result = false;
				break;
			}
		}

		//dlnResult = DlnSpiMasterSetDelayAfterSS(dlnHandle, params->devNum, params->delayAfterSS, &actualDelayAfterSS);
		//dlnResult = DlnSpiMasterSetDelayBetweenFrames(dlnHandle, params->devNum, params->delayBetweenFrame, &actualDelayBetweenFrames);
		dlnResult = DlnSpiMasterEnable(dlnHandle, params->devNum, &masterEnableconflict);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnSpiMasterEnable() failed.\n");
			result = false;
			break;
		}

#if 0
		printf("DeviceSn         : 0x%08X\n", dlnDeviceSn);
		printf("Hardware Type    : %s\n", DLN_VERSION_TO_STRING(dlnVersion.hardwareType));
		printf("Hardware Version : 0x%08X\n", dlnVersion.hardwareVersion);
		printf("Firmware Version : 0x%08X\n", dlnVersion.firmwareVersion);
		printf("Server Version   : 0x%08X\n", dlnVersion.serverVersion);
		printf("Library Version  : 0x%08X\n", dlnVersion.libraryVersion);
#endif
	} while (0);

	if (result == true)
	{
		m_DlnHandle = dlnHandle;
		memcpy(&m_InitParams, params, sizeof(DLN_INIT_PARAMS));
	}

	return result;
}

bool CDln::ping()
{
	bool		result = true;
	DLN_RESULT	dlnResult;
	uint32_t	pingValue[2] = { 0 };
	uint32_t	pingReturnValue[2] = { 0 };
	ULONGLONG	startTick = 0;
	ULONGLONG	endTick = 0;

	pingValue[0] |= (SPI_CMD_PING << 28);

	startTick = GetTickCount64();
	while (1)
	{
		pingValue[0] = DLN_SWAP32(pingValue[0]);
		dlnResult = DlnSpiMasterReadWrite(m_DlnHandle, m_InitParams.devNum, 8, (uint8_t*)pingValue, (uint8_t*)pingReturnValue);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnSpiMasterWrite() error!\n");
			result = false;
			break;
		}

		if (pingReturnValue[0])
		{
			// Got!
			break;
		}

		if (IsTimeout(startTick, 1000) == true)	// Wait for 1000 ms
		{
			// Time out
			result = false;
			break;
		}
	}

	return result;
}

bool CDln::read(uint32_t addr, uint32_t len, string outputFile)
{
	bool		result = true;
	DLN_RESULT	dlnResult;
	uint32_t	cmdRA[2] = { 0 };
	uint32_t*   cmdrf = NULL;
	uint32_t*   cmdrfReturn = NULL;

	do
	{
		uint32_t cmdra_len = len + 4;
		cmdRA[0] |= (SPI_CMD_READ_ALLOCATE << 28);
		cmdRA[0] |= len;
		cmdRA[0] = DLN_SWAP32(cmdRA[0]);
		cmdRA[1] = addr;
		cmdRA[1] = DLN_SWAP32(cmdRA[1]);

		dlnResult = DlnSpiMasterWrite(m_DlnHandle, m_InitParams.devNum, 8, (uint8_t*)cmdRA);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnSpiMasterWrite() error!\n");
			result = false;
			break;
		}

		if (ping() == false)
		{
			printf("Ping command fail.\n");
			result = false;
			break;
		}

		len = (len + 1) * 4;
		cmdrf = (uint32_t*)new uint8_t[len];
		if (cmdrf == NULL)
		{
			printf("Out of memory!\n");
			result = false;
			break;
		}
		memset(cmdrf, 0, len);

		cmdrfReturn = (uint32_t*)new uint8_t[len];
		if (cmdrfReturn == NULL)
		{
			delete[] cmdrf;
			printf("Out of memory!\n");
			result = false;
			break;
		}
		memset(cmdrfReturn, 0, len);

		cmdrf[0] |= (SPI_CMD_READ_FETCH << 28);
		cmdrf[0] = DLN_SWAP32(cmdrf[0]);
		//dlnResult = DlnSpiMasterReadWrite(m_DlnHandle, m_InitParams.devNum, len, (uint16_t*)cmdrf, (uint16_t*)cmdrfReturn);
		dlnResult = DlnSpiMasterReadWrite16(m_DlnHandle, m_InitParams.devNum, len / 2, (uint16_t*)cmdrf, (uint16_t*)cmdrfReturn);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnSpiMasterWrite() error!\n");
			result = false;
			break;
		}

		if (outputFile != "")
		{
			// Output to file
			FILE* pfile = fopen(outputFile.c_str(), "w");
			if (pfile)
			{
				for (uint32_t i = 1; i < ((len / 4)); i++) //liang
				{
					cmdrfReturn[i] = DLN_SWAP32(cmdrfReturn[i]); //liang
					fprintf(pfile, "0x%08X\n", cmdrfReturn[i]);
				}
				fclose(pfile);
			}
		}
		else
		{
			// Print on console
			for (uint32_t i = 1; i < ((len / 4)); i++) //liang
			{
				cmdrfReturn[i] = DLN_SWAP32(cmdrfReturn[i]); //liang
				printf("0x%08X\n", cmdrfReturn[i]);
        
        uint32_t tmp = cmdrfReturn[i];
        printf("0b");
        for (uint32_t j = 0; j < 32; j++) 
        {
          printf("%1d", tmp / (1 << 31));
          tmp <<= 1;
		  if (j % 4 == 3) {
			  printf("_");
		  }
        }
        printf("\n");
			}
			printf("\n");
		}

		delete[] cmdrf;
		delete[] cmdrfReturn;
	} while (0);

	return result;
}

bool CDln::write(uint32_t addr, vector<uint32_t> datas)
{
	bool		result = true;
	DLN_RESULT	dlnResult;
	uint32_t*	cmd_buf = NULL;
	uint32_t	cmd_buf_len = (datas.size() + 2) * 4; // cmd_header + address + cmd_size

	do
	{
		cmd_buf = (uint32_t*)calloc(1, cmd_buf_len);
		if (cmd_buf == NULL)
		{
			printf("Out of memory\n");
			break;
		}

		// Fill command write
		cmd_buf[0] |= (SPI_CMD_WRITE << 28);	// CmdW
		
		cmd_buf[0] |= datas.size();
		
		cmd_buf[0] = DLN_SWAP32(cmd_buf[0]);

		// Fill address
		cmd_buf[1] = addr;	// address
		cmd_buf[1] = DLN_SWAP32(cmd_buf[1]);

		// Copy data & byte swap

		uint32_t cmd_buf_index = 2;
		for (size_t i = 0; i < datas.size(); i++, cmd_buf_index++)
		{
			cmd_buf[cmd_buf_index] = datas[i];
			cmd_buf[cmd_buf_index] = DLN_SWAP32(cmd_buf[cmd_buf_index]);
		}

		// nop in the tail
		//cmd_buf[cmd_buf_len - 1] = 0;	// nop is 0

		dlnResult = DlnSpiMasterWrite(m_DlnHandle, m_InitParams.devNum, cmd_buf_len, (uint8_t*)cmd_buf);
		if (DLN_FAILED(dlnResult))
		{
			printf("DlnSpiMasterWrite() error!\n");
			result = false;
			break;
		}

		free(cmd_buf);

		if (ping() == false)
		{
			return false;
		}
	} while (0);

	return result;
}

bool CDln::nop()
{
	bool		result = true;
	DLN_RESULT	dlnResult;
	uint32_t	nopValue = 0;

	dlnResult = DlnSpiMasterWrite(m_DlnHandle, m_InitParams.devNum, 4, (uint8_t*)&nopValue);
	if (DLN_FAILED(dlnResult))
	{
		printf("DlnSpiMasterWrite() error!\n");
		result = false;
	}

	return result;
}