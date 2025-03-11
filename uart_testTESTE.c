#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define UART_PATH "/dev/ttyS0"

int main() {
    int uart0_filestream = open(UART_PATH, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (uart0_filestream == -1) {
        printf("Erro ao abrir o UART!\n");
        return -1;
    }

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    // Enviar valores A e B
    unsigned char A = 5, B = 10;
    write(uart0_filestream, &A, 1);
    write(uart0_filestream, &B, 1);
    printf("Enviado: A = %d, B = %d\n", A, B);

    // Receber o resultado C da FPGA
    unsigned char C;
    read(uart0_filestream, &C, 1);
    printf("Recebido: C = %d\n", C);

    close(uart0_filestream);
    return 0;
}