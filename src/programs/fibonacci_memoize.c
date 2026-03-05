

// Libraries


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


// Local Library


#include "../lib/compiler_attributes.h"
#include "../lib/debug.h"
#include "../lib/unit_testing.h"
#include "../lib/benchmark.h"


//
// Fibonacci Computing Functions
//
// Note: These all assume that the index is 0 or greater, and isn't big enough
// to result in an answer that overflows an unsigned, 64-bit integer.
//


typedef uint64_t ANSWER_TYPE;


ANSWER_TYPE fibonacci_recursive(const size_t index) {
    if(index == 0 || index == 1) return 1;

    return fibonacci_recursive(index - 1) + fibonacci_recursive(index - 2);
}

ANSWER_TYPE fibonacci_loop(const size_t index) {
    if(index == 0 || index == 1) return 1;

    ANSWER_TYPE a = 1;
    ANSWER_TYPE b = 1;
    ANSWER_TYPE answer = 0;

    for(size_t i = 2; i <= index; i++) {
        answer = a + b;
        a = b;
        b = answer;
    }

    return answer;
}

ANSWER_TYPE fibonacci_memoized(const size_t index, ANSWER_TYPE* memory, const size_t memory_length) {
    const size_t memory_index = index;
    if(memory_index < memory_length && memory[memory_index] > 0) return memory[memory_index];

    ANSWER_TYPE answer;

    if(index == 0 || index == 1) {
        answer = 1;
    }
    else {
        answer = fibonacci_memoized(index - 1, memory, memory_length) + fibonacci_memoized(index - 2, memory, memory_length);
    }

    if(memory_index < memory_length) {
        memory[memory_index] = answer;
    }

    return answer;
}


// Main


int main(int argc, char** argv) {

    //
    // Setup
    //

    const size_t memoize_memory_length = 4096;
    ANSWER_TYPE* memoize_memory = malloc(memoize_memory_length * sizeof(ANSWER_TYPE));
    if(memoize_memory == NULL) {
        printf("Could not initialize memoization memory!");
        EXIT_FAIL();
    }
    memset(memoize_memory, 0, memoize_memory_length);

    //
    // Show output from each method
    //

    for(size_t i = 0; i <= 9; i++) {
        printf("fibonacci_recursive(%zu) = %llu\n", i, fibonacci_recursive(i));
        printf("fibonacci_loop(%zu) = %llu\n", i, fibonacci_loop(i));
        printf("fibonacci_memoize(%zu) = %llu\n", i, fibonacci_memoized(i, memoize_memory, memoize_memory_length));
        printf("(again) fibonacci_memoize(%zu) = %llu\n", i, fibonacci_memoized(i, memoize_memory, memoize_memory_length));

        printf("\n");
    }

    //
    // Tests
    //

    // Tests to make sure fibonacci_recursive returns the correct output

    SECTION("fibonacci_recursive");

    TEST( fibonacci_recursive(0) == 1, "fibonacci_recursive(0) = 1");
    TEST( fibonacci_recursive(1) == 1, "fibonacci_recursive(1) = 1");
    TEST( fibonacci_recursive(2) == 2, "fibonacci_recursive(2) = 2");
    TEST( fibonacci_recursive(5) == 8, "fibonacci_recursive(5) = 8");

    // Tests to make sure fibonacci_memoized returns the correct output
    // (uses fibonacci_recursive() as the "source of truth")

    SECTION("fibonacci_memoized");

    ANSWER_TYPE correct_answer;

    for(size_t i = 0; i < 15; i++) {
        correct_answer = fibonacci_recursive(i);
        TEST(fibonacci_memoized(i, memoize_memory, memoize_memory_length) == correct_answer, "fibonacci_memoized() matches fibonacci_recursive()");
    }

    size_t index_to_test = 16;
    correct_answer = fibonacci_recursive(index_to_test);
    TEST(fibonacci_memoized(index_to_test, NULL, 0) == correct_answer, "fibonacci_memoized() still works with no memory for memoization");

    show_test_results();
    printf("\n\n");

    //
    // Benchmarking
    //

    // Benchmark all three
    {
        const size_t indexes_to_test[] = { 16, 20, 22 };
        const size_t num_tests = sizeof(indexes_to_test) / sizeof(size_t);
        const int num_benchmark_loops = 50000;

        for(size_t i = 0; i < num_tests; i++) {
            index_to_test = indexes_to_test[i];
            printf("Index: %zu\n", index_to_test);
            BENCHMARK_LOOPS(fibonacci_recursive(index_to_test), "fibonacci_recursive()", num_benchmark_loops);
            BENCHMARK_LOOPS(fibonacci_loop(index_to_test), "fibonacci_loop()", num_benchmark_loops);
            BENCHMARK_LOOPS(fibonacci_memoized(index_to_test, memoize_memory, memoize_memory_length), "fibonacci_memoized()", num_benchmark_loops);
            printf("\n");
        }
    }

    // Benchmark only fibonacci_loop() and fibonacci_memoized() for larger indexes
    {
        const size_t indexes_to_test[] = { 30, 40, 55 };
        const size_t num_tests = sizeof(indexes_to_test) / sizeof(size_t);
        const int num_benchmark_loops = 50000;

        for(size_t i = 0; i < num_tests; i++) {
            index_to_test = indexes_to_test[i];
            printf("Index: %zu\n", index_to_test);
            BENCHMARK_LOOPS(fibonacci_loop(index_to_test), "fibonacci_loop()", num_benchmark_loops);
            BENCHMARK_LOOPS(fibonacci_memoized(index_to_test, memoize_memory, memoize_memory_length), "fibonacci_memoized()", num_benchmark_loops);
            printf("\n");
        }
    }

    free(memoize_memory);

    exit(0);
}