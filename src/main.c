#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include "api.h"
#include "twi.h"

twi_device_t twi;

static void usage(void){
#ifdef ENABLE_MAC_CHANGE
	printf("lpc804cli --set-mac <mac>                   Set the MAC address\n");
#endif
	printf("lpc804cli --get-info                        Get uC info\n");
	printf("lpc804cli --get-state                       Get system control state\n");
	printf("lpc804cli --set-reset-holdtime <{3:10}>     Set the hold time for the external reset button\n");
	printf("lpc804cli --set-reset-enabled <0|1>         Enable/disable the external button reset\n");
	printf("lpc804cli --image-info                      Get current image data stored in both flash slots\n");
	//printf("lpc --fwup-init <a|b>                     Firmware upgrade init bank a or bank b\n");
	//printf("lpc --fwup-state                          Firmware upgrade run state\n");
	//printf("lpc --fwup-transfer <new_fw_file>         Firmware upgrade transfer file. NOTE: place file in /tmp\n");
	printf("lpc804cli --fwup-run <a|b> <new_fw_file>    Firmware upgrade run\n");
	printf("lpc804cli --fwup-boot                       Firmware upgrade boot into new firmware. NOTE: will reset main CPU\n");
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

int main(int argc, char **argv)
{
	int retcode;

	//crc_make_table();
	twi_initialise(&twi);
	twi.init();

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

		lpc_get_board_info(&twi);

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

		lpc_fwup_run_state_t fwup_run_state;
		lpc_image_header_t image_header;
		char f_path[50];
		uint8_t bank_id;

		if (!strcmp(argv[2], "a"))
			bank_id = bankId1;
		else if (!strcmp(argv[2], "b"))
			bank_id = bankId2;
		else {
			printf("Expect arg3 to be bank a or b\n");
			goto EXIT;
		}

		sprintf(f_path, "/tmp/%s", argv[3]);
		if ( access( f_path, F_OK ) == -1 ){
			printf("File %s does not exists\n", f_path);
			goto EXIT;
		}


		lpc_fwup_transfer(&twi, bank_id, f_path);
	} else if (argc == 2 && !strcmp(argv[1], "--fwup-boot")) {

		lpc_fwup_boot(&twi);

	} else {
		usage();
	}

EXIT:
	twi.close();
	return retcode;
}
