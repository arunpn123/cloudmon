#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("usage: %s [bytes_to_malloc] [approx_time_to_hold]\n", argv[0]);
        exit(1);
    }

    size_t bytes_to_malloc = atoi(argv[1]);
    int time_to_hold = atoi(argv[2]);

    printf("allocating %d bytes and sleeping for %d seconds\n", bytes_to_malloc, time_to_hold);
    printf("note: do NOT press Ctrl+C or you'll leak the memory!\n");

    char * data = malloc(bytes_to_malloc);

    // write some data in a loop until time_to_hold has elapsed
    // hopefully this will force linux (and xen) to notice we're using
    // the memory.  
    int i, j;
    for(i = 0; i < time_to_hold; i++)
    {
        for(j = 0; j < bytes_to_malloc; j++)
        {
            data[j] = j % 255;
        }

        sleep(1);
    }

    free(data);
    printf("all done.\n");

    return 0;
}

