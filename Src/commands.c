#include <stdio.h>

#include "terminal.h"
#include "commands.h"
#include "nvm.h"

extern const sTermEntry_t hEntry;
extern const sTermEntry_t helpEntry;
extern const sTermEntry_t rebootEntry;
extern const sTermEntry_t rtcEntry;
extern const sTermEntry_t dateEntry;
extern const sTermEntry_t spiEntry;
extern const sTermEntry_t writespiEntry;
extern const sTermEntry_t readspiEntry;
extern const sTermEntry_t erasespiEntry;
extern const sTermEntry_t ackEntry;
extern const sTermEntry_t ddebugEntry;

const sTermEntry_t *term_entries[] =
{
        &hEntry,
        &helpEntry,
        &rebootEntry,
        &rtcEntry,
		&dateEntry,
		&spiEntry,
		&writespiEntry,
		&readspiEntry,
		&erasespiEntry,
		&ackEntry,
		&ddebugEntry,
		&triggerDistanceEntry,
		&triggerTimeEntry,
		&minTimeEntry,
        0
};
