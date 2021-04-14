#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "2310util.h"

#define INITIAL_BUFFER_SIZE 10
#define MIN_ARGC 6
#define STD_RULES_FILE "standard.rules"
#define MIN_SHIP_COUNT 1
#define MIN_SHIP_SIZE 1
#define MIN_MAP_DIM 1
#define MAX_MAP_DIM 26


typedef enum Messages {
    YT,
    MAP,
    OK,
    RULES,
    GUESS,
    HIT,
    SHIP_SUNK,
    MISS,
    ERR,
    SKIP,
    GAME_OVER
} Messages;

// Attempts to read the rules file at the given filepath.
// Updates the provided rules to contain the read information.
// If an error occurs while reading, returns the appropriate
// error code, otherwise returns ERR_OK.
ErrorCode read_rules_file_a(FILE* stream, Rules* rules) {
    char* next;
    char messageName[5];
    int check;
    int j = 0;
    next = read_line(stream);        

    strtrim(next);
    if (is_comment(next)) {
        free(next);
    }
    strncpy(messageName, next, 5);
    messageName[5] = 0;
    check = strcmp(messageName, "RULES");
    if (!check) {
        rules->numRows = next[6] - '0';
        rules->numCols = next[8] - '0';
        rules->numShips = next[10] - '0';
        rules->shipLengths = malloc(sizeof(int) * rules->numShips + 1);
        for (int i = 12; i < strlen(next); i += 2) {
            rules->shipLengths[j] = next[i] - '0';
            j++;
        }
    } else {
        fprintf(stderr, "Error in RULES message");
    }
      
        
    free(next);
        
 
    return ERR_OK;
}

//============================================================================




// Parses the content of the given line of the map file
// and updates the provided map accordingly. If parsing is
// successful, returns true, otherwise returns false.
bool read_map_line_a(char* line, char* mapcsv, Map* map) {
    
    int row = 0;
    char col;
    char direction;
    char dummy;
    int scanCount = sscanf(line, "%c%d %c%c", &col, &row, &direction, &dummy);
    
    if (scanCount != 3 || !isdigit(line[1]) || !is_valid_row(row) ||
            !is_valid_column(col) || !is_valid_direction(direction)) {
        return false;
    }
    add_ship(map, new_ship(0, new_position(col, row), (Direction) direction));
  //  printf("%d", map->numShips);
 
    sprintf(mapcsv + strlen(mapcsv), "%c%d,%c:", (char)col, row,
            (char)direction);
    return true;
}

// Attempts to read the map file at the given filepath. 
// Updates the provided map with the read information. The
// isCPU flag should be set if the map being read is for the 
// CPU player. If an error occurs, returns the appropriate 
// error code, otherwise returns ERR_OK.
char* read_map_file_a(char* filepath, Map* map, bool isCPU) {
    char* mapcsv;
    
    mapcsv = (char*)calloc(5, sizeof(char));
    FILE* infile = fopen(filepath, "r");
    char* next;
    Map newMap = empty_map();

    while ((next = read_line(infile)) != NULL) {
        strtrim(next);
     
        if (is_comment(next)) {
            free(next);
            continue;
        }
        if (!read_map_line_a(next, mapcsv, &newMap)) {
            free(next);
            fclose(infile);
            mapcsv[0] = '1';
            return mapcsv;
        }
        free(next);
        mapcsv = realloc(mapcsv, sizeof(char) * (strlen(mapcsv)) + 6);
                
    }
    mapcsv[strlen(mapcsv) - 1] = 0; 
    fclose(infile);
    memcpy(map, &newMap, sizeof(Map));
    return mapcsv;
}

// Outputs the given hitmap to the given stream.
void print_hitmap_a(HitMap map, FILE* stream, bool hideMisses) {
    
    // Print the column headings
    fprintf(stderr, "   ");
    for (int i = 0; i < map.cols; i++) {
        fprintf(stream, "%c", 'A' + i);
    }
    fprintf(stream, "\n");

    // For each row, print the row heading, followed by the data
    for (int i = 0; i < map.rows; i++) {
        fprintf(stream, "%2d ", i + 1);
        for (int j = 0; j < map.cols; j++) {
            Position pos = {i, j};
            char info = get_position_info(map, pos);
            if (info == HIT_MISS && hideMisses) {
                info = HIT_NONE;
            }
            fprintf(stream, "%c", info);
        }
        fprintf(stream, "\n");
    }
}

//Prints the message associated with the given hit
void print_hit_message_a(HitType type) {
    
    if (type == HIT_MISS) {
        fprintf(stderr, "MISS");
    } else if (type == HIT_HIT) {
        fprintf(stderr, "HIT");
    } else if (type == HIT_SUNK) {
        fprintf(stderr, "SHIP SUNK");
    } else if (type == HIT_REHIT) {
        fprintf(stderr, "Repeated guess");
    }
}

// Prints the message corresponding to the given hit type.
void print_maps_a(HitMap cpuMap, HitMap playerMap) {
    
    print_hitmap_a(playerMap, stderr, false);
    fprintf(stderr, "===\n");
    print_hitmap_a(cpuMap, stderr, false);
}

//Initializes the agent from the given info
GameState init_game(GameInfo info) {
    
    GameState newGame;
    newGame.info = info;
    
    newGame.maps[0] = empty_hitmap(info.rules.numRows, info.rules.numCols);
    newGame.maps[1] = empty_hitmap(info.rules.numRows, info.rules.numCols);
    mark_ships(&newGame.maps[PLR_HUMAN], info.playerMap);
    
    return newGame;
}

typedef struct Guesses {
    char colguess;
    int rowguess;
} Guesses;

//Function for generating the guess from agent A
void a_guess(HitMap map, Guesses* guess, Position* pos) {
    int colguess = 0;
    int rowguess = 0;
    int flag = 0;
    pos->row = 0;
    pos->col = 0;
    for (int i = 0; i < map.rows; i++) {
        for (int j = 0; j < map.cols; j++) {
            if (map.data[map.cols * i + j] == '.') {
                rowguess = i;
                if (rowguess % 2 == 0) {
                    colguess = j;
                }
                flag = 1;
                break;
            }
        }
        if (flag) {
            break;
        }
    }
    if (rowguess % 2 != 0) {
        for (int k = map.cols - 1; k > 0; k--) {
            if (map.data[map.cols * rowguess + k] == '.') {
                colguess = k;
                break;
            }
        }
    }
    Position newPos = new_position((char)colguess + 'A', rowguess + 1);
    memcpy(pos, &newPos, sizeof(Position));
   
}

//Checks if the game is over
void game_over(int winner) {
    fprintf(stderr, "GAME OVER - player %d wins\n", winner);
    exit(0);
}

//Receives the input from the given stream
Messages receive_message(FILE* stream, Position* pos, int* plr) {
    int check;
    char* message;
    message = read_line(stream);
    if (message == NULL) {
        return ERR;
    }
    strtrim(message);
    check = strncmp(message, "YT", 2);
    if (!check) {
        return YT;
    }
    check = strncmp(message, "OK", 2);
    if (!check) {
        return OK;
    }
    check = strncmp(message, "HIT", 3);
    if (!check) {
        *plr = (message[4] - '0');
        pos->col = message[6] - 'A';
        pos->row = (message[7] - '0') - 1;
        return HIT;
    }
    check = strncmp(message, "MISS", 4);
    if (!check) { 
        *plr = (message[5] - '0');
        pos->col = message[7] - 'A';
        pos->row = (message[8] - '0') - 1;
        return MISS;
    }
    check = strncmp(message, "SUNK", 4);
    if (!check) {
        *plr = (message[5] - '0');
        pos->col = message[7] - 'A';
        pos->row = (message[8] - '0') - 1;
        return SHIP_SUNK;
    }
    check = strncmp(message, "EARLY", 5);
    if (!check) {
        exit(0);
    }
    check = strncmp(message, "DONE", 4);
    if (!check) {
        game_over((message[5] - '0'));

        return GAME_OVER;
    }
    return ERR;
}

//Marks the position of the guess on the appropriate map
HitType mark_pos(Position pos, HitMap* hitmap, Map* map, Messages msg) {
    if (msg == HIT) {
        update_hitmap(hitmap, pos, HIT_HIT);
        return HIT_HIT;
    }
    if (msg == MISS) {
        update_hitmap(hitmap, pos, HIT_MISS);
        return HIT_MISS;
    }
    if (msg == SHIP_SUNK) {
        update_hitmap(hitmap, pos, HIT_HIT);
        return HIT_SUNK;
    }
    return HIT_REHIT;
}

//Checks the given seed 
void check_seed(char* seed) {
    for (int i = 0; i < strlen(seed) - 1; i++) {
        if (isalnum(seed[i])) {
            fprintf(stderr, "Invalid seed\n");
            exit(4);
        }
    }
}

int main(int argc, char** argv){
     //int id = atoi(argv[1]);
    if (argc != 4) {
        fprintf(stderr, "Usage: agent id map seed\n");
        exit(1);
    }
    check_seed(argv[3]);
    int id = atoi(argv[1]) - 1;
    int op = !id;
    char* map = argv[2];
    char* test;
    GameInfo info;
    ErrorCode err;
    test = read_map_file_a(map, &info.playerMap, false);
    if (test[0] == '1') {
        fprintf(stderr, "Invalid map file\n");
        exit(3);
    }
      read_map_file_a(map, &info.cpuMap, true);
    err = read_rules_file_a(stdin, &info.rules);
    fflush(stdin);
    if (err != ERR_OK) {
        err_msg(err);
    }
    err = validate_info(info);
    if (err != ERR_OK) {
        err_msg(err); 
    }
    fprintf(stdout, "MAP %s\n", test);
    fflush(stdout);
    GameState state = init_game(info);
    print_maps_a(state.maps[PLR_CPU], state.maps[PLR_HUMAN]);
    int ctr = 0;
    while (1) {
        HitType hit;
        Guesses guess;
        Position pos;
        int plr;
        Messages msg = receive_message(stdin, &pos, &plr);
        fflush(stdin);
        if (msg == ERR) {
            fprintf(stderr, "Communications error\n");
            exit(5);
        }
        if (msg == YT) {
            a_guess(state.maps[op], &guess, &pos);
            printf("GUESS ");
            printf("%c", (char)(pos.col + 65));
            printf("%d\n", pos.row + 1);
            fflush(stdout);
            ctr++;
        }
        if ((msg == HIT || msg == MISS || msg == SHIP_SUNK) && plr == 1) {
            hit = mark_pos(pos, &state.maps[op], &state.info.playerMap, msg);
            print_hit_message_a(hit);
            fprintf(stderr, " player 1 guessed %c%d\n", pos.col + 65, 
                    pos.row + 1);
            fflush(stderr);
        }
        if ((msg == HIT || msg == MISS || msg == SHIP_SUNK) && plr == 2) {
            hit = mark_pos(pos, &state.maps[id], &state.info.cpuMap, msg);
            print_hit_message_a(hit);
            fprintf(stderr, " player 2 guessed %c%d\n", pos.col + 65, 
                    pos.row + 1);
            fflush(stderr);
            print_maps_a(state.maps[PLR_CPU], state.maps[PLR_HUMAN]);
            fflush(stderr);
        }
    }
    return 0;
}

