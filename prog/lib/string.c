
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