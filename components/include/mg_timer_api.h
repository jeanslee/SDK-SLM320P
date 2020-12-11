#include<osi_api.h>
#include <errno.h>




osiTimer_t *MG_osiTimerCreate(osiThread_t *thread, osiCallback_t cb, void *ctx);

void MG_osiTimerDelete(osiTimer_t *timer);

bool MG_osiTimerSetPeriod(osiTimer_t *timer, uint32_t ms, bool periodic);

bool MG_osiTimerSetPeriodRelaxed(osiTimer_t *timer, uint32_t ms, uint32_t relaxed_ms, bool periodic);

bool MG_osiTimerStartLast(timer);

bool MG_osiTimerStart(osiTimer_t *timer, uint32_t ms);

bool MG_osiTimerStop(osiTimer_t *timer);