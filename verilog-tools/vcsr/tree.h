// Parse tree stuff

struct expr
  {
  // Stuff copied from token
  int t;		// Token: operator or constant
  char *str;		// Binary numeric constant or string
  int len;		// String length (number size)

  // Sides of operators
  struct expr *l;	// Left side
  struct expr *r;	// Right side (or single operand)
  struct expr *rr;	// Far right side (for ? operator)

  // Rest of line when this operator was found
  char *rest_of_line;
  };

struct expr *cons(int t);		// Construct a node
struct expr *cons_copy(int t);		// Construct a constant (copy from current token)
struct expr *dup_expr(struct expr *);	// Duplicate
void show_expr(int ind,struct expr *);	// Show

int find_field(struct expr *e,char *name);
struct expr *find_port(struct expr *e,char *name);
