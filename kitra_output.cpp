#define KITRA_OUTPUT_C

#include "kitra_output.h"

#define MAX_BUFFER_SIZE 1024

static char buffer_tx[MAX_BUFFER_SIZE];


/*
* @param k_obj
* @param optional_mask  bit as 1 means that must not be transmitted
*/
char* k_send_packet(void* k_obj, uint32_t optional_mask)
{
  char format[40];
  char cks[3];
  uint32_t len;
  uint32_t id = ((k_input_iap_whoiam*) k_obj)->id; //take just id
  
  memset(buffer_tx,0,sizeof(char)*MAX_BUFFER_SIZE);
  strcpy(buffer_tx,"$KITRA,");
  switch(id)
  {
  case K_INPUT_IAP_WHOIAM:
    strcpy(format,K_INPUT_IAP_WHOIAM_PARAMS);
    kitra_generate_packet(buffer_tx,MAX_BUFFER_SIZE,format,optional_mask,
             ((k_input_iap_whoiam*) k_obj)->id
    );
    break;
  case K_INPUT_IAP_START:
    strcpy(format,K_INPUT_IAP_START_PARAMS);
    kitra_generate_packet(buffer_tx,MAX_BUFFER_SIZE,format,optional_mask,
             ((k_input_iap_start*) k_obj)->id,
             ((k_input_iap_start*) k_obj)->size
    );
    break;
  case K_INPUT_IAP_ABORT:
    strcpy(format,K_INPUT_IAP_ABORT_PARAMS);
    kitra_generate_packet(buffer_tx,MAX_BUFFER_SIZE,format,optional_mask,
             ((k_input_iap_abort*) k_obj)->id
    );
    break;
  case K_INPUT_IAP_BOOT:
    strcpy(format,K_INPUT_IAP_BOOT_PARAMS);
    kitra_generate_packet(buffer_tx,MAX_BUFFER_SIZE,format,optional_mask,
             ((k_input_iap_boot*) k_obj)->id
    );
    break;
  case K_INPUT_GET_FIRMWARE:
    strcpy(format,K_INPUT_GET_FIRMWARE_PARAMS);
    kitra_generate_packet(buffer_tx,MAX_BUFFER_SIZE,format,optional_mask,
             ((k_input_get_firmware*) k_obj)->id
    );
    break;
  case K_INPUT_START_FW_UPGRADE:
    strcpy(format,K_INPUT_START_FW_UPGRADE_PARAMS);
    kitra_generate_packet(buffer_tx,MAX_BUFFER_SIZE,format,optional_mask,
             ((k_input_start_fw_upgrade*) k_obj)->id
    );
    break;
 
  default:
    return false;
  }

  len = strlen(buffer_tx);

  if(len < (MAX_BUFFER_SIZE - 6))
  {
    //add checksum and send
    k_get_checksum(buffer_tx + 1,strlen(buffer_tx),cks);
    buffer_tx[len] = '*';
    memcpy(buffer_tx + len + 1, cks, 2 );
    memcpy(buffer_tx + len + 3, "\r\n" , 2 );
    buffer_tx[len + 5] = 0;
    
  }

  return buffer_tx;
  
}

#undef KITRA_OUTPUT_C
