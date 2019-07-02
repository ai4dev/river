#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// Crashes
#include <sys/mman.h>
#include <signal.h>

// TODO: move this somewhere else
	char* myitoa(int value, char* result, int base) {
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, *ptr1 = result, tmp_char;
		int tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
		} while ( value );

		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while(ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr--= *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
	}
	

int r = 0;

void crash(int x) {
    // Make it more probable to crash (unsigned char <= 256)
    x = x / 3;

    // Segmentation fault
    if (x >= 0 && x <= 10) {
        int a[10];
        for (int i = 0; i < 1000; ++i)
            a[i] = i;
    }        

    // Bus error
    if (x >= 11 && x <= 20) {   
        FILE *f = tmpfile();
        int *m = mmap(0, 4, PROT_WRITE, MAP_PRIVATE, fileno(f), 0);
        *m = 0;
    }

    if (x >= 21 && x <= 30) {
        raise(SIGTERM);
    }

    if (x >= 31 && x <= 40) {
        raise(SIGABRT);
    }

    if (x >= 41 && x <= 50) {   
        raise(SIGFPE);
    }

    if (x >= 51 && x <= 60) {
        sleep(5);
    }
}

int is_uppercase(int c) {
    /*
    char tst[2];
    tst[0] = c;
    tst[1] = '\0';
    write(1, tst, 2);
    fflush(stdout);

    write(1, "\n", 2);
    fflush(stdout);
    */
    return c >= 'A' && c <= 'Z';
}

int is_lowercase(int c) {
    return c >= 'a' && c <= 'z';
}

int is_number(char c) {
    return c >= '0' && c <= '9';
}

int is_operator(char c) {
    return c == '=' || c == '<' || c == '>';
}

int is_voyel(char c) {
    if ((char) c == 'A' || (char) c == 'E' || (char) c == 'I' || (char) c == 'O' || (char) c == 'U' || 
        (char) c == 'a' || (char) c == 'e' || (char) c == 'i' || (char) c == 'o' || (char) c == 'u')
        return 1;
    return 0;
}

// First target
void test_simple(const unsigned char *buf) 
{
    int fst = (int)buf[0];
    int snd = (int)buf[1];
    int thd = (int)buf[2];
    int fth = (int)buf[3];
    int fifth = (int)buf[4];

    fst = 'A';
    snd = '=';
    thd = '1';
    fth = 175;

    if (snd == '=')
        puts("snd este =\n");
    if (is_operator(snd) == 1)
        puts("snd e operator\n");

    if (is_uppercase(fst) || is_lowercase(fst)) {
        if (is_operator(snd)) {
            if (is_number(thd)) 
            {
                crash(fth);
            }            
        }

    }
}


void test_simple_2(const unsigned char *buf) 
{
	printf("cip");
	char a = buf[0];
	char b = buf[1];
	char c = a + b;
	if (c == 10)
	{
		r = 1;
	}
	else
	{
		r = 2;
	}
}

