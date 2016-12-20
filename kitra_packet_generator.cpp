#define KITRA_PACKET_GENERATOR_C

#include "kitra_packet_generator.h"

//strtok that handles empty token
static char* my_strtok(char* str, char sep, char** savepoint, bool is_packet_str)
{
 char* ptr;
 char* target;
 if(str == 0 && *savepoint == 0)
   return 0;
 else if(str != 0)
  target = str;
 else
  target = *savepoint;
 ptr = strchr(target,sep);
 
 if(ptr == 0) //string ended
 {
   if(is_packet_str)
   {
     ptr = strchr(target,'*');
     if(ptr == 0)
       return 0;
     *ptr = 0;
   }
   *savepoint = 0;
   return target;
 }
 
 *ptr = 0;
 *savepoint = ptr + 1;
 
 return target;
}

/*
* @param: packet source received from serial, MUST NOT BE CONST
* @format: list of param type like scanf, but aways comma separated. ex: %d,%hhd,%s, MUST NOT BE CONST
* return true if all ok, false if error
* @note: packet, and format params will be invalid after use. make a copy if you need them
*/
bool kitra_generate(char* packet,char* format,uint32_t* filter_mask, ...)
{
  va_list arguments;
  int8_t param_index;
  char* token_format;
  char* format_savepoint;
  char* token_packet;
  char* packet_savepoint;
  
  param_index = 0;
  *filter_mask = 0;
  token_format = my_strtok(format, ',', &format_savepoint, false);
  token_packet = my_strtok(packet, ',', &packet_savepoint, true);
  
  va_start ( arguments, filter_mask );
  
  while(token_format != 0)
  {
    if(strlen(token_packet) <= 0 || token_packet == 0)
    {
      if(param_index <= 0)
        return false;
      else
        K_SET_BIT(*filter_mask,param_index - 1);
    }
    if(strcmp(token_format,"%d") == 0 || strcmp(token_format,"%x") == 0)
    {
      if(strlen(token_packet) <= 0 || token_packet == 0)
        (va_arg ( arguments, uint32_t* ) );
      else {
        char* endptr;
        long tmp =  strtol(token_packet,&endptr, token_format[1]=='x'?16:0); // 0 -> autofind base
        if(strlen(endptr) == 0 && (tmp != 0 || strcmp(token_packet,"0") == 0)) //strtol return 0 if error
          *(va_arg ( arguments, uint32_t* ) ) = tmp;
        else
          return false;
      }

    }
    else if(strcmp(token_format,"%hhd") == 0 || strcmp(token_format,"%hhx") == 0)
    {
      if(strlen(token_packet) <= 0 || token_packet == 0)
        (va_arg ( arguments, uint8_t* ) );
      else {
        char* endptr;
        long tmp =  strtol(token_packet,&endptr,token_format[3]=='x'?16:0);
        if(strlen(endptr) == 0 && (tmp != 0 || strcmp(token_packet,"0") == 0)) //strtol return 0 if error
          *(va_arg ( arguments, uint8_t* ) ) = tmp;
        else
          return false;
      }
    }
    else if(strcmp(token_format,"%hd") == 0 || strcmp(token_format,"%hx") == 0)
    {
      if(strlen(token_packet) <= 0 || token_packet == 0)
        (va_arg ( arguments, uint16_t* ) );
      else {
        char* endptr;
        long tmp =  strtol(token_packet,&endptr,token_format[2]=='x'?16:0);
        if(strlen(endptr) == 0 && (tmp != 0 || strcmp(token_packet,"0") == 0)) //strtol return 0 if error
          *(va_arg ( arguments, uint16_t* ) ) = tmp;
        else
          return false;
      }
    }
    else if(strcmp(token_format,"%f") == 0)
    {
      if(strlen(token_packet) <= 0 || token_packet == 0)
        (va_arg ( arguments, float* ) );
      else {
        char* endptr;
        float tmp =  strtof(token_packet,&endptr);
        if(strlen(endptr) == 0)
          *(va_arg ( arguments, float* ) ) = tmp;
        else
          return false;
      }
    }
    else if(strcmp(token_format,"%s") == 0 || strcmp(token_format,"%*s") == 0)
    {
      if(strlen(token_packet) <= 0 || token_packet == 0)
        *(va_arg ( arguments, char* ) );
      else if(token_format[1] != '*')
        strcpy((va_arg ( arguments, char* ) ),token_packet);
    }
    else if(strlen(token_format)>1 && token_format[0] == '%' && token_format[strlen(token_format) -1] == 's')
    {
      token_format[strlen(token_format) -1] = '\0';
      long tmp =  strtol(token_format+1,0,0);
      if(strlen(token_packet) < (unsigned int)tmp)
        strcpy((va_arg ( arguments, char* ) ),token_packet);
      else
        return false;
    }
    token_format = my_strtok(0, ',', &format_savepoint, false);
    if(token_format!=0 && strlen(token_format)>1 && token_format[0] == '%' && token_format[strlen(token_format) -1] == 'b')
    {
      token_format[strlen(token_format) -1] = '\0';
      long tmp =  strtol(token_format+1,0,0);
      //anche se copio checksum e '*' fa nulla, tanto so la vera dimensione e il resto lo ignoro
      memcpy((va_arg ( arguments, char* ) ),token_packet + strlen(token_packet) + 1,tmp);
      goto binary_out;
    }
    token_packet = my_strtok(0, ',', &packet_savepoint, true);
    param_index++;
  }
binary_out:
  va_end ( arguments );
  
  return true;
}

/*
* fill buffer string with paramter, used for sending packet out
* @param: format source, MUST NOT BE CONST
* @param: filter_mask bit set means that parameter must be ignored
*/
bool kitra_generate_packet(char* buffer, uint32_t buffer_size,char* format, uint32_t filter_mask, ...)
{
  va_list arguments;
  char* token_format;
  char* format_savepoint;
  uint8_t param_index = 0;
  token_format = my_strtok(format, ',', &format_savepoint, false);
  
  va_start ( arguments, filter_mask );
    
  while(token_format != 0)
  {
    if(!CHECK_BIT(filter_mask,param_index))
    {
      if(strcmp(token_format,"%u") == 0 || strcmp(token_format,"%d") == 0 || strcmp(token_format,"%X") == 0)
        sprintf(buffer + strlen(buffer),token_format,(va_arg ( arguments, int ) ));
      if(strcmp(token_format,"%hu") == 0 || strcmp(token_format,"%hd") == 0 || strcmp(token_format,"%hX") == 0)
        sprintf(buffer + strlen(buffer),token_format,(va_arg ( arguments, int ) ));
      if(strcmp(token_format,"%hhu") == 0 || strcmp(token_format,"%hhd") == 0 || strcmp(token_format,"%hhX") == 0)
        sprintf(buffer + strlen(buffer),token_format,(va_arg ( arguments, int ) ));
      if(strlen(token_format)>1 && token_format[0] == '%' && (token_format[strlen(token_format) -1] == 'f' || token_format[strlen(token_format) -1] == 'g'))
        sprintf(buffer + strlen(buffer),token_format,(va_arg ( arguments, double ) ));
    }
    else
    {
      if(strcmp(token_format,"%u") == 0 || strcmp(token_format,"%d") == 0 || strcmp(token_format,"%X") == 0)
        (va_arg ( arguments, int));
      if(strcmp(token_format,"%hu") == 0 || strcmp(token_format,"%hd") == 0 || strcmp(token_format,"%hX") == 0)
        (va_arg ( arguments, int));
      if(strcmp(token_format,"%hhu") == 0 || strcmp(token_format,"%hhd") == 0 || strcmp(token_format,"%hhX") == 0)
        (va_arg ( arguments, int));
      if(strlen(token_format)>1 && token_format[0] == '%' && (token_format[strlen(token_format) -1] == 'f' || token_format[strlen(token_format) -1] == 'g'))
        (va_arg ( arguments, double ) );
    }
    
    uint32_t index = strlen(buffer);
    buffer[index] = ',';
    buffer[index+1] = 0;
    
    token_format = my_strtok(0, ',', &format_savepoint, false);
    param_index++;
  }
  
  //remove comma
  buffer[strlen(buffer) -1] = 0;
  
  va_end ( arguments );
  
  return true;
}

#undef KITRA_PACKET_GENERATOR_C
