
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

void* memset(void * dst,char ch,int size){
    __asm__(
        "cld\n\t"
	    "rep\n\t"
	    "stosb"
	    :
        :"a" (ch),"D" (dst),"c" (size)
	);
    return dst;
}