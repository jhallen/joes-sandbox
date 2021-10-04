#include <stdio.h>
#include "tree.h"

void ind(int n)
{
    while (n--) printf(" ");
}

void print_tree(struct node *n)
{
    if (n)
    {
        switch (n->what)
        {
            case ncCHAR: printf("(char)%ld", n->i); break;
            case ncUCHAR: printf("(unsigned char)%llu", (unsigned long long)n->i); break;

            case ncSHORT: printf("(short)%ld", n->i); break;
            case ncUSHORT: printf("(unsigned short)%llu", (unsigned long long)n->i); break;

            case ncLONG: printf("(long)%ld", n->i); break;
            case ncULONG: printf("(unsigned long)%llu", (unsigned long long)n->i); break;

            case ncFLOAT: printf("(float)%g", n->f); break;
            case ncDOUBLE: printf("(double)%g", n->d); break;

            case ncSTRING: printf("\"%s\"", n->s); break;

            case ncNAME: printf("%s", n->s); break;

            case ntCHAR: printf("char"); break;
            case ntUCHAR: printf("unsigned char"); break;
            case ntSHORT: printf("short"); break;
            case ntUSHORT: printf("unsigned short"); break;
            case ntLONG: printf("long"); break;
            case ntULONG: printf("unsigned long"); break;
            case ntFLOAT: printf("float"); break;
            case ntDOUBLE: printf("double"); break;
            case ntPTR: printf("* "); print_tree(n->r); break;
            case ntARRAY: printf("[%zu] ", n->siz); print_tree(n->r); break;
            case ntSTRUCT: printf("struct %s { ", n->s ? n->s : "NULL"); print_tree(n->r); printf(" }"); break;
            case ntUNION: printf("union %s { ", n->s ? n->s : "NULL"); print_tree(n->r); printf(" }"); break;

            case nLIST: print_tree(n->l); printf(", "); print_tree(n->r); break;
            case nDECL: printf("DCL %s ", n->s); print_tree(n->r); break;
            case nNOTHING: printf("nothing"); break;

            default: printf("UNKNOWN<%d>", n->what); break;
        }
    }
    else
    {
        printf("NULL");
    }
}

Node *list_append(Node *l, Node *r)
{
    if (!l) return r;
    else {
        l->r = list_append(l->r, r);
        return l;
    }
}

struct nodelist {
    struct nodelist *next;
    char *name;
    Node *body;
};

Node *node_lookup(struct nodelist *l, char *s)
{
    while (l)
        if (!strcmp(l->name, s))
            return l->body;
        else
            l = l->next;
    return 0;
}

struct nodelist *node_add(struct nodelist *l, char *s, Node *body)
{
    struct nodelist *n = (struct nodelist *)malloc(sizeof(struct nodelist));
    n->next = l;
    n->name = s;
    n->body = body;
    return n;
}

void show_nodes(struct nodelist *l)
{
    while (l)
    {
        printf("%s: ", l->name);
        print_tree(l->body);
        printf("\n");
        l = l->next;
    }
}

// Typedef table

struct nodelist *typedefs;

Node *lookup_typedef(char *s)
{
    return node_lookup(typedefs, s);
}

int add_typedef(char *s, Node *body)
{
    if (lookup_typedef(s))
        return -1;
    else {
        typedefs = node_add(typedefs, s, body);
        return 0;
    }
}

void show_typedefs()
{
    show_nodes(typedefs);
}

// Struct table

struct nodelist *structs;

Node *lookup_struct(char *s)
{
    return node_lookup(structs, s);
}

int add_struct(char *s, Node *body)
{
    if (lookup_struct(s))
        return -1;
    else {
        structs = node_add(structs, s, body);
        return 0;
    }
}

void show_structs()
{
    show_nodes(structs);
}

// Union table

struct nodelist *unions;

Node *lookup_union(char *s)
{
    return node_lookup(unions, s);
}

int add_union(char *s, Node *body)
{
    if (lookup_union(s))
        return -1;
    else {
        unions = node_add(unions, s, body);
        return 0;
    }
}

void show_unions()
{
    show_nodes(unions);
}
