// A parameter or defparam

struct param
  {
  struct param *next;
  char *inst;			// Instance name (for defparams)
  char *name;			// Parameter name
  struct expr *expr;		// Parameter's value
  struct param *context;	// Evaluation context for this value
  };

// A declaration

struct decl
  {
  struct decl *next;
  char *name;			// wire name
  struct expr *top;		// [top:bot]
  struct expr *bot;		// signal is one bit if top==bot or top and bot are missing.
  };

// An instantiation
// Used for both instantiation declarations (within module declarations),
// and actual instantiations after elaboration.  For the latter, the design
// begins with an instance called 'top'- an instance of the top level module.

struct inst
  {
  struct inst *next;
  struct inst *mom;		// Parent (0 if top)
  char *name;			// Instance name
  char *module;			// Name of module to instantiate
  struct module *mod;		// Address of module to instantiate (after linking)
  struct expr *connections;	// Port connections

  struct param *pos_defparams;	// Positional defparams
  struct param *pos_last;	// as in: module #(1,2,3) instance (...) ;

  // After elaboration
  struct decl *decls;		// Signal declarations after elaboration
  struct inst *insts;		// Instiantiations within this one
  struct param *params;		// Parameter values for this instance
  };

// Module declarations

struct module
  {
  struct module *next;
  char *name;			// Module name
  struct inst *insts;		// Instance declarations
  struct inst *insts_last;
  struct decl *decls;		// Declarations before elaboration

  int linked;			// Set if we've been linked
  int refcount;			// Set if somebody else instantiated us
  int instcount;		// No. modules instantiated in this tree

  struct param *params;		// Declared module parameters
  struct param *params_last;

  struct param *defparams;	// Defparams (refer to our insts).
  struct param *defparams_last;
  };

struct module *add_module(char *name);
struct inst *add_inst(char *inst_name,char *mod_name);
void link();
void find_top();
void show_tree(int ind,struct module *m);
struct module *modules;
struct module *top;
extern char select_top[];

struct inst *elaborate(struct param *context,char *inst_name,struct param *defparams,struct param *pdp,struct module *m,struct expr *connections,struct inst *mom);
void show_design(int ind,struct inst *i);

struct decl *find_decl(struct decl *list,char *name);
struct param *find_param(struct param *list,char *name);
