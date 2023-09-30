
#define MAX_TOKENS 1000
#define MAX_LEXEMES 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    skipsym = 1, identsym, numbersym, plussym, minussym,
    multsym, slashsym, ifelsym, eqlsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym
} token_type;

typedef struct lexeme {
    token_type class;
    char lexeme[12];
    int currChar;
    int err;
} lexeme;






char currentChar;
int lineNumber = 1;
int columnNumber = 0;

// Function prototypes
lexeme *tokenizeSourceProgram(int flag, char *input);
int isWhitespace(char c);
int word_compare(char tempstr[]);
void skipComment(char *input, int *input_location);

int main(int argc, char *argv[]) {
    char *filename;
    char *input = calloc(MAX_LEXEMES, sizeof(char));
    char tempstr;
    int index = 0;

    if(argc == 2) {
        // If an argument is provided, use it as the filename
        filename = argv[1];
    } else {
        // If no argument is provided, use a default filename
        filename = "test_case_03.txt";
    }


    FILE *sourcefile = fopen(filename, "r");
    if (sourcefile == NULL) {
        printf("Error: Unable to open the source program file '%s'.\n", filename);
        return 1;
    }

    while (fscanf(sourcefile, "%c", &tempstr) != EOF){
        input[index++] = tempstr;
    }
    input[index] = '\0';
    fclose(sourcefile);



    // Call the function to tokenize the source program
    int flag = 0;
    lexeme *finalList = tokenizeSourceProgram(flag, input);
    //TO DO ! TO DO ! TO DO! TO DO!
    //Print the lexeme and tokenType in columns to the console and .txt file
    FILE *output = fopen("output.txt", "w");
    if (output == NULL) {
        printf("Error: Unable to open the output file.\n");
        return 1;
    }
    
    printf("\n");
    fprintf(output, "\n");
    fclose(output);



    return 0;
}

lexeme *tokenizeSourceProgram(int flag, char *input) {
  lexeme *lexemeList = calloc(MAX_LEXEMES, sizeof(lexeme));
    int input_location = 0;
    int lexeme_index = 0;
    int error_flag = 0;
    int tempstr_location = 0;

    char *tempstr = (char*) calloc(12, sizeof(char));

    while(input[input_location] != '\0'){
        int currChar = input[input_location];

        strcpy(tempstr, "");
        tempstr_location = 0;

        if(isWhitespace(currChar) || iscntrl(currChar)){
            input_location++;
            continue;
        }
        // handle comments
        else if(currChar == '/' && input[input_location + 1] == '*'){
            skipComment(input, &input_location);
            continue;
        }
        else if (isalpha(currChar)){
            while(isalnum(input[input_location]) && tempstr_location < 12){
                tempstr[tempstr_location] = input[input_location];
                tempstr_location++;
                input_location++;
            }

            input_location--;

            tempstr[tempstr_location] = '\0';

            if (isalnum(input[input_location + 1])){
                lexemeList[lexeme_index].class = 99;
                lexemeList[lexeme_index].err = 1;

                while(isalnum(input[input_location + 1])){
                    input_location++;
                }

                input_location--;
            }
            else
            {
                int keyword = word_compare(tempstr);

                if(keyword == identsym){
                    lexemeList[lexeme_index].class = identsym;
                    strcpy(lexemeList[lexeme_index].lexeme, tempstr);
                }
                else if (keyword != -1){
                    lexemeList[lexeme_index].class = keyword;
                }
                else
                {
                    lexemeList[lexeme_index].class = 99;
                    lexemeList[lexeme_index].err = 5;
                    error_flag = 1;
                }
            }
        }
        else if (isdigit(currChar)){
            while(isdigit(currChar) && tempstr_location < 5){
                tempstr[tempstr_location] = input[input_location];
                tempstr_location++;
                input_location++;
            }

            input_location--;
            tempstr[tempstr_location] = '\0';

            if(isalnum(input[input_location + 1])){
                if(isdigit(input[input_location + 1])){
                    lexemeList[lexeme_index].class = 99;
                    lexemeList[lexeme_index].err = 2;
                }
                else if (isalpha(input[input_location + 1])){
                    lexemeList[lexeme_index].class = 99;
                    lexemeList[lexeme_index].err = 3;
                }

                while(isalnum(input[input_location + 1])){
                    input_location++;
                }

                input_location--;

            }
            else
            {
                char *digitString = (char*)malloc(sizeof(char) * tempstr_location + 1);
                strcpy(digitString, tempstr);

                lexemeList[lexeme_index].class = numbersym;
                lexemeList[lexeme_index].currChar = atoi(digitString);

                free(digitString);

            }
        }
        else
        {
            switch (currChar){
                case '+':
                    lexemeList[lexeme_index].class = plussym;
                    break;
                case '-':
                    lexemeList[lexeme_index].class = minussym;
                    break;
                case '*':
                    lexemeList[lexeme_index].class = multsym;
                    break;
                case '/':
                    lexemeList[lexeme_index].class = slashsym;
                    break;
                case '=':
                    if(input[input_location + 1] == '='){
                        lexemeList[lexeme_index].class = eqlsym;
                        input_location++;
                    }
                    else
                    {
                        lexemeList[lexeme_index].class = 99;
                        lexemeList[lexeme_index].err = 4;
                        error_flag = 1;
                    }
                    break;
                case '<':
                    if (input[input_location + 1] == '='){
                        lexemeList[lexeme_index].class = leqsym;
                        input_location++;
                    }
                    else if (input[input_location + 1] == '>'){
                        lexemeList[lexeme_index].class = neqsym;
                        input_location++;
                    }
                    else
                    {
                        lexemeList[lexeme_index].class = lessym;
                    }
                    break;
                case '>':
                    if (input[input_location + 1] == '='){
                        lexemeList[lexeme_index].class = geqsym;
                        input_location++;
                    }
                    else if (input[input_location + 1] == '<'){
                        lexemeList[lexeme_index].class = neqsym;
                        input_location++;
                    }
                    else
                    {
                        lexemeList[lexeme_index].class = gtrsym;
                    }
                    break;
                case '(':
                    lexemeList[lexeme_index].class = lparentsym;
                    break;
                case ')':
                    lexemeList[lexeme_index].class = rparentsym;
                    break;
                case ',':
                    lexemeList[lexeme_index].class = commasym;
                    break;
                case ';':
                    lexemeList[lexeme_index].class = semicolonsym;
                    break;
                case '.':
                    lexemeList[lexeme_index].class = periodsym;
                    break;
                case ':':
                    if (input[input_location + 1] == '='){
                        lexemeList[lexeme_index].class = becomessym;
                        input_location++;
                    }
                    else
                    {
                        lexemeList[lexeme_index].class = 99;
                        lexemeList[lexeme_index].err = 6;
                        error_flag = 1;
                    }
                    break;
                default:
                    lexemeList[lexeme_index].class = 99;
                    lexemeList[lexeme_index].err = 7;
                    error_flag = 1;
                    break;
            }
        }
        input_location++;
        lexeme_index++;

        if(flag && !error_flag){
            // PRINT OUT
            return lexemeList;
        }
        else if (flag && error_flag){
            // PRINT OUT ERROR
            return lexemeList;
        }
        else if (!flag && !error_flag)
        {
            return lexemeList;
        }
        else
            return NULL;

    }
}





int isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}



int word_compare(char tempstr[])
{
	if (strcmp(tempstr, "const") == 0)
		return constsym;
	else if (strcmp(tempstr, "var") == 0)
		return varsym;
	else if (strcmp(tempstr, "procedure") == 0)
		return procsym;
	else if (strcmp(tempstr, "call") == 0)
		return callsym;
	else if (strcmp(tempstr, "begin") == 0)
		return beginsym;
	else if (strcmp(tempstr, "end") == 0)
		return endsym;
	else if (strcmp(tempstr, "if") == 0)
		return ifsym;
	else if (strcmp(tempstr, "then") == 0)
		return thensym;
	else if (strcmp(tempstr, "while") == 0)
		return whilesym;
	else if (strcmp(tempstr, "do") == 0)
		return dosym;
	else if (strcmp(tempstr, "read") == 0)
		return readsym;
	else if (strcmp(tempstr, "write") == 0)
		return writesym;
	else if (strcmp(tempstr, "def") == 0)
		return ;
	else if (strcmp(tempstr, "return") == 0)
		return ;
	else if (strcmp(tempstr, "main") == 0)
		return -1;
	else if (strcmp(tempstr, "null") == 0)
		return -1;
	else
		return identsym;
}

void skipComment(char *input, int *input_location) {
    while (input[*input_location] != '*' || input[*input_location + 1] != '/') {
        (*input_location)++;
    }
    (*input_location) += 2;
}
