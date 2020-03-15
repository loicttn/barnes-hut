#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "simulation.h"

#include <GL/glut.h>

static void print_help(void)
{
    printf("USAGE:\n\t./barnes_hut n\n\nPARAMETERS:\n\tn\t"
           "number of bodies in the galaxy\n");
}

int main(int argc, char **argv)
{
    if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        print_help();
        return 0;
    }
    run_simulation(atoi(argv[1]));
    return 0;
}
