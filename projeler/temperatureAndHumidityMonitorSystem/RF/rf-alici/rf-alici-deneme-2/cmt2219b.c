
#include <stdbool.h>
extern bool CMT2219B_GoSleep(void);
/*! ********************************************************
* @name CMT2219B_GoSleep
* @desc Entry SLEEP mode.
* @return TRUE or FALSE
* *********************************************************/
bool CMT2219B_GoSleep(void)
{
 return CMT2219B_AutoSwitchStatus(CMT2219B_GO_SLEEP);
}
/*! ********************************************************
* @name CMT2219B_GoStby
* @desc Entry Sleep mode.
* @return TRUE or FALSE
* *********************************************************/
bool CMT2219B_GoStby(void)
{
 return CMT2219B_AutoSwitchStatus(CMT2219B_GO_STBY);
}
/*! ********************************************************
* @name CMT2219B_GoRFS
* @desc Entry RFS mode.
* @return TRUE or FALSE
* *********************************************************/
bool CMT2219B_GoRFS(void)
{
 return CMT2219B_AutoSwitchStatus(CMT2219B_GO_RFS);
}
/*! ********************************************************
* @name CMT2219B_GoRx
* @desc Entry Rx mode.
* @return TRUE or FALSE
* *********************************************************/
bool CMT2219B_GoRx(void)
{
 return CMT2219B_AutoSwitchStatus(CMT2219B_GO_RX);
}
