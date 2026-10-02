# forkfork-demo

The traditional Unix daemon ritual: fork, setsid, fork again. The demo runs
the full chain and prints every pid, ppid, sid, and pgid so you can watch the
state machine move.

```
$ gcc -O2 -Wall -Wextra -Werror dfork.c -o dfork
$ ./dfork
parent:     pid=13480 ppid=13468 sid=0 pgid=13468
child:      pid=13481 ppid=13480 sid=0
child:      after setsid sid=13481 (== pid: session leader, no controlling tty)
child:      exiting immediately so the grandchild is orphaned
parent:     child reaped (rc ok), waiting for grandchild report...
grandchild: pid=13482 ppid=1 (reparented) sid=13481 pgid=13481
```

The first fork is load-bearing: setsid refuses a process group leader, so the
child must exist before it can leave the old session. After setsid the child's
session id equals its own pid, which is what session leadership looks like.

The second fork is the point of the ritual. A session leader with no
controlling terminal can still acquire one later by opening a terminal device.
The middle child was the session leader, so it exits at once; the grandchild
is not a session leader and can never become one. It is reparented to pid 1,
and the parent reaps the middle child, so no zombie remains anywhere.

The reason for the second fork is the one office the grandchild can never
hold. By giving up eligibility for session leadership, the daemon makes
acquiring a controlling terminal structurally impossible: the second fork buys
ineligibility, and that is the freedom the daemon was after.

Note: stdout is flushed before every `_exit`, because `_exit` performs none
of the standard cleanup. A grandchild that prints and calls `_exit` without
flushing loses its last words when stdout is block-buffered (a pipe).

Modern footnote: under a supervisor like systemd the whole ritual is mostly
obsolete; the supervisor does the detaching. The mechanism remains the
clearest demonstration of what sessions and controlling terminals are.
