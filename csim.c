#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "cachelab.h"

struct Line {
    int valid;
    char* tag;
    char* data;
};

typedef struct Line Line_t;

Line_t* initializeCache(int s, int E, int b);
void printHelp();
void printError(char* msg);



int main(int argc, char *argv[]) {
    // Parse command line arguments
    // -h: help
    int h = 0;
    // -v: verbose
    int v = 0;
    // -s <s> number of set index bits (S = 2^s)
    int s = -1;
    // -E <E> number of lines per set
    int E = -1;
    // -b <b> number of block bits (B = 2^b)
    int b = -1;
    // -t <tracefile> name of trace file to replay
    char* trace = NULL;

    // Parse arguments
    int opt;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt)
        {
        case 'h':
            h = 1;
            break;
        case 'v':
            v = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace = optarg;
            break;
        }
    }

    // If help set, print help
    if (h) {
        printHelp();
        return 0;
    }
    if (v) {
        printf("Verbose printouts enabled.\n");
    }

    // Check all required arguments set
    if (s == -1) {
        printError("s is a required argument that must be set. -s <s>\n");
        return 1;
    }
    if (E == -1) {
        printError("E is a required argument that must be set. -E <E>\n");
        return 1;
    }
    if (b == -1) {
        printError("b is a required argument that must be set. -b <b>\n");
        return 1;
    }
    if (trace == NULL) {
        printError("trace file is required argument that must be set. -t <trace>\n");
        return 1;
    }
    // End Argument parsing

    // Initialize data structures
    Line_t* cacheSets = initializeCache(s, E);
    int hit_count = 0;
    int miss_count = 0;
    int eviction_count = 0;
    // End initialization

    // TODO: Read in trace line-by-line and simulate cache

    // Print Summary
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

Line_t* initializeCache(int s, int E) {
    int set_count = (int) pow(2, s);
    Line_t sets[set_count][E];
    for (int i = 0; i < set_count; i++) {
        for (int j = 0; j < E; j++) {
            sets[i][j] = malloc(sizeof(Line));
            sets[i][j].valid = 0;
        }
    }
    return sets;
}

void printHelp() {
    printf("This is a cache simulator program for project 3 of UNM CS341. This program utilizes several arguments:\n");
    printf("\t-h\t\tOptional help flag that prints usage info.\n");
    printf("\t-v\t\tOptional verbose flag that displays trace info.\n");
    printf("\t-s <s>\t\tNumber of set index bits (S = 2^s is number of sets)\n");
    printf("\t-E <E>\t\tAssociativity (number of lines per set)\n");
    printf("\t-b <b>\t\tNumber of block bits (B = 2^b is block size)\n");
    printf("\t-t <tracefile>\tName of valgrind trace to replay\n");
}

void printError(char* msg) {
    fprintf(stderr, "%s\n", msg);
}
