#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#define INITIAL_BUFFER_SIZE 20

typedef enum Guess {
    ROCK,
    PAPER,
    SCISSORS,
    ERR_GUESS
} Guess;

typedef struct PlayerInfo {
    char* name;
    char* port;
    int matches;
    FILE* read;
    FILE* write;
} PlayerInfo;
    
typedef struct ServerInfo {
    char* eph; 
    unsigned int port;
    FILE* read;
    FILE* write;
     
} ServerInfo;

typedef struct OpponentInfo { 
    char* name;
    int id;
    char* port;
} OpponentInfo;

typedef struct Communication {
    FILE* read;
    FILE* write;
    int fdr;
    int fdw;
} Communication;

typedef enum Condition {
    WIN,
    LOSE,
    TIE,
    ERR
} Condition;

typedef struct MatchResults {
    int* ids;
    char** opponent;
    char** winLose;
} MatchResults;

//Generates a random guess using the psuedo random function
Guess generate_guess() {
    int moveVal = rand() % 3;
    switch (moveVal) {
        case 0:
            return ROCK;
        case 1:
            return PAPER;
        case 2:
            return SCISSORS;
        default:
            return ERR_GUESS;
    }

}

//Reads a line from the given input stream
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

//Generates the rand() seed by iterating through the name of the player
void generate_seed(char* name) {
    int seed = 0;
    for (int i = 0; i < strlen(name); i++) {
        seed += name[i];
    }
    srand(seed);
}

//Initialises the client server, and begins listening on ephenumeral port
int init_server(struct addrinfo* ai, struct addrinfo hints, ServerInfo* info) {
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int err;
    if ((err = getaddrinfo("localhost", info->eph, &hints, &ai))) {
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
    info->port = ntohs(ad.sin_port);
    if (listen(serv, 10)) {
        perror("Listen");
       
    }
             
    return serv;
    
}
  
//Initialises the client, prints out a match request upon connection complete
//else, returns connection error
int init_client(struct addrinfo* ai, struct addrinfo hints, PlayerInfo* info, 
        ServerInfo sinfo) {
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int err;
    if ((err = getaddrinfo("localhost", info->port, &hints, &ai))) {
        freeaddrinfo(ai);
        fprintf(stderr, "%s\n", gai_strerror(err));
        return 1;
    }
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, (struct sockaddr*)ai->ai_addr, sizeof(struct sockaddr))) {
        fprintf(stderr, "Invalid port number\n");       
        exit(4);
    }
    char matchRequest[20];
    sprintf(matchRequest, "MR:%s:%u\n", info->name, sinfo.port);
    int fd2 = dup(fd);
    FILE* to = fdopen(fd, "w");
    FILE* from = fdopen(fd2, "r");

    fprintf(to, matchRequest);
    fflush(to);
    info->write = to;
    info->read = from;
    return 0;
}

//Connects the clients with the given opponent id, saves the file descriptors
//to the given OpponentInfo struct
int connect_client(struct addrinfo* ai, struct addrinfo hints,
        OpponentInfo* info, PlayerInfo pinfo,
        Communication* com) {
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int err;
    if ((err = getaddrinfo("localhost", info->port, &hints, &ai))) {
        freeaddrinfo(ai);
        fprintf(stderr, "%s\n", gai_strerror(err));
        return 1;
    }
     
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, (struct sockaddr*)ai->ai_addr, sizeof(struct sockaddr))) {
        
        fprintf(stderr, "Invalid port number\n");       
        exit(4);
    }
    FILE* to = fdopen(fd, "w");
    com->write = to;
    com->fdw = fd;
    return 0;

}

//Receives a match message from the server, and parses the opponents 
//information into the given struct
int receive_match(OpponentInfo* oinfo, PlayerInfo info){
    char* buffer;
    char* delim = ":";
    buffer = read_line(info.read);
    fflush(info.read);
    char* msg = strtok(buffer, delim);
    if (strcmp(msg, "MATCH")) {
        return -1;
    }
    oinfo->id = atoi(strtok(NULL, delim));
    oinfo->name = strtok(NULL, delim);
    oinfo->port = strtok(NULL, delim);
   
    return 1;
  
}

//Waits for a client connection - blocks until the other client has connected
void listen_for_connection(int serv, ServerInfo* info, struct addrinfo* ai, 
        struct addrinfo hints, 
        OpponentInfo* oinfo, PlayerInfo pinfo, Communication* com) {

    
    connect_client(ai, hints, oinfo, pinfo, com);
    int connfd;
    connfd = accept(serv, 0, 0); 
    int fd2 = dup(connfd);
    FILE* read1 = fdopen(fd2, "r");
    com->read = read1;
    com->fdr = fd2;    

   
}

//Sends the generated move to the opponent
Guess send_move(Communication com) {
    char* move;
    Guess guess;
    guess = generate_guess();
    if (guess == ROCK) {
        move = "ROCK";
    }
    if (guess == SCISSORS) {
        move = "SCISSORS";
    }
    if (guess == PAPER) {
        move = "PAPER";
    }
    fprintf(com.write, "MOVE:%s\n", move);
    fflush(com.write);
    return guess;
}

//Reads the generated move from the opponent
Guess read_move(Communication com) {
    char* buffer;
    buffer = read_line(com.read);
    fflush(com.read);
    buffer += 5;
    if (!strcmp(buffer, "ROCK")) {
        return ROCK;
    }
    if (!strcmp(buffer, "PAPER")) {
        return PAPER;
    }
    if (!strcmp(buffer, "SCISSORS")) {
        return SCISSORS;
    }
    //intf("Player guessed %s\n", buffer);
    return ERR_GUESS;


    
}

//Checks if the opponents guess wins or loses against player guess
Condition check_guess(Guess myguess, Guess opguess) {
    if (myguess == ROCK && opguess == SCISSORS) {
        return WIN;
    }
    if (myguess == SCISSORS && opguess == ROCK) {
        return LOSE;
    }
    if (myguess == PAPER && opguess == ROCK) {
        return WIN;
    }
    if (myguess == PAPER && opguess == SCISSORS) {
        return LOSE;
    }
    if (myguess == SCISSORS && opguess == PAPER) {
        return WIN;
    }
    if (myguess == ROCK && opguess == PAPER) {
        return LOSE;
    }
    return TIE;
}

//Plays a single match until client either reaches 3 games or until there is a 
//tie
Condition play_match(OpponentInfo oinfo, PlayerInfo info, Communication com) {

    int gamecounter = 0;
    int oppscore = 0;
    int playerscore = 0;
    Condition win;
    Guess myguess, opguess;
    while (oppscore != 3 || playerscore != 3) {
        if (gamecounter == 20 && (playerscore == 0 && oppscore == 0)) {
            return TIE;
        }
        if (gamecounter >= 5) {
            if (oppscore > playerscore) {
                return LOSE;
            }
            if (playerscore > oppscore) {
                return WIN;
            }
        }
        myguess = send_move(com);
        opguess = read_move(com);
        if (opguess == ERR_GUESS) {
            return ERR;
        }
        win = check_guess(myguess, opguess);
        if (win == TIE) {
            gamecounter++;
        }
        if (win == WIN) {
            gamecounter++;
            playerscore++;
        }
        if (win == LOSE) {
            gamecounter++;
            oppscore++;
        }
    }
    if (oppscore >= 3) {
        return LOSE;
    } else {
        return WIN;
    }
    return 0;
}

//Checks if the clients name is suitable 
int check_name(char* name) {
    for (int i = 0; i < strlen(name); i++) {
        if (!isalnum(name[i])) {
            return 1;
        }
    }
    if (!strcmp(name, "TIE")) {
        return 1;
    }
    if (!strcmp(name, "ERROR")) {
        return 1;
    }
    return 0;
}

//adds the result of the match to the player wins/losses
void add_results(int i, char* opname, int id, Condition cdn,
        MatchResults* results) {
    results->opponent[i] = (char*)malloc(strlen(opname) * sizeof(char));
    results->winLose[i] = (char*)malloc(7 * sizeof(char));
    if (cdn == WIN) {
        results->winLose[i] = "WIN";
    } 
    if (cdn == LOSE) {
        results->winLose[i] = "LOST";
    }
    if (cdn == TIE) {
        results->winLose[i] = "TIE";
    }
    if (cdn == ERR) {
        results->winLose[i] = "ERROR";
    }

        
    results->opponent[i] = opname;
    results->ids[i] = id;
}

//Checks if the port command is valid
void check_port(char* port) {
    char* dummy = port;
    for (int i = 0; i < strlen(dummy); i++) {
        if (!isdigit(dummy[i])) {
            fprintf(stderr, "Invalid port number\n");
            exit(4);
        }
    }
}

//Checks if the arguments given are valid
void check_args(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: rpsclient name matches port\n");
        exit(1);
    }
    if (check_name(argv[1])) {
        fprintf(stderr, "Invalid name\n");
        exit(2);
    }
    if (isdigit(atoi(argv[2])) || atoi(argv[2]) == 0) {
        fprintf(stderr, "Invalid match count\n");
        exit(3);
    }

}

//Closes all client and server connections
void close_connections(FILE* f1, FILE* f2, FILE* f3, FILE* f4) {
    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
}

//Plays the game until a match has finished
void play_game(PlayerInfo info, ServerInfo sinfo, Communication com, 
        OpponentInfo oinfo,
        MatchResults results){
    struct addrinfo* servAi = 0;
    struct addrinfo servHints;
    init_server(servAi, servHints, &sinfo);
    int serv;
    int gamecount = 0;
    while (gamecount < info.matches) {
        struct addrinfo* ai = 0;
        struct addrinfo hints;
        struct addrinfo* aiC = 0;
        struct addrinfo chints;
        Condition cdn;
        serv = init_server(servAi, servHints, &sinfo);
        init_client(ai, hints, &info, sinfo);
        receive_match(&oinfo, info);
        listen_for_connection(serv, &sinfo, aiC, chints, &oinfo, info, &com);
        cdn = play_match(oinfo, info, com);
        add_results(gamecount, oinfo.name, oinfo.id, cdn, &results);
        if (cdn == WIN) {
            fprintf(info.write, "RESULT:%d:%s\n", results.ids[gamecount],
                    info.name);
            fflush(info.write);
        }
        if (cdn == LOSE) {
            fprintf(info.write, "RESULT:%d:%s\n", results.ids[gamecount],
                    oinfo.name);
            fflush(info.write);
        } 
        if (cdn == TIE || cdn == ERR) {
            fprintf(info.write, "RESULT:%d:%s\n", results.ids[gamecount],
                    results.winLose[gamecount]);
            fflush(info.write);
        }
        close_connections(info.read, info.write, com.read, com.write);
        free(ai);
        free(servAi);
        free(aiC);
        usleep(250);
        gamecount++;
    }
}

//main function
int main(int argc, char** argv) {
    check_args(argc, argv);
    check_port(argv[3]);
    generate_seed(argv[1]);
    PlayerInfo info;
    ServerInfo sinfo;
    OpponentInfo oinfo;
    Communication com;
    MatchResults results;
    sinfo.eph = "0";
    info.name = argv[1];
    info.matches = atoi(argv[2]);
    info.port = argv[3];
    results.opponent = (char**)malloc(info.matches * sizeof(char*));
    results.winLose = (char**)malloc(info.matches * sizeof(char*));
    results.ids = (int*)malloc(info.matches * sizeof(int));
    play_game(info, sinfo, com, oinfo, results);
    for (int i = 0; i < info.matches; i++) {
        printf("%d %s %s\n", results.ids[i], results.opponent[i],
                results.winLose[i]);
    }
    return 0;
}

