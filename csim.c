#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "cachelab.h"

/* define line max length */
#define MAX_LENGTH 255

/* define Line struct and typedef */
struct Line {
    int valid;
    long tag;
    long last_used;
};
typedef struct Line Line_t;

/* Function prototypes */
char* loadData(Line_t cacheSets[], long tag, long set, int E, int* hit_count_p, int* miss_count_p, int* eviction_count_p);
char* saveData(Line_t cacheSets[], long tag, long set, int E, int* hit_count_p, int* miss_count_p, int* eviction_count_p);
void evict(Line_t cacheSets[], long tag, long set, int E, int line);
void initializeCache(Line_t sets[], int set_count, int E);
void substring(char newStr[], char str[], int start, int end);
long extract(int num, int length, int offset);
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
    // int start = 3; // start of address
    // int end = -1; // end of address, exclusive (to be set)
    if (traceFile) {
        while (fgets(buff, MAX_LENGTH, traceFile)) {
            // Ignore instruction lines
            if (buff[0] != ' ') {
                continue;
            }

            char instruction;
            char addStr[MAX_LENGTH];
            sscanf(buff, " %c %s", &instruction, addStr);
            addStr[strcspn(addStr, ",")] = 0;

            char line[MAX_LENGTH];
            strcpy(line, buff);
            memmove(line, line+1, strlen(line));
            line[strcspn(line, "\n")] = 0;

            // extract instruction data
            // find end of address

            // char* c = strchr(buff, ',');
            // end = strlen(addStr);
            // printf("end=%d\n", end);

            // parse address
            // char addStr[end-start+1];
            // substring(addStr, buff, start, end);
            unsigned long address = strtoul(addStr, NULL, 16);
            // extract set
            long set = extract(address, s, b);
            // extract tag
            long tag = extract(address, 64-(s+b), s+b);

            // Validate set
            if (set >= set_count) {
                printError(strcat("Invalid set: ", buff));
                return 2;
            }

            // Determine which operation to perform
            char res_out[MAX_LENGTH];
            char* result;
            char* tmp;
            switch (instruction)
            {
            case 'L':
                // TODO: Load data
                result = loadData(cacheSets, tag, set, E, &hit_count, &miss_count, &eviction_count);
                strcpy(res_out, result);
                break;
            case 'S':
                // TODO: Store data
                result = saveData(cacheSets, tag, set, E, &hit_count, &miss_count, &eviction_count);
                strcpy(res_out, result);
                break;
            case 'M':
                // TODO: Modify data  
                result = loadData(cacheSets, tag, set, E, &hit_count, &miss_count, &eviction_count);
                tmp = saveData(cacheSets, tag, set, E, &hit_count, &miss_count, &eviction_count);
                strcpy(res_out, result);
                strcat(res_out, " ");
                strcat(res_out, tmp);
                break;
            default:
                // If not valid instruction, skip.
                // Should never get here.
                printError(strcat("Invalid instruction found: ", buff));
                break;
            }
            if (v) {
                // printf("%s %s, addStr=%s, address=%ld, s=%ld, t=%ld\n", line, res_out, addStr, address, set, tag);
                printf("%s %s\n", line, res_out);
            }
        }
    }
    fclose(traceFile);

    // Print Summary
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

char* loadData(Line_t cacheSets[], long tag, long set, int E, int* hit_count_p, int* miss_count_p, int* eviction_count_p) {
    printf("==IN LOAD==\n");
    printf("DATA: tag=%ld, set=%ld\n", tag, set);
    int leastRecentIndex = 0;
    long oldestTime = -1;
    // iterate through set lines
    for (int line = 0; line < E; line++) {
        // get line
        Line_t setLine = cacheSets[set * E + line];
        printf("set=%ld, line=%d, valid=%d, tag=%ld\n", set, line, setLine.valid, setLine.tag);
        // if line valid, and tag matches
        if (setLine.valid) {
            if (tag == setLine.tag) {
                printf("HIT\n");
                // update hit count, last_used, and return
                *hit_count_p = *hit_count_p + 1;
                setLine.last_used = time(NULL);
                return "hit";
            }
            // valid, but not matching tag
            if (oldestTime == -1 || setLine.last_used < oldestTime) {
                // new least recently used found, update variables
                oldestTime = setLine.last_used;
                leastRecentIndex = line;
            }
        }
    }
    // no valid and matching tab, miss
    *miss_count_p = *miss_count_p + 1;
    if (oldestTime == -1) {
        printf("MISS\n");
        // open lines available
        cacheSets[set * E + leastRecentIndex].valid = 1;
        cacheSets[set * E + leastRecentIndex].tag = tag;
        cacheSets[set * E + leastRecentIndex].last_used = time(NULL);
        return "miss";
    } else {
        printf("MISS EVICT\n");
        // no open line, evict oldest
        evict(cacheSets, tag, set, E, leastRecentIndex);
        // update evict count
        *eviction_count_p = *eviction_count_p + 1;
        return "miss eviction";
    }    
}

char* saveData(Line_t cacheSets[], long tag, long set, int E, int* hit_count_p, int* miss_count_p, int* eviction_count_p) {
    printf("==IN SAVE==\n");
    printf("DATA: tag=%ld, set=%ld\n", tag, set);
    int leastRecentIndex = 0;
    long oldestTime = -1;
    // iterate through set lines
    for (int line = 0; line < E; line++) {
        printf("==IN LINE %d==\n", line);
        // get line
        Line_t setLine = cacheSets[set * E + line];
        printf("set=%ld, line=%d, valid=%d, tag=%ld\n", set, line, setLine.valid, setLine.tag);
        // if line valid, and tag matches
        if (setLine.valid) {
            if (tag == setLine.tag) {
                printf("HIT\n");
                // update hit count and return
                *hit_count_p = *hit_count_p + 1;
                return "hit";
            }
            // valid, but not matching tag
            if (oldestTime == -1 || setLine.last_used < oldestTime) {
                // new least recently used found, update variables
                oldestTime = setLine.last_used;
                leastRecentIndex = line;
            }
        }
    }
    // no match found, need to save/evict
    *miss_count_p = *miss_count_p + 1;
    if (oldestTime == -1) {
        printf("MISS\n");
        // open lines available
        cacheSets[set * E + leastRecentIndex].valid = 1;
        cacheSets[set * E + leastRecentIndex].tag = tag;
        cacheSets[set * E + leastRecentIndex].last_used = time(NULL);
        return "miss";
    } else {
        printf("MISS EVICT\n");
        // no open line, evict oldest
        evict(cacheSets, tag, set, E, leastRecentIndex);
        // update evict count
        *eviction_count_p = *eviction_count_p + 1;
        return "miss eviction";
    }    
}

void evict(Line_t cacheSets[], long tag, long set, int E, int line) {
    cacheSets[set * E + line].valid = 1;
    cacheSets[set * E + line].tag = tag;
    cacheSets[set * E + line].last_used = time(NULL);
}

void initializeCache(Line_t sets[], int set_count, int E) {
    for (int set = 0; set < set_count; set++) {
        for (int lineOffset = 0; lineOffset < E; lineOffset++) {
            sets[set * E + lineOffset].valid = 0;
            sets[set * E + lineOffset].last_used = -1;
        }
    }
}

void substring(char newStr[], char str[], int start, int end) {
    for (; start < end; start++) {
        newStr[start] = str[start];
    }
}

long extract(int num, int length, int offset) {
    unsigned long mask = pow(2, length);
    mask--;
    long res = num >> offset;
    res = res & mask;
    return res;
    // return (long)(num >> offset) & mask;
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
