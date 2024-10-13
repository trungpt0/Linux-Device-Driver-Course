#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define READ_IOCTL _IOR('a', 'a', int[10])
#define WRITE_IOCTL _IOW('a', 'b', int[10])
#define RW_IOCTL _IOWR('a', 'c', int)

int main()
{
    int fd;
    fd = open("/dev/rootv_device", O_RDWR);
    if (fd < 0) {
        printf("cannot open device file\n");
        return 0;
    }

    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n = 2;

    printf("writing a number to multi with array to driver:\n");
    scanf("%d", &n);
    ioctl(fd, WRITE_IOCTL, arr);

    printf("Multiplying array by factor %d\n", n);
    ioctl(fd, RW_IOCTL, &n);

    printf("read array value from driver\n");
    ioctl(fd, READ_IOCTL, arr);
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }

    close(fd);
    return 0;
}