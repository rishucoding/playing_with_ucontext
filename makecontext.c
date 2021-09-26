//How can I swictch between different functions when a program is executing? 
//Let's try to understand the power of "makecontext"

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

static ucontext_t uctx_main, uctx_func1, uctx_func2;
//These are the user context for main function, and two other functions.

// Interesting: one line function for handling the error
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

static void func1(void){
    //dummy function
    printf("func1: started\n");
    printf("func1: am going to do swapcontext(&uctx_func1, &uctx_func2)\n");
    // I think we specify the current context and the new context which we want to be scheduled(program order)
    if (swapcontext(&uctx_func1, &uctx_func2) == -1){ handle_error("swapcontext func1 --> func2 didn't happen! "); }

    printf("func1: returning\n");
}

static void func2(void){
    //dummy function
    printf("func2: started\n");
    printf("func2: am going to do swapcontext(&uctx_func2, &uctx_func1)\n");
    // I think we specify the current context and the new context which we want to be scheduled(program order)
    if (swapcontext(&uctx_func2, &uctx_func1) == -1){ handle_error("swapcontext func2 --> func1 didn't happen! "); }

    printf("func2: returning\n");
}

int main (int argc, char *argv[]){
    char func1_stack[16384];
    char func2_stack[16384];

    if(getcontext(&uctx_func1) == -1){handle_error("getcontext for func1 failed! ");}

    //create the context for func1
    uctx_func1.uc_stack.ss_sp = func1_stack;
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
    uctx_func1.uc_link = &uctx_main;
    //link context to func1
    makecontext(&uctx_func1, func1, 0);

    //let's do the same for func2
    if(getcontext(&uctx_func2) == -1){handle_error("getcontext for func2 failed!");} // ? when would this return -1

    //create the context for func2
    uctx_func2.uc_stack.ss_sp = func2_stack;
    uctx_func2.uc_stack.ss_size = sizeof(func2_stack);
    
    /*Successor context is f1(), unless argc > 1 */ // argc indicates how many arguments are there
    uctx_func2.uc_link = (argc > 1) ? NULL: &uctx_func1;
    makecontext(&uctx_func2, func2, 0);

    printf("main: swapcontext(&uctx_main, &uctx_func2)\n");
    //this is like a try catch block
    if(swapcontext(&uctx_main, &uctx_func2) == -1){handle_error("swapcontext for main --> func2 failed");}

    printf("main: exiting\n");
    exit(EXIT_SUCCESS);


}



