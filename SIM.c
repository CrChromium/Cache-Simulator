#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>


#define BLOCK_SIZE 64

typedef struct {
    uint64_t tag;
    bool valid;
    bool dirty;
    uint64_t last_used;
} CacheBlock;

unsigned int cache_size;
unsigned int assoc;
unsigned int num_sets;

int replacement;
int wb;

uint64_t Hit = 0;
uint64_t Miss = 0;
uint64_t reads = 0;
uint64_t writes = 0;
uint64_t num_access = 0;

void Update_lru(unsigned int set, unsigned int way, CacheBlock* cache);
void Update_fifo(unsigned int set, unsigned int way, CacheBlock* cache);
unsigned int eject(unsigned int set, CacheBlock* cache);

void Simulate_access(char op, uint64_t add, CacheBlock *cache) {

    //printf("Simulated\n");
    uint64_t block = add / BLOCK_SIZE;
    unsigned int set = block % num_sets;
    uint64_t tag = block / num_sets;

    /*printf("Address = 0x%" PRIx64 "\n", add);
    printf("Set = %u\n", set);
    printf("Tag = %" PRIu64 "\n", tag);*/

    unsigned int base = set * assoc;

    for(unsigned int way = 0; way < assoc; way++) {
        
        CacheBlock *line = &cache[base + way];

        if (line->valid && line->tag == tag) {
            Hit++;

            //printf("hit\n");

            if (replacement == 0){
                Update_lru(set, way, cache);
            }

            if (op == 'W'){
                
                if (wb == 0)
                    writes++;
                else
                    line->dirty = true;
            }
            return;
        }
    }

    Miss++;

    //printf("Miss\n");
    reads++;

    for(unsigned int way = 0; way < assoc; way++){

        CacheBlock *line = &cache[base + way];

        if (!line->valid) {
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

            if (replacement == 0){
                Update_lru(set, way, cache);
            } else {
                Update_fifo(set, way, cache);
            }

            return;
        }
    }

    unsigned int ejected = eject(set, cache);

    CacheBlock* line = &cache[base+ejected];

    if (wb == 1 && line->dirty)
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

    num_access++;

    cache[set * assoc + way].last_used = num_access;
}

void Update_fifo(unsigned int set, unsigned int way, CacheBlock* cache) {

    num_access++;

    cache[set * assoc + way].last_used = num_access;
}

unsigned int eject(unsigned int set, CacheBlock* cache) {

    unsigned int ejected = 0;

    uint64_t oldest = cache[set * assoc].last_used;

    for (unsigned int way = 1; way < assoc; way++) {

        CacheBlock *line = &cache[set * assoc + way];

        if (line->last_used < oldest) {

            oldest = line->last_used;
            ejected = way;
        }
    }

    return ejected;
}

int main(int argc, char *argv[]) {
    
    if (argc != 6){
        printf("Usage: ./SIM <CACHE_SIZE> <ASSOC> <REPLACEMENT> <WB> <TRACE_FILE>\n");
        return 1;
    }

    cache_size = strtoull(argv[1], NULL, 10);
    assoc = strtoull(argv[2], NULL, 10);

    replacement = atoi(argv[3]);
    wb = atoi(argv[4]);

    char *trace_file = argv[5];

    /*Maybe implement a check if arguments are acceptable*/

    num_sets = cache_size / (BLOCK_SIZE * assoc);
    
    char op;
    uint64_t add;
    FILE *file = fopen(trace_file, "r");

    unsigned int total_blocks = cache_size / BLOCK_SIZE;

    CacheBlock *cache = calloc(total_blocks, sizeof(CacheBlock));



    /*printf("Cache size = %u\n", cache_size);
    printf("Associativity = %u\n", assoc);
    printf("Replacement policy = %s\n", replacement == 0 ? "LRU" : "FIFO");
    printf("Write Policy = %s\n", wb == 0 ? "Write-through" : "Write-back");
    printf("Number of sets = %u\n", num_sets);*/

    if (!file){
        printf("Error: Could not open the trace file.\n");
        return 1;
    }

    while(fscanf(file, " %c %" SCNx64, &op, &add) == 2) {

        Simulate_access(op, add, cache);
    }

    fclose(file);

    /*printf("Hits: %" PRIu64 "\n", Hit);
    printf("Misses: %" PRIu64 "\n", Miss);*/
    printf("Miss Ratio: %f\n", (double)Miss / (Hit + Miss));
    printf("Reads: %" PRIu64 "\n", reads);
    printf("Writes: %" PRIu64 "\n", writes);

    return 0;
}