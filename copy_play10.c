// Just getcontext() and setcontext()
//
// All the lines between if() and setcontext() would be part of the infinite loop. 
// I learned to identify the loop boundaries properly.

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

static ucontext_t uctx_main, uctx_func1, uctx_func2, uctx_dummy;
//These are the user context for main function, and two other functions.

// Interesting: one line function for handling the error
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

long long int func1_stack[16384];
long long int func1_dummy_stack[16384];
int gl_rec_cnt = 3;
int cmd = 0;

void foo(){
    if(getcontext(&uctx_dummy) == -1){handle_error("getcontext for func1 failed! ");}
    long long int *ptr = (uctx_dummy.uc_mcontext.gregs[10] + 8); // RBP + 8 is address of RA
    uctx_dummy.uc_mcontext.gregs[16] = *ptr; // setting RIP to next inst after foo() 
    
    //change RA of dummy context so to return to main after exiting func1
    int j = 0;
    for(j = 0;j < 16384; j++){
        func1_dummy_stack[j] = func1_stack[j];
    }

    //set the correct RBP and RSP
    long long int *rbp = (uctx_dummy.uc_mcontext.gregs[10]);//stores RBP of func1 active stack frame
    rbp = (*rbp);
    long long int *rsp = (uctx_dummy.uc_mcontext.gregs[15]);//stores RSP of func1 active stack frame
    int index_rbp = 0;
    int index_rsp = 0;
    //printf("Start of array is: %llx\n", func1_stack);
    //printf("RBP stores: %llx", rbp);
    //printf("Value at RBP address is: %llx", *rbp);
    for( j = 0; j < 16384; j++ ){ // opt by rev search
        if(*(rbp) == func1_dummy_stack[j]){
            index_rbp = j;
            //printf("RBP index is: %d\n", index_rbp);
            break;
        }
    }
    
    for( j = 0; j < 16384; j++ ){ // opt by rev search
        if(*(rsp) == func1_dummy_stack[j]){
            index_rsp = j;
            //printf("RSP index is: %d\n", index_rsp);
            break;
        }
    }
    uctx_dummy.uc_stack.ss_sp = func1_dummy_stack;
    uctx_dummy.uc_stack.ss_size = sizeof(func1_dummy_stack);
    uctx_dummy.uc_link = &uctx_main;
    int offset = rbp - func1_stack; // this also works as rbp finally points in the stack only
    uctx_dummy.uc_mcontext.gregs[10] = func1_dummy_stack + offset; // set the RBP
    uctx_dummy.uc_mcontext.gregs[15] = func1_dummy_stack + index_rsp; // set the RSP

    cmd = 1;
    //if(swapcontext(&uctx_main, &uctx_dummy) == -1){handle_error("swapcontext for main --> func2 failed");}
    setcontext(&uctx_dummy);
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
        //printf("LINE AFTER FOO! \n");
        //printf("value of a is: %d \n", a);
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



