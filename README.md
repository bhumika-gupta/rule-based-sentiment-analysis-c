# rule-based-sentiment-analysis-c

Rule-based Sentiment Analysis using VADER Lexicon\

<ins>Introduction</ins>:
Sentiment analysis is a method for examining a text to identify the
sentiment or perspective that is being expressed. Predefined rules and
patterns are used in rule-based sentiment analysis, like the one used in
this project, to determine sentiment. The goal of this project is tot
use the VADER (Valence Aware Dictionary and Sentiment Reasoner) lexicon
to create a foundational rule-based sentiment analysis tool.

<ins>Project Overview</ins>:
The project consists of the three main files: `parser.c`, `parser.h`,
and `Makefile`. The `parser.c` file contains the implementation of
functions to parse the VADER lexicon file, to calculate sentiment scores
for sentences, and the main function to process input files. The
`parser.h` file declares the data structures and function prototypes
used in `parser.c`. The `Makefile` is used to compile the project.

<ins>Implementation Details</ins>:

-   **Parsing VADER Lexicon:** The `parser_lexicon` function reads the
    VADER lexicon file, parses each line to extract words, sentiment
    scores, and other relevant information, and stores them in a data
    structure.

-   **Calculating Sentiment Scores:** The `calculate_score` function
    tokenizes input sentences, matches each word with entries in the
    VADER lexicon, and computes the sentiment score for the sentence.

-   **Main Function:** The main function reads input files, processes
    each line using the `calculate_score` function, and prints the
    sentiment score for each sentence.

<ins>Usage</ins>:
To compile the project, run the command `make` in the terminal. This
will generate an executable named `mySA`. The project can be executed
using the command `./mySA vader_lexicon.txt input_file.txt`, where
`vader_lexicon.txt` is the file containing the VADER lexicon, and
`input_file.txt` is the file containing sentences to analyze.

<ins>Results</ins>:
With the `vader_lexicon.txt` used in this project being the following:

                VADER is smart, handsome, and funny.
                VADER is smart, handsome, and funny!
                VADER is very smart, handsome, and funny.
                VADER is VERY SMART, handsome, and FUNNY.
                VADER is VERY SMART, handsome, and FUNNY!!!
                VADER is VERY SMART, uber handsome, and FRIGGIN FUNNY!!!
                VADER is not smart, handsome, nor funny.
                The book was good.
                At least it isn't a horrible book.
                The book was only kind of good.
                The plot was good, but the characters are uncompelling and the dialog is not great.
                Today SUX!
                Today only kinda sux! But I'll get by, lol
                Make sure you :) or :D today!
                Not bad at all

and after the usage as described in the previous section and the code
included in the Appendix, the following is the terminal output:

<ins>Conclusion</ins>:
Using the VADER vocabulary, this project offers a straightforward but
powerful rule-based sentiment analysis tool. Opinion mining, customer
feedback analysis, sentiment analysis on social media, and many other
uses are all possible with it.The project is a useful complement to jobs
involving natural language processing because of its simplicity and ease
of usage.

---

Appendix

`parser.c`

            
            #include <stdio.h>
            #include <stdlib.h>
            #include <string.h>
            #include "parser.h" // include header file for the parser functions
            #include <ctype.h>
            
            
            #define LINE_LEN 256 // max line length
            #define WORD_LEN 50 // max word length
            
            
            struct words *lexicon = NULL; // pointer to store words from vader_lexicon.txt
            int word_count = 0; // number of words read
            
            
            // function to parse the vader_lexicon file
            void parse_lexicon(const char *filename) {
                FILE *vader_file = fopen(filename, "r"); // open the file in read mode
                if (vader_file == NULL) { // check if the opening of the file was successful
                    printf("There is an error in opening the file.\n");
                    exit(1);
                }
                
                // initialize variables for lexicon (word list) and word count, and set initial capacity
                int capacity = 10; // initial capacity of the lexicon array
                lexicon = malloc(capacity * sizeof(struct words)); // allocate memory for lexicon
                if (lexicon == NULL) { // check if memory allocation was successful
                    perror("Memory allocation failed");
                    fclose(vader_file);
                    exit(1);
                }
                
                char line_buffer[LINE_LEN]; // buffer to store each line read from file
                // while loop: read lines from the file until the end is reached
                while(fgets(line_buffer, sizeof(line_buffer), vader_file)) {
                    char word_buffer[WORD_LEN]; // buffer to store word
                    float score, SD; // variables to store sentiment score and standard deviation
                    int SIS_array[10]; // array to store sentiment intensity scores
                    
                    // read word, score, SD, and SIS_array from each line in the file
                    if (sscanf(line_buffer, "%s %f %f [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",
                    word_buffer, &score, &SD,
                    &SIS_array[0], &SIS_array[1], &SIS_array[2], &SIS_array[3], &SIS_array[4],
                    &SIS_array[5], &SIS_array[6], &SIS_array[7], &SIS_array[8], &SIS_array[9]) == 13) {
                        
                        // resize lexicon if necessary
                        if (word_count >= capacity) {
                            capacity *= 2; // double the capacity
                            struct words *temp = realloc(lexicon, capacity * sizeof(struct words)); // reallocate memory for lexicon
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
                        (lexicon)[word_count].score = score; // copy score
                        (lexicon)[word_count].SD = SD; // copy standard deviation
                        memcpy((lexicon)[word_count].SIS_array, SIS_array, sizeof(SIS_array)); // copy sentiment intensity scores
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
                char *tk;
                char copy[LINE_LEN];
                strcpy(copy, sentence); // create a copy of the sentence
                
                tk = strtok(copy, delimiters); // tokenize the copy
                while (tk != NULL) {
                    tk[strcspn(tk, "\n")] = 0; // remove newline character from token
                    // convert token to lowercase for case-insensitive comparison
                    for (int i = 0; tk[i]; i++) {
                        tk[i] = tolower(tk[i]);
                    }
                    //printf("\nToken: \"%s\"\n", tk); // debugging: print token for inspection
                    
                    // compare each word with the lexicon and calculate the total score
                    for (int i = 0; i < word_count; i++) {
                        // printf("Lexicon Word: %s\n", lexicon[i].word); // Debugging: Print lexicon word for inspection
                        if (strcmp(lexicon[i].word, tk) == 0) {
                            total_score += lexicon[i].score;
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
            
            
            

`parser.h`

            
            #ifndef PARSER_H
            #define PARSER_H
            
            struct words {
                char *word;
                float score;
                float SD;
                int SIS_array[10];
            };
            
            extern struct words *lexicon;
            extern int word_count;
            
            void parse_lexicon(const char *filename);
            float calculate_score(const char *sentence);
            
            
            
            
            #endif /* PARSER_H */_{#include "functions.h"
            

`Makefile`

            CC = gcc
            CFLAGS = -Wall -Wextra -std=c99
            
            SOURCES = parser.c
            OBJ = $(SOURCES:.c=.o)
            EXECUTABLE = mySA
            
            all: $(EXECUTABLE)
            $(EXECUTABLE): $(OBJ)
            $(CC) $(CFLAGS) $(OBJ) -o $@ -lm
            
            %.o: %.c
            $(CC) $(CFLAGS) -c $< -o $@
            
            clean:
            rm -f $(OBJ) $(EXEC)
