#include <libc/stdio.h>
#include <fs/filesystem.h>

// forward declaration of our test function
extern void fs_test();

int main(int argc, char **argv)
{
    printf("LumaOS Initialization Process\n");
    printf("============================\n\n");
    
    // run filesystem test
    printf("Running filesystem test...\n");
    fs_test();
    
    printf("\nInitialization complete. Starting shell...\n\n");
    
    // start the shell or main system process
    while(1) {
        // this is where the shell would run
        __asm__ volatile("hlt");
    }

    return 0;
}
