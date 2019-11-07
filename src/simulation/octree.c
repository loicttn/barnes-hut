#include "simulation.h"

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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

/* FUNCTION: get_body_interval
 * --------------------------------
 * gets the position of the body in the current 3D-cube
 * 
 * the position returned is the 3D sub-cube index in which the body is
 *      * BSW : Bottom-South-West
 *      * BSE : Bottom-South-East
 *      * BNW : Bottom-North-West
 *      * BNE : Bottom-North-East
 *      * TSW : Top-South-West
 *      * TSE : Top-South-East
 *      * TNW : Top-North-West
 *      * TNE : Top-North-East
 *
 * body: body to check if in range
 * min: minimum of the 3D interval
 * max: maximum of the 3D interval
 * 
 * returns: the position of the body in the current 3D-cube
 */
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
    // returns -1 only if body is not in the 3D cube defined with parameters
    return -1;
}

/* FUNCTION: move_to_parent_node
 * --------------------------------------
 * creates a parent node in tree->child[position] and
 * copies the initial content (type CHILD) to one of 
 * the new parent children in the good sub-cube
 * 
 * tree: parent node where tree->child[position] will
 * be moved to parent-type node
 * 
 * position: position of the child to transform to parent node
 */
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
    tree->child[position]->child[new_pos]->parent = tree->child[position];
}

/* FUNCTION: insert_node
 * --------------------------------------
 * insert a body in the octree recursively
 * 
 * tree: root of the octree (the root must have a type PARENT)
 * body: body to insert in the octree
 */
static void insert_node(oct_node_t *tree, body_t *body)
{
    int position = get_body_interval(body, tree->min, tree->max);

    if (tree->child[position]) {
        if (tree->child[position]->type == CHILD)
            move_to_parent_node(tree, position); 
        insert_node(tree->child[position], body);
    } else {
        tree->child[position] = (oct_node_t *)calloc(sizeof(oct_node_t), 1);
        tree->child[position]->body = body;
        tree->child[position]->type = CHILD;
        tree->child[position]->parent = tree;
        // copy data to current node so that when we will calculate the forces
        // they will be calculated based on the position of bodies before
        // the force are applied on bodies
        tree->child[position]->weight = tree->child[position]->body->weight;
        tree->child[position]->position[X] = tree->child[position]->body->x;
        tree->child[position]->position[Y] = tree->child[position]->body->y;
        tree->child[position]->position[Z] = tree->child[position]->body->z;
    }
}

/* FUNCTION: clean_tree
 * --------------------------------------
 * cleans an octree recursively
 * 
 * tree: root of the octree (the root must have a type PARENT)
 */
void clean_tree(oct_node_t *tree)
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

/* FUNCTION: create_octree
 * --------------------------------------
 * initiates the octree in which all the calculus are going to be made
 *
 * bodies: list of all the bodies in the universe
 * 
 * returns: the pointer of the octree root
 */
oct_node_t *create_octree(bodies_t *bodies)
{
    oct_node_t *tree = (oct_node_t *)calloc(sizeof(oct_node_t), 1);

    tree->min[X] = 0;
    tree->min[Y] = 0;
    tree->min[Z] = 0;
    tree->max[X] = GALAXY_SIZE;
    tree->max[Y] = GALAXY_SIZE;
    tree->max[Z] = GALAXY_SIZE;
    tree->type = PARENT;
    while (bodies) {
        insert_node(tree, bodies->body);
        bodies = bodies->next;
    }
    return tree;
}

/* FUNCTION: calculate_node_gravity_center
 * --------------------------------------
 *
 * calculates the gravity cented of each parent node of the octree
 * 
 * sets node->weight as the sum of all children masses
 * sets node->postion as the weighted average of the 3D position (weighted
 * with the weight of each child body)
 * 
 * tree: parent node of an octree
 * 
 */
void calculate_nodes_gravity_center(oct_node_t *tree)
{
    for (int i = 0; i < 8; i += 1) {
        if (tree->child[i] == NULL)
            continue;
        if (tree->child[i]->type == PARENT)
            calculate_nodes_gravity_center(tree->child[i]);
        tree->weight += tree->child[i]->weight;
        tree->position[X] += tree->child[i]->weight * tree->child[i]->position[X];
        tree->position[Y] += tree->child[i]->weight * tree->child[i]->position[Y];
        tree->position[Z] += tree->child[i]->weight * tree->child[i]->position[Z];
    }
    if (tree->weight == 0)
        exit(1);
    tree->position[X] /= tree->weight;
    tree->position[Y] /= tree->weight;
    tree->position[Z] /= tree->weight;
}

static void apply_forces_on_node(oct_node_t *mover, oct_node_t *attractor)
{
    
}

/*
 * calculates the quotient s / d, where s is the width of the region
 * represented by the internal node, and d is the distance between the body
 * and the node's center of mass.
 */
static float get_action_ratio(oct_node_t *mover, oct_node_t *attractor)
{
    // width of the region (as each parent region is a cube, we will get its
    // value from the absolute value of the difference between min[X] and max[X])
    // if attractor is CHILD type, s = 1.0
    float s = 1.0;
    // distance between mover and attractor, 3d distance formula is
    // 	√((x2 - x1)² + (y2 - y1)² + (z2 - z1)²)
    float d; 

    if (attractor->type == PARENT)
        s = fabs(attractor->max[X] - attractor->min[X]);
    d = (float)pow((double)(attractor->position[X] - mover->position[X]), 2) +
        (float)pow((double)(attractor->position[Y] - mover->position[Y]), 2) +
        (float)pow((double)(attractor->position[Z] - mover->position[Z]), 2);
    d = sqrtf(d);
    return s / d;
}

// node must be CHILD and parent must be PARENT type
static void get_forces_on_node(oct_node_t *node, oct_node_t *parent)
{
    float action_ratio;

    for (int i = 0; i < 8; i += 1) {
        if (parent->child[i] == NULL || parent->child[i] == node)
            continue;
        action_ratio = get_action_ratio(node, parent->child[i]);
        if (action_ratio < THETA)
            apply_forces_on_node(node, node->parent->child[i]);
        else if (node->parent->child[i]->type == PARENT)
            get_forces_on_node(node, parent->child[i]);
    }
}


void run_forces(oct_node_t *tree)
{
    for (int i = 0; i < 8; i += 1) {
        if (tree->child[i] == NULL)
            continue;
        if (tree->child[i]->type == PARENT)
            run_forces(tree->child[i]);
        else
            get_forces_on_node(tree->child[i], tree);
    }
}