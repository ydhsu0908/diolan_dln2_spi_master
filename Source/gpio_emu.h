#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <bitset>
#include <string>

using namespace std;

#define DLN2_SPI_MODE     1          //Using DLN-2 SPI+GPIO mode or GPIO mode only
#define DLN2_SPI_8bit     1          //0: 16-bit, 1: 8-bit

int             gpio_emu_spi_command_prompt();
void            gpio_emu_spi_test_clock();
void            gpio_emu_spi_reset();
void            gpio_emu_spi_businit();
void            gpio_emu_spi_write(uint32_t address_32, uint32_t data_32);
std::bitset<32> gpio_emu_spi_read(uint32_t address_32);
void            gpio_emu_test_code_1();
void            gpio_emu_test_code_2(int cnt);
void            gpio_emu_test_code_3(int cnt);
char*           gpio_emu_spi_reg_str(uint32_t address);
