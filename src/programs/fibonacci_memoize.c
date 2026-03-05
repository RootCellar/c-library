

// Libraries


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// Local Library


#include "../lib/compiler_attributes.h"
#include "../lib/debug.h"
#include "../lib/unit_testing.h"
#include "../lib/benchmark.h"


//
// Fibonacci Computing Functions
//
// Note: These all assume that the index is 1 or greater, and isn't big enough
// to result in an answer that overflows an unsigned, 64-bit integer.
//


typedef uint64_t ANSWER_TYPE;


ANSWER_TYPE fibonacci_recursive(const size_t index) {
    if(index == 1 || index == 2) return 1;

    return fibonacci_recursive(index - 1) + fibonacci_recursive(index - 2);
}

ANSWER_TYPE fibonacci_loop(const size_t index) {
    if(index == 1 || index == 2) return 1;

    ANSWER_TYPE a = 1;
    ANSWER_TYPE b = 1;
    ANSWER_TYPE answer = 0;

    for(size_t i = 3; i <= index; i++) {
        answer = a + b;
        a = b;
        b = answer;
    }

    return answer;
}

ANSWER_TYPE fibonacci_memoized(const size_t index, ANSWER_TYPE* memory, size_t memory_length) {
    return 0; // Not finished!
}


// Main


int main(int argc, char** argv) {

    const size_t memoize_memory_length = 4096;
    ANSWER_TYPE* memoize_memory = malloc(memoize_memory_length * sizeof(ANSWER_TYPE));
    if(memoize_memory == NULL) {
        printf("Could not initialize memoization memory!");
        EXIT_FAIL();
    }

    for(size_t i = 1; i <= 10; i++) {
        printf("fibonacci_recursive(%zu) = %llu\n", i, fibonacci_recursive(i));
        printf("fibonacci_loop(%zu) = %llu\n", i, fibonacci_loop(i));
        printf("fibonacci_memoize(%zu) = %llu\n", i, fibonacci_memoized(i, memoize_memory, memoize_memory_length));
    }

    free(memoize_memory);

    exit(0);
}