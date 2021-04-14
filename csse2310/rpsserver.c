#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#define INITIAL_BUFFER_SIZE 80

typedef struct Scores {
    char name[10][20];
    int wins[10];
    int loss[10];
    int tie[10];
    int counter;
} Scores;

typedef struct Message {
    char* msg;
} Message;

typedef struct Players {
    int plrid;
    int port;
    char* name;
    int wins;
    int losses;
    int ties;
    int pastResult;
    int matchid;
    FILE* read;
    FILE* write;
} Players;

typedef struct WaitingPlayers {
    Players players;
    int counter;
} WaitingPlayers;

typedef struct MessageInfo {
    FILE* read;
    FILE* write;
    Players player;
} MessageInfo;

typedef struct MatchResults {
    Players* players;
    int counter;
    int matchid;
    MessageInfo minfo;
    pthread_mutex_t lock;
    Scores scores;
    pthread_t* threadIds;
    int connectcount;
    int plrid;
    
} MatchResults;

typedef enum ClientMessage {
    MR,
    RESULT,
    ERROR
} ClientMessage;

typedef struct MatchOutcome {
    char* opponent;
    char* wlt;
} MatchOutcome;

//Reads a line from the given file stream
char* read_line(FILE* stream) {
    
    int bufferSize = INITIAL_BUFFER_SIZE;
    char* buffer = malloc(sizeof(char) * bufferSize);
    int numRead = 0;
    int next;

    while (1) {
        next = fgetc(stream);
        if (next == EOF && numRead == 0) {
            free(buffer);
            return NULL;
        }
        if (numRead == bufferSize - 1) {
            bufferSize *= 2;
            buffer = realloc(buffer, sizeof(char) * bufferSize);
        }
        if (next == '\n' || next == EOF) {
            buffer[numRead] = '\0';
            break;
        }
        buffer[numRead++] = next;
    }
    return buffer;
}

//Parses the message from the client - returns either MR or RESULT
ClientMessage parse_message(char* message, Players* player,
        MatchOutcome* outcome) {
    char* name;
    int port;
    char* request;
    const char delim[2] = ":";
    char* rest = message;
    fflush(stdout);
    request = strtok_r(rest, delim, &rest);
    if (!strcmp(request, "MR")) {
        name = strtok_r(rest, delim, &rest);
        port = atoi(strtok_r(rest, delim, &rest));
        player->name = name;
        player->port = port;
        return MR;
    }
    if (!strcmp(request, "RESULT")) { 
        outcome->opponent = strtok_r(rest, delim, &rest);
        outcome->wlt = strtok_r(rest, delim, &rest);
        return RESULT;
    }
    return ERROR;
    

}

//Sends the match request to the next two clients waiting
void send_mr(MatchResults* results, int id) {
    if (id % 2 == 0) {
        fprintf(results->players[id - 1].write, "MATCH:%d:%s:%d\n",
                results->matchid, 
                results->players[id - 2].name, results->players[id - 2].port); 
        fflush(results->players[id - 1].write);
        fprintf(results->players[id - 2].write, "MATCH:%d:%s:%d\n",
                results->matchid, 
                results->players[id - 1].name, results->players[id - 1].port);
        fflush(results->players[id - 2].write); 
        results->matchid++;
    }


}

//Checks the results of the match, updates scores accordingly
void check_results(MatchResults* results, int id, MatchOutcome outcome) {
    if (!strcmp(outcome.wlt, "ERROR")) {
        return;
    }
    if (!strcmp(outcome.wlt, results->players[id - 1].name)) {
        results->players[id - 1].pastResult = 1;
        results->players[id].pastResult = 0;

        if (id % 2 == 0) {
            if (results->players[id - 1].pastResult >
                    results->players[id - 2].pastResult) {
                results->players[id - 1].wins += 1;
                results->scores.wins[results->scores.counter - 1]++;
            }
        }
    } 
    if (!strcmp(outcome.wlt, results->players[id - 2].name)) {
        results->players[id - 2].pastResult = 1;
        results->players[id - 1].pastResult = 0;
        if (id % 2 == 0) {
            if (results->players[id - 2].pastResult >
                    results->players[id - 1].pastResult) {
                results->players[id - 1].losses += 1;
                results->scores.loss[results->scores.counter - 1]++;


            }
        }
    }
    if (!strcmp(outcome.wlt, "TIE")) {
        results->players[id].pastResult = 2;
        if (id % 2 == 0) {
            if (results->players[id - 2].pastResult ==
                    results->players[id - 1].pastResult) {
                results->players[id - 1].ties += 1;
                results->scores.tie[results->scores.counter - 1]++;

            }
        }
    }
}

//Thread function to handle the match request from the client.  Each time a
//connection arrives on the listen port, it connects and a thread executes
//to handle the messaging between the two programs
void* handle_request(void* v) {

    MatchResults* results = (MatchResults*)v;
    pthread_mutex_lock(&results->lock);
    char* buffer;
    ClientMessage message;
    results->plrid++;
    MatchOutcome outcome;

    buffer = read_line(results->players[results->plrid - 1].read);
    fflush(results->players[results->plrid - 1].read);
    message = parse_message(buffer, &results->players[results->plrid - 1],
            &outcome);
    int flag = 0;
    for (int i = 0; i < results->plrid - 1; i++) {
        if (!strcmp(results->players[results->plrid - 1].name,
                results->scores.name[i])) {
            flag = 1;
        }
    }
    if (!flag) {
        strcpy(results->scores.name[results->plrid - 1], 
                results->players[results->plrid - 1].name);
        results->scores.counter++;
    }

    
    if (message == MR) {
        send_mr(results, results->plrid);
    }
    pthread_mutex_unlock(&results->lock);
    while (1) {
        if (message == RESULT) {
            pthread_mutex_lock(&results->lock);
        
            check_results(results, results->plrid, outcome);
            pthread_mutex_unlock(&results->lock);

            break;
        }
    }
    return NULL;
}

//Function to handle sighup, not working
void* sighandle(void* v) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);
    return 0;
}

//Function that initialises the server, prints out the port and listens
//for connections.  Upon connection, start up a thread for communication
int init_server(struct addrinfo* ai, struct addrinfo hints, char* port,
        MatchResults* results) {
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int err;
    if ((err = getaddrinfo("localhost", port, &hints, &ai))) {
        freeaddrinfo(ai);
        fprintf(stderr, "%s\n", gai_strerror(err));
        return 1;
    }
    int serv = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(serv, (struct sockaddr*)ai->ai_addr, sizeof(struct sockaddr))) {
        perror("Binding");
        return 3;
    }

    struct sockaddr_in ad;
    memset(&ad, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    if (getsockname(serv, (struct sockaddr*)&ad, &len)) {
        perror("sockname");
        return 4;
    }
    printf("%u\n", ntohs(ad.sin_port));
    fflush(stdout);

    if (listen(serv, 10)) {
        perror("Listen");
        return 4;
    }
    int connfd;
    int fdcount = 0;
    results->connectcount = -1;
    results->players = (Players*) malloc(2 * sizeof(Players));
    while (connfd = accept(serv, 0, 0), connfd >= 0) {
        int fd2 = dup(connfd);
        FILE* read = fdopen(fd2, "r");
        FILE* write = fdopen(connfd, "w");
        results->players[fdcount].read = read;
        results->players[fdcount].write = write;
        pthread_t threadId;
        fdcount++;
        pthread_create(&threadId, NULL, handle_request, (void*)results);
    }
    return 0;
}

//Main function, requires no arguments
int main(int argc, char** argv) {
    if (argc != 1) {
        fprintf(stderr, "Usage: rpsserver\n");
        exit(1);
    }

    struct addrinfo* ai = 0;
    struct addrinfo hints;
    char* port;
    MatchResults results;
    results.counter = 0;
    results.matchid = 1;
    results.plrid = 0;

    if (argc < 2) {
        port = "0";
    } else {
        port = argv[1];
    }
    if (pthread_mutex_init(&results.lock, NULL) != 0) {
        printf("Error loeading lock\n");
        return 1;
    }
    pthread_t sigthread;
    
    pthread_create(&sigthread, NULL, sighandle, (void*) &results);

    
    init_server(ai, hints, port, &results);
    return 0;
}

