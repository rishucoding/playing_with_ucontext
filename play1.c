// Just getcontext() and setcontext()
//
// All the lines between if() and setcontext() would be part of the infinite loop. 
// I learned to identify the loop boundaries properly.

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
    //now my uctx_func1 has the present active context in it. 
    //Lets see where it goes. 
    printf("am I in infinite looP? \n");
    setcontext(&uctx_func1); 
    
    printf("main: exiting\n");
    exit(EXIT_SUCCESS);


}



