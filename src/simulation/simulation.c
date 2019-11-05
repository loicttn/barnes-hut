#include <stdlib.h>
#include <stdbool.h>

#include "simulation.h"

#include <stdio.h>

/* FUNCTION: init_bodies
 * --------------------------------
 * initiates all the bodies in the universe
 *
 * n: number of bodies to include (at random position with random mass) in the
 * universe
 * 
 * returns: a simple linked list of bodies_t containing all the created bodies
 */
static bodies_t *init_bodies(int n)
{
    bodies_t *head = (bodies_t *)calloc(sizeof(bodies_t), 1);
    bodies_t *tail = head;

    do {
        tail->body = (body_t *)calloc(sizeof(body_t), 1);
        tail->body->weight = rand() % 1000 + 100;
        tail->body->x = (float)(rand() % GALAXY_SIZE);
        tail->body->y = (float)(rand() % GALAXY_SIZE);
        tail->body->z = (float)(rand() % GALAXY_SIZE);
        if (n) {
            tail->next = (bodies_t *)calloc(sizeof(bodies_t), 1);
            tail = tail->next;
        }
    } while (n--);
    return head;
}

/* FUNCTION: clean_bodies
 * --------------------------------------
 * cleans all bodies in bodies_t linked list
 *
 * bodies: list of bodies to cleanup
 */
static void clean_bodies(bodies_t *body)
{
    if (body->next)
        clean_bodies(body->next);
    free(body->body);
    free(body);
}

/* FUNCTION: log
 * --------------------------------------
 * display the octree with the following format :
 *      * P for a parent node
 *      * C for a child node
 *      * . for an empty child
 * displays line per line and then indent for each
 * parent node
 *
 * bodies: list of bodies to cleanup
 */
static void log(oct_node_t *tree, int n)
{
    for (int i = 0; i < n; i += 1)
        printf("\t");
    for (int i = 0; i < 8; i += 1) {
        if (tree->child[i] == NULL)
            printf(" . ");
        else if (tree->child[i]->type == PARENT)
            printf(" P ");
        else
            printf(" C ");
    }
        printf("\n");
    for (int i = 0; i < 8; i += 1) {
        if (tree->child[i] && tree->child[i]->type == PARENT) {
            log(tree->child[i], n + 1);
        }
    }
}

/* FUNCTION: run_simulation
 * --------------------------------------
 * rules all the step of the simulation
 *      *  initialization of bodies
 *      *  main loop :
 *          *  create octree
 *          *  make the calculation and update the bodies
 *          *  display
 *          *  clean the octree
 *      *  clean all
 *
 * n: number of bodies to include (at random position
 * with random weight) in the universe
 */
void run_simulation(int n)
{
    bodies_t *bodies = init_bodies(n);
    oct_node_t *octree;

    for (int i = 0; i < 1; i += 1) {
        octree = create_octree(bodies, n);
        log(octree, 0);
        // run_forces(octree, bodies);
        // display(bodies);
        // display_debug(octree);
        clean_tree(octree);
        octree = NULL;
    }
    clean_bodies(bodies);
    bodies = NULL;
}