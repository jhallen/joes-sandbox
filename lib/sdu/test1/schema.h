// Example schema
// First structure is root

STRUCT(foo,
  INTEGER(num)
  STRING(str)
  SUBSTRUCT(bar,fred)
  LIST(items,item)
  LIST(list,fred)
  )

STRUCT(fred,
  INTEGER(a)
  INTEGER(b)
  )

STRUCT(item,
  INTEGER(val)
  )
