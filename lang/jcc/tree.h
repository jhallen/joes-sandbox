/* Syntax tree */

#include <stdlib.h>
#include <string.h>

typedef struct node {
    int what;
    union {
        double d;
        float f;
        long i;
        unsigned long u;
        struct {
            union {
                char *s; // String or name pointer
                size_t siz; // Array size
                struct node *l;
            };
            union {
                size_t len; // String length
                struct node *r;
            };
        };
    };
} Node;

enum {
    // Constants
    ncCHAR,
    ncUCHAR,
    ncSHORT,
    ncUSHORT,
    ncLONG,
    ncULONG,
    ncFLOAT,
    ncDOUBLE,
    ncSTRING,
    ncNAME,

    // Types
    ntVOID,
    ntCHAR,
    ntUCHAR,
    ntSHORT,
    ntUSHORT,
    ntLONG,
    ntULONG,
    ntFLOAT,
    ntDOUBLE,
    ntPTR, /* Pointer to type in r */
    ntARRAY, /* Array: type in r, array size in siz */
    ntSTRUCT, /* Structure: list of declarations in r, Name in s */
    ntUNION, /* Union: list of declarations in r, Name in s */

    // Declaration
    nDECL, /* Type in r, Name in s */
    nLIST, /* Content in l, Next in r */
    nNOTHING, /* There is nothing here. */
};

//#define cons(...) ({ Node *n = (Node *)malloc(sizeof(Node)); *n = (Node){ __VA_ARGS__ }; n; })
#define cons(...) (Node *)memcpy(malloc(sizeof(Node)), &(Node){ __VA_ARGS__ }, sizeof(Node))

void print_tree(Node *n);
Node *list_append(Node *l, Node *r);

// Typedef table

Node *lookup_typedef(char *s);
int add_typedef(char *s, Node *body);
void show_typedefs();

// Struct table

Node *lookup_struct(char *s);
int add_struct(char *s, Node *body);
void show_structs();

// Union table

Node *lookup_union(char *s);
int add_union(char *s, Node *body);
void show_unions();
