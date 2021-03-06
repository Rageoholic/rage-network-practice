#define _POSIX_C_SOURCE 2000000L

#include "rutils/debug.h"
#include "rutils/network.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

local void SigchildHandler(int s)
{
    ignore s;
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }

    errno = saved_errno;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s hostname\n", argv[0]);
        return 1;
    }

    TCPSocket sock = CreateTCPServerSocket(argv[1], 10);
    if (!IsValidSocket(sock))
    {
        PrintError("server: CreateTCPServerSocket");
        return 2;
    }
    struct sigaction sa = {.sa_handler = SigchildHandler,
	                   .sa_flags = SA_RESTART};

    /* We need to set up a signal handler for SigChild in order to clean
     up our child processes TODO: We probably need a better interface
     for this? This seems shitty. Also implementing it on windows
     couldn't hurt*/
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        PrintError("SetSignalHandler");
        return 1;
    }

    puts("server listening for connections");

    while (true)
    {
        SockAddr theirAddr = {};

        TCPSocket conn = AcceptConnection(sock, &theirAddr);

        assert(theirAddr._s != NULL);
        if (!IsValidSocket(conn))
        {
            PrintError("AcceptConnection");
            continue;
        }

        char s[MAX_ADDR_STR_LEN];

        printf("server: connected to %s\n", SockAddrToStr(&theirAddr, s));

        DestroySockAddr(theirAddr);
        int returnPid = fork();

        if (returnPid == -1)
        {
            ERROR("fork failed");
        }
        else if (!returnPid)
        {
            DestroySocket(sock); /* Forked process doesn't care about new
                            connections */
            if (TCPSendData(conn, "Hello, world!", 13) == -1)
            {
                PrintError("send");
            }
	    printf("Data sent\n");
            DestroySocket(conn);
            exit(0);
        }
        DestroySocket(conn); /* Parent doesn't give two shits about the
                            external connection */
    }
    return 0;
}
