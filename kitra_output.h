#ifndef KITRA_OUTPUT_H
#define KITRA_OUTPUT_H

#include "kitra_packet_generator.h"

#ifdef KITRA_OUTPUT_C
#define EXTERN                  /* nothing */
#define INITIALIZER(...)        = __VA_ARGS__
#else
#define EXTERN                  extern
#define INITIALIZER(...)        /* nothing */
#endif /* DEFINE_VARIABLES */

typedef enum{
  K_INPUT_IAP_WHOIAM = 514,
  K_INPUT_IAP_START = 515,
  K_INPUT_IAP_ABORT = 516,
  K_INPUT_IAP_NEW_PACKET = 517,
  K_INPUT_IAP_BOOT = 518,
  K_INPUT_GET_FIRMWARE = 512,
  K_INPUT_START_FW_UPGRADE = 513
}einput_cmd;

typedef struct{
  uint32_t id;
}k_input_iap_whoiam;
#define K_INPUT_IAP_WHOIAM_N_PARAMS 1
EXTERN const char* K_INPUT_IAP_WHOIAM_PARAMS INITIALIZER("%d");

typedef struct{
  uint32_t id;
  uint32_t size; //in byte
}k_input_iap_start;
#define K_INPUT_IAP_START_N_PARAMS 2
EXTERN const char* K_INPUT_IAP_START_PARAMS INITIALIZER("%d,%d");

typedef struct{
  uint32_t id;
}k_input_iap_abort;
#define K_INPUT_IAP_ABORT_N_PARAMS 1
EXTERN const char* K_INPUT_IAP_ABORT_PARAMS INITIALIZER("%d");

#define K_INPUT_IAP_NEW_PACKET_MAX_SIZE 1024
typedef struct{
  uint32_t id;
  uint16_t size;
  char buffer[K_INPUT_IAP_NEW_PACKET_MAX_SIZE];
}k_input_iap_new_packet;
#define K_INPUT_IAP_NEW_PACKET_N_PARAMS 3
EXTERN const char* K_INPUT_IAP_NEW_PACKET_PARAMS INITIALIZER("%d,%hd,%1024b");

typedef struct{
  uint32_t id;
}k_input_iap_boot;
#define K_INPUT_IAP_BOOT_N_PARAMS 1
EXTERN const char* K_INPUT_IAP_BOOT_PARAMS INITIALIZER("%d");

typedef struct{
  uint32_t id;
}k_input_start_fw_update;
#define K_INPUT_START_FW_UPDATE_N_PARAMS 1
EXTERN const char* K_INPUT_START_FW_UPDATE_PARAMS INITIALIZER("%d");

typedef struct{
  uint32_t id;
}k_input_get_firmware;
#define K_INPUT_GET_FIRMWARE_N_PARAMS 1
EXTERN const char* K_INPUT_GET_FIRMWARE_PARAMS INITIALIZER("%d");

typedef struct{
  uint32_t id;
}k_input_start_fw_upgrade;
#define K_INPUT_START_FW_UPGRADE_N_PARAMS 1
EXTERN const char* K_INPUT_START_FW_UPGRADE_PARAMS INITIALIZER("%d");


char* k_send_packet(void* k_obj, uint32_t optional_mask);

#endif
