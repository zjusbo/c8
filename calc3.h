#define GETSYMBOLNAME(x) x->type==typeId?x->id.i:x->arr.id

typedef enum {typePara, typeFun, typeStr, typeArr,typeArrR, typeArrP, typeCon, typeId,typeIdR, typeIdP,typeOpr } nodeEnum;//typeArrR,typeIdR(reference)

typedef enum {CHARTYPE, INTTYPE, BOOLTYPE, VOIDTYPE } SymbolType;
typedef enum {false,true}bool;

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

/* identifiers */
typedef struct {
    char *i;                      /* subscript to sym array */
} idNodeType;

typedef struct {
	char *value;
} strNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

typedef struct {
	char *id;
	int dimension;
	struct nodeTypeTag *index[1];
} arrNodeType;

typedef struct {
	char *name;
	int returnType;
	struct nodeTypeTag *stmt;
	struct nodeTypeTag *paraHead;
} funNodeType;

typedef struct {
	struct nodeTypeTag *name;
	int type;
	struct nodeTypeTag *next;
} paraNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
		arrNodeType arr;        /* array  */
		strNodeType str;		/* string */
		funNodeType fun;        /* function */
		paraNodeType para;      /* parameter*/
	};
} nodeType;

typedef struct funnode{
     int funid;
     int label;
     char *name;
     int returnType;
	 int numofpara;
     nodeType * parameter;
     struct funnode * next;
 }FunNode;

