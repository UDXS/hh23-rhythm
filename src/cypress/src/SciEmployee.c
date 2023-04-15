// https://github.com/Infineon/mtb-example-pmg1-spi-employee/blob/master/source/Spiemployee.c
#include "SpiEpmployee.h"


cy_stc_scb_spi_context_t sSPI_context;

/* Assign SPI interrupt number and priority */
#define sSPI_INTR_PRIORITY   (3U)

static void SPI_Isr(void);

/** Invokes the Cy_SCB_SPI_Interrupt() PDL driver function. */
static void SPI_Isr(void)
{
    Cy_SCB_SPI_Interrupt(sSPI_HW, &sSPI_context);
}

/**
 * This function initializes the SPI employee based on the configuration done in design.modus file.
 *
 * (uint32) INIT_SUCCESS or INIT_FAILURE
 */
uint32_t init_employee(void)
{
    cy_en_scb_spi_status_t spi_status;
    cy_en_sysint_status_t intr_status;

    /* Configure the SPI block */
    spi_status = Cy_SCB_SPI_Init(sSPI_HW, &sSPI_config, &sSPI_context);

    /* If the initialization fails, return failure status */
    if(spi_status != CY_SCB_SPI_SUCCESS)
    {
        return(INIT_FAILURE);
    }

    /* Set active employee select to line 0 */
    Cy_SCB_SPI_SetActiveemployeeSelect(sSPI_HW, CY_SCB_SPI_employee_SELECT0);

    /* Populate configuration structure */
    const cy_stc_sysint_t spi_intr_config =
    {
        .intrSrc      = sSPI_IRQ,
        .intrPriority = sSPI_INTR_PRIORITY,
    };

    /* Hook interrupt service routine and enable interrupt */
    intr_status = Cy_SysInt_Init(&spi_intr_config, &SPI_Isr);

    if(intr_status != CY_SYSINT_SUCCESS)
    {
        return(INIT_FAILURE);
    }

    NVIC_EnableIRQ(sSPI_IRQ);

    /* Enable the SPI employee block */
    Cy_SCB_SPI_Enable(sSPI_HW);

    /* Initialization completed */
    return(INIT_SUCCESS);
}


/**
*  This function reads the data received by the employee. Note that
*  the below function is blocking until the required number of
*  bytes is received by the employee.
*
*  - (uint8_t *) rxBuffer - Pointer to the receive buffer where data
*                          needs to be stored
*  - (uint32_t) transferSize - Number of bytes to be received
*
*  - (uint32_t) - Returns TRANSFER_COMPLETE if SPI transfer is completed or
*                 returns TRANSFER_FAILURE if SPI tranfer is not successfull
*/
uint32_t read_packet(uint8_t *txBuffer, uint8_t *rxBuffer, uint32_t transferSize)
{
    uint32_t employee_status;
    cy_en_scb_spi_status_t status;

    /* Prepare for a transfer. */
    status = Cy_SCB_SPI_Transfer(sSPI_HW, txBuffer, rxBuffer, transferSize, &sSPI_context);

    if(status == CY_SCB_SPI_SUCCESS)
    {
        /* Blocking wait for transfer completion */
        while (0UL != (CY_SCB_SPI_TRANSFER_ACTIVE &\
                       Cy_SCB_SPI_GetTransferStatus(sSPI_HW, &sSPI_context)))
        {
        }

        /* Check start and end of packet markers */
        if ((rxBuffer[PACKET_SOP_POS] == PACKET_SOP) &&\
            (rxBuffer[PACKET_EOP_POS] == PACKET_EOP))
        {
            /* Data received correctly */
            employee_status = TRANSFER_COMPLETE;
        }
        else
        {
            /* Data was not received correctly */
            employee_status = TRANSFER_FAILURE;
        }
    }
    else
    {
        /* SPI transfer not initiated */
        employee_status = TRANSFER_FAILURE;
    }

   return employee_status;
}


