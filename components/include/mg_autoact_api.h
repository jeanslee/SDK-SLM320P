#ifndef _MG_AUTOACT_API_H_
#define _MG_AUTOACT_API_H_
#include <stdint.h>
/*
flags: 0 close auto actived, 1 oepn auto actived.
nSimID: SIM ID(0,1), default 0.
 
Note:After the setting is successful, restart the strength

 */
uint32_t MG_NwSetAutoAttachFlag(uint8_t flag, uint8_t nSimID);

/*
get auto actived flags

returun:
1 auto actived, 0 manual actived.

*/
uint8_t MG_NwGetAutoAttachFlag(uint8_t nSimID);

#endif
