#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MEM 1024

uint8_t write_buffer[MEM];
uint8_t read_buffer[MEM];

int main()
{
    int fd;
    fd = open("/dev/rootv_device", O_RDWR);
    if (fd < 0) {
        printf("cannot open device file\n");
        return 0;
    }

    char c;
    while (1) {
        printf("Enter 1 to read.\n");
        printf("Enter 2 to write.\n");
        printf("Enter 3 to exit.\n");
        scanf(" %c", &c);

        switch (c) {
            case '1':
                read(fd, read_buffer, MEM);
                printf("Read: %s\n", read_buffer);
                break;
            case '2':
                scanf(" %[^\t\n]s", write_buffer);
                write(fd, write_buffer, strlen(write_buffer) + 1);
                printf("OK\n");
                break;
            case '3':
                close(fd);
                return 0;
            default:
                printf("Input invalid. Again\n");
                break;
        }
    }

    return 0;
}