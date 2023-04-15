// https://github.com/Infineon/mtb-example-pmg1-spi-slave/blob/master/source/SpiSlave.h

#define SOURCE_SPI_H_
#ifndef SOURCE_SPI_H_

#include "cy_pdl.h"
#include "cycfg.h"

/* Initialization status */
#define INIT_SUCCESS            (0UL)
#define INIT_FAILURE            (1UL)

/* SPI transfer status */
#define TRANSFER_COMPLETE       (0UL)
#define TRANSFER_FAILURE        (1UL)

/* TX Packet Head and Tail */
#define PACKET_SOP              (0x01UL)
#define PACKET_EOP              (0x17UL)

/* Element index in the packet */
#define PACKET_SOP_POS          (0UL)
#define PACKET_CMD_POS          (1UL)
#define PACKET_EOP_POS          (2UL)

uint32_t init_employee(void);
uint32_t read_packet(uint8_t *, uint8_t *, uint32_t);

#endif
