#include "hd.h"
#include "protect.h"
#include "process.h"
#include "print.h"
#include "type.h"
#include "string.h"
#include "message.h"

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
    // printString("HD_HANDLER!\n", -1);
    hd_status = in_byte(REG_STATUS);
    sys_ipc_int_send(PID_HD);
    // printString("HD_HANDLER DONE!\n", -1);
}

uint8_t device_reg_generate(uint32_t lba_mode, uint32_t drive, uint32_t lba_low_addr_4){
    return 0xA0 | ((lba_mode & 1) << 6) | ((drive & 1) << 4) | (lba_low_addr_4 & 0xF);
}

void wait_for_hd_busy(){
    while((in_byte(REG_STATUS) & STATUS_BSY) != 0){
        //todo: yield
    }
}

void hd_cmd_out(HD_CMD* cmd){
	wait_for_hd_busy();

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
    wait_for_hd_busy();
    port_read_16(REG_DATA, hdbuf, 512 * 2);
    printString("HD size: 0x", -1);
    uint32_t sectors = ((int)((uint16_t*)hdbuf)[61] << 16) + ((uint16_t*)hdbuf)[60];
    printInt32(sectors * 512 / 1000000);
    printString("\n", -1);
    printString((char*)((uint16_t*)hdbuf + 27), 40);
    printString("\n", -1);
}

void hd_read(Message* msg){
    // get data from message
    MData_HD_Read* msg_data = &msg->mdata_hd_read;
    uint32_t sector = msg_data->sector;
    uint8_t* buf_ptr = (uint8_t*)msg_data->buf_addr;
    uint32_t remain_len = msg_data->len;
    uint32_t drive = 0;

    // printString("read sector no.", -1);printInt32(sector);printString("\n", -1);

    // set command registers
    HD_CMD cmd;
    cmd.features = 0;
    cmd.count = remain_len / SECTOR_SIZE + 1;
    cmd.lba_low = sector & 0xFF;
    cmd.lba_mid = (sector >> 8) & 0xFF;
    cmd.lba_high = (sector >> 16) & 0xFF;
    cmd.device = device_reg_generate(1, drive, (sector >> 24) & 0xF);
    cmd.command = ATA_READ;
    hd_cmd_out(&cmd);

    // copy data from sector to buffer
    uint8_t hdbuf[SECTOR_SIZE * 2];
    do{

        uint32_t copy_len = remain_len < SECTOR_SIZE ? remain_len : SECTOR_SIZE;
        sys_ipc_recv(PID_INT, 0);
    	wait_for_hd_busy();
        port_read_16(REG_DATA, hdbuf, SECTOR_SIZE);
        
        memcpy(buf_ptr, hdbuf, copy_len);

        buf_ptr += copy_len;
        remain_len -= copy_len;
        // printString("remain: ",-1);printInt32(remain_len);printString("\n",-1);
    }while(remain_len > 0);
    // printString("read finish! \n", -1);
    msg->mdata_response.status = RESPONSE_SUCCESS;
    sys_ipc_send(msg->src_pid, msg);
}

void hd_write(Message* msg){
    // get data from message
    MData_HD_Read* msg_data = &msg->mdata_hd_read;
    uint32_t sector = msg_data->sector;
    uint8_t* buf_ptr = (uint8_t*)msg_data->buf_addr;
    uint32_t remain_len = msg_data->len;
    uint32_t drive = 0;

    // set command registers
    HD_CMD cmd;
    cmd.features = 0;
    cmd.count = remain_len / SECTOR_SIZE + 1;
    cmd.lba_low = sector & 0xFF;
    cmd.lba_mid = (sector >> 8) & 0xFF;
    cmd.lba_high = (sector >> 16) & 0xFF;
    cmd.device = device_reg_generate(1, drive, (sector >> 24) & 0xF);
    cmd.command = ATA_WRITE;
    hd_cmd_out(&cmd);

    // copy data from sector to buffer
    uint8_t hdbuf[SECTOR_SIZE * 2];
    do{
        uint32_t copy_len = remain_len < SECTOR_SIZE ? remain_len : SECTOR_SIZE;
        memcpy(hdbuf, buf_ptr, copy_len);
        wait_for_hd_busy();
        port_write_16(REG_DATA, hdbuf, SECTOR_SIZE);
        sys_ipc_recv(PID_INT, 0);

        buf_ptr += copy_len;
        remain_len -= copy_len;
        // printString("remain: ",-1);printInt32(remain_len);printString("\n",-1);
    }while(remain_len > 0);
    // printString("write finish! \n", -1);
    msg->mdata_response.status = RESPONSE_SUCCESS;
    sys_ipc_send(msg->src_pid, msg);
}

void hd_dev_open(Message* msg){

}

void hd_main(){
    Message msg;
    while(1){
        // printString("hd process recving...\n", -1);
        sys_ipc_recv(PID_ANY, &msg);
        if(msg.src_pid == 1)
            continue;
        switch(msg.type){
            case MSG_HD_DEV_OPEN:
                hd_dev_open(&msg);
                break;
            case MSG_HD_READ:
                // printString("hd get read msg from ", -1);printInt32(msg.src_pid);printString("\n", -1);
                hd_read(&msg);
                break;
            case MSG_HD_WRITE:
                // printString("hd get write msg from ", -1);printInt32(msg.src_pid);printString("\n", -1);
                hd_write(&msg);
                break;
            default:
                break;
        }
    }
}