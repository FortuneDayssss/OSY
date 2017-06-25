#include "protect.h"
#include "print.h"

int main(int argc, char** argv){
    
    printString("main\n", -1);

    while(1){}
}

void test(){
    printString("test\n", -1);
}