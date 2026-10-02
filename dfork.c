#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(void) {
    printf("parent:     pid=%d ppid=%d sid=%d pgid=%d\n",
           getpid(), getppid(), getsid(0), getpgid(0));
    fflush(stdout);
    pid_t c = fork();
    if (c < 0) { perror("fork"); return 1; }
    if (c == 0) {
        printf("child:      pid=%d ppid=%d sid=%d\n", getpid(), getppid(), getsid(0));
        fflush(stdout);
        if (setsid() < 0) { perror("setsid"); _exit(1); }
        printf("child:      after setsid sid=%d (== pid: session leader, no controlling tty)\n",
               getsid(0));
        fflush(stdout);
        pid_t g = fork();
        if (g < 0) { perror("fork2"); _exit(1); }
        if (g == 0) {
            sleep(2);
            printf("grandchild: pid=%d ppid=%d (reparented) sid=%d pgid=%d\n",
                   getpid(), getppid(), getsid(0), getpgid(0));
            fflush(stdout);
            _exit(0);
        }
        printf("child:      exiting immediately so the grandchild is orphaned\n");
        fflush(stdout);
        _exit(0);
    }
    int st; waitpid(c, &st, 0);
    printf("parent:     child reaped (rc ok), waiting for grandchild report...\n");
    fflush(stdout);
    sleep(3);
    return 0;
}
