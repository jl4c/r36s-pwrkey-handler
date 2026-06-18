#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <linux/input.h>

#define DEVICE "/dev/input/event0"

static void flush_until_quiet(int fd)
{
    struct input_event ev;
    fd_set fds;
    struct timeval tv;

    fcntl(fd, F_SETFL, O_NONBLOCK);
    for (;;) {
        while (read(fd, &ev, sizeof(ev)) > 0);
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        fcntl(fd, F_SETFL, 0);
        int r = select(fd + 1, &fds, NULL, NULL, &tv);
        fcntl(fd, F_SETFL, O_NONBLOCK);
        if (r <= 0) break;
    }
    fcntl(fd, F_SETFL, 0);
}

int main(void)
{
    int input_fd;
    struct input_event ev;

    input_fd = open(DEVICE, O_RDONLY);
    if (input_fd < 0)
        return 1;

    while (read(input_fd, &ev, sizeof(ev)) == sizeof(ev)) {
        if (ev.type != EV_KEY || ev.code != KEY_POWER || ev.value != 1)
            continue;

        /* esperar release antes de suspender */
        while (read(input_fd, &ev, sizeof(ev)) == sizeof(ev))
            if (ev.type == EV_KEY && ev.code == KEY_POWER && ev.value == 0)
                break;

        system("loginctl suspend");
        flush_until_quiet(input_fd);
    }

    close(input_fd);
    return 0;
}
