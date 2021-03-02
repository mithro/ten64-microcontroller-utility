
#ifndef API_H
#define API_H

#include <inttypes.h>
#include "twi.h"

#define LPC_I2C_ADDRESS                         0x7E

#define LPC_FWUP_IMAGE_MAGIC                    0xBEAFCAFE
#define LPC_HEADER_PAD_SIZE                     42U
#define LPC_XFER_BLOCK_SIZE                     128U

#define LPC_API_MSG_HEADER_SIZE                 4U
#define LPC_API_DATA_MAX_SIZE                   128U
#define LPC_API_HEADER_PREAMB                   0xCABE


typedef enum {
    aeSuccess 	= 0U,
    aeInvalidCmd = 1U,
    aeInvalidLength,
    aeInvalidCrc,
    aeUnknown
} lpc_error_t;

typedef enum {
    bsNand = 0U,
    bsSdcard
} lpc_boot_src_t;

typedef enum {
    stateIdle = 0U,
    stateInit,
    stateXfer,
    stateCheck,
} lpc_fwup_state_t;

typedef enum {
    bankId1 = 1U,
    bankId2 = 2U
} lpc_fwup_bank_id_t;

typedef enum {
    fwupSuccess = 0U,
    fwupErrBankIdInvalid,
    fwupErrGetInfoInvalid,
    fwupErrEraseSector,
    fwupErrBlankCheckSector,
    fwupErrWrite,
    fwupErrCompare,
    fwupErrCheckNoMagic,
    fwupErrCheckCrcFailed,
    fwupErrUnknown
} lpc_fwup_error_t;

typedef enum {
    imgVerifySuccess = 0U,
    imgVerifyErrMagic,
    imgVerifyErrCrc,
    imgVerifyNoImage
} lpc_image_verify_error_t;

typedef enum {
    imgInBankA = 1U,
    imgInBankB = 2U
} lpc_image_location_t;

typedef enum {
    imgTypeStable = 1U,
    imgTypeCandidate,
    imgTypeUnstable,
    imgTypeOldStable,
    imgTypeForceOldStable,
    imgTypeClear
} lpc_image_type_t;

typedef enum {
    apiBdInfoSetMac 		= 0U,
    apiBdInfoGetInfo,
    apiSysCtlGetState,
    apiSysCtlSetResetHoldtime,
    apiSysCtlSetResetEnable,
    apiSysCtlSetNextBootsource,
    apiSysCtlSystemReset,
    apiFwupGetInfo,
    apiFwupState,
    apiFwupInit,
    apiFwupXfer,
    apiFwupCheck,
    apiFwupBoot,
    apiMax
} lpc_api_t;

typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} __attribute__((packed)) lpc_fw_version_t;

typedef struct {
    uint8_t atx_power_state;
    uint8_t current_boot_source;
    uint8_t next_boot_source;
    uint8_t switch_boot_source;
    uint8_t reset_enabled;
    uint8_t reset_holdtime;
    uint8_t switch_reset_ignore;
} __attribute__((packed)) lpc_system_state_t;

typedef struct {
    uint8_t mac[6];
} __attribute__((packed)) lpc_mac_t;

typedef struct {
    uint8_t mac[6];
    uint32_t cpuId[4];
    lpc_fw_version_t fw_version;
} __attribute__((packed)) lpc_bdinfo_t;

typedef struct {
    uint8_t time;
} __attribute__((packed)) lpc_reset_holdtime_t;

typedef struct {
    uint8_t enable;
} __attribute__((packed)) lpc_reset_enable_t;

typedef struct {
    uint8_t source;
} __attribute__((packed)) lpc_boot_source_t;

typedef struct {
    uint32_t magic;
    uint32_t crc;
    uint16_t img_length;
    uint8_t img_type;
    uint8_t ver_major;
    uint8_t ver_minor;
    uint8_t ver_patch;
    char git_short_sha[8];
    uint8_t pad[LPC_HEADER_PAD_SIZE];
} __attribute__((packed)) lpc_image_header_t;

typedef struct {
   lpc_image_header_t bankAImg;
   lpc_image_header_t bankBImg;
} __attribute__((packed)) lpc_image_info_t;

typedef struct {
    uint8_t state;
    uint8_t error;
    uint8_t blocksReceived;
    uint8_t blocksFlashed;
} __attribute__((packed)) lpc_fwup_run_state_t;

typedef struct {
    uint16_t preamb;
    uint8_t cmd;
    uint8_t len;
    uint8_t data[LPC_API_DATA_MAX_SIZE];
}  __attribute__((packed)) lpc_api_message_t;

uint8_t lpc_set_mac(const twi_device_t *twi, const lpc_mac_t * mac);
uint8_t lpc_get_board_info(const twi_device_t *twi);
uint8_t lpc_get_system_state(const twi_device_t *twi);
uint8_t lpc_set_reset_holdtime(const twi_device_t * twi, const lpc_reset_holdtime_t * reset_time);
uint8_t lpc_set_reset_enabled(const twi_device_t * twi, const lpc_reset_enable_t * reset_enabled);
//uint8_t api_set_next_boot_source(const twi_device_t *twi, const lpc_boot_source_t * boot_source);*/

uint8_t lpc_fwup_get_info(const twi_device_t *twi, lpc_image_info_t * img_info);
uint8_t lpc_fwup_init(const twi_device_t *twi, const lpc_fwup_bank_id_t bank_id, const uint8_t blocks_to_send);
uint8_t lpc_fwup_state(const twi_device_t *twi, lpc_fwup_run_state_t * run_state);
uint8_t lpc_fwup_transfer(const twi_device_t *twi, const uint8_t bank_id, const char *fw_file_path);
uint8_t lpc_fwup_check(const twi_device_t *twi, const lpc_image_header_t *img_header);
uint8_t lpc_fwup_boot(const twi_device_t *twi);

#endif /* API_H */
