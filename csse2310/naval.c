#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>





/* Struct to store gamestate of both players */
typedef struct GameBoard {
    int rows;
    int columns;
    char** board;
} GameBoard;

/*Function to parse the CPU turns file from a given file name. Utilises realloc
 * as the number of CPU turns is unknown*/
char* parse_turns(char filename[256]){
    char* str;
    FILE* turns;
    str = (char*)malloc(2 * sizeof(char));
    turns = fopen(filename, "r");
    char c;
    int bufferLine = 0;
    int i = 0;
    int mem = 1;
    if (turns == 0) { 
        fprintf(stderr, "Missing CPU turns file\n");
        exit(40);
    }
    while ((c = fgetc(turns)) != EOF) { 

        if (c == '#') {
            bufferLine = 1;
            continue;
        }
        
        if (c == ' ') {
            continue;
        }
        if (c == '\n') {
            if (bufferLine == 1) {
                bufferLine = 0;
                continue;
            }
            str = (char*)realloc(str, sizeof(char) + 2 * mem);
            str[i++] = ' ';
            mem++;
            continue;
        }
        if (bufferLine == 0) {
            str = (char*)realloc(str, sizeof(char) + 2 * mem);
            str[i] = c;
            i++;
            mem++;
        }
    }   
    return str;
}

/*Function to parse the map files from a given filename, uses dynamic 
 * allocation for an array of map locations in which the funciton returns a 
 * pointer to*/
char* parse_map(char filename[256], int numShips, int num, int bounds){
    char* locations;
    char c;
    int bufferLine = 0, counter = bufferLine, size = counter, i = size;
    if (1 <= bounds && bounds <= 9) {
        size = 4;
    } else {
        size = 5;
    }
    locations = (char*)malloc(sizeof(char) * numShips + 1);
    FILE* map = fopen(filename, "r");
    if (map == 0 && num == 0) {
        fprintf(stderr, "Missing player map file\n");
        exit(30);
    }
    if (map == 0 && num == 1) {
        fprintf(stderr, "Missing CPU map file\n");
        exit(31);
    } else {
        while ((c = fgetc(map)) != EOF) {
            if (c == '#') {
                bufferLine = 1;
                continue;
            }
            if (c == ' ') {
                counter++;
                continue;
            }
            if (c == '\n') {
                bufferLine = 0;
                if (counter > size) {
                    if (num == 0) {
                        fprintf(stderr, "Error in player map file\n");
                        exit(100);
                    }
                    if (num == 1) {
                        fprintf(stderr, "Error in CPU map file\n");
                        exit(110);
                    }
                }
                counter = 0;
                continue;
            }
            if (bufferLine == 0) {
                locations[i++] = c;
                counter++;
            }
        }
    }
    return locations;
}
                                  
/*Function to parse the game rules, again using malloc in order for the 
 * function to return a pointer to the dynamically allocated integer array*/
int* parse_rules(char filename[256]){
    FILE* rulesFile;
    int* rules;
    int i = 0;
    int numError = 0;
    int c;
    int bufferLine = 0;
    rules = (int*)malloc(sizeof(int) * 34);
    rulesFile = fopen(filename, "r");
    if (rulesFile == 0) {
        fprintf(stderr, "Missing rules file\n");
        exit(20);
    } else { 
        while ((c = fgetc(rulesFile)) != EOF) {
            if (c == '#') {
                bufferLine = 1;
                continue;
            }
            if (c == ' ') {
                continue;
            }
            if (c == '\n') {
                bufferLine = 0;
                continue;
            }
            if (!isdigit(c)) {
                numError = 1; //expecting an integer
                continue;
            }
            if (bufferLine == 0) {
                rules[i] = c - '0'; //convert the current char to an integer
            }

            i++;
        }    
    }
    if (numError == 1) {
        fprintf(stderr, "Error in rules file\n");
        exit(50);
    }
    
    return rules;
}

/*Short function to reduce repeated code, if an overlap occurs, it returns
 * which board it appears on*/
int check_overlap(int turn){
    if (turn == 1) {
        return 1;
    }
    return 2;
          
}

/*Function to reduce repeated code as well as to keep functions under 50 lines.
 * Checks if the current ship segment is within the bounds of the board*/
int check_ship_bounds(int bound, int pos, int turn){
    
    if (pos < 0 || pos > bound - 1) {
        
        if (turn == 1) {
            return 3;
        } else {
            return 4;
        }
    }    

    return 0;
      
}

/*Function to check if there is an overlap bu checking if there is already an
 * integer at the current position*/
int check_valid(GameBoard gboard, int row, int col, int erro, int turn, int
        shipNum){
    if (isdigit(gboard.board[row][col])) {
        erro = check_overlap(turn);
    }
    gboard.board[row][col] = shipNum + '0';
    return erro;
}

/*Function to place the ships on the given game board, using the map file 
 * given in the command line
 * Some of the variable assignments in the following function are quite messy,
 * however, it was the only way to get the function working and under 50 lines.
 * Whilst it may look like there is repeated code, each conditional statement
 * has slightly different error checking and placement, which is important
 * to the functionailty of the program*/
int place_ships(char* positions, GameBoard gboard, int* shipOrder, int turn){
    char pos;
    int colCount = 0, rowCount = 1, posCount = 2, lengthCount = 3;
    int erro = 0, errb = erro, shipNum = 1, col, row;
    for (int i = 0; i < shipOrder[2]; i++) {
        col = positions[colCount] - 65, row = positions[rowCount] - 49;
        pos = positions[posCount];
        if (pos == 'S') { 
            for (int j = 0; j < shipOrder[lengthCount]; j++) {
                errb = check_ship_bounds(gboard.rows, row, turn);
                if (errb) { 
                    return errb;
                }
                erro = check_valid(gboard, row, col, erro, turn, shipNum);
                row++;
            }
        }
        if (pos == 'W') {
            for (int j = 0; j < shipOrder[lengthCount]; j++) {
                errb = check_ship_bounds(gboard.rows, col, turn);
                if (errb) {
                    return errb;
                } 
                erro = check_valid(gboard, row, col, erro, turn, shipNum);
                col--;
            }
        }
        if (pos == 'N') {
            for (int j = 0; j < shipOrder[lengthCount]; j++) {
                errb = check_ship_bounds(gboard.rows, row, turn);
                if (errb) {
                    return errb;
                }  
                erro = check_valid(gboard, row, col, erro, turn, shipNum);
                row--;
            }
        }
        if (pos == 'E') {
            for (int j = 0; j < shipOrder[lengthCount]; j++) {
                errb = check_ship_bounds(gboard.rows, col, turn);
                if (errb) {
                    return errb;
                } 
                erro = check_valid(gboard, row, col, erro, turn, shipNum);
                col++;
            }
        }
        colCount += 3, rowCount += 3, posCount += 3, shipNum++, lengthCount++;
    }
    return erro;
}

/*Function to open stdin and parse the user input.  Returns a pointer to a 
 * dynamically allocated char array*/
char* get_user_input(){
    char str[100];
    int count = 0;
    char* input;
    int err;
    input = (char*)malloc(sizeof(char) * 3);
    while (1) {
        printf("(Your move)>");
        if (fgets(str, sizeof(str), stdin)) {
            for (int i = 0; str[i]; i++) {
                if (str[i] != ' ') {
                    str[count++] = str[i];
                }   
            }
            str[count] = '\0';
            break;
        }
        if (feof(stdin)) {
            fprintf(stderr, "Bad guess\n");
            input[0] = '2';
            return input;
        }
    }
    
    if (str[0] == '\n' || str[1] == '\0') { 
        err = 1;
    }
    for (int i = 0; i < strlen(str) - 1; i++) {
        if (!isalnum(str[i])) {
            err = 1;
        }
    }
    input = str;
    if (err == 1) {
        printf("Bad guess\n");
        input[0] = '1';
    }
    return input;
}

/*Steps through the given CPU guesses, and returning a pointer to the current
 * guess, if valid*/
char* get_cpu_input(char* guesses, int num){
    int i = 0;
    char* str = (char*)malloc(sizeof(char) * 6);
    while (1) {
        if (guesses[num] == ' ' || guesses[num] == '\0') {
            break;
        }
        str[i] = guesses[num];
        i++;
        num++;
    }
    str[i] = '\0';
    if (str[0] == '\0') {
        printf("(CPU move)>"); //No turns left, dont print a new line
    } else {
        printf("(CPU move)>%s\n", str);
    }
    return str;
}

/*Iterates through the board to check if there are any ships of the given 
 * number left*/
int check_sunk(GameBoard gboard, int shipNum){
    int counter = 0;
    for (int i = 0; i < gboard.rows; i++) {
        for (int j = 0; j < gboard.columns; j++) {
            if (gboard.board[i][j] == (char) shipNum) {
                ++counter;
            }
        }
    }
    return counter;
}

/*Function to check if the guess passed in is valid, and if so, update the 
 * board accordingly - whether it was a hit or a miss.  if the guess is 'bad',
 * function will print "Bad guess" and wait for another response.  If the 
 * response is invalid, the function will throw the appropriate error*/
int check_hit(GameBoard gboard, char* guess, int turn){
    int length;
    if (1 <= gboard.rows && gboard.rows <= 9) {
        length = 2;
    } else {
        length = 3;
    }
    if (turn) {
        if (strlen(guess) - 1 != length) {
            printf("Bad guess\n");
            return 1;
        }
    } else {
        if (strlen(guess) != length) {
            printf("Bad guess\n");
            return 1;
        }
    }
    int col = guess[0] - 65;
    int row = guess[1] - 49;
    int sunk;
    if (row > gboard.rows - 1 || col > gboard.columns - 1) {
        printf("Bad guess\n");
        return 1;
    }
    int a = gboard.board[row][col];
    if (gboard.board[row][col] == '/' || gboard.board[row][col] == '*') {
        printf("Repeated Guess\n");
        return 1;
    }   
    if (isdigit(a)) {
        printf("Hit\n");
        gboard.board[row][col] = '*';
        sunk = check_sunk(gboard, a);
        if (!sunk) {
            printf("Ship sunk\n");
        }
        return 0;
    } else {
        printf("Miss\n");
        if (turn) {
            gboard.board[row][col] = '/';
        }
        return 0;
    }
}

/*Function to iterate through the board and check if there are any remaining
 * alpha-numeric characters.  If there is not, then the game is over and the
 * function will return 0*/
int check_game_over(GameBoard gboard) {
    for (int i = 0; i < gboard.rows; i++) {
        for (int j = 0; j < gboard.columns; j++) {
            if (isalnum(gboard.board[i][j])) {
                return 0;
            }
        }
    }
    return 1;
}

/*Allocates memory for the array in the given GameBoard struct according to 
 * the board size specified in the rules file*/
GameBoard init_board(int boardLength1, int boardWidth1) {
    GameBoard playerBoard;
    playerBoard.rows = boardLength1;
    playerBoard.columns = boardWidth1;
    int boardLength = boardLength1;
    int boardWidth = boardWidth1;
    playerBoard.board = (char**)malloc(boardLength * sizeof(char*));
    for (int i = 0; i < boardLength; i++) {
        playerBoard.board[i] = (char*)malloc(boardWidth * sizeof(char));
    }
    for (int i = 0; i < boardLength; i++) {
        for (int j = 0; j < boardWidth; j++) {
            playerBoard.board[i][j] = '.';
        }
    }
    return playerBoard;
}

/*Iterates through the arrays in the player board and computer board, and
 * prints out the board accordingly, hiding the ships on the CPU board*/
void print_board(GameBoard playerBoard, GameBoard computerBoard){
    int boardLength = playerBoard.rows;
    int boardWidth = playerBoard.columns;
    int boardNum = 1;
    char header[boardWidth + 1];
    for (int j = 0; j < boardWidth; j++) {
        header[j] = j + 65;
    }
    header[boardWidth] = 0;
    printf("   %s\n", header);
    for (int i = 0; i < boardLength; i++) {
        if (boardNum < 10) {        
            printf(" %d ", boardNum);
        } else {
            printf("%d ", boardNum);
        }
        boardNum++;
        for (int j = 0; j < boardWidth; j++) {
            if (isalnum(computerBoard.board[i][j])) {
                printf(".");
            } else {
                printf("%c", computerBoard.board[i][j]);
            }
        }
        printf("\n");
    }
    printf("===\n");
    boardNum = 1;
    printf("   %s\n", header);
    for (int i = 0; i < boardLength; i++) {
        if (boardNum < 10) {        
            printf(" %d ", boardNum);
        } else {
            printf("%d ", boardNum);
        }
        boardNum++;            
        for (int j = 0; j < boardWidth; j++) {
            printf("%c", playerBoard.board[i][j]);
        }
        printf("\n");
    }
}

/*Error checking function, returns error according to passed integer. Switch
 * statement also suitable, however, there is not a significant amount of 
 * conditions*/
void check_bounds(int num){
    if (num == 1) {
        fprintf(stderr, "Overlap in player map file\n");
        exit(60);
    }
    if (num == 2) { 
        fprintf(stderr, "Overlap in CPU map file\n");
        exit(70);
    }
    if (num == 3) {
        fprintf(stderr, "Out of bounds in player map file\n");
        exit(80);
    }
    if (num == 4) {
        fprintf(stderr, "Out of bounds in CPU map file\n");
        exit(90);
    }
}

/*Function that handles gameplay, will loop forever unless an error is thrown
 * or the gameOver variable changes, returning nothing, therefore prompting 
 * main() to return 0*/
void play_game(GameBoard g1, GameBoard g2, char* cpuguess){
    int gameOver = 0;
    int playerTurn = 1;
    int cpuTurn = 0;
    int userInput = 0;
    int x = 0;
    while (!gameOver) {
        while (playerTurn) {
            char* usr = get_user_input();
            if (usr[0] == '1') {
                continue;
            }
            if (usr[0] == '2') {
                userInput = 1;

            }      
            if (userInput == 1) {
                gameOver = 1;
                exit(130);
                break;
            }
            playerTurn = check_hit(g2, usr, 1);

            if (check_game_over(g2)) {
                gameOver = 1;
                printf("Game over - you win\n");
                exit(0);
            }
        }
        cpuTurn = 1;
        while (cpuTurn) {
            char* cpu = get_cpu_input(cpuguess, x);
            if (cpu[0] == '\0' || cpu == NULL) {
             //   printf("\n");
                fprintf(stderr, "CPU player gives up\n");
                exit(140);
            }
            cpuTurn = check_hit(g1, cpu, 0);
            x += strlen(cpu) + 1;
            if (check_game_over(g1)) {
                gameOver = 1;
                printf("Game over - you lose\n");
                exit(0);
            }
        }
        playerTurn = 1;
        print_board(g1, g2);
    }
}

/*Main function to handle arguments error, as well as initialise everything,
 * and execute play_game*/
int main(int argc, char** argv){
    if (argc != 5) {
        fprintf(stderr, "Usage: naval rules playermap cpumap turns\n");
        exit(10);
    }
    int* rules;
    rules = parse_rules(argv[1]);
    int boardLength = rules[0];
    int boardWidth = rules[1];
    GameBoard g1, g2;
    g1 = init_board(boardLength, boardWidth);
    g2 = init_board(boardLength, boardWidth);
    char* playermap;
    char* cpumap;
    char* cpuguess;
    playermap = parse_map(argv[2], rules[2], 0, boardLength);
    cpumap = parse_map(argv[3], rules[2], 1, boardLength);
    cpuguess = parse_turns(argv[4]);
    int checkPlayerBounds = place_ships(playermap, g1, rules, 1);
    int checkCPUBounds = place_ships(cpumap, g2, rules, 0);
    check_bounds(checkPlayerBounds);
    check_bounds(checkCPUBounds);    
    print_board(g1, g2);
    play_game(g1, g2, cpuguess);
    return 0;
}
        
