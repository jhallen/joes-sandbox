// Verilog tokenizer

// Get next token

struct tok_hist
  {
  struct tok_hist *next, *prev;

  int tok;

  char *file;
  int line;
  char *str;
  int len;
  int col;
  };

int token(void);			// Get next token

struct tok_hist *tok;			// Current token

void unget_tok(struct tok_hist *t);	// Backup

void show_tok(int c);		// Display token in printable form
void error();			// Print error message

struct scanner_state
  {
  struct path *inc_paths;
  char *(*getmore)();
  char *ptr;
  int col;
  int lvl;
  int line;
  char *name;
  int eof;
  FILE *infile;
  char inbuf[4096];
  struct source *source_stack;
  char tok_file[1024];
  int tok_line;
  char tok_str[4096];
  int tok_len;
  int tok_col;
  struct macro *macros;
  struct tok_hist *tok, *last_tok, *ungot_tok;
  };

struct scanner_state *save_scanner_state();
void restore_scanner_tate(struct scanner_state *ss);

// After calling token(), these variables contain additional information
// about it:
//
// tok->file
// tok->line
// tok->str
// tok->len

// Push source file
// Scanning will continue until file exhausted, then we pop back to
// previous file.

int source_push(char *name, int no_error);

// Add include path

void add_path(char *path);

// Define a macro

void mk_macro(char *name,char *body);

// Get copy of rest of line

char *rest_of_line();

// Scan rest of line for /* word word word */

void scan_comment(char *t,char **ptrs);

char *ptr;
