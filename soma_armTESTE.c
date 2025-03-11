#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>

#define FPGA_BASE_ADDR 0xC0000000
#define REG_OFFSET 0x00

int main() {
    int mem_fd;
    void *fpga_map;
    volatile uint32_t *fpga_reg;

    
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        printf("Erro ao abrir /dev/mem\n");
        return -1;
    }

    
    fpga_map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, FPGA_BASE_ADDR);
    if (fpga_map == MAP_FAILED) {
        printf("Erro no mmap\n");
        return -1;
    }

    fpga_reg = (uint32_t *)(fpga_map + REG_OFFSET);

    uint16_t A = 10;
    uint16_t B = 20;
    uint32_t dados = (A << 16) | B;

    printf("Enviando A = %d, B = %d para a FPGA...\n", A, B);
    *fpga_reg = dados;

    usleep(1000);

    uint32_t C = *fpga_reg;
    printf("Resultado recebido da FPGA: C = %d\n", C);

    return 0;
}