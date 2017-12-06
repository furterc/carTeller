#include <stdio.h>

#include "terminal.h"
#include "commands.h"

extern const sTermEntry_t hEntry;
extern const sTermEntry_t helpEntry;
extern const sTermEntry_t rebootEntry;
extern const sTermEntry_t rtcEntry;
extern const sTermEntry_t dateEntry;
extern const sTermEntry_t pulseEntry;
extern const sTermEntry_t spiEntry;
extern const sTermEntry_t writespiEntry;
extern const sTermEntry_t readspiEntry;
extern const sTermEntry_t ddebugEntry;
extern const sTermEntry_t carDistEntry;
extern const sTermEntry_t carTimeEntry;

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
		&pulseEntry,
		&ddebugEntry,
		&carDistEntry,
		&carTimeEntry,
        0
};
