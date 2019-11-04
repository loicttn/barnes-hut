#include <stdlib.h>
#include <stdbool.h>

#include "simulation.h"

#include <stdio.h>

/* FUNCTION: body_in_range
 * --------------------------------
 * checks if the given body is in the 3D range min -> max
 * 
 * body: body to check if in range
 * min: minimum of the 3D interval
 * max: maximum of the 3D interval
 * 
 * returns: true if body is in range
 */
static bool body_in_range(body_t *body, float min_x, float max_x,
                                        float min_y, float max_y,
                                        float min_z, float max_z)
{
    bool res;

    res = body->x >= min_x && body->x <= max_x;
    res = res && body->y >= min_y && body->y <= max_y;
    res = res && body->z >= min_z && body->z <= max_z;

    return res;
}

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
        tail->body->weight = rand();
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


// get body's interval in current context (min and max)
static int get_body_interval(body_t *body, float min[3], float max[3])
{
    if (body_in_range(body, min[X], min[X] + (max[X] - min[X]) / 2,
                            min[Y], min[Y] + (max[Y] - min[Y]) / 2,
                            min[Z], min[Z] + (max[Z] - min[Z]) / 2))
        return BSW;
    if (body_in_range(body, min[X] + (max[X] - min[X]) / 2, max[X],
                            min[Y], min[Y] + (max[Y] - min[Y]) / 2,
                            min[Z], min[Z] + (max[Z] - min[Z]) / 2))
        return BSE;
    if (body_in_range(body, min[X], min[X] + (max[X] - min[X]) / 2,
                            min[Y] + (max[Y] - min[Y]) / 2, max[Y],
                            min[Z], min[Z] + (max[Z] - min[Z]) / 2))
        return BNW;
    if (body_in_range(body, min[X] + (max[X] - min[X]) / 2, max[X],
                            min[Y] + (max[Y] - min[Y]) / 2, max[Y],
                            min[Z], min[Z] + (max[Z] - min[Z]) / 2))
        return BNE;
    if (body_in_range(body, min[X], min[X] + (max[X] - min[X]) / 2,
                            min[Y], min[Y] + (max[Y] - min[Y]) / 2,
                            min[Z] + (max[Z] - min[Z]) / 2, max[Z]))
        return TSW;
    if (body_in_range(body, min[X] + (max[X] - min[X]) / 2, max[X],
                            min[Y], min[Y] + (max[Y] - min[Y]) / 2,
                            min[Z] + (max[Z] - min[Z]) / 2, max[Z]))
        return TSE;
    if (body_in_range(body, min[X], min[X] + (max[X] - min[X]) / 2,
                            min[Y] + (max[Y] - min[Y]) / 2, max[Y],
                            min[Z] + (max[Z] - min[Z]) / 2, max[Z]))
        return TNW;
    if (body_in_range(body, min[X] + (max[X] - min[X]) / 2, max[X],
                            min[Y] + (max[Y] - min[Y]) / 2, max[Y],
                            min[Z] + (max[Z] - min[Z]) / 2, max[Z]))
        return TNE;
    return -1;
}

static void move_to_parent_node(oct_node_t *tree, int position)
{
    oct_node_t *save = tree->child[position];
    int new_pos;

    tree->child[position] = NULL;
    tree->child[position] = (oct_node_t *)calloc(sizeof(oct_node_t), 1);
    tree->child[position]->type = PARENT;
    switch (position) {
        case BSW:
            tree->child[position]->min[X] = tree->min[X];
            tree->child[position]->min[Y] = tree->min[Y];
            tree->child[position]->min[Z] = tree->min[Z];
            tree->child[position]->max[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->max[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->max[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            break;
        case BSE:
            tree->child[position]->min[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->min[Y] = tree->min[Y];
            tree->child[position]->min[Z] = tree->min[Z];
            tree->child[position]->max[X] = tree->max[X];
            tree->child[position]->max[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->max[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            break;
        case BNW:
            tree->child[position]->min[X] = tree->min[X];
            tree->child[position]->min[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->min[Z] = tree->min[Z];
            tree->child[position]->max[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->max[Y] = tree->max[Y];
            tree->child[position]->max[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            break;
        case BNE:
            tree->child[position]->min[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->min[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->min[Z] = tree->min[Z];
            tree->child[position]->max[X] = tree->max[X];
            tree->child[position]->max[Y] = tree->max[Y];
            tree->child[position]->max[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            break;
        case TSW:
            tree->child[position]->min[X] = tree->min[X];
            tree->child[position]->min[Y] = tree->min[Y];
            tree->child[position]->min[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            tree->child[position]->max[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->max[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->max[Z] = tree->max[Z];
            break;
        case TSE:
            tree->child[position]->min[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->min[Y] = tree->min[Y];
            tree->child[position]->min[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            tree->child[position]->max[X] = tree->max[X];
            tree->child[position]->max[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->max[Z] = tree->max[Z];
            break;
        case TNW:
            tree->child[position]->min[X] = tree->min[X];
            tree->child[position]->min[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->min[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            tree->child[position]->max[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->max[Y] = tree->max[Y];
            tree->child[position]->max[Z] = tree->max[Z];
            break;
        case TNE:
            tree->child[position]->min[X] = tree->min[X] + (tree->max[X] - tree->min[X]) / 2;
            tree->child[position]->min[Y] = tree->min[Y] + (tree->max[Y] - tree->min[Y]) / 2;
            tree->child[position]->min[Z] = tree->min[Z] + (tree->max[Z] - tree->min[Z]) / 2;
            tree->child[position]->max[X] = tree->max[X];
            tree->child[position]->max[Y] = tree->max[Y];
            tree->child[position]->max[Z] = tree->max[Z];
            break;
    }
    new_pos = get_body_interval(save->body,
                                tree->child[position]->min,
                                tree->child[position]->max);
    tree->child[position]->child[new_pos] = save; 
}

// here tree must be a PARENT
static void insert_node(oct_node_t *tree, body_t *body)
{
    int position;

    position = get_body_interval(body, tree->min, tree->max);
    if (position == -1)
        return;
    if (tree->child[position] && tree->child[position]->type == PARENT)
        insert_node(tree->child[position], body);
    else if (tree->child[position] && tree->child[position]->type == CHILD) {
        move_to_parent_node(tree, position); 
        insert_node(tree->child[position], body);
    }
    else {
        tree->child[position] = calloc(sizeof(oct_node_t), 1);
        tree->child[position]->body = body;
        tree->child[position]->type = CHILD;
    }
}

/* FUNCTION: create_octree
 * --------------------------------------
 * initiates the octree in which all the calculus are going to be made
 *
 * bodies: list of all the bodies in the universe
 * 
 * returns: the pointer of the octree root
 */
static oct_node_t *create_octree(bodies_t *bodies, int nb_bodies)
{
    oct_node_t *tree = calloc(sizeof(oct_node_t), 1);

    tree->min[X] = 0;
    tree->min[Y] = 0;
    tree->min[Z] = 0;
    tree->max[X] = GALAXY_SIZE;
    tree->max[Y] = GALAXY_SIZE;
    tree->max[Z] = GALAXY_SIZE;
    tree->type = PARENT;
    tree->node_weight = nb_bodies;
    while (bodies) {
        insert_node(tree, bodies->body);
        bodies = bodies->next;
    }
    return tree;
}

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

static void clean_tree(oct_node_t *tree)
{
    for (int i = 0; i < 8; i += 1)
        if (tree->child[i] == NULL)
            continue;
        else if (tree->child[i]->type == CHILD)
            free(tree->child[i]);
        else
            clean_tree(tree->child[i]);
    free(tree);
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
 * n: number of bodies to include (at random position with random mass) in the
 * universe
 * 
 * returns: nothing, returns when simulation is over
 */
void run_simulation(int n)
{
    bodies_t *bodies = init_bodies(n);
    oct_node_t *octree;

    for (int i = 0; i < 1; i += 1) {
        octree = create_octree(bodies, n);
        printf(" P\n");
        log(octree, 0);
    //     run_forces(octree, bodies);
    //     display(bodies);
    //     display_debug(octree);
        clean_tree(octree);
        octree = NULL;
    }
    clean_bodies(bodies);
    bodies = NULL;
}