/*
 * =============================== GENERAL ===============================
 */

static const int GALAXY_SIZE = 2000;


void run_simulation(int n);

/*
 * =============================== BODIES ===============================
 */

typedef struct body_s {
    float x;                        // 3D position between 0 and GALAXY_SIZE
    float y;                        // 3D position between 0 and GALAXY_SIZE
    float z;                        // 3D position between 0 and GALAXY_SIZE
    float weight;                   // weight of the body in UNIT
    float velocity[3];              // 3D velocity vector
} body_t;

typedef struct bodies_s {
    body_t *body;
    struct bodies_s *next;
} bodies_t;

/*
 * =============================== OCTREE ===============================
 */

typedef enum type_e {
    CHILD = 0,
    PARENT = 1,
} type_et;

typedef struct oct_node_s {
    type_et type;                   // parent or child node ?
    body_t *body;                   // each node corresponds to a body
    float min[3];                   // min position of the interval
    float max[3];                   // max position of the interval
    float position[3];              // if child => copy of body position
                                    // if parent => average position of children
    float weight;                   // if child => copy of body weight
                                    // if parent => average weight of children
    struct oct_node_s *child[8];    // children
} oct_node_t;


oct_node_t *create_octree(bodies_t *bodies);
void clean_tree(oct_node_t *tree);
void calculate_nodes_gravity_center(oct_node_t *tree);
void run_forces(oct_node_t *tree);


static const int X = 0;              // coordinates in min/max arrays   
static const int Y = 1;              // coordinates in min/max arrays   
static const int Z = 2;              // coordinates in min/max arrays   

static const int BSW = 0;            // Bottom Sud West   
static const int BSE = 1;            // Bottom Sud East   
static const int BNW = 2;            // Bottom North West   
static const int BNE = 3;            // Bottom North East   
static const int TSW = 4;            // Top South West   
static const int TSE = 5;            // Top South East   
static const int TNW = 6;            // Top North West   
static const int TNE = 7;            // Top North East

static const float THETA = 1.0;