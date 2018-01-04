#include "hd.h"
#include "protect.h"
#include "process.h"
#include "print.h"
#include "type.h"

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


}

void hd_main(){
    Message msg;
    while(1){
        sys_ipc_recv(PID_ANY, &msg);
        printString("msg from user!\npid", -1);
        printInt32(msg.src_pid);
        printString("\n", -1);
        hd_identify(0);
    }
}