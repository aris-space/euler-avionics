#ifndef _W25QXXCONFIG_H
#define _W25QXXCONFIG_H

extern SPI_HandleTypeDef hspi4;

#define _W25QXX_SPI hspi4
#define _W25QXX_CS_GPIO SPI4_CS_GPIO_Port
#define _W25QXX_CS_PIN SPI4_CS_Pin
#define _W25QXX_USE_FREERTOS 1
#define _W25QXX_DEBUG 0

#endif
