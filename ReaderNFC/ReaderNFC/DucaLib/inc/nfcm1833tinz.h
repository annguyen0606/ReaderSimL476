/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NFCM1833TINZ_H
#define __NFCM1833TINZ_H

#include "nfcm1833_module.h"

#ifdef STM32L432xx
#include "stm32l4xx_hal.h"
#endif /*STM32L432xx*/

#ifdef STM32F401xC
#include "stm32f4xx_hal.h"
#endif /*STM32F401CB*/

#define SPI_TIMEOUT 50

ping_status ping_module_spi (void);
protocol_status_t select_tag_type_spi (tag_type_t tag_type);
send_recv_status_t sendrecv_spi (uint8_t* command, uint8_t command_len, cr95hf_recv_format_t* data_result);
send_recv_status_t sendrecv_spi_with_iso14443A (uint8_t* command, uint8_t command_len, cr95hf_recv_format_t* data_result);
send_recv_status_t getDeviceID_spi (uint8_t* deviceID);
send_recv_status_t getDeviceID_type4A_spi (uint8_t* deviceID);
HAL_StatusTypeDef spi_reset_cr95hf (void);
void WakeUp_TinZ (void);

#endif /*__NFCM1833TINZ_H */