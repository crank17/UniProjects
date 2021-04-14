#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "2310util.h"

#define INITIAL_BUFFER_SIZE 11
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


//============================================================================

/// Attempts to read the rules file at the given filepath.
// Updates the provided rules to contain the read information.
// If an error occurs while reading, returns the appropriate
// error code, otherwise returns ERR_OK.
ErrorCode read_rules_file_b(FILE* stream, Rules* rules) {
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

// Parses the content of the given line of the map file
// and updates the provided map accordingly. If parsing is
// successful, returns true, otherwise returns false.
bool read_map_line_b(char* line, char* mapcsv, Map* map) {
    
    int row;
    char col, direction, dummy;
    int scanCount = sscanf(line, "%c%d %c%c", &col, &row, &direction, &dummy);
    
    if (scanCount != 3 || !isdigit(line[1]) || !is_valid_row(row) ||
            !is_valid_column(col) || !is_valid_direction(direction)) {
        return false;
    }
    add_ship(map, new_ship(0, new_position(col, row), (Direction) direction));
    sprintf(mapcsv + strlen(mapcsv), "%c%d,%c:", col, row, direction);
    return true;
}

// Attempts to read the map file at the given filepath. 
// Updates the provided map with the read information. The
// isCPU flag should be set if the map being read is for the 
// CPU player. If an error occurs, returns the appropriate 
// error code, otherwise returns ERR_OK.
char* read_map_file_b(char* filepath, Map* map, bool isCPU) {
    char* mapcsv;
    mapcsv = (char*)malloc(5 * sizeof(char));
    FILE* infile = fopen(filepath, "r");
    char* next;
    Map newMap = empty_map();

    while ((next = read_line(infile)) != NULL) {
        strtrim(next);
     
        if (is_comment(next)) {
            free(next);
            continue;
        }
        if (!read_map_line_b(next, mapcsv, &newMap)) {
            free(next);
            fclose(infile);
            mapcsv[0] = '1';
            return mapcsv;
        }
        free(next);
        mapcsv = realloc(mapcsv, sizeof(char*) * 5);
                
    }
    mapcsv[strlen(mapcsv) - 1] = 0; 
    fclose(infile);
    memcpy(map, &newMap, sizeof(Map));
    return mapcsv;
}

// Outputs the given hitmap to the given stream.
void print_hitmap_b(HitMap map, FILE* stream, bool hideMisses) {
    
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

// Checks if the given target position will hit the given ship
// Returns true if it will and updates index with the position
// where the ship will be hit (i.e. 0 is the tip), else returns false. 
void print_hit_message_b(HitType type) {
    
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
void print_maps_b(HitMap cpuMap, HitMap playerMap) {
    
    print_hitmap_b(cpuMap, stderr, false);
    fprintf(stderr, "===\n");
    print_hitmap_b(playerMap, stderr, false);
}

// Initialises and returns a new game using the given
// command line arguments and game information.
GameState init_game(GameInfo info) {
    
    GameState newGame;
    newGame.info = info;
    
    // Set up player inputs
   // Set up hit maps
    newGame.maps[0] = empty_hitmap(info.rules.numRows, info.rules.numCols);
    newGame.maps[1] = empty_hitmap(info.rules.numRows, info.rules.numCols);
    mark_ships(&newGame.maps[PLR_HUMAN], info.playerMap);
   // printf("%s", newGame.maps[0].data);

    // Set up move functions
 
    return newGame;
}

typedef struct Guesses {
    char colguess;
    int rowguess;
} Guesses;

//Adds guess to the given position
void add_guess(Position* pos, int row, int col) {
    pos->row = row;
    pos->col = col;
}

//Generates a random guess using the rand() call
void randguess(Position* pos, HitMap map) {
    int colguess, rowguess;
    rowguess = rand() % (map.rows);
    colguess = rand() % (map.cols);
    add_guess(pos, rowguess, colguess);
}

typedef struct TargetList {
    int targetrow[20];
    int targetcol[20];
    int size;
} TargetList;

//Removes an elemnent at a specified index
void remove_element(int* tgt, int index, int len) {
    for (int i = index; i < len; i++) {
        tgt[i] = tgt[i + 1];
    }
}

//Function for generating the B agent guess
void b_guess(HitMap map, TargetList* target, bool attack, Rules rules,
        bool hit, Position* pos){
    if (target->size == 1 && !hit) {
        target->size = 0;
        attack = false;
    }
    int colguess, rowguess;
    if (!attack) {
        randguess(pos, map);
        Position checkpos;
        checkpos.col = pos->col;
        checkpos.row = pos->row;
        while (1) {
            if (!(map.data[map.cols * checkpos.row + checkpos.col] == '.')) {
                randguess(pos, map);
                checkpos.col = pos->col;
                checkpos.row = pos->row;
            } else {
                break;
            }
        }
        target->size = 0;
        target->targetrow[target->size] = pos->row;
        target->targetcol[target->size] = pos->col;
        target->size++;
        return;
    }
    rowguess = target->targetrow[0], colguess = target->targetcol[0];
    remove_element(target->targetrow, 0, target->size);
    remove_element(target->targetcol, 0, target->size);
    target->size--;
    if (attack && hit) {
        Position currentPos;
        int nextrow[4] = {rowguess - 1, rowguess, rowguess + 1, rowguess};
        int nextcol[4] = {colguess, colguess + 1, colguess, colguess - 1};
        for (int i = 0; i < 4; i++) {
            currentPos.row = nextrow[i], currentPos.col = nextcol[i];
            if (position_in_bounds(rules, currentPos)) {
                if (map.data[map.cols * currentPos.row + currentPos.col]
                        == '.') {
                    target->targetrow[target->size] = currentPos.row;
                    target->targetcol[target->size] = currentPos.col;
                    target->size++;
                }
            }
        }  
        add_guess(pos, target->targetrow[0], target->targetcol[0]);
    }
    if (attack && !hit) {
        add_guess(pos, target->targetrow[0], target->targetcol[0]);
    }
}

//Checks if game is over
void game_over(int winner) {
    fprintf(stderr, "GAME OVER - player %d wins\n", winner);
    exit(0);
}

//Receives a message from the specified stream and places it into the given
//position
Messages receive_message(FILE* stream, Position* pos, int* plr) {
    int check;
    char* message;
    message = read_line(stream);
    if (message == NULL) {
        return ERR;
    }
    strtrim(message);
    check = strcmp(message, "YT");
    if (!check) {
        return YT;
    }
    check = strcmp(message, "OK");
    if (!check) {
        return OK;
    }
    check = strncmp(message, "HIT", 3);
    if (!check) {
        *plr = (message[4] - '0') - 1;
        pos->col = message[6] - 'A';
        pos->row = (message[7] - '0') - 1;
        return HIT;
    }
    check = strncmp(message, "MISS", 4);
    if (!check) {
        *plr = (message[5] - '0') - 1;
        pos->col = message[7] - 'A';
        pos->row = (message[8] - '0') - 1;
        return MISS;
    }
    check = strncmp(message, "SUNK", 4);
    if (!check) {
        *plr = (message[5] - '0') - 1;
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

//Marks the given position on the specified hitmap
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

//Checks the seed is an integer given in command line
void check_seed(char* seed) {
    for (int i = 0; i < strlen(seed) - 1; i++) {
        if (isalnum(seed[i])) {
            fprintf(stderr, "Invalid seed\n");
            exit(4);
        }
    }
}

int main(int argc, char** argv){
    if (argc != 4) {
        fprintf(stderr, "Usage: agent id map seed\n");
        exit(1);
    
    }
    check_seed(argv[3]);
    int id = atoi(argv[1]) - 1;
    int op = !id;
    srand(atoi(argv[3]));
    char* map = argv[2];
    char* test;
    GameInfo info;
    ErrorCode err;
    err = read_rules_file_b(stdin, &info.rules);
    if (err != ERR_OK) {
        err_msg(err);
    }
    test  =  read_map_file_b(map, &info.playerMap, false);
    if (err != ERR_OK) {
        printf("Uh Oh1 ");
    }
    read_map_file_b(map, &info.cpuMap, true);
    HitType hit;
    err = validate_info(info);
    if (err != ERR_OK) {
        err_msg(err); 
    } 
    fprintf(stdout, "MAP %s\n", test);
    fflush(stdout);
    GameState state = init_game(info);
    info.maps[0] = info.playerMap;
    info.maps[1] = info.cpuMap;
    print_maps_b(state.maps[0], state.maps[1]);
    bool attack = false;
    bool ishit = false;
    TargetList target;
    Position pos;
    int printcount = 0;
    while (1) {
        int plr = 2;
        Messages message = receive_message(stdin, &pos, &plr);
        if (message == GAME_OVER) {
            break;
        }
        if (message == ERR) {
            fprintf(stderr, "Communications error\n");
            exit(5);
        }
        if ((message == HIT || message == MISS || message == SHIP_SUNK) &&
                (plr == op)) {
            hit = mark_pos(pos, &state.maps[id], &info.maps[id], message);
            print_hit_message_b(hit);
            printcount++;
            fprintf(stderr, " player %d  guessed %c%d\n", (plr + 1), pos.col +
                    65, pos.row + 1);
            fflush(stderr);
        }
        if ((message == HIT || message == SHIP_SUNK) && (plr == id)) {
            attack = true;
            ishit = true;
            hit = mark_pos(pos, &state.maps[op], &info.maps[op], message);
            print_hit_message_b(hit);
            fprintf(stderr, " player %d guessed %c%d\n", plr + 1, pos.col +
                    65, pos.row + 1);
            fflush(stderr);
            printcount++;
           
        }
        if ((message == MISS) && plr == id) {
            printcount++;
            ishit = false;
            hit = mark_pos(pos, &state.maps[op], &info.maps[op], message);
            print_hit_message_b(hit);
            fprintf(stderr, " player %d guessed %c%d\n", plr + 1, pos.col +
                    65, pos.row + 1);
            fflush(stderr);
        }
        if (printcount == 2) {
            print_maps_b(state.maps[0], state.maps[1]);
            printcount = 0;
        }
        if (message == YT) {
            if (target.size == 1 && !ishit) {
                target.size = 0;
                attack = false;
            }
            if (target.size == 0) {
                attack = false;
            }
            b_guess(state.maps[op],&target, attack, info.rules, ishit,
                     &pos);
            fprintf(stdout, "GUESS %c%d\n", pos.col + 65, pos.row + 1);
            fflush(stdout);
        }       
                   
    }
    return 0;
}

