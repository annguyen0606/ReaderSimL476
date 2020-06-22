/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NFCM1833TINY_H
#define __NFCM1833TINY_H

#include "nfcm1833_module.h"

#ifdef STM32L432xx
#include "stm32l4xx_hal.h"
#endif /*STM32L432xx*/

#ifdef STM32F401xC
#include "stm32f4xx_hal.h"
#endif

ping_status ping_module_uart(void);
protocol_status_t select_tag_type_uart(tag_type_t tag_type);
send_recv_status_t sendrecv_uart(uint8_t * command, uint8_t command_len, cr95hf_recv_format_t * data_result);
send_recv_status_t getDeviceID_uart(uint8_t * deviceID);

#endif /*__NFCM1833TINY_H */