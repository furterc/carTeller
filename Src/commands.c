#include <stdio.h>

#include "terminal.h"
#include "commands.h"

extern const sTermEntry_t hEntry;
extern const sTermEntry_t helpEntry;
extern const sTermEntry_t rebootEntry;
extern const sTermEntry_t rtcEntry;
extern const sTermEntry_t dateEntry;
extern const sTermEntry_t pulseEntry;
extern const sTermEntry_t ddebugEntry;

const sTermEntry_t *term_entries[] =
{
        &hEntry,
        &helpEntry,
        &rebootEntry,
        &rtcEntry,
		&dateEntry,
		&pulseEntry,
		&ddebugEntry,
        0
};
