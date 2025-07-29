#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 2000

char lexemes[MAX][50];
char tokens[MAX][20];
int lines[MAX];
int entryCount = 0;
int lastKeywordLine = 0;

void loadTable() {
    FILE *fp = fopen("sample_table.txt", "r");
    if (!fp) {
        printf("Could not open sample_table.txt\n");
        exit(1);
    }

    char line[100];
    fgets(line, sizeof(line), fp); // Skip header

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%s %d %s", lexemes[entryCount], &lines[entryCount], tokens[entryCount]);
        if (strcmp(tokens[entryCount], "keyword") == 0) {
            if (lines[entryCount] > lastKeywordLine)
                lastKeywordLine = lines[entryCount];
        }
        entryCount++;
    }

    fclose(fp);
}

int findInTable(char *word, const char *type) {
    for (int i = 0; i < entryCount; i++) {
        if (strcmp(lexemes[i], word) == 0 && strcmp(tokens[i], type) == 0)
            return lines[i];
    }
    return -1;
}

void appendToTable(char *word, const char *type) {
    if (findInTable(word, type) == -1) {
        int newLine = entryCount == 0 ? 1 : lines[entryCount - 1] + 1;
        FILE *fp = fopen("sample_table.txt", "a");
        fprintf(fp, "%s \t\t %d\t %s\n", word, newLine, type);
        fclose(fp);

        strcpy(lexemes[entryCount], word);
        strcpy(tokens[entryCount], type);
        lines[entryCount] = newLine;
        entryCount++;
    }
}

int main() {
    FILE *fp = fopen("inp.txt", "r");
    if (!fp) {
        printf("Cannot open inp.txt\n");
        return 1;
    }

    loadTable();

    char ch, ch1, buffer[50], operators[] = "+-*/%";
    int l = 1, i, j = 0, skipString = 0;

    printf("Lexeme: \t L.no:\t Token:");

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\"') {
            skipString = !skipString;
            continue;
        }

        if (skipString) continue;

        for (i = 0; i < 5; ++i) {
            if (ch == operators[i]) {
                printf("\n%c \t\t %d\t Arithmetic Operator", ch, l);
                break;
            }
        }

        if (i == 5) {
            if (isalnum(ch)) {
                buffer[j++] = ch;
                ch1 = fgetc(fp);
                if (!isalnum(ch1)) {
                    buffer[j] = '\0';
                    fseek(fp, -1, SEEK_CUR);

                    if (strcmp(buffer, "main") == 0) {
                        j = 0;
                        continue;
                    }

                    if (isdigit(buffer[0])) {
                        appendToTable(buffer, "number");
                        int ln = findInTable(buffer, "number");
                        printf("\n%s \t\t %d\t number, %d", buffer, l, ln);

                    } 
                    else if (findInTable(buffer, "keyword") != -1) {
                        printf("\n%s \t\t %d\t keyword", buffer, l);  // Use inp.txt line number
                    } 
                    else {
                        appendToTable(buffer, "identifier");
                        int tableLine = findInTable(buffer, "identifier");  // Sample table line no
                        printf("\n%s \t\t %d\t identifier, %d", buffer, l, tableLine);  // ✅ Final format
                    }

                    j = 0;
                } else {
                    fseek(fp, -1, SEEK_CUR);
                }
            }

            else if (ch == '<') {
                ch1 = fgetc(fp);
                if (ch1 == '=')
                    printf("\n<= \t\t %d\t Relop LE", l);
                else {
                    fseek(fp, -1, SEEK_CUR);
                    printf("\n< \t\t %d\t Relop LT", l);
                }
            }

            else if (ch == '>') {
                ch1 = fgetc(fp);
                if (ch1 == '=')
                    printf("\n>= \t\t %d\t Relop GE", l);
                else {
                    fseek(fp, -1, SEEK_CUR);
                    printf("\n> \t\t %d\t Relop GT", l);
                }
            }

            else if (ch == '=') {
                ch1 = fgetc(fp);
                if (ch1 == '=')
                    printf("\n== \t\t %d\t Relop EQ", l);
                else {
                    fseek(fp, -1, SEEK_CUR);
                    printf("\n= \t\t %d\t Assign OP", l);
                }
            }

            else if (ch == '\n') {
                l++;
            }
        }
    }

    fclose(fp);
    printf("\n");
    return 0;
}
