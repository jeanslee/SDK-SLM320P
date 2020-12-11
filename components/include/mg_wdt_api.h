#include "osi_log.h"
#include "drv_wdt.h"
#include <errno.h>

bool MG_drvWatchdogStart(uint32_t reset_ms);

bool MG_drvWatchdogReload(uint32_t reset_ms);

void MG_drvWatchdogStop(void);
