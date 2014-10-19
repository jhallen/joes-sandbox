// Verilog tokenizer

// Get next token

int token(void);		// Get next token

void unget_tok(int t);		// Unget token t
void show_tok(int c);		// Display token in printable form
void error();			// Print error message

// After calling token(), these variables contain additional information
// about it:

extern char tok_file[];		// File it was found in
int tok_line;			// File line number
extern char tok_str[];		// Strings, identifiers, and numbers
int tok_len;			// String length, or number size

// Push source file
// Scanning will continue until file exhausted, then we pop back to
// previous file.

void source_push(char *name);

// Add include path

void add_path(char *path);

// Define a macro

void mk_macro(char *name,char *body);

// Get copy of rest of line

char *rest_of_line();

// Scan rest of line for /* word word word */

void scan_comment(char *t,char **ptrs);
