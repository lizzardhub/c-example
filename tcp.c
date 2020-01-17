#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum {
    PORT = 72510,
    CONNECTIONS = 10,
    BUFSIZE = 1024
};

void die(int condition, char *msg) {
    if (condition) {
        printf("ERROR: %s\n", msg);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    /*printf("%d\n", getpid());
    srand(time(NULL));
    while (1) {
        sleep(1);
        printf("Tossed coin and found %d...\n", rand() % 2);
    }*/
    int is_server = !strcmp(argv[1], "-s");

    int portal = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    die(portal < 0, "create socket");

    int opt = 1;
    die(setsockopt(portal, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt)) < 0, "setsockopt");

    // Server
    if (is_server) {
    printf("Acting as server\n");
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PORT); // HOST TO NETWORK (SHORT)

    die(bind(portal, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0,
        "bind");
    die(listen(portal, CONNECTIONS) < 0, "listen");

    struct sockaddr_in remote_addr;
    while (1) {
        printf("Receiving...\n");
        int addrsz = sizeof(remote_addr);
        memset(&remote_addr, 0, addrsz);
        int spawn_portal = accept(portal, (struct sockaddr *) &remote_addr,
            &addrsz);
        die(spawn_portal < 0, "accept new connection");

        char buf[BUFSIZE + 1] = {0};
        int len = read(spawn_portal, &buf, BUFSIZE);
        die(len < 0, "read from socket");
        printf("Buffer received !%s!\n", buf);
    }

    } else {
    printf("Acting as client\n");
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT); // HOST TO NETWORK (SHORT)
    die(inet_pton(AF_INET, "127.0.0.1", &remote_addr.sin_addr) != 1, "host ip");
    die(connect(portal, (struct sockaddr *) &remote_addr, sizeof(remote_addr)) < 0, "connect");

    char hello[] = "Hello my dear server!";
    die(send(portal, argv[2], strlen(argv[2]) + 1, 0) < 0, "send");
    }
    return 0;
}
