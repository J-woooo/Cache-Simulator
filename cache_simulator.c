// FILE: hw6.c
// NAME: Chris Turgeon
// ARGS: ./a.out 2 LRU ex01.dat

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { false, true } bool;


///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

int next_use(int input[], int input_cnt, int value, int curr_idx) 
	/*\ 
	 * This function takes in an array of numbers and an index and value to search for.
	 * It returns the index of the next instance of the number occurring in the array.
	\*/
{
	int i;
	for (i = curr_idx + 1; i < input_cnt; i++) {
		if (input[i] == value) {
			return i;
		}
	}
	return -1;
}


///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

void LRU_cache(int input[], int input_cnt, int associativity) 
	/*\
	 * This function simulates the LRU cache.
	\*/
{

	// Initialize and declare output variables.
	int cache_accesses = input_cnt;
	int cache_hits = 0;
	int cache_misses = 0;
	int i = 0;

	// Allocate the cache structure based on associativity.
	int **cache;
	int set_count = 256 / associativity;
	if (associativity == 1) {
		cache = (int **)malloc( sizeof(int *) * set_count );
		for (i = 0; i < set_count; i++) { // First idx is value, second is LRU stage.
			cache[i] = (int *)malloc( sizeof(int) * 2 );
			cache[i][0] = 0;
			cache[i][1] = 0;
		}
		for (i = 0; i < input_cnt; i++) { 
			int loc = input[i] % set_count; 
			if (cache[loc][0] == 0) { // No value exists there, so add it.
				//printf("%d (miss)\n", input[i]);
				cache[loc][0] = input[i]; 
				cache[loc][1] = i;
				cache_misses++;
			}   
			else if (cache[loc][0] == input[i]) { // HIT, update refresh value in cache
				//printf("%d (hit)\n", input[i]);
				cache[loc][1] = i;
				cache_hits++;
			} else { // Replace cache block with new value.
				//printf("%d (miss)\n", input[i]);
				cache[loc][0] = input[i];
				cache[loc][1] = i;
				cache_misses++;
			}
		} 
	}

	else if (associativity == 2) {
		cache = (int **)malloc( sizeof(int *) * set_count ); 
		for (i = 0; i < set_count; i++) { // First 2 idx's are values, second 2 are LRU stages.
			cache[i] = (int *)malloc( sizeof(int) * 4 );
			cache[i][0] = 0;
			cache[i][1] = 0;
			cache[i][2] = 0;
			cache[i][3] = 0;
		}
		for (i = 0; i < input_cnt; i++) {
			int loc = input[i] % set_count;
			if (cache[loc][0] == 0 && cache[loc][1] == 0) { // No value is there now, so add one.
				//printf("%d (miss)\n", input[i]);
				cache[loc][0] = input[i];
				cache[loc][2] = i;
				cache_misses++;
			}
			else if (cache[loc][0] == input[i]) { // Hit with 1st number.
				//printf("%d (hit)\n", input[i]);
				cache[loc][2] = i;
				cache_hits++;
			}
			else if (cache[loc][1] == input[i]) { // Hit with 2nd number.
				//printf("%d (hit)\n", input[i]);
				cache[loc][3] = i;
				cache_hits++;
			}
			else if (cache[loc][0] == 0) { // 1st value is only one available, hit.
				//printf("%d (miss)\n", input[i]);
				cache[loc][0] = input[i];
				cache[loc][2] = i;
				cache_misses++;
			}
			else if (cache[loc][1] == 0 && cache[loc][0] != 0) { // 2nd value is only one available, hit.
				//printf("%d (miss)\n", input[i]);
				cache[loc][1] = input[i];
				cache[loc][3] = i;
				cache_misses++;
			} else { // Replace LRU.
				//printf("%d (miss)\n", input[i]);
				if (cache[loc][2] <= cache[loc][3]) { // Replace the 1st number and new stage.
					cache[loc][0] = input[i];
					cache[loc][2] = i;
				} else { // Replace the 2nd number and new stage.
					cache[loc][1] = input[i];
					cache[loc][3] = i;
				}
				cache_misses++;
			}
		}
	} 
	else if (associativity == 4){
		cache = (int **)malloc( sizeof(int *) * set_count );
		for (i = 0; i < set_count; i++) { // First 4 idx's are values, second 4 are LRU stages.
			cache[i] = (int *)malloc( sizeof(int) * 8 );
			int k;
			for (k = 0; k < 8; k++) { // Initialize to all 0.
				cache[i][k] = 0;
			}
		}
		for (i = 0; i < input_cnt; i++) {
			bool found = false;
			int cnt = 0, j = 0; 
			int loc = input[i] % set_count;
			for (j = 0; j < 4; j++) {
				if (input[i] == cache[loc][j]) { // Hit, update LRU.
					//printf("%d (hit)\n", input[i]);
					cache[loc][j+4] = i;
					cache_hits++;
					found = true;
				}
				if (cache[loc][j] != 0) {
					cnt++;
				}
			}
			if (cnt == 0 && !found) { // Whole set is empty, so add it.
				//printf("%d (miss)\n", input[i]);
				cache[loc][0] = input[i];
				cache[loc][4] = i;
				cache_misses++;
			} 
			else if (cnt > 0 && cnt < 4 && !found) { // There is at least one empty block.
				//printf("%d (miss)\n", input[i]);
				int j;
				for (j = 0; j < 4; j++) {
					if (cache[loc][j] == 0) { // Found an empty slot.
						cache[loc][j] = input[i];
						cache[loc][j+4] = i;
						break;
					}
				}
				cache_misses++;
			} 
			if (cnt == 4 && !found) { // All slots are full so replace LRU.
				//printf("%d (miss)\n", input[i]);
				int min = 99999, idx = 0, j = 0;
				for (j = 0; j < 4; j++) {
					if (cache[loc][j+4] <= min && cache[loc][j] != 0) {  
						min = cache[loc][j+4];
						idx = j;
					}
				}
				cache[loc][idx] = input[i];
				cache[loc][idx+4] = i;
				cache_misses++;
			}	
		}
	}
	else if(associativity == 8){
		cache = (int **)malloc(sizeof(int *) * set_count);
		for (i = 0; i < set_count; i++) { // First 4 idx's are values, second 4 are LRU stages.
			cache[i] = (int *)malloc(sizeof(int) * 16);
			int k;
			for (k = 0; k < 16; k++) { // Initialize to all 0.
				cache[i][k] = 0;
			}
		}
		for (i = 0; i < input_cnt; i++) {
			bool found = false;
			int cnt = 0, j = 0;
			int loc = input[i] % set_count;
			for (j = 0; j < 8; j++) {
				if (input[i] == cache[loc][j]) { // Hit, update LRU.
					//printf("%d (hit)\n", input[i]);
					cache[loc][j + 8] = i;
					cache_hits++;
					found = true;
				}
				if (cache[loc][j] != 0) {
					cnt++;
				}
			}
			if (cnt == 0 && !found) { // Whole set is empty, so add it.
				//printf("%d (miss)\n", input[i]);
				cache[loc][0] = input[i];
					cache[loc][8] = i;
					cache_misses++;
				}
				else if (cnt > 0 && cnt < 8 && !found) { // There is at least one empty block.
					//printf("%d (miss)\n", input[i]);
					int j;
					for (j = 0; j < 8; j++) {
						if (cache[loc][j] == 0) { // Found an empty slot.
							cache[loc][j] = input[i];
							cache[loc][j + 8] = i;
							break;
						}
					}
					cache_misses++;
				}
				if (cnt == 8 && !found) { // All slots are full so replace LRU.
					//printf("%d (miss)\n", input[i]);
					int min = 99999, idx = 0, j = 0;
				for (j = 0; j < 8; j++) {
					if (cache[loc][j + 8] <= min && cache[loc][j] != 0) {
						min = cache[loc][j + 8];
						idx = j;
					}
				}
				cache[loc][idx] = input[i];
				cache[loc][idx + 8] = i;
				cache_misses++;
			}
		}
	}
	else if (associativity == 16) {
	cache = (int **)malloc(sizeof(int *) * set_count);
	for (i = 0; i < set_count; i++) { // First 4 idx's are values, second 4 are LRU stages.
		cache[i] = (int *)malloc(sizeof(int) * 32);
		int k;
		for (k = 0; k < 16; k++) { // Initialize to all 0.
			cache[i][k] = 0;
		}
	}
	for (i = 0; i < input_cnt; i++) {
		bool found = false;
		int cnt = 0, j = 0;
		int loc = input[i] % set_count;
		for (j = 0; j < 16; j++) {
			if (input[i] == cache[loc][j]) { // Hit, update LRU.
				//printf("%d (hit)\n", input[i]);
				cache[loc][j + 16] = i;
				cache_hits++;
				found = true;
			}
			if (cache[loc][j] != 0) {
				cnt++;
			}
		}
		if (cnt == 0 && !found) { // Whole set is empty, so add it.
			//printf("%d (miss)\n", input[i]);
			cache[loc][0] = input[i];
			cache[loc][16] = i;
			cache_misses++;
		}
		else if (cnt > 0 && cnt < 16 && !found) { // There is at least one empty block.
			//printf("%d (miss)\n", input[i]);
			int j;
			for (j = 0; j < 16; j++) {
				if (cache[loc][j] == 0) { // Found an empty slot.
					cache[loc][j] = input[i];
					cache[loc][j + 16] = i;
					break;
				}
			}
			cache_misses++;
		}
		if (cnt == 16 && !found) { // All slots are full so replace LRU.
			//printf("%d (miss)\n", input[i]);
			int min = 99999, idx = 0, j = 0;
			for (j = 0; j < 16; j++) {
				if (cache[loc][j + 8] <= min && cache[loc][j] != 0) {
					min = cache[loc][j + 16];
					idx = j;
				}
			}
			cache[loc][idx] = input[i];
			cache[loc][idx + 16] = i;
			cache_misses++;
		}
	}
	}
	// Output cache statistics.
	float hit_rate = (float)cache_hits / (float)cache_accesses;
	printf("Cache accesses: %d\n", cache_accesses);
	printf("Cache hits: %d\n", cache_hits);
	printf("Cache misses: %d\n", cache_misses);
	printf("Overall hit rate: %f\n", hit_rate);

	// Free dynamically allocated memory.
	for (i = 0; i < set_count; i++) {
		free(cache[i]);
	}
	free(cache);
}



///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

int main(int argc, char *argv[]) 
{
	// Make sure there are 4 arguments given on command line.
	if (argc != 4) {
		perror("Error: Invalid Number of Arguments!");
		return EXIT_FAILURE;
	}
	
	// Open file and check to see if it's opened correctly.
	FILE *file;
	if ( (file = fopen(argv[3], "r")) == NULL ) {
		perror("Error: Inavlid Input File!\n");
		return EXIT_FAILURE;
	} 

	// Needed values for output and program structure.
	int cache_size = 256;
	int associativity = atoi(argv[1]);
	int cache_sets;
	char *algorithm = argv[2];
	bool is_LRU = false;

	// If invalid associativity value, exit program.
	if ( associativity != 1 && associativity != 2 && associativity != 4 && associativity != 8 && associativity != 16) {
		fprintf(stderr, "Error: Invalid Associativity Entered!\n");
		return EXIT_FAILURE;
	}

	if ( strcmp(algorithm, "LRU") == 0 ) { // LRU Algorithm 
		is_LRU = true;
	}
	else { // Invalid Entry
		fprintf(stderr, "Error: Invalid Algorithm Entered!\n");
		return EXIT_FAILURE;
	}

	cache_sets = cache_size / associativity;
	printf("Cache size: %d\n", cache_size);
	printf("Cache associativity: %d\n", associativity);
	printf("Cache sets: %d\n", cache_sets);
	printf("Cache algorithm: %s\n", algorithm);

	// Read in the data into an array.
	int i = 0, input_cnt = 0;
	int *input = (int *)malloc( sizeof(int) * 1024 );
	memset(input, 0, sizeof(int) * 1024);
	while ( fscanf(file, "%d", &i) != EOF) {
		input[input_cnt] = i;
		input_cnt++;
	}

	if (is_LRU)
		LRU_cache(input, input_cnt, associativity);
	
	free(input);
	return EXIT_SUCCESS;
}