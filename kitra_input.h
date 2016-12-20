#ifndef KITRA_INPUT_
#define KITRA_INPUT_H

#include "kitra_packet_generator.h"

#ifdef KITRA_INPUT_C
#define EXTERN                  /* nothing */
#define INITIALIZER(...)        = __VA_ARGS__
#else
#define EXTERN                  extern
#define INITIALIZER(...)        /* nothing */
#endif /* DEFINE_VARIABLES */

#define CMD_HOLDER_SIZE 2048
typedef struct{
  char obj[CMD_HOLDER_SIZE];
  uint32_t size;
  uint32_t optional_mask;
}t_cmd_holder;

typedef enum{
  PARSE_OK,
  PARSE_BAD_CHECKSUM,
  PARSE_BAD_SIZE,
  PARSE_UNDEFINED_CMD,
  PARSE_ERROR
}eparse_result;


typedef enum{
  K_OUTPUT_IAP_WHOIAM = 615,
  K_OUTPUT_IAP_ACK = 616,
  K_OUTPUT_IAP_NACK = 617,
  K_OUTPUT_IAP_END = 618,
  K_OUTPUT_GET_FW_RESPONSE = 614
}eoutput_cmd;

typedef struct{
  uint32_t id;
}k_output_iap_whoiam;
#define K_OUTPUT_IAP_WHOIAM_N_PARAMS 1
EXTERN const char* K_OUTPUT_IAP_WHOIAM_PARAMS INITIALIZER("%*s,%d"); 

typedef struct{
  uint32_t id;
}k_output_iap_ack;
#define K_OUTPUT_IAP_ACK_N_PARAMS 1
EXTERN const char* K_OUTPUT_IAP_ACK_PARAMS INITIALIZER("%*s,%d");
  
typedef struct{
  uint32_t id;
}k_output_iap_nack;
#define K_OUTPUT_IAP_NACK_N_PARAMS 1
EXTERN const char* K_OUTPUT_IAP_NACK_PARAMS INITIALIZER("%*s,%d");

typedef struct{
  uint32_t id;
}k_output_iap_end;
#define K_OUTPUT_IAP_END_N_PARAMS 1
EXTERN const char* K_OUTPUT_IAP_END_PARAMS INITIALIZER("%*s,%d");

typedef struct{
  uint32_t id;
  uint32_t fw_version;
  uint32_t hw_version;
  uint32_t extra;
}k_output_get_fw_response;
#define K_OUTPUT_GET_FW_RESPONSE_N_PARAMS 4
EXTERN const char* K_OUTPUT_GET_FW_RESPONSE_PARAMS INITIALIZER("%*s,%d,%d,%d,%d");

void k_init_input();
void k_process_input(char byte);
eparse_result k_parse_packet(char* packet,void* k_obj, uint32_t* packet_size, uint32_t* optional_mask);
t_cmd_holder* k_get_last_cmd();
bool k_get_last_cmd_safe(void* buffer, uint8_t* n_optional);
#endif
