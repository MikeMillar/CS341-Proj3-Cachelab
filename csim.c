#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cachelab.h"

/* define line max length */
#define MAX_LENGTH 255

struct Line {
    int valid;
    char* tag;
    char* data;
};

typedef struct Line Line_t;

void initializeCache(Line_t sets[], int set_count, int E);
void extractAddress(char address[], char line[], int start, int end);
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
        printf("Verbose printouts enabled.");
    }

    // Check all required arguments set
    if (s == -1) {
        printError("s is a required argument that must be set. -s <s>");
        return 1;
    }
    if (E == -1) {
        printError("E is a required argument that must be set. -E <E>");
        return 1;
    }
    if (b == -1) {
        printError("b is a required argument that must be set. -b <b>");
        return 1;
    }
    if (trace == NULL) {
        printError("trace file is required argument that must be set. -t <trace>");
        return 1;
    }
    // End Argument parsing

    // Initialize data structures
    int set_count = pow(2, s);
    Line_t cacheSets[set_count * E];
    initializeCache(cacheSets, set_count, E);

    int hit_count = 0;
    int miss_count = 0;
    int eviction_count = 0;
    // End initialization

    // TODO: Read in trace line-by-line and simulate cache
    FILE* traceFile = fopen(trace, "r");
    char buff[MAX_LENGTH];
    // substring variables
    int start = 3; // start of address
    int end = -1; // end of address, exclusive (to be set)
    if (file) {
        while (fgets(buff, MAX_LENGTH, traceFile)) {
            // Ignore instruction lines
            if (buff[0] != ' ') {
                continue;
            }
            // extract instruction data
            char* c = strchr(buff, ',');
            end = (int)(c - buff);
            char address[end-start+1];
            // Determine which operation to perform
            switch (buff[1])
            {
            case 'L':
                // TODO: Load data
                break;
            case 'S':
                // TODO: Store data
                break;
            case 'M':
                // TODO: Modify data
                break;
            default:
                // If not valid instruction, skip.
                // Should never get here.
                printError(strcat("Invalid instruction found: ", buff));
                break;
            }

        }
    }
    fclose(traceFile);

    // Print Summary
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

void initializeCache(Line_t sets[], int set_count, int E) {
    for (int set = 0; set < set_count; set++) {
        for (int lineOffset = 0; lineOffset < E; lineOffset++) {
            sets[set * E + lineOffset].valid = 0;
        }
    }
}

void extractAddress(char address[], char line[], int start, int end) {

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
