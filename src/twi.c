
#include "twi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>


static twi_device_t * dev;


/**
 * @brief twi_send
 * @param data          pointer to data to send
 * @param len           length of data to send
 * @return              bytes sent if greator than or equal to 0
 *                      -1 for error. See errno for corresponding error
 */
static int twi_send(uint8_t * data, int len){

	int rc = write(dev->fd, data, len);

	return rc;
}

/**
 * @brief twi_recv
 * @param data          pointer to data for received
 * @param len           length of data received
 * @return              bytes received if greator than or equal to 0
 *                      -1 for error. See errno for corresponding error
 */
static int twi_recv(uint8_t * data, int len){

	int rc = read(dev->fd, data, len);

	return rc;
}

/**
 * @brief twi_init
 * @return
 */
static int twi_init(void){

	int fd;
	int rc;
	char * dev_name = TWI_BUS_ADDRESS;

	fd = open(dev_name, O_RDWR);
	if (fd < 0) {
		rc = fd;
		printf("twi_init: open error %d\n", rc);
		goto FAIL_OPEN;
	}

	rc = ioctl(fd, I2C_SLAVE, LPC804_SLAVE_ADDRESS);
	if (rc < 0) {
		printf("twi_init: ioctl error %d\n", rc);
		goto FAIL_IOCTL;
	}

	dev->fd = fd;

	return 0;

FAIL_IOCTL:
	close(fd);

FAIL_OPEN:
	return rc;
}

/**
 * @brief twi_close
 */
static void twi_close(void){
	close(dev->fd);
}

void twi_initialise(twi_device_t * twi_dev){
	dev = twi_dev;
	dev->init = twi_init;
	dev->write = twi_send;
	dev->read = twi_recv;
	dev->close = twi_close;
}
