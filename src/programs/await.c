
#include <stdio.h>

#include "../lib/sthread.h"

struct my_await_function_data {
    int input;
    int output;
};

void my_await_function(void* input) {
    struct my_await_function_data* data = (struct my_await_function_data*) input;

    data->output = data->input + 1;
}

int main() {

    printf("\n\n");

    struct my_await_function_data data;

    data.input = 5;

    printf("Spinning off asynchronous task...\n");
    struct async_exec_data async_data = run_asynchronous(my_await_function, &data);

    // Do other stuff...
    printf("Doing some other stuff in the mean time while the other thread runs...\n");

    printf("Awaiting asynchronous thread...\n");
    await_async_function(async_data);
    int result = data.output;

    printf("Output: %d \n", result);

}