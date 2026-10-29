#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>


#define BLOCK_SIZE 64

/*Data structure for cache block*/
typedef struct {
    uint64_t tag;
    bool valid;
    bool dirty;
    uint64_t last_used;
} CacheBlock;

/*Global variables*/
/*Unsigned int because these numbers aren't large*/
unsigned int cache_size;
unsigned int assoc;
unsigned int num_sets;

/*Bools for replacement policy and write policy*/
bool replacement;
bool wb;

/*Must use uint64_t because these numbers could be VERY large*/
uint64_t Hit = 0;
uint64_t Miss = 0;
uint64_t reads = 0;
uint64_t writes = 0;
uint64_t num_access = 0; //Number of times the memory is accessed

/*Function prototypes*/
void Update_lru(unsigned int set, unsigned int way, CacheBlock* cache);
void Update_fifo(unsigned int set, unsigned int way, CacheBlock* cache);
unsigned int eject(unsigned int set, CacheBlock* cache);

/*This function simulates accessing the cache*/
void Simulate_access(char op, uint64_t add, CacheBlock *cache) {

    //Gets the block, set, and tag for the address
    uint64_t block = add / BLOCK_SIZE;
    unsigned int set = block % num_sets;
    uint64_t tag = block / num_sets;

    //Calculates the base, used for determining the cache line
    unsigned int base = set * assoc;

    for(unsigned int way = 0; way < assoc; way++) { //For each way, loop
        
        CacheBlock *line = &cache[base + way]; //Make a line for each way

        if (line->valid && line->tag == tag) { //If the line is valid and the tag matches, hit
            Hit++;

            if (replacement == 0){ //Update the lru policy
                Update_lru(set, way, cache);
            }

            if (op == 'W'){
                
                if (wb == 0) //Write if write-through
                    writes++;
                else //Mark dirty otherwise
                    line->dirty = true;
            }
            return;
        }
    }

    //If it wasnt a hit, it's a miss
    Miss++;

    reads++;

    //Check for an empty cache line
    for(unsigned int way = 0; way < assoc; way++){

        CacheBlock *line = &cache[base + way];

        if (!line->valid) { //If the line is empty, write to it
            line->tag = tag;
            line->valid = true;
            line->dirty = false;

            if (op == 'W') { //Update for wb

                if (wb == 0) {
                    writes++;
                }
                else {
                    line->dirty = true;
                }
            }

            if (replacement == 0){ //Update replacement policies
                Update_lru(set, way, cache);
            } else {
                Update_fifo(set, way, cache);
            }

            return;
        }
    }

    unsigned int ejected = eject(set, cache); //Mark a cache as set to be removed

    CacheBlock* line = &cache[base+ejected]; 

    if (wb == 1 && line->dirty) //If the write policy is write-back, write to mem
        writes++;

    line->tag = tag;
    line->valid = true;
    line->dirty = false;

    if (op == 'W') {

        if (wb == 0) {
            writes++;
        }
        else {
            line->dirty = true;
        }
}

    if (replacement ==0){
        Update_lru(set, ejected, cache);
    } else {
        Update_fifo(set, ejected, cache);
    }
}

void Update_lru(unsigned int set, unsigned int way, CacheBlock* cache) {

    num_access++; //Indicate that there was a memory access

    cache[set * assoc + way].last_used = num_access; //Give it a time stamp of when it was used
}

//Fifo is identical to lru, but we dont update it on hit
void Update_fifo(unsigned int set, unsigned int way, CacheBlock* cache) {

    num_access++;

    cache[set * assoc + way].last_used = num_access;
}

//This function ejects the block depending on the replacement policy
unsigned int eject(unsigned int set, CacheBlock* cache) {

    unsigned int ejected = 0; //The first way is assumed to have the ejected block

    uint64_t oldest = cache[set * assoc].last_used; //Checks the time stamp of way 0

    for (unsigned int way = 1; way < assoc; way++) {

        CacheBlock *line = &cache[set * assoc + way]; //Gets the time stamp of each other way

        if (line->last_used < oldest) { //If any way has an earlier time stamp, mark it as the oldest

            oldest = line->last_used;
            ejected = way;
        }
    }

    return ejected; //Eject the oldest
}

int main(int argc, char *argv[]) {
    
    if (argc != 6){
        printf("Usage: ./SIM <CACHE_SIZE> <ASSOC> <REPLACEMENT> <WB> <TRACE_FILE>\n");
        return 1;
    }

    //Stores each input argument
    cache_size = strtoull(argv[1], NULL, 10);
    assoc = strtoull(argv[2], NULL, 10);

    replacement = atoi(argv[3]);
    wb = atoi(argv[4]);

    char *trace_file = argv[5];

    num_sets = cache_size / (BLOCK_SIZE * assoc);
    
    //Stores the operation and the address
    char op;
    uint64_t add;
    FILE *file = fopen(trace_file, "r");

    unsigned int total_blocks = cache_size / BLOCK_SIZE;

    //Allocates memory for each block
    CacheBlock *cache = calloc(total_blocks, sizeof(CacheBlock));

    if (!file){
        printf("Error: Could not open the trace file.\n");
        return 1;
    }

    //Loops through each line of the file until scanf fails
    while(fscanf(file, " %c %" SCNx64, &op, &add) == 2) {
        Simulate_access(op, add, cache);
    }

    fclose(file);

    //Outputs the values
    printf("Miss ratio %f\n", (double)Miss / (Hit + Miss));
    printf("write %" PRIu64 "\n", writes);
    printf("read %" PRIu64 "\n", reads);
    

    return 0;
}