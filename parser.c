#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h" // include header file for the parser functions
#include <ctype.h> // for tolower function


#define LINE_LEN 256 // max line length
#define WORD_LEN 50 // max word length


struct words *lexicon = NULL; // pointer to store words from vader_lexicon.txt
int word_count = 0; // number of words read


// function to parse the vader_lexicon file
void parse_lexicon(const char *filename) { // making it const ensures that
                                        // the function doens't modify the
                                        // the content of the filename string
    FILE *vader_file = fopen(filename, "r"); // open the file in read mode
    if (vader_file == NULL) { // check if the opening of the file was successful
        printf("There is an error in opening the file.\n");
        exit(1);
    }

    // initialize variables for lexicon and word count, and set initial capacity
    int capacity = 10; // initial capacity of the lexicon array
    // "initial capacity" refers to the starting size of the dynamically
    // allocated memory block for the 'lexicon' array
    // it's the amount of memory allocated initially to hold a certain number 
    // of words from the lexicon file

    // allocate memory for lexicon based on initial capacity
    lexicon = malloc(capacity * sizeof(struct words)); 
    // dynamic allocation allows program to handle lexicons of varying sizes 
    // without wasting memory
    // size of memory allocated is determined by multiply the capacity by
    // the size of the 'stuct words'

    // the program starts with this initial capacity, and if 
    // more space is needed due to additional words, it 
    // dynamically reallocates more memory

    // if lexicon points to a null memory address (mem location
    // that doesn't contain any valid data or object)
    if (lexicon == NULL) { // check if memory allocation was successful
        perror("Memory allocation failed");
        // 'perror' is a standard library function that prints a descriptive error
        // message; used to print an error message if mem allocation fails
        fclose(vader_file);
        exit(1);
    }

    char line_buffer[LINE_LEN]; // buffer to store each line read from file
    // while loop: read lines from the file until the end is reached
    while(fgets(line_buffer, sizeof(line_buffer), vader_file)) {
        // 'fgets' is a standard library function used to read a line from a 
        // file stream (like 'vader_file') and stores it in the specified 
        // buffer ('line_buffer')

        char word_buffer[WORD_LEN]; // buffer to store word
        // "buffer" meaning -> this serves as a temporary storage space for
        // words read from the lexicon file before they're copied into the
        // lexicon array

        float score, SD; // variables to store sentiment score and standard deviation
        int SIS_array[10]; // array to store sentiment intensity scores
        
        // read word, score, SD, and SIS_array from each line in the file
        if (sscanf(line_buffer, "%s %f %f [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",
                    word_buffer, &score, &SD,
                    &SIS_array[0], &SIS_array[1], &SIS_array[2], &SIS_array[3], &SIS_array[4],
                    &SIS_array[5], &SIS_array[6], &SIS_array[7], &SIS_array[8], &SIS_array[9]) == 13) {
            // 'sscanf' is a standard library function that reads formatted
            // input from a string ('line_buffer') and stores the parsed values
            // into specified variables ('word_buffer', 'score', etc)

            // the == 13 in the if statement is checking whether sscanf 
            // successfully parses all the expected values from line_buffer 
            // according to the provided format string, which contains 
            // placeholders for 13 items to be parsed from each line in the file

            // resize lexicon if necessary
            if (word_count >= capacity) { 
                // if the condition is true, it indidcates that the current
                // capacity is insufficient to store more words, and the array
                // needs to be resized

                capacity *= 2; // double the capacity
                struct words *temp = realloc(lexicon, capacity * sizeof(struct words)); // reallocate memory for lexicon
                // 'realloc' is used to resize the lexicon array when its capacity is reached
                // the new capacity is twice the current capacity (line 50)
                // if mem reallocation fails, an error message is printed and the program exits

                // struct words *temp is a temporary pointer used to store the result
                // of reallocating memory for the lexicon array
                // realloc dynamically reallocates memory for the lexicon array,
                // increasing its capacity to accomodate more words

                if (temp == NULL) { // check if memory reallocation was successful
                    perror ("Memory allocation failed");
                    fclose(vader_file);
                    free(lexicon);
                    exit(1);

                }
                lexicon = temp; // update lexicon pointer
            }

            // allocate memory for the new word and copy values
            (lexicon)[word_count].word = strdup(word_buffer); // copy word
            // ^ dynamically allocates mem for each word in the lexicon and 
            // copies the word from the buffer using 'strdup' ensures that 
            // each word is store in a seperate memory location,
            // preventing issues with overwriting or memory corruption
            // 'strdup' dynamically allocates memory for a new string
            // and copies the contents of the source string into it
            // it automatically allocates memory of the right size, avoiding 
            // buffer overflow issues

            // using 'strdup' ensures that each word from the lexicon is
            // stored in a newly allocated memory block, preventing potential
            // issues with buffer overflows or memory corruption

            // 'strcpy' requries the destination ubffer buffer to be 
            // pre-allocated with sufficient space to accomodate the 
            // copied string; need to know the exact size of string in 
            // advance or risk buffer overflow
            // but 'strdup' on the other hand, dynamically allocates memory for
            // the new string, ensuring that sufficient space is allocated
            // to hold the entire string
            // this eliminates the need for manual memory management and
            // reduces the risk of buffer overflow errors

            // essentially strdup does memory allocation and string copying 
            // process into a single function call


            (lexicon)[word_count].score = score; // copy score
            (lexicon)[word_count].SD = SD; // copy standard deviation
            memcpy((lexicon)[word_count].SIS_array, SIS_array, sizeof(SIS_array)); // copy sentiment intensity scores
            // 'memcpy' is a standard library function used to copy a block of
            // memory from a source ('SIS_array') to a destination 
            // ((lexicon)[word_count].SIS_array)
            
            (word_count)++; // increment word count
        }
    }
    fclose(vader_file); // close the vader_lexicon.txt file

}

// function to calculate the sentiment score for a sentence
float calculate_score(const char *sentence) {
    float total_score = 0.0; // total sentiment score
    int word_counter = 0; // counter for number of words

    // tokenize the sentence and process each word
    char *delimiters = " \t\n\r,;?!\"."; // define delimiters
    // delimiters are characters used to separate tokens (words) within
    // a string
    // purpose of delimiters: tokenzie the input sentence, splitting it 
    // into individual words based on these specified characters


    char *tk;
    char copy[LINE_LEN];
    strcpy(copy, sentence); // create a copy of the sentence
    // 'strcpy' is a standard library function used to copy a string from
    // a source ('sentence') to a destination ('copy')
    // creates a copy of the input sentence before tokenization

    tk = strtok(copy, delimiters); // tokenize the copy
    // 'strtok' is used to tokenize the input sentence using specified delimiters
    // initializes the tokenization process by passing the input string 'copy' and
    // delimiters to strtok

    // delimiters such as spaces, tabs, or punctuation marks

    // 'strtok' is used to split each sentence from the input file into
    // individual words
    // by specifying delimiters, it allows the program to process each word
    // separately, making it easier to compare them with the words in the lexicon
    
    // the tokens generated by 'strtok' are then converted to lowercase
    // and compared with words in the lexicon to calculate sentiment scores

    while (tk != NULL) {
        tk[strcspn(tk, "\n")] = 0; // remove newline character from token
        // 'strcspn' is a standard library function that calculates the 
        // length of the intial segment of a string ('tk') consisting of 
        // characters not in another string '"\n"'

        // that line calculates the position of the newline character in the token 
        // 'tk' which is then set to '0' to remove the newline character

        // convert token to lowercase for case-insensitive comparison
        for (int i = 0; tk[i]; i++) {
                tk[i] = tolower(tk[i]);
            }
        //printf("\nToken: \"%s\"\n", tk); // debugging: print token for inspection
        
        // compare each word with the lexicon and calculate the total score
        for (int i = 0; i < word_count; i++) {
            // printf("Lexicon Word: %s\n", lexicon[i].word); // Debugging: Print lexicon word for inspection
            if (strcmp(lexicon[i].word, tk) == 0) {
                // 'strcmp' is a standard library function used to compare 2
                // strings (lexicon[i].word and tk)
                
                // if the 2 strings are equal, 'strcmp' returns 0, indicating a 
                // match btwn the current word in the lexicon and the token

                total_score += lexicon[i].score;
                // if a match is found, the sentiment score of the word is added
                // to the total score for the sentence

                break;
            }
            else {
                continue;
            } 
        }
        tk = strtok(NULL, delimiters); // move to the next token
        word_counter++; // increment word counter
    
    }
    return (word_counter > 0) ? total_score / word_counter : 0.0; // calculate avg score for the sentence
}

int main(int argc, char *argv[]) {

    if (argc != 3) { // check if correct number of command-line arguments are provided 
        printf("Usage: %s vader_lexicon.txt input_file.txt\n", argv[0]);
        exit(1);
    }
    

    parse_lexicon(argv[1]); // parse the lexicon file

    // char line[256];
    // int line_number = 1;

    printf("\n%s\n", (lexicon)[10].word);
    




    FILE *input_file = fopen(argv[2], "r"); // open input file

    if (input_file == NULL) {  // check if file opening was successful
        fprintf(stderr, "Error opening the input file.\n");
        free(lexicon);
        exit(1);
    }
    
    // print header for output
    printf("          string sample                                                              score\n");
    printf("-----------------------------------------------------------------------------------------------\n");
    
    // process each line in the validation.txt file (input file)
    char buffer_val[LINE_LEN];
    
    while (fgets(buffer_val, sizeof(buffer_val), input_file)) { 

        buffer_val[strcspn(buffer_val, "\r\n")] = 0; // remove newline characters
        float score = calculate_score(buffer_val); // calculate sentiment score for each sentence
        printf("%-85s %.2f\n", buffer_val, score); // print string sample and score
    }
    
    fclose(input_file); // close input file

    // free the memory that was allocated for lexicon
    for (int i = 0; i < word_count; i++) {
        free(lexicon[i].word);
    }
    free(lexicon);

    return 0; // exit program
}


