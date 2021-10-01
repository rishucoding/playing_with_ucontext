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

long long int func1_stack[512];
int gl_rec_cnt = 3;
int cmd = 0;

void foo(){
cmd = 1;
}


static void func1(void* arg){
    
    int a = (int *)arg;
    if(cmd == 0){
        printf("func1 - value of a is: %d\n", a);
    }
    else{
        printf("dummy - value of a is: %d\n", a);
    }

    if(a==3){
        foo(); // this would do context switch
    }

    if(a==5){
        printf("dummy\n");
        printf("Exit Value of A = %d\n",a);
        return NULL;
    }
    else{
        func1((void *)(a+1));
        if(cmd == 0){
            printf("func1\n");
        }
        else{
            printf("dummy\n");
        }

        printf("Exit Value of A = %d\n",a);
        return NULL;
    }
    
    //printf("am going to enter the foo funciton: \n");
    //foo();
    printf("func1: returning\n");
}

int main (int argc, char *argv[]){
    char func2_stack[16384];

    if(getcontext(&uctx_func1) == -1){handle_error("getcontext for func1 failed! ");}
    //now my uctx_func1 has the present active context in it. 
    //Lets see where it goes. 
    //printf("am I in infinite looP? \n");
    //setcontext(&uctx_func1);
    uctx_func1.uc_stack.ss_sp = func1_stack;
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
    uctx_func1.uc_link = &uctx_main;
    makecontext(&uctx_func1, func1, 1, (void *)(1));
    if(swapcontext(&uctx_main, &uctx_func1) == -1){handle_error("swapcontext for main --> func2 failed");}
    
    printf("main: exiting\n");
    exit(EXIT_SUCCESS);


}



