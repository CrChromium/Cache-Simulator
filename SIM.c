#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>


#define BLOCK_SIZE 64


/*long long int tag_array[NUM_SETS][ASSOC];
long long int lru_position[NUM_SETS][ASSOC];
bool dirty[NUM_SETS][ASSOC];*/

int Hit = 0;
int Miss = 0;

void Update_lru(long long int add);
void Update_fifo(long long int add);

/*void Simulate_access(char op, long long int add) {
    int set = (add / BLOCK_SIZE) % NUM_SETS;
    long long int tag = add / BLOCK_SIZE;

    for(int i = 0; i < ASSOC; i++) {
        if(tag == tag_array[set][i]){
            Hit++;

            if(lru policy){
                Update_lru(add);
            } else {
                Update_fifo(add);
            }
        } else {
            Miss++;
        }
    }
}*/

void Update_lru(long long int add) {

}

void Update_fifo(long long int add) {

}

int main(int argc, char *argv[]) {
    
    if (argc != 6){
        printf("Usage: ./SIM <CACHE_SIZE> <ASSOC> <REPLACEMENT> <WB> <TRACE_FILE>\n");
        return 1;
    }

    unsigned long long cache_size = strtoull(argv[1], NULL, 10);
    unsigned long long assoc = strtoull(argv[2], NULL, 10);

    int replacement = atoi(argv[3]);
    int wb = atoi(argv[4]);

    char *trace_file = argv[5];

    /*Maybe implement a check if arguments are acceptable*/

    unsigned long long num_sets = cache_size / (BLOCK_SIZE * assoc);
    
    char op;
    uint64_t add;
    FILE *file = fopen(trace_file, "r");

    printf("Cache size = %llu\n", cache_size);
    printf("Associativity = %llu\n", assoc);
    printf("Replacement policy = %s\n", replacement == 0 ? "LRU" : "FIFO");
    printf("Write Policy = %s\n", wb == 0 ? "Write-through" : "Write-back");
    printf("Number of sets = %llu\n", num_sets);





    if (!file){
        printf("Error: Could not open the trace file.\n");
        return 1;
    }

    while(fscanf(file, " %c %" SCNx64, &op, &add) == 2) {
        

        uint64_t block = add / BLOCK_SIZE;
        uint64_t set = block % num_sets;
        uint64_t tag = block / num_sets;

        printf("Address = %" PRIx64 "\n", add);
        printf("Set = %" PRIu64 "\n", set);
        printf("Tag = %" PRIu64 "\n", tag);


        //Simulate_access(op, add);
    }

    fclose(file);

    printf("Hits: %d\n", Hit);
    printf("Misses: %d\n", Miss);

    return 0;
}