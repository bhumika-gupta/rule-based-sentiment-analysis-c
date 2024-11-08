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




#endif /* PARSER_H */