#include <stdio.h>
#include <unistd.h>
#include <errno.h>
int main(void) {
    if (setpgid(0, 0) < 0) { perror("setpgid"); return 1; }
    printf("now a process group leader: pid=%d pgid=%d\n", getpid(), getpgid(0));
    if (setsid() < 0) { printf("setsid failed as promised: errno=%d (EPERM)\n", errno); return 0; }
    printf("setsid unexpectedly succeeded\n"); return 2;
}
