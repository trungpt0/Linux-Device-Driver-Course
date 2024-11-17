#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/rootv_device"
#define SIG_NUM 44
#define REG_CURRENT_TASK _IOW('a', 'a', int32_t *)

static void signal_handler(int sig)
{
    printf("received signal: %d\n", sig);
}

int main()
{
    int fd;
    int task_pid = getpid();
    struct sigaction sa;

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NUM, &sa, NULL);

    fd = open("/dev/rootv_device", O_RDWR);
    if (fd < 0) {
        printf("cannot open device file\n");
        return 1;
    }

    if (ioctl(fd, REG_CURRENT_TASK, &task_pid)) {
        printf("ioctf failed\n");
        close(fd);
        exit(1);
    }

    while (1) {
        pause();
    }

    close(fd);
    return 0;
}