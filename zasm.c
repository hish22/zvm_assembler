//   /* 00 */  0, 9, 7,     /* ADD 9, 7     */
//   /* 03 */  1, 8, 4,     /* SUB 8, 4     */
//   /* 06 */  0, 5, 8,     /* ADD 5, 8     */
//   /* 09 */  6, 2, 77,    /* LDI R2, 77   */
//   /* 12 */  7, 2, 23,    /* ADR R2, 23   */
//   /* 15 */  8, 2, 5,     /* SUR R2, 5    */
//   /* 18 */  2, 3, 6,     /* MUL 3, 6     */
//   /* 21 */  3, 9, 3,     /* DIV 9, 3     */
//   /* 24 */  4, 7, 2,     /* MOD 7, 2     */
//   /* 27 */  6, 0, 20,    /* LDI R0, 20   */
//   /* 30 */  6, 1, 20,    /* LDI R1, 20   */
//   /* 33 */  11, 45, 0,   /* JMP 45       */
//   /* 36 */  6, 0, 70,    /* LDI R0, 70   */
//   /* 39 */  9, 0, 0,     /* INC R0       */
//   /* 42 */  10, 0, 0,    /* DEC R0       */
//   /* 45 */  12, 0, 1,    /* CMP R0, R1   */
//   /* 48 */  13, 36, 0,   /* JE  36       */
//   /* 51 */  14, 0, 17,   /* STI [0], 17  */
//   /* 54 */  15, 3, 0,    /* STI R3, [0]  */
//   /* 57 */  16, 22, 0,   /* PUSH 22      */
//   /* 60 */  16, 77, 0,   /* PUSH 77      */
//   /* 63 */  17, 2,  0,   /* POP R2       */
//   /* 66 */  17, 3,  0,   /* POP R3       */
//   /* 69 */  5, 0, 0,     /* STP 0         */

// Tokenizer + Parser

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char* instruction = "ADD 5,5 \n SUB 2,5 \n ADR R1, 2";
// static char* tmp_buildup = "";
// STRING: "ADD 5, 5"
// READ CHAR BY CHAR -> A D D - 5 - 5
// TOKENIZE -> [ADD_INST_TOKEN,NUMBER_OPRAND_TOKEN,NUMBER_OPRAND_TOKEN]

typedef struct TOKEN {
    char* type;
    int value;
}TOKEN_t;


#define SIZE_OF_INSTRUCTIONS 18
#define NUMBER_OF_REGISTERS 4
#define INSTRUCTION_DEFINED_SIZE 3

static char* instructions[] = {
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "STP",
    "LDI",
    "ADR",
    "SUR",
    "INC",
    "DEC",
    "JMP",
    "CMP",
    "JE",
    "STI",
    "LDM",
    "PUSH",
    "POP"
};

static char* registers[] = {
    "R0",
    "R1",
    "R2",
    "R3"
};

TOKEN_t register_tokenizer(char *id) {
    for(int i=0; i < NUMBER_OF_REGISTERS; i++) {
        if(!strcmp(registers[i],id)) {
            TOKEN_t inst = {"register",i};
            return inst;
        }
    }
    return (TOKEN_t) {NULL,0};
}

TOKEN_t identifier_tokenizer(char *id) {
    for(int i=0; i < SIZE_OF_INSTRUCTIONS; i++) {
        if(!strcmp(id,"JZ")) {
            TOKEN_t inst = {"instruction",i-1};
            return inst;
        } else if(!strcmp(instructions[i],id)) {
            TOKEN_t inst = {"instruction",i};
            return inst;
        }
    }
    return (TOKEN_t) {NULL,0};
}

TOKEN_t number_tokenizer(char *id) {
    TOKEN_t number = {"number",atoi(id)};
    return number;
}

#define REGISTER(idx) line[idx] == 'R'
#define REGISTERS_RANGE(idx) (line[idx] - '0') >= 0 && (line[idx] - '0') <= 3

#define CHAR_RANGE(idx) line[idx] >= 65 && line[idx] <= 90
#define NUM_RANGE(idx) line[idx] >= 48 && line[idx] <= 57
#define COMMA(idx) line[idx] == 44

int id_reader(int pos, char tok[], char* line) {
    int i = 0;
    while (CHAR_RANGE(pos+i)) {
        tok[i] = line[pos + i];
        i++;
    }
    tok[i] = '\0';
    return i;
}

int num_reader(int pos, char tok[], char* line) {
    int i = 0;
    while (NUM_RANGE(pos+i)) {
        tok[i] = line[pos + i];
        i++;
    }
    tok[i] = '\0';
    return i;
}

int reg_reader(int pos, char tok[], char* line) {
    int i = 1;
    tok[0] = line[pos];
    while (REGISTERS_RANGE(pos+i)) {
        tok[i] = line[pos + i];
        i++;
    }
    tok[i] = '\0';
    return i;
}

void assembler(char* line,TOKEN_t token_buffer[]) {
    int counter = 0;
    int token_counter = 0;
    char tok[256];
    while (line[counter] != '\0') {
        if(REGISTER(counter)) {
            counter += reg_reader(counter,tok,line);
            TOKEN_t rtoken = register_tokenizer(tok);
            printf("{Type: %s, Value: %d}\n",rtoken.type,rtoken.value);

            token_buffer[token_counter] = rtoken;
            token_counter++;

        } else if(CHAR_RANGE(counter)) {
            counter += id_reader(counter,tok,line);
            TOKEN_t ctoken = identifier_tokenizer(tok);
            printf("{Type: %s, Value: %d}\n",ctoken.type,ctoken.value);

            token_buffer[token_counter] = ctoken;
            token_counter++;

        } else if(NUM_RANGE(counter)) {
            counter += num_reader(counter,tok,line);
            TOKEN_t ntoken = number_tokenizer(tok);
            printf("{Type: %s, Value: %d}\n",ntoken.type,ntoken.value);
                        
            token_buffer[token_counter] = ntoken;
            token_counter++;

        } else if(COMMA(counter)) {
            tok[0] = ',';
            counter++;
            TOKEN_t COMMA_TOKEN = {"comma",','};
            printf("{Type: %s, Value: %d}\n",COMMA_TOKEN.type, COMMA_TOKEN.value);

        } else {
            counter++;
        }
        tok[0] = '\0';
    }
}

// COMMAND PARSER

#define MAX_LINE 1024

void reset_token_buffer(TOKEN_t token_buffer[3]) {
    for(int i = 0; i < INSTRUCTION_DEFINED_SIZE; i++) {
        token_buffer[i].value = 0;
        token_buffer[i].type = 0;
    }
}

// WRITE TO zx FILE
void write_zx_file(char filename[], TOKEN_t token_buffer[]) {
    FILE * zx_fh = fopen(filename,"ab");
    if(zx_fh != NULL) {
        int temp_values[INSTRUCTION_DEFINED_SIZE];
        for(int i = 0; i < INSTRUCTION_DEFINED_SIZE; i++) {
            temp_values[i] = token_buffer[i].value;
        }
        // One single write operation
        fwrite(temp_values, sizeof(int), INSTRUCTION_DEFINED_SIZE, zx_fh);
        fclose(zx_fh);
    }
}

// COLORING

void write_zx_html_file(char *zasm_filename) {
    char html_filename[256];
    char *dot = strrchr(zasm_filename, '.');
    if (dot) {
        int base_len = dot - zasm_filename;
        strncpy(html_filename, zasm_filename, base_len);
        html_filename[base_len] = '\0';
        strcat(html_filename, ".zx.html");
    } else {
        strcpy(html_filename, zasm_filename);
        strcat(html_filename, ".zx.html");
    }

    FILE *src = fopen(zasm_filename, "r");
    if (!src) return;
    FILE *html_fh = fopen(html_filename, "w");
    if (!html_fh) { fclose(src); return; }

    fprintf(html_fh,
        "<!DOCTYPE html>\n<html>\n<head>\n"
        "<meta charset=\"UTF-8\">\n<style>\n"
        "body { background:#1e1e1e; color:#d4d4d4; font-family:monospace; font-size:14px; white-space:pre; padding:16px; }\n"
        ".instruction { color:#569cd6; }\n"
        ".register { color:#4ec9b0; }\n"
        ".number { color:#b5cea8; }\n"
        ".comma { color:#d4d4d4; }\n"
        "</style>\n</head>\n<body>\n");

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), src)) {
        int i = 0;
        while (line[i] != '\0') {
            if (REGISTER(i)) {
                char tok[256];
                int len = reg_reader(i, tok, line);
                TOKEN_t t = register_tokenizer(tok);
                fprintf(html_fh, "<span class=\"%s\">%s</span>", t.type, tok);
                i += len;
            } else if (CHAR_RANGE(i)) {
                char tok[256];
                int len = id_reader(i, tok, line);
                TOKEN_t t = identifier_tokenizer(tok);
                fprintf(html_fh, "<span class=\"%s\">%s</span>", t.type, tok);
                i += len;
            } else if (NUM_RANGE(i)) {
                char tok[256];
                int len = num_reader(i, tok, line);
                fprintf(html_fh, "<span class=\"number\">%s</span>", tok);
                i += len;
            } else if (COMMA(i)) {
                fprintf(html_fh, "<span class=\"comma\">,</span>");
                i++;
            } else {
                fputc(line[i], html_fh);
                i++;
            }
        }
    }

    fprintf(html_fh, "</body>\n</html>\n");
    fclose(html_fh);
    fclose(src);
}

void cmd_parser(int argv, char **args) {
    if(argv < 2) {
        printf("Error: No command provided.\n");
        return;
    }

    char *flag = args[1];
    char *zasm_name = args[2];

    if(flag[0] == '-') {
        switch (flag[1]) {
        case 'a':
            FILE *fh = fopen(zasm_name,"r");
            if(fh != NULL) {
                char inst[MAX_LINE];
                TOKEN_t token_buffer[3] = {0};
                char original_zasm[256];
                strcpy(original_zasm, zasm_name);
                char *new_filename = strtok(zasm_name,".");
                strcat(new_filename,".zx");
                if (!remove(new_filename)) {
                    printf("%s %s\n",new_filename,"Removed");
                }
                while (fgets(inst, MAX_LINE, fh) != NULL) {
                    assembler(inst,token_buffer);
                    write_zx_file(new_filename,token_buffer);
                    reset_token_buffer(token_buffer);
                }
                fclose(fh);
                write_zx_html_file(original_zasm);
            }
            break;

        default:
            printf("%s","No such Command");
            break;
        }
    }

}

int main(int argv, char **args) {
    cmd_parser(argv,args);
    return 0;
}