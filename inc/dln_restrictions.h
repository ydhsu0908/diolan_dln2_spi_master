#ifndef DLN_RESTRICTIONS_H
#define DLN_RESTRICTIONS_H

#if defined(_MSC_VER) && _MSC_VER < 1600
#include "stdint.h"
#else // _MSC_VER
#include <stdint.h>
#endif // _MSC_VER

typedef uint8_t DLN_RESTRICTION;

#define DLN_RSTR_NO_RESTICTION          ((DLN_RESTRICTION) 0x00)
#define DLN_RSTR_MUST_BE_DISABLED       ((DLN_RESTRICTION) 0x01)
#define DLN_RSTR_MUST_BE_ENABLED        ((DLN_RESTRICTION) 0x02)
#define DLN_RSTR_MUST_BE_IDLE           ((DLN_RESTRICTION) 0x03)

#define DLN_RSTR_SET_NOT_SUPPORTED ((DLN_RESTRICTION) 0xFE)
#define DLN_RSTR_COMMAND_NOT_SUPPORTED  ((DLN_RESTRICTION) 0xFF)

#endif // DLN_RESTRICTIONS_H
