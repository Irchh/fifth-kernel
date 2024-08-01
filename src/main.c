extern long long add(long long a, long long b);

unsigned char * uart = (unsigned char *)0x10000000; 
void putchar(char c) {
	*((unsigned char *)0x10000000) = c;
	return;
}
 
void print(const char * str) {
	while(*str != '\0') {
		putchar(*str);
		str++;
	}
	return;
}

int kernel_main() {
    long long i = add(4, 5);
    char n[] = {'0' + i, '\0'};
    print(n);
    print("Hello, rv64!\n");
    while(1);
}
