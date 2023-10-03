
#define MAX_TOKENS 1000
#define MAX_LEXEMES 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//enum for token types
typedef enum {
    skipsym = 1, identsym, numbersym, plussym, minussym,
    multsym, slashsym, ifelsym, eqlsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym, elsesym
} token_type;
//lexeme struct
typedef struct lexeme {
    token_type class;
    char lexeme[12];
    int currChar;
    int err;
} lexeme;


//Make string array called errors to print out errors to stdout
char errors[8][100] = {"Error 1: Identifier too long", "Error 2: Number too long", "Error 3: Invalid symbol", "Error 4: Invalid identifier", "Error 5: Invalid keyword", "Error 6: Invalid assignment operator", "Error 7: Invalid symbol after assignment operator", "Error 8: Invalid symbol after number"};






char currentChar;
int lineNumber = 1;
int columnNumber = 0;

// Function prototypes
lexeme *tokenizeSourceProgram(char *input);
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
        // Otherwise, prompt the user for a filename
        printf("Enter the name of the file to be compiled: ");
        scanf("%s", filename);
    }


    FILE *sourcefile = fopen(filename, "r");
    if (sourcefile == NULL) {
        printf("Error: Unable to open the source program file '%s'.\n", filename);
        return 1;
    }
    //read in the file
    while (fscanf(sourcefile, "%c", &tempstr) != EOF){
        printf("%c", tempstr);
        input[index++] = tempstr;
    }
    //null terminator for the input
    input[index] = '\0';
    fclose(sourcefile);



    lexeme *finalList = tokenizeSourceProgram(input);
    



    return 0;
}

lexeme *tokenizeSourceProgram(char *input) {
  lexeme *lexemeList = calloc(MAX_LEXEMES, sizeof(lexeme));
    int input_location = 0;
    int lexeme_index = 0;
    int error_flag = 0;
    int tempstr_location = 0;

    char *tempstr = (char*) calloc(12, sizeof(char));

    //loop through the input string
    while(input[input_location] != '\0'){
        //print lexemelist at lexemeindex
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
        //handle identifiers
        else if (isalpha(currChar)){
            while(isalnum(input[input_location]) && tempstr_location < 12){
                tempstr[tempstr_location] = input[input_location];
                tempstr_location++;
                input_location++;
            }

            input_location--;

            tempstr[tempstr_location] = '\0';

            

            if (isalnum(input[input_location + 1])){
                //finish parsing the invalid identifier
                while(isalnum(input[input_location])){
                    tempstr[tempstr_location] = input[input_location];
                    tempstr_location++;
                    input_location++;
                }
                lexemeList[lexeme_index].class = 99;
                lexemeList[lexeme_index].err = 1;
                //print the whole invalid identifier

                while(isalnum(input[input_location + 1])){
                    input_location++;
                }
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
                    strcpy(lexemeList[lexeme_index].lexeme, tempstr);
                }
                else
                {
                    lexemeList[lexeme_index].class = 99;
                    lexemeList[lexeme_index].err = 5;
                    error_flag = 1;
                }

            }
        }
        //handle numbers
        else if (isdigit(currChar)){
            int tooLong = 0;
            while(isdigit(currChar)){

                if(tempstr_location >= 5){
                    tooLong = 1;
                }
                tempstr[tempstr_location] = input[input_location];
                tempstr_location++;
                input_location++;
                currChar = input[input_location];
            }
            tempstr[tempstr_location] = '\0';
            if(tooLong){
                lexemeList[lexeme_index].class = 99;
                lexemeList[lexeme_index].err = 2;
            }
            else if(isalnum(input[input_location + 1])){
                if(isdigit(input[input_location + 1])){
                    //finish parsing the invalid number
                    
                    while(isdigit(input[input_location+1])){
                        tempstr[tempstr_location] = input[input_location];
                        tempstr_location++;
                        input_location++;
                    }
                    lexemeList[lexeme_index].class = 99;
                    lexemeList[lexeme_index].err = 2;
                }
                
                else if (isalpha(input[input_location + 1])){
                    //tokenize the digits, lower the index, rebuild the tempstr
                    char *digitString = (char*)malloc(sizeof(char) * tempstr_location + 1);
                    strcpy(digitString, tempstr);
                    lexemeList[lexeme_index].class = numbersym;
                    strcpy(lexemeList[lexeme_index].lexeme, digitString);
                    lexemeList[lexeme_index].currChar = atoi(digitString);
                    free(digitString);
                    //rebuild the tempstring
                    strcpy(tempstr, "");
                    input_location -= tempstr_location;
                    input_location++;
                    tempstr_location = 0;
                    lexeme_index++;
                    //reverse the index
                    while(isalnum(input[input_location]) && tempstr_location < 12){
                        tempstr[tempstr_location] = input[input_location];
                        tempstr_location++;
                        input_location++;
                    }
                    lexemeList[lexeme_index].class = identsym;
                    strcpy(lexemeList[lexeme_index].lexeme, tempstr);


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
                strcpy(lexemeList[lexeme_index].lexeme, digitString);
                lexemeList[lexeme_index].currChar = atoi(digitString);
                free(digitString);
            }
        }
        else
        {
            //switch statement for all the symbols
            switch (currChar){
                case '+':
                    lexemeList[lexeme_index].class = plussym;
                    lexemeList[lexeme_index].lexeme[0] = '+'; 
                    break;
                case '-':
                    lexemeList[lexeme_index].class = minussym;
                    lexemeList[lexeme_index].lexeme[0] = '-';
                    break;
                case '*':
                    lexemeList[lexeme_index].class = multsym;
                    lexemeList[lexeme_index].lexeme[0] = '*';
                    break;
                case '/':
                    lexemeList[lexeme_index].class = slashsym;
                    lexemeList[lexeme_index].lexeme[0] = '/';
                    break;
                case '=':
                    lexemeList[lexeme_index].lexeme[0] = '=';
                    if(input[input_location + 1] == '='){
                        lexemeList[lexeme_index].class = eqlsym;
                        lexemeList[lexeme_index].lexeme[1] = '=';
                        input_location++;
                    }
                    else
                    {
                        lexemeList[lexeme_index].class = eqlsym;
                        lexemeList[lexeme_index].lexeme[0] = '=';

                    }
                    break;
                case '<':
                    lexemeList[lexeme_index].lexeme[0] = '<';
                    if (input[input_location + 1] == '='){
                        lexemeList[lexeme_index].lexeme[1] = '=';
                        lexemeList[lexeme_index].class = leqsym;
                        input_location++;
                    }
                    else if (input[input_location + 1] == '>'){
                        lexemeList[lexeme_index].lexeme[1] = '>';
                        lexemeList[lexeme_index].class = neqsym;
                        input_location++;
                    }
                    else
                    {
                        lexemeList[lexeme_index].class = lessym;
                    }
                    break;
                case '>':
                    lexemeList[lexeme_index].lexeme[0] = '>';
                    if (input[input_location + 1] == '='){
                        lexemeList[lexeme_index].lexeme[1] = '=';
                        lexemeList[lexeme_index].class = geqsym;
                        input_location++;
                    }
                    else if (input[input_location + 1] == '<'){
                        lexemeList[lexeme_index].lexeme[1] = '<';
                        lexemeList[lexeme_index].class = neqsym;
                        input_location++;
                    }
                    else
                    {
                        lexemeList[lexeme_index].class = gtrsym;
                    }
                    break;
                case '(':
                    lexemeList[lexeme_index].lexeme[0] = '(';
                    lexemeList[lexeme_index].class = lparentsym;
                    break;
                case ')':
                    lexemeList[lexeme_index].lexeme[0] = ')';
                    lexemeList[lexeme_index].class = rparentsym;
                    break;
                case ',':
                    lexemeList[lexeme_index].lexeme[0] = ',';
                    lexemeList[lexeme_index].class = commasym;
                    lexemeList[lexeme_index].lexeme[0] = ',';
                    break;
                case ';':
                    lexemeList[lexeme_index].lexeme[0] = ';';
                    lexemeList[lexeme_index].class = semicolonsym;
                    break;
                case '.':
                    lexemeList[lexeme_index].lexeme[0] = '.';
                    lexemeList[lexeme_index].class = periodsym;
                    break;
                case ':':
                    lexemeList[lexeme_index].lexeme[0] = ':';
                    if (input[input_location + 1] == '='){
                        lexemeList[lexeme_index].lexeme[1] = '=';
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

 

    }
    printf("Lexeme Table\n\n");
    printf("lexeme\t\ttoken type\n");
    for(int i = 0; i < lexeme_index; i++){
        if(lexemeList[i].class == 99){
            printf("%s\n", errors[lexemeList[i].err - 1]);
        }
        else
        {
            printf("%s\t\t", lexemeList[i].lexeme);
            printf("%d\n", lexemeList[i].class);
        }
       
    }
    printf("\n\n");
    printf("Token List:\n");
    //print out each token number and also print out the lexeme if it is an identifier or number

    for(int i = 0; i < lexeme_index; i++){
        if((lexemeList[i].class == 2 || lexemeList[i].class == 3) && lexemeList[i].err == 0){
            printf("%d %s ", lexemeList[i].class, lexemeList[i].lexeme);
        }
        else if(lexemeList[i].err == 0){
            printf("%d ", lexemeList[i].class);
        }
    }
    FILE *output = fopen("output.txt", "w");
    if (output == NULL) {
        printf("Error: Unable to open the output file.\n");
        return 1;
    }
    fprintf(output,"Lexeme Table\n\n");
    fprintf(output,"lexeme\t\ttoken type\n");
    for(int i = 0; i < lexeme_index; i++){
        if(lexemeList[i].class == 99){
            fprintf(output, "%s\n", errors[lexemeList[i].err - 1]);
        }
        else
        {
            fprintf(output, "%s\t\t", lexemeList[i].lexeme);
            fprintf(output, "%d\n", lexemeList[i].class);
        }
       
    }
    fprintf(output, "\n\n");
    fprintf(output, "Token List:\n");
    //print out each token number and also print out the lexeme if it is an identifier or number

    for(int i = 0; i < lexeme_index; i++){
        if((lexemeList[i].class == 2 || lexemeList[i].class == 3) && lexemeList[i].err == 0){
            fprintf(output, "%d %s ", lexemeList[i].class, lexemeList[i].lexeme);
        }
        else if(lexemeList[i].err == 0){
            fprintf(output, "%d ", lexemeList[i].class);
        }
    }

    fclose(output);
    free(lexemeList);
    free(tempstr);
    free(input);
    return lexemeList;
   

}




//function to determine if a character is whitespace
int isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}


//function to compare words
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
	else if (strcmp(tempstr, "ifel") == 0)
		return ifelsym;
    else if (strcmp(tempstr, "else") == 0)
        return elsesym;
	else
		return identsym;
}

//function to skip comments

void skipComment(char *input, int *input_location) {
    while (input[*input_location] != '*' || input[*input_location + 1] != '/') {
        (*input_location)++;
    }
    (*input_location) += 2;
}
