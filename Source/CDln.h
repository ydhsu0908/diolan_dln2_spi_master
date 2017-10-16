#include <iostream>
#include <vector>
#include <dln.h>
using namespace std;
#pragma once

typedef struct _DLN_INIT_PARAMS
{
	uint32_t devNum;
	uint32_t cpol;
	uint32_t cpha;
	uint32_t frameSize;
	uint32_t delayBetweenFrame;
	uint32_t delayAfterSS;
	uint32_t clock;
}DLN_INIT_PARAMS;

class CDlnException : public std::exception
{
};

class CDln
{
public:
	CDln();
	~CDln();

	bool init(DLN_INIT_PARAMS* params);
	bool init();
	bool read(uint32_t addr, uint32_t len, string outputFile);
	bool write(uint32_t addr, vector<uint32_t> datas);
	bool ping();
	bool nop();

private:

private:
	HDLN			m_DlnHandle;
	DLN_INIT_PARAMS	m_InitParams;
};