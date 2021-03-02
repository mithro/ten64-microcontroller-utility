#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "api.h"

uint8_t lpc_set_mac(const twi_device_t *twi, const lpc_mac_t * mac){
	lpc_api_message_t send;
	int rc;
	int send_len;

	send.preamb = LPC_API_HEADER_PREAMB;
	send.cmd = apiBdInfoSetMac;
	send.len = sizeof(lpc_mac_t);
	memcpy(send.data, (uint8_t *)mac, sizeof(lpc_mac_t));

	send_len = send.len + LPC_API_MSG_HEADER_SIZE;

	rc = twi->write((uint8_t *)&send, send_len);
	if (rc != send_len){
		printf("Set mac: write failed\n");
		return 1;
	}

	return 0;
}

uint8_t lpc_get_board_info(const twi_device_t *twi){
	lpc_api_message_t send;
	lpc_api_message_t recv;
	int rc;
	int recv_len;
	int send_len;

	send.preamb = LPC_API_HEADER_PREAMB;
	send.cmd = apiBdInfoGetInfo;
	send.len = 0;

	send_len = send.len + LPC_API_MSG_HEADER_SIZE;
	rc = twi->write((uint8_t *)&send, send_len);
	if (rc != send_len){
		printf("Get board info: write failed\n");
		return 1;
	}

	usleep(10000);

	recv_len = sizeof(lpc_bdinfo_t) + LPC_API_MSG_HEADER_SIZE;
	rc = twi->read((uint8_t *)&recv, recv_len);
	if (rc != recv_len){
		printf("Get board info: read failed\n");
		return 1;
	}

	if (recv.preamb != LPC_API_HEADER_PREAMB){
		printf("Get board info: Recv header has no preamb\n");
		return 1;
	}

	if (recv.cmd != apiBdInfoGetInfo){
		printf("Get board info: Expecting api cmd = %d, got %d\n", apiBdInfoGetInfo, recv.cmd);
		return 1;
	}

	if (recv.len != sizeof(lpc_bdinfo_t)){
		printf("Get board info: Expecting len = %lu, got %d\n", sizeof(lpc_bdinfo_t), recv.len);
		return 1;
	}

	lpc_bdinfo_t * info = (lpc_bdinfo_t *)recv.data;
	printf("LPC uC info: \n");
	printf("    MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", info->mac[0],info->mac[1],info->mac[2],info->mac[3],info->mac[4],info->mac[5]);
	printf("    UUID: %08X:%08X:%08X:%08X\n",info->cpuId[0],info->cpuId[1],info->cpuId[2],info->cpuId[3]);
	printf("    Firmware: v%d.%d.%d\n",info->fw_version.major, info->fw_version.minor, info->fw_version.patch);

	return 0;
}

uint8_t lpc_get_system_state(const twi_device_t *twi){

	lpc_api_message_t send;
	lpc_api_message_t recv;
	int rc;
	int recv_len;
	int send_len;

	send.preamb = LPC_API_HEADER_PREAMB;
	send.cmd = apiSysCtlGetState;
	send.len = 0;

	send_len = send.len + LPC_API_MSG_HEADER_SIZE;

	rc = twi->write((uint8_t *)&send, send_len);
	if (rc != send_len){
		printf("Get system state: write failed\n");
		return 1;
	}

	usleep(10000);

	recv_len = sizeof(lpc_system_state_t) + LPC_API_MSG_HEADER_SIZE;
	rc = twi->read((uint8_t *)&recv, recv_len);
	if (rc != recv_len){
		printf("Get system state: read failed\n");
		return 1;
	}

	if (recv.preamb != LPC_API_HEADER_PREAMB){
		printf("Get system state: Recv header has no preamb\n");
		return 1;
	}

	if (recv.cmd != apiSysCtlGetState){
		printf("Get system state: Expecting api cmd = %d, got %d\n", apiSysCtlGetState, recv.cmd);
		return 1;
	}

	if (recv.len != sizeof(lpc_system_state_t)){
		printf("Get system state: Expecting len = %lu, got %d\n", sizeof(lpc_system_state_t), recv.len);
		return 1;
	}

	lpc_system_state_t * state = (lpc_system_state_t *)recv.data;
	printf("LPC uC system control state: \n");
	if (state->atx_power_state){
		printf("    Atx powered: Yes\n");
	} else {
		printf("    Atx powered: No\n");
	}

	if (state->current_boot_source == bsNand){
		printf("    Current boot source: nand\n");
	} else if (state->current_boot_source == bsSdcard){
		printf("    Current boot source: sdcard\n");
	} else {
		printf("    Current boot source: unknown\n");
	}

	if (state->next_boot_source == bsNand){
		printf("    Next boot source: nand\n");
	} else if (state->next_boot_source == bsSdcard){
		printf("    Next boot source: sdcard\n");
	} else {
		printf("    Next boot source: unknown\n");
	}

	if (state->switch_boot_source == bsNand){
		printf("    Switch boot source: nand\n");
	} else if (state->switch_boot_source == bsSdcard){
		printf("    Switch boot source: sdcard\n");
	} else {
		printf("    Switch boot source: unknown\n");
	}

	if (state->reset_enabled){
		printf("    Reset enabled: yes\n");
	} else {
		printf("    Reset enabled: no\n");
	}

	printf("    Reset hold time: %d seconds\n", state->reset_holdtime);

	if (state->switch_reset_ignore){
		printf("    Switch set to ignore resets: yes\n");
	} else {
		printf("    Switch set to ignore resets: no\n");
	}

	return 0;
}

uint8_t lpc_set_reset_holdtime(const twi_device_t * twi, const lpc_reset_holdtime_t * reset_time){

	lpc_api_message_t send;
	int rc;
	int send_len;

	send.preamb = LPC_API_HEADER_PREAMB;
	send.cmd = apiSysCtlSetResetHoldtime;
	send.len = sizeof(lpc_reset_holdtime_t);
	memcpy(send.data, (uint8_t *)reset_time, sizeof(lpc_reset_holdtime_t));

	send_len = send.len + LPC_API_MSG_HEADER_SIZE;

	rc = twi->write((uint8_t *)&send, send_len);
	if (rc != send_len){
		printf("Set reset holdtime: write failed\n");
		return 1;
	}

	return 0;
}

uint8_t lpc_set_reset_enabled(const twi_device_t * twi, const lpc_reset_enable_t * reset_enabled){
	lpc_api_message_t send;
	int rc;
	int send_len;

	send.preamb = LPC_API_HEADER_PREAMB;
	send.cmd = apiSysCtlSetResetEnable;
	send.len = sizeof(lpc_reset_enable_t);
	memcpy(send.data, (uint8_t *)reset_enabled, sizeof(lpc_reset_enable_t));

	send_len = send.len + LPC_API_MSG_HEADER_SIZE;

	rc = twi->write((uint8_t *)&send, send_len);
	if (rc != send_len){
		printf("Set reset enable: write failed\n");
		return 1;
	}

	return 0;
}


/*uint8_t api_set_next_boot_source(const twi_device_t *twi, const lpc_boot_source_t * boot_source);*/

uint8_t lpc_fwup_get_info(const twi_device_t *twi, lpc_image_info_t * img_info) {
	uint8_t send = apiFwupGetInfo;
	uint8_t recv[1+sizeof(lpc_image_info_t)];
	twi->write(&send, 1);

	usleep(10000);

	twi->read(recv, 1+sizeof(lpc_image_info_t));

	if (recv[0] != apiFwupGetInfo){
		printf("Expecting api cmd = %d, got %d\n", apiFwupGetInfo, recv[0]);
		return 1;
	}

	lpc_image_info_t * info = (lpc_image_info_t *)&recv[1];

	/*printf("Raw: \n");
	for (int i=0; i<sizeof(lpc_image_info_t); i++){
		printf("[%02X]",recv[1+i]);
	}
	printf("\n");*/

	if (info->bankAImg.magic != LPC_FWUP_IMAGE_MAGIC){
		printf("LPC uC image bank A invalid\n");
	} else {
		printf("LPC uC image bank A info\n");
		if (info->bankAImg.img_type == imgTypeStable)
			printf("    Type: Stable\n");
		else if (info->bankAImg.img_type == imgTypeCandidate)
			printf("    Type: Candidate\n");
		else if (info->bankAImg.img_type == imgTypeUnstable)
			printf("    Type: Unstable\n");
		else if (info->bankAImg.img_type == imgTypeForceOldStable)
			printf("   Type: Old stable\n");
		else
			printf("   Type: Unknown\n");

		printf("    Size: %d B\n", info->bankAImg.img_length);
		printf("    Version: %d.%d.%d\n", info->bankAImg.ver_major, info->bankAImg.ver_minor, info->bankAImg.ver_patch);
		printf("    Git sha: %s\n", info->bankAImg.git_short_sha);
	}

	if (info->bankBImg.magic != LPC_FWUP_IMAGE_MAGIC){
		printf("LPC uC image bank B invalid\n");
	} else {
		printf("LPC uC image bank B info\n");
		if (info->bankBImg.img_type == imgTypeStable)
			printf("    Type: Stable\n");
		else if (info->bankBImg.img_type == imgTypeCandidate)
			printf("    Type: Candidate\n");
		else if (info->bankBImg.img_type == imgTypeUnstable)
			printf("    Type: Unstable\n");
		else if (info->bankBImg.img_type == imgTypeForceOldStable)
			printf("   Type: Old stable\n");
		else
			printf("   Type: Unknown\n");

		printf("    Size: %d B\n", info->bankBImg.img_length);
		printf("    Version: %d.%d.%d\n", info->bankBImg.ver_major, info->bankBImg.ver_minor, info->bankBImg.ver_patch);
		printf("    Git sha: %s\n", info->bankBImg.git_short_sha);
	}

	if (img_info != NULL){
		memcpy(img_info, info, sizeof(lpc_image_info_t));
	}

	return 0;
}

uint8_t lpc_fwup_init(const twi_device_t *twi, const lpc_fwup_bank_id_t bank_id, const uint8_t blocks_to_send){
	uint8_t send[3];
	send[0] = apiFwupInit;
	send[1] = bank_id;
	send[2] = blocks_to_send;
	if (3 != twi->write(send, 3))
		return 1;

	return 0;
}

uint8_t lpc_fwup_transfer(const twi_device_t *twi, const uint8_t bank_id, const char *fw_file_path/*, lpc_image_header_t * img_header*/){

	uint8_t rc;
	uint32_t blocks;
	uint8_t send[1+LPC_XFER_BLOCK_SIZE];
	FILE * fp;
	lpc_image_header_t img_header;
	lpc_fwup_run_state_t run_state;

	fp = fopen(fw_file_path, "r");
	if (fp == NULL){
		printf("File %s open failed\n", fw_file_path);
		rc = 1;
		goto OPEN_FAIL;
	}

	fseek(fp, 0L, SEEK_END);
	int32_t file_size = ftell(fp);

	if (file_size != 13312){
		printf("File size is should 13312 B, got %d B\n", file_size);
		rc = 1;
		goto EXIT;
	}

	// get the header info should be at 64B from the eof
	fseek(fp, 13248, SEEK_SET);
	size_t bytes = fread(&img_header, 1, 64U, fp);

	if (img_header.magic != LPC_FWUP_IMAGE_MAGIC) {
		printf("Image header magic invalid\n");
		rc = 2;
		goto EXIT;
	}

	// calculate blocks to send, don't send header which is last 64B in the file
	size_t rem = img_header.img_length % LPC_XFER_BLOCK_SIZE;
	if (rem)
		blocks = img_header.img_length/LPC_XFER_BLOCK_SIZE + 1;
	else
		blocks = img_header.img_length/LPC_XFER_BLOCK_SIZE;

	/* INIT */
	if (lpc_fwup_init(twi, bank_id, (uint8_t)blocks)){
		printf("Fwup init failed\n");
		goto EXIT;
	}

	sleep(1);

	/* STATE after init */
	if (lpc_fwup_state(twi, &run_state)){
		printf("Fwup get state after init failed\n");
		goto EXIT;
	}

	if (run_state.state != stateInit){
		printf("Fwup state %d, expecting init\n", run_state.state);
		goto EXIT;
	}

	if (run_state.error != fwupSuccess){
		printf("Fwup init state error = %d\n", run_state.error);
		goto EXIT;
	}

	sleep(1);

	send[0] = apiFwupXfer;
	printf("Start transfer\n");

	// goto start of file
	fseek(fp, 0L, SEEK_SET);

	for (int i=0; i<blocks; i++){
		memset(&send[1], 0xFF, LPC_XFER_BLOCK_SIZE);

		// if last block only read remainder bytes otherwise read block size
		size_t to_read = (i==(blocks-1))? rem : LPC_XFER_BLOCK_SIZE;
		size_t bytes = fread(&send[1], 1, to_read, fp);
		// send on the twi
		twi->write(send, 1 + LPC_XFER_BLOCK_SIZE);
		printf("Progress = %d/%d\n",i+1,blocks);
		usleep(100000);
	}
	printf("\nEnd transfer\n");

	sleep(1);

	/* STATE after transfer */
	if (lpc_fwup_state(twi, &run_state)){
		printf("Fwup get state after transfer failed\n");
		goto EXIT;
	}

	if (run_state.state != stateXfer){
		printf("Fwup state %d, expecting transfer\n", run_state.state);
		goto EXIT;
	}

	if (run_state.error != fwupSuccess){
		printf("Fwup transfer state error = %d\n", run_state.error);
		goto EXIT;
	}

	sleep(1);

	/* CHECK */
	if (lpc_fwup_check(twi, &img_header)){
		printf("Fwup check failed\n");
		goto EXIT;
	}

	sleep(1);

	/* STATE after check */
	if (lpc_fwup_state(twi, &run_state)){
		printf("Fwup get state after check failed\n");
		goto EXIT;
	}

	if (run_state.state != stateCheck){
		printf("Fwup state %d, expecting check\n", run_state.state);
		goto EXIT;
	}

	if (run_state.error != fwupSuccess){
		printf("Fwup transfer state error = %d\n", run_state.error);
		goto EXIT;
	}

	printf("Fwup transfer done\n");


EXIT:
	fclose(fp);
	return rc;

OPEN_FAIL:
	return rc;
}

uint8_t lpc_fwup_check(const twi_device_t *twi, const lpc_image_header_t *img_header){
	size_t length = 1+sizeof(lpc_image_header_t);
	uint8_t send[length];
	send[0] = apiFwupCheck;
	memcpy(&send[1], (uint8_t *)img_header, sizeof(lpc_image_header_t));
	if (length != twi->write(send, length))
		return 1;

	return 0;
}

uint8_t lpc_fwup_boot(const twi_device_t *twi){
	uint8_t send = apiFwupBoot;
	twi->write(&send, 1);
	return 0;
}

uint8_t lpc_fwup_state(const twi_device_t *twi, lpc_fwup_run_state_t *run_state) {
	uint8_t send = apiFwupState;
	uint8_t recv[1+sizeof(lpc_fwup_run_state_t)];
	if (1 != twi->write(&send, 1))
		return 1;

	usleep(10000);

	uint8_t exp_recv_len = 1+sizeof(lpc_fwup_run_state_t);
	printf("lpc_fwup_state: Expect %d bytes\n", exp_recv_len);
	if (exp_recv_len != twi->read(recv, exp_recv_len))
		return 1;

	if (recv[0] != apiFwupState){
		printf("Expecting api cmd = %d, got %d\n", apiFwupState, recv[0]);
		return 1;
	}

	memcpy((uint8_t *)run_state, &recv[1], sizeof(lpc_fwup_run_state_t));

	printf("Fwup run state\n");
	printf("    State: %d\n", run_state->state);
	printf("    Error: %d\n", run_state->error);
	printf("    Blocks transferred: %d\n", run_state->blocksReceived);
	printf("    Blocks flashed: %d\n", run_state->blocksFlashed);

	return 0;
}
