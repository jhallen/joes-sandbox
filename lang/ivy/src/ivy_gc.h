Str *alloc_str(char *s, size_t len);
Var *alloc_var();
Entry *alloc_entry();
Obj *alloc_obj(int size);
Fun *alloc_fun(Func *func, Obj *scope);
void gc_protect_done();
void protect_obj(Obj *);
