#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE_PATH "/dev/rootv_device"

int main()
{
    int fd;
    char state;

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        printf("cannot open device file\n");
        return -1;
    }

    printf("Press - '1': ON LED | '0': OFF LED\n");
    printf("Press 'q' to quit\n");
    while (1) {
        state = getchar();
        if (state == '1') {
            write(fd, "1", 1);
        } else if (state == '0') {
            write(fd, "0", 1);
        } else if (state == 'q') {
            break;
        }
    }

    return 0;
}