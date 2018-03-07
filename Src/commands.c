#include <stdio.h>

#include "terminal.h"
#include "commands.h"

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
extern const sTermEntry_t triggerDistanceEntry;
extern const sTermEntry_t triggerTimeEntry;

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
        0
};
