// Example schema
// First structure is root

STRUCT(foo,
  INTEGER(val)
  STRING(val_name)
  LIST(items,item)
  )

STRUCT(item,
  INTEGER(val)
  )
