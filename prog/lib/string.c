
void* memcpy(void * dst,const void * src, int size){
    __asm__(
        "cld\n\t"
	    "rep\n\t"
	    "movsb"
	    :
        :"c" (size),"S" (src),"D" (dst)
	);
    return dst;
}

void* memset(void * dst, char ch, int size){
    __asm__(
        "cld\n\t"
	    "rep\n\t"
	    "stosb"
	    :
        :"a" (ch),"D" (dst),"c" (size)
	);
    return dst;
}

char* strcpy(char * dst,const char * src){
    __asm__(
        "cld\n"
        "1:\tlodsb\n\t"
        "stosb\n\t"
        "testb %%al,%%al\n\t"
        "jne 1b"
        ::"S" (src),"D" (dst));
    return dst;
}

int strlen(const char* s){
    register int __res ;
    __asm__(
        "cld\n\t"
        "repne\n\t"
        "scasb\n\t"
        "notl %0\n\t"
        "decl %0"
        :"=c" (__res):"D" (s),"a" (0),"0" (0xffffffff));
    return __res;
}

int strcmp(const char * cs,const char * ct){
    register int __res ;
    __asm__(
        "cld\n"
        "1:\tlodsb\n\t"
        "scasb\n\t"
        "jne 2f\n\t"
        "testb %%al,%%al\n\t"
        "jne 1b\n\t"
        "xorl %%eax,%%eax\n\t"
        "jmp 3f\n"
        "2:\tmovl $1,%%eax\n\t"
        "jl 3f\n\t"
        "negl %%eax\n"
        "3:"
        :"=a" (__res):"D" (cs),"S" (ct));
    return __res;
}