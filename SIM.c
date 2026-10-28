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

void Update_lru(long long int add);
void Update_fifo(long long int add);

void Simulate_access(char op, uint64_t add, CacheBlock *cache) {

    uint64_t block = add / BLOCK_SIZE;
    uint64_t set = block % num_sets;
    uint64_t tag = block / num_sets;

    printf("Address = 0x%" PRIx64 "\n", add);
    printf("Set = %" PRIu64 "\n", set);
    printf("Tag = %" PRIu64 "\n", tag);

    size_t base = (size_t)(set * assoc);

    for(size_t way = 0; way < assoc; way++) {
        
        CacheBlock *line = &cache[base + way];

        if (line->valid && line->tag == tag) {
            Hit++;

            printf("Hit here: 0x%" PRIx64 "\n", add);

            return;
        }
    }

    Miss++;
    printf("Miss here: 0x%" PRIx64 "\n", add);

    for(size_t way = 0; way < assoc; way++){

        CacheBlock *line = &cache[base + way];

        if (!line->valid) {
            line->tag = tag;
            line->valid = true;

        }
    }
}

void Update_lru(long long int add) {


}

void Update_fifo(long long int add) {

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



    printf("Cache size = %u\n", cache_size);
    printf("Associativity = %u\n", assoc);
    printf("Replacement policy = %s\n", replacement == 0 ? "LRU" : "FIFO");
    printf("Write Policy = %s\n", wb == 0 ? "Write-through" : "Write-back");
    printf("Number of sets = %u\n", num_sets);

    if (!file){
        printf("Error: Could not open the trace file.\n");
        return 1;
    }

    while(fscanf(file, " %c %" SCNx64, &op, &add) == 2) {

        Simulate_access(op, add, cache);
    }

    fclose(file);

    printf("Hits: %" PRIu64 "\n", Hit);
    printf("Misses: %" PRIu64 "\n", Miss);

    return 0;
}