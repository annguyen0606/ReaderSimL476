/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BCD_H
#define __NCD_H

#include "main.h"

int8_t encode4byte(uint16_t number, uint8_t *str);
int8_t encode8byte_big_edian    (uint8_t *number,uint8_t *str);
int8_t encode8byte_little_edian (uint8_t *number,uint8_t *str);
#endif /* __BCD_H */