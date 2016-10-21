#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <err.h>
#include <sysexits.h>

typedef int32_t (*fptr)(int32_t);

int main(int argc, char *argv[]) {
    int page = getpagesize();
    uint8_t *code = mmap(NULL, page, PROT_READ | PROT_WRITE,
            MAP_ANON | MAP_PRIVATE, -1, 0);
    if(code == MAP_FAILED) err(EX_OSERR, "mmap");

    if(argc < 2) return EX_USAGE;
    int32_t term = (int32_t)strtol(argv[1], NULL, 10);

    code[0] = 0x48; code[1] = 0xc7; code[2] = 0xc0;
    code[3] = (uint8_t)term;
    code[4] = (uint8_t)(term >> 8);
    code[5] = (uint8_t)(term >> 16);
    code[6] = (uint8_t)(term >> 24);
    code[7] = 0x48; code[8] = 0x01; code[9] = 0xf8;
    code[10] = 0xc3;

    int error = mprotect(code, page, PROT_READ | PROT_EXEC);
    if(error) err(EX_OSERR, "mprotect");
    fptr fn = (fptr)code;
    printf("%d %d %d\n", fn(1), fn(2), fn(3));

    return EX_OK;
}
