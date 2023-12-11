#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<stdio.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

enum FPTR_TYPE{
    REGULAR_FILE,
    STDIN
};
long int countNumberOfBytes(FILE* fptr, enum FPTR_TYPE fptrType){
    if(fptrType == STDIN){
        long int numberOfBytes = 0;
        for(char c = getc(fptr); c != EOF; c = getc(fptr)){
            numberOfBytes++;
        }
        return numberOfBytes;
    }
    //simply move fptr to end of file and read its position
    if(!fseek(fptr, 0, SEEK_END)){
        return ftell(fptr);  
    }
    return -1;
}

long countNumberOfLines(FILE* fptr){
    long lineCount = 0;

    //read the number of \n characters until we reach EOF
    for(char c = fgetc(fptr); !feof(fptr); c = fgetc(fptr)){
        if(c == '\n'){
            lineCount++;
        }
    }
    return lineCount;
}


long countNumberOfWords(FILE* fptr){
    long wordCount = 1;
    char c = fgetc(fptr);

    //initial check for empty file
    if(feof(fptr)){
        return 0;
    }else if (isspace(c)){ //word count hasn't begun yet if first char is space char
        wordCount = 0;
    }

    for(; !feof(fptr); c = fgetc(fptr)){
        if(isspace(c) != 0){

        //"peek" at the next character
            char next_char = fgetc(fptr);
            if(feof(fptr)){
                break;
            }

            //only increase word count when the next char is
            //not a punctuation type character
            if(!isspace(next_char)){
                wordCount++;
            }

            //make sure to decrement the file pointer back
            ungetc(next_char,fptr);
        }
    }
    return wordCount;
}
long countNumberOfCharacters(FILE* fptr){
    long numChars = 0;

    for(char c = fgetc(fptr); !feof(fptr); c = fgetc(fptr)){
        numChars++;
    }
    return numChars;
}

//essentially just iterates through the file and performs multiple arguments at once
void countBytesLinesAndWords(FILE* fptr){
    long wordCount = 1;
    long lineCount = 0;
    long int numberOfBytes = 0;
    char c = fgetc(fptr);
    if(feof(fptr)){
        wordCount = 0;
    }
    for(; !feof(fptr); c = fgetc(fptr)){
        numberOfBytes++;
        if(c == '\n'){
            lineCount++;
        }
        if(isspace(c) != 0){
            char next_char = fgetc(fptr);
            if(feof(fptr)){
                break;
            }

            if(!isspace(next_char)){
                wordCount++;
            }

            ungetc(next_char,fptr);
        }
    }
    printf("%lu %lu %ld\n", lineCount, wordCount, numberOfBytes);
}

int main(int argc, char *argv[]){
    if(argc > 3){
        printf("ERROR: TOO MANY ARGUMENTS\n");
        return 0;
    }

    FILE *fptr;

    if(argc == 1){
      
        //I can't think of a workaround at the moment. Some code will be duplicated
        //but as of right now I can't figure out a way to reset the fptr that corresponds
        //to stdin.
        fptr = stdin;
        countBytesLinesAndWords(fptr);
        return 0;
    }

    char* argv1 = argv[1];
    //no argument was provided but a file presumably was
    if(argc == 2 && 
    strcmp(argv1, "-c") && 
    strcmp(argv1, "-l") && 
    strcmp(argv1, "-w") && 
    strcmp(argv1, "-m")){
    
        fptr = fopen(argv1, "r");
        if(!fptr){
            printf("ERROR: COULD NOT READ FILE %s\n", argv[2]);
            return 0;
        }

        
        long int numberOfBytes = countNumberOfBytes(fptr, REGULAR_FILE);
        //need to reset file pointer after each subsequent function call
        if(fseek(fptr, 0, SEEK_SET)){
            printf("ERROR: COULD NOT RESET FILE POINTER");
        }
        long numberOfLines = countNumberOfLines(fptr);
        if(fseek(fptr, 0, SEEK_SET)){
            printf("ERROR: COULD NOT RESET FILE POINTER");
        }
        long numberOfWords = countNumberOfWords(fptr);

        if(numberOfBytes == -1){
            printf("ERRROR: COULD NOT READ NUMBER OF BYTES FROM %s", argv1);
            return 0;
        }

        printf("%ld %lu %lu %s\n", numberOfLines, numberOfWords, numberOfBytes, argv1);
        return 0;
    }

    //if an argument was presumably provided but no file
    if(argc == 2 && 
        (!strcmp(argv1, "-c") || 
        !strcmp(argv1, "-w") || 
        !strcmp(argv1, "-l") || 
        !strcmp(argv1, "-m"))){
        fptr = stdin;
        char* argument = argv1;
        if(!strcmp(argument, "-c")){
            long int numberOfBytes = countNumberOfBytes(fptr, STDIN);
            printf("%ld\n", numberOfBytes);
        }else if(!strcmp(argument, "-l")){
            printf("%lu\n", countNumberOfLines(fptr));
        }else if(!strcmp(argument, "-w")){
            printf("%lu\n", countNumberOfWords(fptr));
        }

        return 0;
    }else if (argc == 1){

    }
    char* fileName = argv[2];
    char* argument = argv[1];
    fptr = fopen(fileName, "r");

    //an argument AND a file were provided
    if(!strcmp(argument, "-c")){
        long int numberOfBytes = countNumberOfBytes(fptr, REGULAR_FILE);
        if(numberOfBytes == -1){
            fclose(fptr);
            printf("ERRROR: COULD NOT READ NUMBER OF BYTES FROM %s", fileName);
            return 0;
        }
        printf("%ld %s\n", numberOfBytes, fileName);
    }
    else if(!strcmp(argument, "-l")){
        printf("%lu %s\n", countNumberOfLines(fptr), fileName);
    }
    else if(!strcmp(argument, "-w")){
        printf("%lu %s\n", countNumberOfWords(fptr), fileName);
    }
    else if(!strcmp(argument, "-m")){
        printf("%lu %s\n", countNumberOfCharacters(fptr), fileName);
    }

    if(fptr != stdin){
        fclose(fptr);
        return 0;
    }


    printf("ERROR: COULD NOT PARSE COMMAND");
    return 0;
}