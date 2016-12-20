#define KITRA_INPUT_C

#include "kitra_input.h"

#define MAX_BUFFER_SIZE 1024

static char tmp_cmd_holder[CMD_HOLDER_SIZE];
static uint32_t tmp_cmd_index = 0;
bool tmp_processing = false;
bool new_packet = false;
static t_cmd_holder cmd_holder;

//return true success, false failure
eparse_result k_parse_packet(char* packet,void* k_obj, uint32_t* packet_size, uint32_t* optional_mask)
{
  char cks[3];
  char format[80];
  uint32_t len = strlen(packet);
  uint32_t id;
  bool generate_ris = false;

  if(len >= MAX_BUFFER_SIZE || len < 13)
    return PARSE_BAD_SIZE;

  
  k_get_checksum(packet + 1, len - 6, cks);
  if(cks[0] != packet[len - 4] || cks[1] != packet[len - 3]) //bad checksum?
    return PARSE_BAD_CHECKSUM;

  sscanf(packet,"$KITRA,%d",&id);

  switch(id)
  {
  case K_OUTPUT_IAP_WHOIAM:

    strcpy(format,K_OUTPUT_IAP_WHOIAM_PARAMS);
    generate_ris = kitra_generate(packet,format,optional_mask,
           &((k_output_iap_whoiam*) k_obj)->id
    );
    *packet_size = sizeof(k_output_iap_whoiam);

    break;
  case K_OUTPUT_IAP_ACK:
    strcpy(format,K_OUTPUT_IAP_ACK_PARAMS);
    generate_ris = kitra_generate(packet,format,optional_mask,
           &((k_output_iap_ack*) k_obj)->id
    );
    *packet_size = sizeof(k_output_iap_ack);
    break;
  case K_OUTPUT_IAP_NACK:
    strcpy(format,K_OUTPUT_IAP_NACK_PARAMS);
    generate_ris = kitra_generate(packet,format,optional_mask,
           &((k_output_iap_nack*) k_obj)->id
    );
    *packet_size = sizeof(k_output_iap_nack);
    break;
  case K_OUTPUT_IAP_END:
    strcpy(format,K_OUTPUT_IAP_END_PARAMS);
    generate_ris = kitra_generate(packet,format,optional_mask,
           &((k_output_iap_end*) k_obj)->id
    );
    *packet_size = sizeof(k_output_iap_end);
    break;
  case K_OUTPUT_GET_FW_RESPONSE:
    strcpy(format,K_OUTPUT_GET_FW_RESPONSE_PARAMS);
    generate_ris = kitra_generate(packet,format,optional_mask,
           &((k_output_get_fw_response*) k_obj)->id,
           &((k_output_get_fw_response*) k_obj)->hw_version,
           &((k_output_get_fw_response*) k_obj)->fw_version,
           &((k_output_get_fw_response*) k_obj)->extra
    );
    *packet_size = sizeof(k_output_iap_end);
    break;
  default:
    return PARSE_UNDEFINED_CMD;
  }
  
  return (generate_ris == false?PARSE_ERROR:PARSE_OK);
}

void k_init_input()
{

}

//0 if none. buffer could be overwritten
t_cmd_holder* k_get_last_cmd()
{
  if(new_packet)
  {
    new_packet = false;
    return &cmd_holder;
  }
  return NULL;
}

bool k_get_last_cmd_safe(void* buffer, uint8_t* n_optional)
{
  if(new_packet)
  {
    new_packet = false;
    memcpy(buffer,cmd_holder.obj,cmd_holder.size);
    *n_optional = cmd_holder.optional_mask;
    return true;
  }
  return false;
}

//i should only receive one packet per time, because i must send an ack for every commmand
void k_process_input(char byte)
{

  if(tmp_processing==false && byte == '$') //begin
  {
    tmp_cmd_index = 0;
    tmp_processing = true;
  }

  tmp_cmd_holder[tmp_cmd_index] = byte;
  tmp_cmd_index++;

 if(byte == '\n' && tmp_processing == true) //end
  {
    tmp_cmd_holder[tmp_cmd_index] = 0;
    tmp_processing = false;
    memset(&cmd_holder,0,sizeof(t_cmd_holder));
    if(k_parse_packet(tmp_cmd_holder,cmd_holder.obj,&cmd_holder.size,&cmd_holder.optional_mask) == PARSE_OK)
    {
      new_packet = true;
    }
  }
 if(tmp_cmd_index >= CMD_HOLDER_SIZE)
  {
    tmp_cmd_index = 0;
    tmp_processing = false;
  }

      
}

#undef KITRA_INPUT_C
