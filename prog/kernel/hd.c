#include "hd.h"
#include "protect.h"
#include "process.h"
#include "print.h"
#include "type.h"
#include "string.h"

uint8_t hd_status;

void hd_handler();

void init_hd(){
    printString("Hard Disk number  ", -1);
    printInt8(*(int*)(0x475));
    printString("\n", -1);
    // set hard disk interrupt handler
    set_irq_handler(AT_WINI_IRQ, hd_handler);
}

void hd_handler(){
    printString("HD_HANDLER!\n", -1);
    hd_status = in_byte(REG_STATUS);
    sys_ipc_int_send(PID_HD);
    printString("HD_HANDLER DONE!\n", -1);
}

uint8_t device_reg_generate(uint32_t lba_mode, uint32_t drive, uint32_t lba_low_addr_4){
    return 0xA0 | ((lba_mode & 1) << 6) | ((drive & 1) << 4) | (lba_low_addr_4 & 0xF);
}

void hd_cmd_out(HD_CMD* cmd){
	while(in_byte(REG_STATUS) & STATUS_BSY){
        //todo: yield
    }

	out_byte(REG_DEV_CTRL, 0);

	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR,  cmd->count);
	out_byte(REG_LBA_LOW,  cmd->lba_low);
	out_byte(REG_LBA_MID,  cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE,   cmd->device);

	out_byte(REG_CMD,     cmd->command);
}

void hd_identify(int drive){
    HD_CMD cmd;
    cmd.device = device_reg_generate(0, drive, 0);
    printInt8(cmd.device);
    printString("\n", -1);
    cmd.command = ATA_IDENTIFY;

    printString("send...\n", -1);
    hd_cmd_out(&cmd);
    sys_ipc_recv(PID_INT, 0);
    printString("get!\n", -1);

    uint8_t hdbuf[SECTOR_SIZE * 2];
    port_read_16(REG_DATA, hdbuf, 512 * 2);
    printString("HD size: 0x", -1);
    uint32_t sectors = ((int)((uint16_t*)hdbuf)[61] << 16) + ((uint16_t*)hdbuf)[60];
    printInt32(sectors * 512 / 1000000);
    printString("\n", -1);
    printString((char*)((uint16_t*)hdbuf + 27), 40);
    printString("\n", -1);
}


void hd_read_write(uint32_t drive, uint32_t sector, uint32_t* buf, uint32_t len, uint32_t is_read){
    //set command registers
    HD_CMD cmd;
    cmd.features = 0;
    cmd.count = len / SECTOR_SIZE + 1;
    cmd.lba_low = sector & 0xFF;
    cmd.lba_mid = (sector >> 8) & 0xFF;
    cmd.lba_high = (sector >> 16) & 0xFF;
    cmd.device = device_reg_generate(1, drive, (sector >> 24) & 0xF);
    cmd.command = is_read ? ATA_READ : ATA_WRITE;
    hd_cmd_out(&cmd);
    

    //copy data
    uint8_t hdbuf[SECTOR_SIZE * 2];
    uint32_t copy_offset = 0;
    int remain = len;
    do{
        uint32_t copy_len = remain < SECTOR_SIZE ? remain : SECTOR_SIZE;
        if(is_read){
            printString("read...wait data...\n", -1);
            sys_ipc_recv(PID_INT, 0);
            printString("read...data ok!\n", -1);
            port_read_16(REG_DATA, hdbuf, SECTOR_SIZE);
            memcpy((uint8_t*)buf + copy_offset, hdbuf, copy_len);
        }
        else{
            memcpy(hdbuf, (uint8_t*)buf + copy_offset, copy_len);
            printString("before write...\n", -1);
            port_write_16(REG_DATA, hdbuf, SECTOR_SIZE);
            sys_ipc_recv(PID_INT, 0);
            printString("write finish...\n", -1);
        }
        copy_offset += SECTOR_SIZE;
        remain -= SECTOR_SIZE;
        printString("remain: ",-1);printInt32(remain);printString("\n",-1);
    }while(remain > 0);
    printString("read / write finish! \n", -1);
}

void hd_main(){
    Message msg;
    while(1){
        sys_ipc_recv(PID_ANY, &msg);
        printString("msg from user!\npid", -1);
        printInt32(msg.src_pid);
        printString("\n", -1);
        // hd_identify(0);

        //test code for hard disk read / write ----------------------todo----------------
        uint32_t temp = 0xBBEECCAA;
        hd_read_write(0, 0, &temp, 4, 0);
        printString("\nhd test: ", -1);
        printInt32(temp);printString("\n", -1);
    }
}