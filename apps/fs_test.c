#include <fs/filesystem.h>
#include <stdio.h>
#include <string.h>

void fs_test() {
    printf("Starting filesystem test...\n");
    
    // check if root filesystem is initialized
    if (!filesystem_root) {
        printf("Error: Root filesystem not initialized!\n");
        return;
    }
    
    printf("Root filesystem present at %p\n", filesystem_root);
    
    // try to list root directory
    printf("\nListing root directory:\n");
    printf("====================\n");
    
    struct Dirent *dir;
    int i = 0;
    while ((dir = read_directory(filesystem_root, i++)) != 0) {
        printf("  %s\n", dir->name);
    }
    
    printf("\nFilesystem test completed.\n");
}

int main(int argc, char **argv) {
    fs_test();
    return 0;
}
