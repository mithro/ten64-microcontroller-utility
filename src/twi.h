#ifndef TWI_H
#define TWI_H

#include <inttypes.h>
#include <stdint.h>

#define LPC804_SLAVE_ADDRESS        0x7E
#define TWI_BUS_ADDRESS             "/dev/i2c-0"


typedef enum {
	twiSuccess = 0U,
	twiError = 1U
} twi_error_t;

typedef struct {
	int32_t fd;
	int (*init)(void);
	int (*write)(uint8_t *, int);
	int (*read)(uint8_t *, int);
	void (*close)(void);
} twi_device_t;

void twi_initialise(twi_device_t * twi_dev);

#endif 
