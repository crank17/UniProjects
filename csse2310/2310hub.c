#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "2310util.h"


#define INITIAL_BUFFER_SIZE 10
#define MIN_ARGC 5
#define STD_RULES_FILE "standard.rules"
#define MIN_SHIP_COUNT 1
#define MIN_SHIP_SIZE 1
#define MIN_MAP_DIM 1
#define MAX_MAP_DIM 26

// Prints the message corresponding to the given hit type.
void print_hit_message_hub(HitType type, FILE* stream) {
    
    if (type == HIT_MISS) {
        fprintf(stream, "MISS");
    } else if (type == HIT_HIT) {
        fprintf(stream, "HIT");
    } else if (type == HIT_SUNK) {
        fprintf(stream, "SHIP SUNK");
    } else if (type == HIT_REHIT) {
        fprintf(stream, "REPEATED GUESS");
    }
}

// Prints the given maps to stdout.
void print_maps_hub(HitMap cpuMap, HitMap playerMap) {
    printf("**********\n");
    printf("ROUND 0\n");
    print_hitmap(cpuMap, stdout, false);
    printf("===\n");
    print_hitmap(playerMap, stdout, false);
}

//Parses the rules into a comma separated file to send to agents
char* convert_rules(Rules rules) {
    char* rulesCsv;
    rulesCsv = (char*)malloc(sizeof(char) * 5);
    sprintf(rulesCsv, "%d,%d,%d,", rules.numRows, rules.numCols,
            rules.numShips);
    for (int i = 0; i < rules.numShips; i++) {
        rulesCsv = realloc(rulesCsv, sizeof(char) * 1);
        sprintf(rulesCsv + strlen(rulesCsv), "%d,", rules.shipLengths[i]);

    }
    rulesCsv[strlen(rulesCsv) - 1] = '\n';
    return rulesCsv;
}

typedef struct Config {
    char* agent1;
    char* map1;
    char* agent2;
    char* map2;
} Config;

//Function for reading the config file
ErrorCode read_config(char* filename, Config* config) {
    char* line;
    FILE* file = fopen(filename, "r");
    if (!file) {
        return ERR_BAD_RULES;
    }
    while (1) {
        line = read_line(file);
        if (!is_comment(line)) {
            break;
        }
    }
            
            
    char* tok;
    tok = strtok(line, ",");
    config->agent1 = tok;
    config->map1 = strtok(NULL, ",");
    config->agent2 = strtok(NULL, ",");
    config->map2 = strtok(NULL, ",");
    return ERR_OK;
}

//Converts the comma separated map file to a usable map file
void convert_map(char* mapcsv, Map* map) {
    char* tok;
    char delim[2] = ":";
    char col, dir;
    int row;
    mapcsv += 4;
    tok = strtok(mapcsv, delim);   
    while (tok != NULL) {
        sscanf(tok, "%c%d,%c", &col, &row, &dir);  
        add_ship(map, new_ship(0, new_position(col, row), (Direction) dir));
        tok = strtok(NULL, delim);
    }
}

//Function to exit on communication error
void coms_error() {
    fprintf(stderr, "Communications error\n");
    exit(5);
}

//Initialises the game by printing the rules to the agents and expecting a 
//map message to be returned
GameState init_game(FILE* a1, FILE* a2, FILE* b1, FILE* b2, GameInfo* info) {
    char* amap;
    char* bmap;
    char message[10];
    bool waiting = true;
    char* rulesCsv = convert_rules(info->rules);
    GameState newGame;
    Map newMapA = empty_map();
    newGame.maps[0] = empty_hitmap(info->rules.numRows, info->rules.numCols);
    newGame.maps[1] = empty_hitmap(info->rules.numRows, info->rules.numCols);
    fprintf(a2, "RULES %s", rulesCsv);
    fprintf(b2, "RULES %s", rulesCsv);
    fflush(a2);
    fflush(b2);
    while (waiting) {
        amap = read_line(a1);
        fflush(a1);
        strncpy(message, amap, 3);
        message[3] = 0;
        if (strcmp(message, "MAP")) { 
            coms_error();
        } else {
            convert_map(amap, &newMapA);
            memcpy(&info->playerMap, &newMapA, sizeof(Map));
            waiting = false;
            break;
        }
    }
    Map newMapB = empty_map();
    waiting = true;
    while (waiting) {
        bmap = read_line(b1);
        fflush(b1);
        strncpy(message, bmap, 3);
        message[3] = 0;
        if (strcmp(message, "MAP")) { 
            coms_error();
        } else {
            convert_map(bmap, &newMapB);
            memcpy(&info->cpuMap, &newMapB, sizeof(Map));
            waiting = false;
            break;
        }
    }
    return newGame;
}

typedef enum Message {
    MISS,
    HIT,
    GUESS,
    OK,
    SUNK
} Message;

//Function to read a guess from the agent, and put it into a position struct
Message read_guess(FILE* stream, GameInfo info, Position* pos, int plr) {
    char* input;
    char message[6];
    while (1) {
        input = read_line(stream);
        fflush(stream);
        memset(message, '\0', sizeof(message));
        strncpy(message, input, 5);
        message[5] = 0;
        if (!strcmp(message, "GUESS")) {
            Position readPos;
            input += 6;
            if (!read_position(input, info, &readPos)) {
                return OK;
            }
            memcpy(pos, &readPos, sizeof(Position));
            return OK;
        }
        coms_error();
    }
}

//Plays the game, sending and handling messages between the hub and the agents
void play_game(GameState state, FILE* a1, FILE* a2, FILE* b1, FILE* b2) {
    Message msg;
    int player = 1;
    Position pos;
    HitType hit;
    while (1) {
        if (player == 1) {
            fprintf(a2, "YT\n");
            fflush(a2);
            msg = read_guess(a1, state.info, &pos, player);
            if (msg == OK) {
                fprintf(a2, "OK\n");
            } else {
                continue;
            }
            hit = mark_ship_hit(&state.maps[1], &state.info.cpuMap, pos);
            print_hit_message_hub(hit, a2);
            fprintf(a2, " %d,%c%d\n", player, pos.col + 65, pos.row + 1);
            fflush(a2);
            print_hit_message_hub(hit, b2);
            fprintf(b2, " %d,%c%d\n", player, pos.col + 65, pos.row + 1);
            fflush(b2);
            print_hit_message_hub(hit, stdout);
            printf(" player %d guessed %c%d\n", player, pos.col + 65,
                    pos.row + 1);
            player = 2;
        }
        if (player == 2) {
            fprintf(b2, "YT\n");
            fflush(b2);
            msg = read_guess(b1, state.info, &pos, player);
            if (msg == OK) {
                fprintf(b2, "OK\n");
            } else {
                continue;
            }
            hit = mark_ship_hit(&state.maps[0], &state.info.playerMap, pos);
            print_hit_message_hub(hit, a2);
            fprintf(a2, " %d,%c%d\n", player, pos.col + 65, pos.row + 1);
            fflush(a2);
            print_hit_message_hub(hit, b2);
            fprintf(b2, " %d,%c%d\n", player, pos.col + 65, pos.row + 1);
            fflush(b2);
            print_hit_message_hub(hit, stdout);
            printf(" player %d guessed %c%d\n", player, pos.col + 65,
                    pos.row + 1);
            player = 1;
        }
        print_maps_hub(state.maps[0], state.maps[1]);
    }
}

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);
    if (argc != 3) {
        fprintf(stderr, "Usage: 2310hub rules config\n");
        exit(1);
    }
    char* rules = argv[1];
    char* configfile = argv[2];
    ErrorCode err;
    GameInfo info;
    err = read_rules_file(rules, &info.rules);
    if (err != ERR_OK) {
        fprintf(stderr, "Error reading rules\n");
        exit(2);
    }
    Config config;
    err = read_config(configfile, &config);
    if (err != ERR_OK) {
        fprintf(stderr, "Error reading config\n");
        exit(3);
    }
    pid_t childA, childB;
    int atohub[2];
    int hubtoa[2];
    int btohub[2];
    int hubtob[2];
    int errpipe[2];
    int devnull = open("/dev/null", O_WRONLY);
    pipe(atohub);
    pipe(hubtoa);
    pipe(btohub);
    pipe(hubtob);
    pipe(errpipe);
    childA = fork();
    if (childA == 0) {
        dup2(atohub[1], 1);
        dup2(hubtoa[0], fileno(stdin));
        dup2(devnull, 2);
        close(atohub[0]);
        close(hubtoa[1]);
        write(errpipe[1], "2", 1);
        if (execlp(config.agent1, "dummy", "1", config.map1, "1", NULL) < 0) {
            write(errpipe[1], "1", 1);
            close(errpipe[1]);
            exit(0);

        }
    } else {
        childB = fork();
        if (childB == 0) {
            dup2(btohub[1], 1);
            dup2(hubtob[0], fileno(stdin));
            dup2(devnull, 2);
            close(btohub[0]);
            close(hubtob[1]);
        } else {
            FILE* awrite = fdopen(atohub[0], "r");
            close(atohub[1]);
            FILE* hubwritea = fdopen(hubtoa[1], "w");
            close(hubtoa[0]);
            FILE* bwrite = fdopen(btohub[0], "r");
            close(btohub[1]);
            FILE* hubwriteb = fdopen(hubtob[1], "w");
            close(hubtob[0]);
            char buff[4];
            read(errpipe[0], buff, 2);
            if (buff[1] == '1') {
                fprintf(stderr, "Error starting agents\n");
                exit(4);
            }
            GameState state = init_game(awrite, hubwritea, bwrite, hubwriteb,
                    &info);
            validate_info(info);
            state.info = info;
            mark_ships(&state.maps[0], info.playerMap);
            mark_ships(&state.maps[1], info.cpuMap);
            print_maps_hub(state.maps[0], state.maps[1]);
            play_game(state, awrite, hubwritea, bwrite, hubwriteb);   
            
        }
    }
}
