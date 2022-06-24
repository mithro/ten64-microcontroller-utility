/* Ten64 Microcontroller Utility
 * Copyright 2021 Traverse Technologies Australia
 *
 * SPDX-License-Identifier:	MIT
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include "api.h"
#include "twi.h"

twi_device_t twi;

static void usage(const char *progname){
#ifdef ENABLE_MAC_CHANGE
	printf("%s --set-mac <mac>                   Set the MAC address\n", progname);
#endif
	printf("%s --get-info                        Get uC info\n", progname);
	printf("%s --get-state                       Get system control state\n", progname);
	printf("%s --set-reset-holdtime <{3:10}>     Set the hold time for the external reset button\n", progname);
	printf("%s --set-reset-enabled <0|1>         Enable/disable the external button reset\n", progname);
	printf("%s --image-info                      Get current image data stored in both flash slots\n", progname);
	printf("%s --fwup-run <a|b> <new_fw_file>    Firmware upgrade run\n", progname);
	printf("%s --fwup-boot                       Firmware upgrade boot into new firmware. NOTE: will reset main CPU\n", progname);
	printf("%s %-33s %s",progname,"--fwup-next-slot","Get the slot the next image should be installed in (use carefully!)\n");
	printf("%s %-33s %s",progname,"--get-version","Get microcontroller version as a six-digit number (major-minor-patch)\n");
	printf("%s %-33s %s",progname,"--get-bootcount","Get system bootcount (since reboot) and next boot part register\n");
	printf("%s %-33s %s",progname,"--get-next-bootpart", "Return the next bootpart (by itself, no annotation), or exit=1 if not set\n");
	printf("%s %-33s %s",progname,"--set-next-bootpart <x> <ttl>","Set next bootpart to <x> with a TTL of <ttl> reboots\n");
}

static uint8_t get_arg_value(char * arg, const char * key, char * value){
	char delimeter[2] = "=";
	char * token;

	if (strstr(arg, key) && strstr(arg, delimeter)){
		token = strtok(arg, delimeter);
		// first token should be the key
		if (!strcmp(token, key)){
			// next token should be the value
			token = strtok(NULL, delimeter);
			strcpy(value, token);
			return 0;
		} else
			return 1;
	} else
		return 2;
}

int process_fwup_run(const twi_device_t *twi, char **argv)
{
	lpc_fwup_run_state_t fwup_run_state;
	lpc_image_header_t image_header;
	uint8_t bank_id;
	uint8_t retcode;
	const char *fw_path = argv[3];
	char bank = argv[2][0];

	switch (bank)
	{
	case 'a':
		bank_id = bankId1;
		break;
	case 'b':
		bank_id = bankId2;
		break;
	default:
		fprintf(stderr, "Image bank/slot attribute should be 'a' or 'b'\n");
		return -EINVAL;
		break;
	}

	if (access(fw_path, F_OK) == -1)
	{
		printf("File %s does not exists\n", fw_path);
		return -ENOENT;
	}

	retcode = lpc_fwup_transfer(twi, bank_id, fw_path);
	return retcode;
}

int main(int argc, char **argv)
{
	int retcode;

	twi_initialise(&twi);
	retcode = twi.init();

	if (retcode < 0) {
		fprintf(stderr, "ERROR: Unable to establish I2C/TWI connection to microcontroller\n");
		return -ENOTCONN;
	}

	if (argc == 3 && !strcmp(argv[1],"--set-mac")){
#ifdef ENABLE_MAC_CHANGE
		uint32_t len = strlen(argv[2]);
		if (len != 12){
			printf("Expecting length 12 for mac addresss, got %d\n", len);
			goto EXIT;
		}
		uint32_t macint[6];
		sscanf(argv[2], "%02X%02X%02X%02X%02X%02X", &macint[0], 
													&macint[1],
													&macint[2],
													&macint[3],
													&macint[4],
													&macint[5]);
		lpc_mac_t lpc_mac;
		for (int i=0; i<6; i++){
			lpc_mac.mac[i] = (uint8_t)macint[i];
		}

		lpc_set_mac(&twi, &lpc_mac);
#else
		printf("ERROR: MAC setting has not been enabled in this utility build\n");
		retcode = -ENOTSUP;
#endif
	} else if (argc == 2 && !strcmp(argv[1], "--get-info")){

		lpc_get_board_info(&twi, 0);

	} else if (argc >= 2 && !strcmp(argv[1], "--get-version")) {
		lpc_get_board_info(&twi, 1);
	} else if (argc == 2 && !strcmp(argv[1], "--get-state")){

		lpc_get_system_state(&twi);

	} else if (argc == 3 && !strcmp(argv[1], "--set-reset-holdtime")){

		int32_t holdtime_arg;
		sscanf(argv[2], "%d", &holdtime_arg);
		if (holdtime_arg < 3 || holdtime_arg > 10){
			printf("Invalid reset holdtime, valid range is {3:10}\n");
			goto EXIT;
		}

		lpc_reset_holdtime_t reset_holdtime;
		reset_holdtime.time = holdtime_arg;
		lpc_set_reset_holdtime(&twi, &reset_holdtime);

	} else if (argc == 3 && !strcmp(argv[1], "--set-reset-enabled")){
		int32_t enable_arg;
		sscanf(argv[2], "%d", &enable_arg);
		if (enable_arg != 0 && enable_arg != 1){
			printf("Invalid reset enabled, valid range is {0:1}\n");
			goto EXIT;
		}
		lpc_reset_enable_t reset_enable;
		reset_enable.enable = enable_arg;
		lpc_set_reset_enabled(&twi, &reset_enable);

	 } else if (argc == 2 && !strcmp(argv[1], "--image-info")) {

		lpc_fwup_get_info(&twi, NULL);

	 } else if (argc == 4 && !strcmp(argv[1], "--fwup-run")){

		retcode = process_fwup_run(&twi, argv);

	} else if (argc == 2 && !strcmp(argv[1], "--fwup-boot")) {

		lpc_fwup_boot(&twi);

	} else if (argc == 2 && !strcmp(argv[1], "--fwup-next-slot")) {
		retcode = ten64_mcu_fwup_get_next_slot(&twi);
	} else if (argc == 2 && !strcmp(argv[1], "--get-bootcount")) {
		retcode = lpc_get_next_bootpart(&twi,0);
	} else if (argc == 2 && !strcmp(argv[1], "--get-next-bootpart")) {
		retcode = lpc_get_next_bootpart(&twi,1);
	} else if (argc >= 3 && !strcmp(argv[1], "--set-next-bootpart")) {
		char next_boot_part = argv[2][0];
		uint8_t ttl = 1;
		if (argc == 4) {
			ttl = atoi(argv[3]);
		}
		printf("Setting next bootpart: %c TTL: %d\n", next_boot_part, ttl);
		lpc_set_next_bootpart(&twi, next_boot_part, ttl);
	} else {
		usage(argv[0]);
	}

EXIT:
	twi.close();
	return retcode;
}
