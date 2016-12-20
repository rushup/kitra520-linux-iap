#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include "serial.h"
#include "kitra_input.h"
#include "kitra_output.h"

using namespace std;

#define KITRA_LINUX_IAP_VER 1

serial k_serial;
char* buffer;
int buffer_index = 0;
int size;

void loop_boot();

int main (int argc, char **argv)
{
    ifstream in_file;
    if(argc < 2)
    {
        printf("Missing params\n");
        printf("Ex: /dev/tty0 fw.bin\n");
        return -1;
    }
    printf("Kitra firmwre upgrade\n");
    printf("Opening %s\n",argv[1]);
    if(k_serial.serialOpen(argv[1],115200) != -1)
    {
        printf("Serial found\n");


        if(argc == 3)
        {

            printf("Reading %s\n",argv[2]);
            in_file.open(argv[2],ios::binary);
            in_file.seekg(0,ios::end);
            size = in_file.tellg();
            in_file.seekg(0);

            if(size > 0)
            {
                buffer = new char[size];
                in_file.read(buffer,size);
                in_file.close();
                loop_boot(); 
            }
            else
                printf("File not found");
        }
        else
        {
            printf("No file specified \n");
            buffer = NULL;
            loop_boot();
        }

    }
    else
        printf("Serial error\n");
    /**/
    return 0;
}

void k_output_iap_whoiam_handler(t_cmd_holder* cmd)
{
    k_input_iap_start k_obj;
    k_obj.id = K_INPUT_IAP_START;

    k_obj.size = size;
    buffer_index = 0;

    k_serial.serialPuts(k_send_packet(&k_obj,0));

    printf("Device found\n");
    printf("Clearing flash\n");

}

void k_output_iap_ack_handler(t_cmd_holder* cmd, bool nack)
{
    char packet[2048];
    char chks[10];
    int psize = 0;
    static int last_size = 0;

    if(nack && buffer_index > 1024)
        buffer_index -= last_size;

    if((size - buffer_index) >= 1024)
    {
        psize = 1024;
    }
    else
        psize = size - buffer_index;


    if(psize > 0)
    {
        sprintf(packet,"$KITRA,%d,%d,",K_INPUT_IAP_NEW_PACKET,psize);
        int presize = strlen(packet);
        memcpy(packet + strlen(packet), buffer + buffer_index,psize);
        buffer_index += psize;
        last_size = psize;
        k_get_checksum(packet + 1,presize + psize - 1,chks);
        psize = presize + psize;
        packet[psize++] = '*';
        packet[psize++] = chks[0];
        packet[psize++] = chks[1];
        packet[psize++] = '\r';
        packet[psize++] = '\n';
        packet[psize] = 0;
        for(int i=0;i<psize;i++)
            k_serial.serialPutchar(packet[i]);
    }

    printf("\rProgress %d%%",((buffer_index * 100)/size));
    fflush( stdout );

}

void k_output_iap_nack_handler(t_cmd_holder* cmd)
{
    k_output_iap_ack_handler(cmd,1);
}

void k_output_iap_new_end_handler(t_cmd_holder* cmd)
{
    printf("\nUpdate done\n");
    printf("Reboot the system and kitra\n");
    exit(0);
}

void k_output_get_firmware_response_handler(t_cmd_holder* cmd)
{
    int c;
    k_output_get_fw_response* k_obj = (k_output_get_fw_response*)cmd->obj;
    printf("Kitra HW_%d FW_%d EXTRA_%d \n",k_obj->hw_version,k_obj->fw_version,k_obj->extra);
    printf("Do you want to switch to bootloader mode (WIll reboot system) ? (y/n): ");

    while(1)
    {
        c = getchar();
        if( c == 'y')
        {
            k_input_start_fw_upgrade k_obj_res;
            k_obj_res.id = K_INPUT_START_FW_UPGRADE;
            char* ptr = k_send_packet(&k_obj_res,0);
            k_serial.serialPrintf(ptr);
            printf("\n Rebooting system...");
            system("shutdown now");
            break;
        }
        else
            exit(0);
    }
}


void loop_boot()
{
    /* If the device answers to the GET FIRMWARE request it is in application mode
     * if it doesn't it is in bootloader mode */
    clock_t start, end;
    char* ptr;
    bool wait_version = false;

    k_input_get_firmware k_obj_get_fw;
    k_obj_get_fw.id = K_INPUT_GET_FIRMWARE;
    ptr = k_send_packet(&k_obj_get_fw,0);
    k_serial.serialPrintf(ptr);
    start = clock();

    while(1)
    {
        t_cmd_holder* cmd;
        cmd = k_get_last_cmd();
        if(cmd != 0)
        {
            uint32_t id = ((k_input_iap_whoiam*) cmd->obj)->id; //just get id

            switch(id)
            {
                case K_OUTPUT_IAP_WHOIAM:
                    k_output_iap_whoiam_handler(cmd);
                    break;
                case K_OUTPUT_IAP_ACK:
                    k_output_iap_ack_handler(cmd,0);
                    break;
                case K_OUTPUT_IAP_NACK:
                    k_output_iap_nack_handler(cmd);
                    break;
                case K_OUTPUT_IAP_END:
                    k_output_iap_new_end_handler(cmd);
                    break;
                case K_OUTPUT_GET_FW_RESPONSE:
                    wait_version = true;
                    printf("Got version\n");
                    k_output_get_firmware_response_handler(cmd);
                    break;
            }
        }

        end = clock();
        if(wait_version == false && ((end - start)/CLOCKS_PER_SEC) > 2)
        {
            wait_version = true;
            if(buffer == NULL)
            {
                printf("We are in boot mode, no file specified \n");
                exit(0);
            }

            k_input_iap_whoiam k_obj_whoiam;
            k_obj_whoiam.id = K_INPUT_IAP_WHOIAM;
            ptr = k_send_packet(&k_obj_whoiam,0);
            k_serial.serialPrintf(ptr);
        }


        int n = k_serial.serialDataAvail();
        for(int i=0; i<n;i++)
        {
            int ris = k_serial.serialGetchar();
            if(ris >= 0 && ris <= 127)
            {
                k_process_input((char) ris);
            }
        }
    }
}

