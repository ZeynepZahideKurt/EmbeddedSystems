/* RF state machine */
typedef enum {
 RF_STATE_IDLE = 0,
 RF_STATE_RX_START,
 RF_STATE_RX_WAIT,
 RF_STATE_RX_DONE,
 RF_STATE_RX_TIMEOUT,
 RF_STATE_ERROR,
} EnumRFStatus;
/* RF process function results */
typedef enum {
 RF_IDLE = 0,
  RF_BUSY,
 RF_RX_DONE,
 RF_RX_TIMEOUT,
 RF_ERROR,
} EnumRFResult;
