
# Structured data utilities

Joe Allen
Feb 6, 2005

Type: make

Try: ./sdu test.xml

schema.h defines the structure of your data

test.c example use

## SDU

Keywords: UML, Universal Modeling Language, OML, Object Modeling
Language, XML, CPP, C-Preprocessor, Object-Oriented Database, Schema,
intermediate language, structured data, JSON, LISP, lightweight,
serialize, intermediate language.

_SDU_ (Structured Data Utilities) is a library for specifying,
loading, validating and storing structured data in various formats,
including XML, JSON, LISP, indented and binary prefix format.  Loaded data
is accessible through first class data structures within the target
programming language (C or C++).  This means that the target language's
dereferencing operators are used to access each element of the database. 
Furthermore, the element names (in data formats which have explicit element
names like XML) appear directly as language identifiers (structure member
names).

_SDU_ is lightweight for two reasons: first the modeling
language in which the schema for the structured data is given is built onto
two sets of C-preprocessor macros.  One set is used to convert the schema
into the meta data needed to load the data.  The other set converts the
schema into native language structure definitions.  The example which
follows will make this clear.

The second reason is that only a minimal subset of each structured data
format is supported.  This applies specifically to the XML format, as the
others are all pretty simple to begin with.  For XML, the basic subset is
this: no attributes, only UTF-8 character set is allowed, no document type,
no CDATA, no entity declrations, and tag names must be legal C-language
identifiers.

## Example

Here is an example schema.  This schema is placed in a header file called
"schema.h".  The library is actually just two files: meta.h and meta.c. 
Both of these files include "schema.h".

``` C
    /* A Book */

    STRUCT(order
       STRING(title)
       INTEGER(size)
       LIST(items,item)
       )

    STRUCT(item
       STRING(name)
       INTEGER(price)
       )
```

Here is a legal XML data file for the above schema:


``` XML
    <?xml version="1.0">

    <order>

    <title>Hello</title>

    <size>123</size>

    <items>
    <item><name>Joe Allen</name><price>34</price></item>
    <item><name>Bill Allen</name><price>29</price></item>
    </items>

    </order>
```

Here is how each element is accessed from C:

Path               | Description
----               | -----------
root->title        | The name
root->size         | The size
root->items        | The first item
root->items->name  | Name of first item
root->items->price | Price of first item
root->items->next  | The second item


In fact, these C structures get defined:


``` C
    struct order
      {
      /* Standard header */

      struct order *next;	/* Next structure in list */
      struct base *mom;	/* Parent */
      char *_name;		/* String name of structure type */
      struct meta *_meta;	/* Meta definition of structure */

      /* User's data */

      char *title;
      int size;
      struct item *items;
      };

    struct item
      {
      /* Standard header */

      struct item *next;	/* Next in list */
      struct base  *mom;	/* Parent */
      char *_name;		/* "item" */
      struct meta *_meta;

      /* User's data */

      char *name;
      int price;
      };
```

## Usage

"struct base" is the base class for all structures defined in the
schema.

These constructs are used to define your database schema:


Syntax                    | Meaning
------                    | -------
STRUCT(name,&lt;contents&gt;)   | Define a structure
STRING(name)              | Declare a string within a structure
INTEGER(name)             | Declare an integer within a structure
LIST(name,structure-name) | Declare a list of structures within a structure

There must be a least one structure defined in schema.h

When a database is loaded, all strings and structures are malloced from the heap.

The following functions are provided:

Function                                                       | Description
--------                                                       | -----------
struct base *xml_parse(FILE *f,struct meta *expect);           | Load an XML formatted file from stream 'f'.  'expect' is pointer to meta data of the expected root structure.  If _metadata_ is placed here, the first structure defined in schema.h is used.
void xml_print(FILE *f,int indent,struct base *b);             | Write database as XML formatted file to stream 'f'.  <em>indent</em> is starting indentation level (set to zero).
void lisp_print(FILE *f,int indent,struct base *b);            | Write database as LISP formatted file
void lisp_print_untagged(FILE *f,int indent,struct base *b);   | Write database as LISP formatted file (primitive types get no tags)
void indent_print_untagged(FILE *f,int indent,struct base *b); | Write database as indented file (primitive types get no tags)
void json_print(FILE *f,int indent,struct base *b);            | Write database as JSON formatted file
struct base *mk(char *name);                                   | Create a structure which exists in the schema
struct meta *metafind(char *name);                             | Find meta data for named structure type.

## Todo

* More primitive types
* Pointers
* C++ version of code
* Allow polymorphism in lists
