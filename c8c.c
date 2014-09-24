#include <stdio.h>
#include <stdlib.h>
#include "calc3.h"
#include "y.tab.h"
#include <string.h>
#define DEBUG1
static int lbl;
static int lel;//label of exception
extern bool iserror;
extern char err[50];
extern char *errtext;
extern int thisFunc;
extern int paraCount;
extern bool inglb;
extern FunNode *fn;
struct stack{
    int size;
    int i;
int (*data)[2];//data[0] for start lable, data[1] for end lable.
}s;
 FunNode* findFuncById(int fid);
bool insertFunction(SymbolType type, char* name, nodeType* paras, int funid);
void storeInnerOffsetInIn(nodeType* p);
FunNode * getFunc(char * name);
FunNode *fn = NULL;
char *nameOverload(char* name, nodeType *paras);
char *nameOverloadwhencall(char * funname,nodeType * paras);
typedef struct symbolnode{
    char * name;//Variable name
    int funcid;
    SymbolType type;
    int absoluteoffset;//Variable offset in assembler, based on bs
    int relativeoffset;//Variable offset in assembler, based on fp
    int dimension;
    int * size;//size of array size[0] -> size[dimension-1] from high dimension to low dimension
    int totalsize;//totalsize of this variable
    struct symbolnode * next;
} SymbolNode;
void showSymbol(SymbolNode * node);
SymbolNode *sn = NULL;

bool insertParas(nodeType* paras, int fid);
bool insertSymbolDriver(SymbolType type, nodeType* p,int funcid);
bool insertSymbol(char * name, SymbolType type, int dimension, int *size,int funcid);
SymbolNode * getSymbol(char * name,int funcid);
bool isDeclared(nodeType* p,int funcid);
void storeOffsetInIn(nodeType* p);//store dynamical offset value of specified variable in 'in'
//void init();//reserve label for build in functions
void testOutofBoundary(nodeType* p);//test whether value of 'in' is larger than p->totalsize(out of index)

void push(int start, int end);
int top(int index);
int* pop();

int ex(nodeType *p) {
    int lblx, lbly, lbl1, lbl2, lblz, lbl3;
    int i,j;
	int relativeoffsetTmp;
    SymbolType type;
    char* fun_name;
    FunNode* func;
    int numofparas;
    nodeType* paras;
	char *valueP;
    int *size;
	SymbolNode *symTmp;
	SymbolNode *ssTmp;
    if (!p) return 0;
    switch(p->type) {
        case typeCon:       
        printf("\tpush\t%d\n", p->con.value); 
        break;  
        case typeId:
		ssTmp = getSymbol(p->id.i,thisFunc);
		if(ssTmp == NULL)
			ssTmp = getSymbol(p->id.i, -1);
		if(ssTmp->funcid == -1)
		{
			printf("\tpush\tsb[%d]\t//variable:\n", ssTmp->relativeoffset);
		}
		else
			printf("\tpush\tfp[%d]\t//variable:%s\n", ssTmp->relativeoffset,p->id.i);
        break;
		 case typeIdR:
            printf("\tpush\t%d\n",getSymbol(p->id.i,thisFunc)->relativeoffset);






            printf("\tpush\tfp\n");
            printf("\tadd\t//get address of variable:%s\n",p->id.i);
            break;
        case typeIdP:
            printf("\tpush\tfp[%d]\n",getSymbol(p->id.i,thisFunc)->relativeoffset);
            printf("\tpop\tin\n");
            printf("\tpush\tsb[in]\t//push *%s in stack\n",p->id.i);
            break;
        case typeStr:
		break;
        case typeArr:
        storeOffsetInIn(p);
        testOutofBoundary(p);
		ssTmp = getSymbol(p->id.i,thisFunc);
		if(ssTmp == NULL)
			ssTmp = getSymbol(p->id.i, -1);
		if(ssTmp->funcid == -1)
		{
			printf("\tpush\tsb[in]\t//array: %s\n",p->arr.id);
        }
		else
		{
			printf("\tpush\tfp[in]\t//array: %s\n",p->arr.id);
		}
		break;
		  case typeArrR:
            storeOffsetInIn(p);
            testOutofBoundary(p);
            printf("\tpush\tin\n");
            printf("\tpush\tfp\n");
            printf("\tadd\t//get address of array:%s\n",p->arr.id);
            break;
        case typeArrP:
            storeInnerOffsetInIn(p);
            printf("\tpush\tin\n");
            printf("\tpush\tfp[%d]\t//array: %s\n",getSymbol(p->arr.id,thisFunc)->relativeoffset,p->arr.id);



            printf("\tadd\n");
            printf("\tpop\tin\n");
            printf("\tpush\tsb[in]\t//push *%s[?] in stack\n",p->arr.id);

            break;
        case typeFun:
			if(!(func = getFunc(nameOverload(p->fun.name,p->fun.paraHead)))){
                printf("function %s not declared\n",p->fun.name);
                exit(-1);
            }
            printf("L%03d:\t//%s\n",func->label,func->name);
            ex(p->fun.stmt);
        break;
        case typeOpr:
            switch(p->opr.oper) {
                case INTTYPE: 
                case CHARTYPE://Declaration
                    if(p->opr.oper == INTTYPE)
                        type = INTTYPE;
                    else if(p->opr.oper == CHARTYPE)
                        type = CHARTYPE;
                    else{
                        printf("Compile Error(1072): Unknown indentifier type\n");
                        exit(-1);
                    }
                    switch(p->opr.op[0]->type){
                        case typeId://A variable
                            printf("\t//allocate space for variable: %s\n",p->opr.op[0]->id.i);
                            printf("\tpush\tsp\n");//allocate space in stack
                            printf("\tpush\t1\n");
                            printf("\tadd\t\n");
                            printf("\tpop\tsp\n");
							if(inglb==true){
							printf("\tpush\tfp\n");//allocate space in stack
                            printf("\tpush\t1\n");
							printf("\tadd\t\n");
                            printf("\tpop\tfp\n");
							}
                            break;
						 case typeIdR:
                        case typeIdP:
                            printf("\t//allocate space for variable: %s\n",p->opr.op[0]->id.i);
                            printf("\tpush\tsp\n");//allocate space in stack
                            printf("\tpush\t1\n");
                            printf("\tadd\t\n");
                            printf("\tpop\tsp\n");
                            break;
                        case typeArr://An array
                            printf("\t//allocate space for array: %s\n",p->opr.op[0]->arr.id);
                            size = (int *)malloc(sizeof(int) * p->opr.op[0]->arr.dimension);
                            for(i = 0; i < p->opr.op[0]->arr.dimension; i++){
                                ex(p->opr.op[0]->arr.index[i]);
                            }
                            for(i = 0; i < p->opr.op[0]->arr.dimension - 1; i++){
                                printf("\tmul\t\n");
                                }
                            //allocate space in stack
							if(inglb==true){
								printf("\tpush\tin\n");
								printf("\tpop\tin\n");
								printf("\tpush\tin\n");
								printf("\tpush\tin\n");
								printf("\tpush\tsp\n");
								printf("\tadd\t\n");//allocate space in stack
								printf("\tpop\tsp\n");
								printf("\tpush\tfp\n");
								printf("\tadd\t\n");//allocate space in stack
								printf("\tpop\tfp\n");
							}
                            else{
								printf("\tpush\tsp\n");
								printf("\tadd\t\n");
								printf("\tpop\tsp\n");
							}
                            break;
						case typeArrR:
                        case typeArrP:
                            printf("\t//allocate space for array: %s\n",p->opr.op[0]->arr.id);
                            size = (int *)malloc(sizeof(int) * p->opr.op[0]->arr.dimension);
                            for(i = 0; i < p->opr.op[0]->arr.dimension; i++){
                                ex(p->opr.op[0]->arr.index[i]);
                            }
                            for(i = 0; i < p->opr.op[0]->arr.dimension - 1; i++){
                                printf("\tmul\t\n");
                                }


                            printf("\tpush\tsp\n");







                            printf("\tadd\t\n");//allocate space in stack





                            printf("\tpop\tsp\n");

                            break;
                        default:
                            printf("Compile Error(1070): Unknown declaration type. <%d>\n",p->opr.op[0]->type);
                            exit(-1);
                            break;
                    }
                    break;
                case CODEBLOCK:
                    ex(p->opr.op[0]);
                    ex(p->opr.op[1]);
                    break;
                case FOR:
                    printf("\t//for loop initialization\n");
                    ex(p->opr.op[0]);
                    printf("\t//for loop condition test\n");
                    printf("L%03d:\n", lblx = lbl++);
                    ex(p->opr.op[1]);
                    printf("\tj0\tL%03d", lbly = lbl++);
                    lblz = lbl++;
                    push(lblz,lbly);      
                    printf("\t//for loop body\n");  
                    ex(p->opr.op[3]);
                    printf("L%03d:\t//for loop iterater\n", lblz);
                    ex(p->opr.op[2]);
                    printf("\tjmp\tL%03d\n", lblx);
                    printf("\t//end of for loop\n");
                    printf("L%03d:\n", lbly);
                    pop();
                    break;
                case RETURN:
                  if(thisFunc == 0){//in main function
                    printf("\tend\n");
                    return;
                  }
                   printf("\t//return\n");
                   ex(p->opr.op[0]);
                //   printf("\tpop\tfp[%d]\n", p->opr.op[1]->con.value-4);
                   printf("\tret\n");  
		   break;
                case CALL:
		
		fun_name = nameOverloadwhencall((char*)p->opr.op[0],p->opr.op[1]);//get function name;

                    for(paras = p->opr.op[1];paras != NULL; paras = paras->para.next){
                        ex(paras->para.name);
                    }
                   
                    func = getFunc(fun_name);
                    
                    printf("\tcall\tL%03d,%d\n",func->label,func->numofpara);
                    if(func->returnType == VOIDTYPE){
                       printf("\tpush\t0\n");
                       printf("\tmul\n");
                       printf("\tadd\t//clear return value\n");
                   }
		
                break;
                case WHILE:
                    printf("L%03d:\n", lbl1 = lbl++);
                    ex(p->opr.op[0]);
                    printf("\tj0\tL%03d\n", lbl2 = lbl++);
                    push(lbl1,lbl2);
                    ex(p->opr.op[1]);
                    printf("\tjmp\tL%03d\n", lbl1);
                    printf("L%03d:\n", lbl2);
                    pop();
                break;
                case DO-WHILE:
                    lbl1 = lbl++;
                    lbl2 = lbl++;
                    lbl3 = lbl++;
                    push(lbl3,lbl2);
                    printf("L%03d:\n",lbl1);
                    ex(p->opr.op[0]);
                    printf("L%03d:\n", lbl3);
                    ex(p->opr.op[1]);
                    printf("\tj0\tL%03d\n", lbl2);
                    printf("\tjmp\tL%03d\n", lbl1);
                    printf("L%03d:\n", lbl2);
                    pop();
                break;
                case BREAK: printf("\tjmp\tL%03d\n", top(1)); break;
                case CONTINUE: printf("\tjmp\tL%03d\n", top(0)); break;
                case IF:
                ex(p->opr.op[0]);
                if (p->opr.nops > 2) {
                    printf("\tj0\tL%03d\n", lbl1 = lbl++);
                    ex(p->opr.op[1]);
                    printf("\tjmp\tL%03d\n", lbl2 = lbl++);
                    printf("L%03d:\n", lbl1);
                    ex(p->opr.op[2]);
                    printf("L%03d:\n", lbl2);
                } else {
                    printf("\tj0\tL%03d\n", lbl1 = lbl++);
                    ex(p->opr.op[1]);
                    printf("L%03d:\n", lbl1);
                }
                break;
               case READ:
                    switch(p->opr.op[0]->type){
			case typeId:
				symTmp = getSymbol(p->opr.op[0]->id.i, thisFunc);
					if(symTmp==NULL)
						symTmp = getSymbol(p->opr.op[0]->id.i, -1);
				break;
			case typeArr:
				symTmp = getSymbol(p->opr.op[0]->arr.id, thisFunc);
						if(symTmp==NULL)
							symTmp = getSymbol(p->opr.op[0]->arr.id, -1);
				break;	
			}	
		    if(symTmp->type==CHARTYPE)
                    	printf("\tgetc\n");
		    else
				printf("\tgeti\n");
                    storeOffsetInIn(p->opr.op[0]); 
                    if(p->opr.op[0]->type == typeArr)
                        testOutofBoundary(p->opr.op[0]); 

                    if(p->opr.op[0]->type == typeArrP || p->opr.op[0]->type == typeIdP){
                        printf("\tpush\tfp[in]\n");
                        printf("\tpop\tin\t//store content of %s in in\n",p->opr.op[0]->type == typeArrP?p->opr.op[0]->arr.id:p->opr.op[0]->id.i);
                        printf("\tpop\tsb[in]\t\n");
                    }
                    else if(p->opr.op[0]->type == typeArr || p->opr.op[0]->type == typeId)             
                        printf("\tpop\tfp[in]\n");
                    else{
                        lyyerror("Error: &[variable] can not be modified.\n");iserror = true;
                        
                    }
                break;

               case PRINT:  		
					switch(p->opr.op[0]->type){
					case typeId:
					case typeIdR:
					case typeIdP:					    
						symTmp = getSymbol(p->opr.op[0]->id.i, thisFunc);
						if(symTmp==NULL)
							symTmp = getSymbol(p->opr.op[0]->id.i, -1);
						if((symTmp->dimension==1) && (*(symTmp->size)<=1))
						{

						    
							if(symTmp->type==CHARTYPE)
							{	
								ex(p->opr.op[0]);	
								printf("\tputc\n");
							}
							else
							{

								ex(p->opr.op[0]);	

								printf("\tputi\n");
							}
							
						}
						else if((symTmp->dimension==1) && (*(symTmp->size)>1) && (symTmp->type==CHARTYPE))
						{
							int i;
							for(i = 0; i < *(symTmp->size); i++){
								printf("\tpush\t%d\n", symTmp->relativeoffset+i);
								printf("\tpop\tin\n");
								printf("\tpush\tfp[in]\t//array\n");
								printf("\tputc_\n");
							}
							printf("\tpush\t\'\'\n");
							printf("\tputc\n");
						}
						break;
					case typeOpr:
						if(p->opr.op[0]->opr.oper == CALL)
						{
							ex(p->opr.op[0]);
							fun_name = nameOverloadwhencall((char*)p->opr.op[0]->opr.op[0],p->opr.op[0]->opr.op[1]);
							func = getFunc(fun_name);
							if(func->returnType == CHARTYPE)
								printf("\tputc\n");
							else
								printf("\tputi\n");
						}
						else{
							ex(p->opr.op[0]);	
							printf("\tputi\n");
						}
						break;
					case typeCon:
						ex(p->opr.op[0]);	
						printf("\tputi\n");
						break;
					case typeArrR:
					case typeArrP:
						ex(p->opr.op[0]);	
						ssTmp = getSymbol(p->opr.op[0]->arr.id,thisFunc);


						if(ssTmp!=NULL){
						if((ssTmp->type) == CHARTYPE)
							printf("\tputc\n");
						else
							printf("\tputi\n");
						}
						break;
					case typeArr:
						ex(p->opr.op[0]);	
						ssTmp = getSymbol(p->opr.op[0]->arr.id,thisFunc);
						if(ssTmp == NULL)
							ssTmp = getSymbol(p->opr.op[0]->arr.id,-1);
						if(ssTmp!=NULL){
						if((ssTmp->type) == CHARTYPE)
							printf("\tputc\n");
						else
							printf("\tputi\n");
						}
						break;
					case typeStr:
						printf("\tpush\t\"%s\"\n",p->opr.op[0]->str.value);
						printf("\tputs\n");
						break;
					}
                break;
		  case PRINT_:  		
					switch(p->opr.op[0]->type){
					case typeId:
					case typeIdR:
					case typeIdP:					    
						symTmp = getSymbol(p->opr.op[0]->id.i, thisFunc);
						if(symTmp==NULL)
							symTmp = getSymbol(p->opr.op[0]->id.i, -1);
						if((symTmp->dimension==1) && (*(symTmp->size)<=1))
						{

						    
							if(symTmp->type==CHARTYPE)
							{	
								ex(p->opr.op[0]);	
								printf("\tputc_\n");
							}
							else
							{

								ex(p->opr.op[0]);	

								printf("\tputi_\n");
							}
							
						}
						else if((symTmp->dimension==1) && (*(symTmp->size)>1) && (symTmp->type==CHARTYPE))
						{
							int i;
							for(i = 0; i < *(symTmp->size); i++){
								printf("\tpush\t%d\n", symTmp->relativeoffset+i);
								printf("\tpop\tin\n");
								printf("\tpush\tfp[in]\t//array\n");
								printf("\tputc_\n");
							}
						}
						break;
					case typeOpr:
						if(p->opr.op[0]->opr.oper == CALL)
						{
							ex(p->opr.op[0]);
							fun_name = nameOverloadwhencall((char*)p->opr.op[0]->opr.op[0],p->opr.op[0]->opr.op[1]);
							func = getFunc(fun_name);
							if(func->returnType == CHARTYPE)
								printf("\tputc_\n");
							else
								printf("\tputi_\n");
						}
						else{
							ex(p->opr.op[0]);	
							printf("\tputi_\n");
						}
						break;
					case typeCon:
						ex(p->opr.op[0]);	
						printf("\tputi_\n");
						break;
					case typeArrR:
					case typeArrP:
						ex(p->opr.op[0]);	
						ssTmp = getSymbol(p->opr.op[0]->arr.id,thisFunc);


						if(ssTmp!=NULL){
						if((ssTmp->type) == CHARTYPE)
							printf("\tputc_\n");
						else
							printf("\tputi_\n");
						}
						break;
					case typeArr:
						ex(p->opr.op[0]);	
						ssTmp = getSymbol(p->opr.op[0]->arr.id,thisFunc);
						if(ssTmp == NULL)
							ssTmp = getSymbol(p->opr.op[0]->arr.id,-1);
						if(ssTmp!=NULL){
						if((ssTmp->type) == CHARTYPE)
							printf("\tputc_\n");
						else
							printf("\tputi_\n");
						}
						break;
					case typeStr:
						printf("\tpush\t\"%s\"\n",p->opr.op[0]->str.value);
						printf("\tputs_\n");
						break;
					}
                break;
                case '=':
					if(p->opr.op[1]->type == typeStr)
					{
						ssTmp = getSymbol(p->opr.op[0]->arr.id,thisFunc);
						valueP = p->opr.op[1]->str.value;
						relativeoffsetTmp = ssTmp->relativeoffset;
						int j;
						j = 0;
						while(*valueP != 0)
						{
							printf("\tpush\t%d\n", relativeoffsetTmp++);
							printf("\tpop\tin\n");
							printf("\tpush\t\'%c\'\n",(int)*valueP);// " not include in con.value
							printf("\tpop\tfp[in]\t\n");
							valueP++;
							j++;
						}
						for(; j < *(ssTmp->size); j++){
							printf("\tpush\t%d\n", relativeoffsetTmp++);
							printf("\tpop\tin\n");
							printf("\tpush\t0\n");// " not include in con.value
							printf("\tpop\tfp[in]\t\n");
						}
					}
					else{
						int temptype;
					    	temptype = p->opr.op[0]->type;
						if(p->opr.op[0]->type==typeArr)	
							ssTmp = getSymbol(p->opr.op[0]->arr.id, -1);
						else
							ssTmp = getSymbol(p->opr.op[0]->id.i, -1);							
						ex(p->opr.op[1]);
						if(p->opr.op[0]->type == typeArr || p->opr.op[0]->type == typeId)
						 {
						 	storeOffsetInIn(p->opr.op[0]);
						 	if(p->opr.op[0]->type == typeArr)
						 	    testOutofBoundary(p->opr.op[0]);
							if(ssTmp != NULL)
								printf("\tpop\tsb[in]\n");
							else
								printf("\tpop\tfp[in]\n");
						}
						else if(p->opr.op[0]->type == typeArrP){					
						    storeInnerOffsetInIn(p->opr.op[0]);
						    printf("\tpush\tin\n");
						    printf("\tpush\tfp[%d]\n",getSymbol(p->opr.op[0]->arr.id,thisFunc)->relativeoffset);
						    printf("\tadd\n");
						    printf("\tpop\tin\n");
						    printf("\tpop\tsb[in]\t//store value in *%s\n",temptype == typeArrP?p->opr.op[0]->arr.id:p->opr.op[0]->id.i);
						}
						else if(p->opr.op[0]->type == typeIdP){
						   storeOffsetInIn(p->opr.op[0]);
						   printf("\tpush\tfp[in]\n");
						   printf("\tpop\tin\n");
						   printf("\tpop\tsb[in]\n");
						}
						else{
						printf("error?!\n"); iserror = true;
						}
					}
					break;
                case UMINUS:    
                    ex(p->opr.op[0]);
                    printf("\tneg\n");
                break;
            default:/*Expr*/
/*semicolon*/
        ex(p->opr.op[0]);
        ex(p->opr.op[1]);
        switch(p->opr.oper) {
            case '+':   printf("\tadd\n"); break;
            case '-':   printf("\tsub\n"); break; 
            case '*':   printf("\tmul\n"); break;
            case '/':   printf("\tdiv\n"); break;
            case '%':   printf("\tmod\n"); break;
            case '<':   printf("\tcomplt\n"); break;
            case '>':   printf("\tcompgt\n"); break;
            case GE:    printf("\tcompge\n"); break;
            case LE:    printf("\tcomple\n"); break;
            case NE:    printf("\tcompne\n"); break;
            case EQ:    printf("\tcompeq\n"); break;
            case AND:   printf("\tand\n"); break;
            case OR:    printf("\tor\n"); break;
        }
    }
}
return 0;
}
void storeInnerOffsetInIn(nodeType* p){
if(p->type != typeArrP) return;
    ex(p->arr.index[0]);
    printf("\tpop\tin\n");
}
void storeOffsetInIn(nodeType* p){
		SymbolNode *ssTmp;
        int offset;//Calculate varaible offset to fp
        int i,j;
        printf("\tpush\t0\n");
        printf("\tpop\tin\n");//in = 0
        switch(p->type){
			case typeIdR:
            case typeIdP:
            offset = getSymbol(p->id.i,thisFunc)->relativeoffset;
            break;
            case typeId:
			ssTmp = getSymbol(p->id.i, -1);
			if(ssTmp == NULL)
				offset = getSymbol(p->id.i,thisFunc)->relativeoffset;
			else
				offset = ssTmp->relativeoffset;
			break;
            case typeArr:
			ssTmp = getSymbol(p->arr.id, -1);
			if(ssTmp == NULL)
				ssTmp = getSymbol(p->arr.id, thisFunc);
            offset = ssTmp->relativeoffset;
            for(i = 0; i < p->arr.dimension;i++){
                ex(p->arr.index[i]);
                for(j = i + 1; j < p->arr.dimension; j++){
                    printf("\tpush\t%d\n",ssTmp->size[j]);
                    printf("\tmul\t\n");
                }
                printf("\tpush\tin\n");
                printf("\tadd\t\n");
                printf("\tpop\tin\n");
            }
            break;
			case typeArrR:
            case typeArrP:
            offset = getSymbol(p->arr.id,thisFunc)->relativeoffset;

            for(i = 0; i < p->arr.dimension;i++){
                ex(p->arr.index[i]);
                for(j = i + 1; j < p->arr.dimension; j++){
                    printf("\tpush\t%d\n",getSymbol(p->arr.id,thisFunc)->size[j]);
                    printf("\tmul\t\n");
                }
                printf("\tpush\tin\n");
                printf("\tadd\t\n");
                printf("\tpop\tin\n");
            }
            break;
            default:
            printf("Compile Error(1072): Unknown type\n");
            break;
        }
        printf("\tpush\t%d\n",offset);
        printf("\tpush\tin\n");
        printf("\tadd\n");
        printf("\tpop\tin\n");
    }
    void setEmpty(){
        if (s.data)
        {
            free(s.data);
            s.data = NULL;
            s.size = 0;
            s.i = -1;
        }
    }
    void push(int start, int end){
    if(!s.data){//Didn't creat
        s.data = (int (*)[2])malloc(sizeof(int [2]) * 100);
    s.i = -1;
    s.size = 100;   
}
    else if(s.i == s.size - 1){//Full, double the size
        int (*new_data)[2] = (int (*)[2])malloc(sizeof(int [2]) * s.size * 2);
        memcpy(new_data,s.data,sizeof(int [2]) * s.size);
        free(s.data);
        s.data = new_data;
        s.size = s.size * 2;
        //s.i remains unchange
    }
    s.i++;
    s.data[s.i][0] = start;
    s.data[s.i][1] = end;
}
int top(int index){
    if(index != 0 && index != 1){
        printf("Compile Error(0001):\n");
        exit(-1);
    }
    return s.data[s.i][index];
}
int * pop(void){
    if(s.i == -1){
        printf("Compile Error(1060): stack is empty!\n");
        exit(-1);   
    }
    return s.data[s.i--];
}
/*Maintain Symbol Table*/
bool insertParas(nodeType* paras, int fid){
    //thisFunc, paraCount
   nodeType * temp;
    int *size;
    int n;
    char * name;
    size = (int *)malloc(sizeof(int));
    *size = 0;
    n = 0;
    for(temp = paras;temp != NULL; temp = temp->para.next){
    int type = temp->para.name->type;
        if(type == typeId || type == typeIdR || type == typeIdP)
            name = temp->para.name->id.i;

        else if(type == typeArr || type == typeArrR || type == typeArrP)
            name = temp->para.name->arr.id;
        else{
            printf("error 1055\n");
            exit(-1);
        }
        insertSymbol(name, temp->para.type, 1, size, fid);
        getSymbol(name, fid)->absoluteoffset = -1;
        getSymbol(name, fid)->relativeoffset = -3-paraCount+n;
        n++;
    }
    
}
bool insertSymbolDriver(SymbolType type, nodeType* p,int funcid){
   int i;
    int *size;
    if(!p) return false;
    switch(p->type){
                    case typeId://A variable
                    case typeIdP:
                    size = (int *)malloc(sizeof(int));
                    *size = 1;
                        return insertSymbol(p->id.i,type,1,size,funcid);//name type dimesion size
                        break;
                    case typeArr://An array
                    size = (int *)malloc(sizeof(int) * p->arr.dimension);
                    int tempsize;
                    for(i = 0;i < p->arr.dimension; i++){
                        tempsize = inter(p->arr.index[i]);
                        if(tempsize > 0)
                            size[i] = tempsize;
                        else{
                            strcat(err,"Error: size of array is zero or negative"); errtext = p->arr.id;iserror = true;
                            return false;
                        }                        
                    }
                        return insertSymbol(p->arr.id, type, p->arr.dimension, size,funcid);//last parameter: size
                        break;


                    /*error*/
                    case typeIdR:
                    case typeArrR:
                    case typeArrP:
                        lyyerror("Error: the declaration is illegal");iserror = true;
                    break;
                    default:
                        lyyerror("Error: ");
                    break;
    }
 }
bool insertSymbol(char * name, SymbolType type, int dimension, int *size,int funcid){
                    SymbolNode * p, *lastNode;
                    int thisrelativeoffset = 0;
                    int thisabsoluteoffset = 0;
                    int i = 0;
                    int j = 0;
                    if(sn==NULL){
                        if(!(sn = (SymbolNode *)malloc(sizeof(SymbolNode)))){
                            strcat(err,"Error: Can not allocate memory for symbol "); errtext = name; 
                            return false;
                        }
                        sn->relativeoffset = 0;
                        sn->absoluteoffset = 0;
    sn->name = name;//Name has already been allocated enough space in bison
    sn->dimension = dimension;
    sn->funcid = funcid;
        sn->size = size;//* Size has already been allocated enough space in ex() 
        sn->totalsize = 0;  
        for(i = 0; i < dimension ; i++){
            int thissize = size[i];
            for(j = i+1; j < dimension ; j++)
                thissize *= size[j];
            sn->totalsize += thissize;
        }
        sn->type = type;
        sn->next = NULL;
    }
    else{
        for(p = sn; p != NULL;p = p->next){
            if (!strcmp(p->name,name))//Same symbol name
            {
                if(p->funcid == funcid){
                    strcat(err,"Error: Variable redeclared"); errtext = name; 
                    return false;
                }
            }
            if(p->funcid == funcid)
                thisrelativeoffset += p->totalsize;
            thisabsoluteoffset +=p->totalsize;
            lastNode = p;
        }
        if(!(lastNode->next = (SymbolNode *)malloc(sizeof(SymbolNode)))){
            strcat(err,"Error: Can not allocate memory for symbol "); errtext = name; 
            return false;
        }
        /*Move pointer to last node*/
        lastNode = lastNode->next;
        /*Copy infomation*/
        lastNode->name = name;
        lastNode->funcid = funcid;
        lastNode->dimension = dimension;
        lastNode->size = size;
        lastNode->totalsize = 0;
        for(i = 0; i < dimension ; i++){
            int thissize = size[i];
            for(j = i+1; j < dimension ; j++)
                thissize *= size[j];
            lastNode->totalsize += thissize;
        }
        lastNode->type = type;
        lastNode->relativeoffset = thisrelativeoffset;
        lastNode->absoluteoffset = thisabsoluteoffset;
        lastNode->next = NULL;
    }
    return true;    
}
bool isDeclared(nodeType* p,int funcid){
    switch(p->type){
		case typeIdR:
        case typeIdP:
        if(getSymbol(p->id.i,funcid) == NULL)//symbol not exist




            return false;

        else
            return true;
        break;
        case typeId:
        if(getSymbol(p->id.i,funcid) == NULL)//symbol not exist
        {
			if(getSymbol(p->id.i,-1) != NULL)
				return true;
			else
				return false;
		}
        else
            return true;
        break;
        case typeArr:
        if(getSymbol(p->arr.id,funcid) == NULL)//symbol not exist
        {
			if(getSymbol(p->arr.id,-1) != NULL)
				return true;
			else
				return false;
		}
        else
            return true;
		break;
		case typeArrR:
        case typeArrP:
        if(getSymbol(p->arr.id,funcid) == NULL)//symbol not exist




            return false;

        else
            return true;
        break;
    }
}
void showSymbol(SymbolNode * node)
{
int i;
    if(!node) {printf("symol is null!\n");return;}
	printf("name = %s\n",node->name);
	printf("funcid = %d\n",node->funcid);
	printf("type = %d\n",node->type);
	printf("absoluteoffset = %d\n",node->absoluteoffset);
	printf("relativeoffset = %d\n",node->relativeoffset);
	printf("dimension = %d\n",node->dimension);
	for(i = 0; i < node->dimension; i++){
	    if(!node->size[i]) {printf("size[%d] not exist!\n",i);continue;}
		printf("size[%d] = %d\n",i,node->size[i]);
	}
	printf("totalsize = %d\n",node->totalsize);
}
SymbolNode * getSymbol(char * name, int funcid){
    SymbolNode * p;    
    #ifdef DEBUG
    printf("search %s in func %d",name,funcid);
    #endif
    for(p = sn; p != NULL;p = p->next){
        #ifdef DEBUG
        printf("%s  in func = %d\n",p->name,p->funcid);
        #endif
        if (!strcmp(p->name,name) && p->funcid == funcid)//Same symbol name
        {
            return p;
        }
    }
    return NULL;
    //exit(-1);
}
FunNode* findFuncById(int fid){
  FunNode *p;
  if(!fn) {printf("Error: 1064 No function!\n"); return NULL;}
  for(p = fn;p != NULL;p = p->next){
      if(p->funid == fid){
          return p;
      }
  } 
  printf("Error: function not found. in findfuncbyid\n");
  iserror = true;
  return NULL;
}
bool insertFunction(SymbolType type, char* name, nodeType* paras, int funid){
     FunNode * pTmp, *lastNode;

	 int numofpara;
     nodeType* thispara;
     char* overloadedname;
     overloadedname = nameOverload(name,paras);
     #ifdef DEBUG
         printf("insert function<%s>\n",overloadedname);
     #endif
     if(!fn){
     if(!(fn = (FunNode *)malloc(sizeof(FunNode)))){
         strcat(err,"Error: Can not allocate memory for function "); errtext = name; 
         return false;
     }
	 for(thispara = paras, numofpara = 0; thispara != NULL; thispara = thispara->para.next){
         numofpara++;
     }
     fn->funid = funid;

	 fn->numofpara = numofpara;
     fn->name = overloadedname;
     fn->returnType = type;
     fn->parameter = paras;
     fn->label = lbl++;
     fn->next = NULL;
         }
     else{
      for(pTmp = fn; pTmp != NULL;pTmp = pTmp->next){
             if (!strcmp(pTmp->name, overloadedname))//Same symbol name
             {
                 strcat(err,"Error: Function redeclared"); errtext =  overloadedname; 
             return false;
             }
             lastNode = pTmp;
        }
      if(!(lastNode->next = (FunNode *)malloc(sizeof(FunNode)))){
         strcat(err,"Error: Can not allocate memory for function "); errtext = name; 
         return false;
         }
         /*Move pointer to last node*/
         lastNode = lastNode->next;
         /*Copy infomation*/
         lastNode->name = overloadedname;
         lastNode->funid = funid;
         lastNode->returnType = type;
     lastNode->parameter = paras;
     lastNode->label = lbl++;
     lastNode->next = NULL;
     }
     return true;
     
 }
 FunNode * getFunc(char * name){
     
     FunNode * p;    
     #ifdef DEBUG
         printf("search variable<%s>\n",name);
         printf("function table:\n");
     #endif
     for(p = fn; p != NULL;p = p->next){  
     #ifdef DEBUG
         printf("\t%s\n",p->name);
     #endif
         if (!strcmp(p->name,name))
         {
             return p;
         }
     }
     return NULL;
     //exit(-1);
 }
/**
 * Test whether the index of array is out of boundary.
 */
 void testOutofBoundary(nodeType* p){
    int lbx;
    SymbolNode * symbol;
    switch(p->type){
		   case typeIdR:
        case typeIdP:
            return;
		break;
        case typeId:
        symbol = getSymbol(p->id.i,thisFunc);
		if(symbol == NULL)
			symbol = getSymbol(p->id.i,-1);
        break;
        case typeArr:
        symbol = getSymbol(p->arr.id,thisFunc);
		if(symbol == NULL)
			symbol = getSymbol(p->arr.id,-1);
        break;
		case typeArrR:
        case typeArrP:
           return;


        break;
        default:
        printf("Compile Error(1071): Unknown type.\n");
        exit(-1);
        break;
    }
    printf("\tpush\tin\n");
    printf("\tpush\t%d\n",symbol->totalsize + symbol->relativeoffset);
    printf("\tcomplt\n");// 1 if in < upperbound
    printf("\tpush\tin\n");    
    printf("\tpush\t%d\n",symbol->relativeoffset);
    printf("\tcompge\n"); // 1 if in >= lowerbound
    printf("\tand\n");//if (in < upperbound && in >= lowerbound) jump
    printf("\tj1\tL%03d\n",lbx = lbl++);
    printf("\tpush\t\"Runtime Error(100): Index out of boundary.\"\n");
    printf("\tcall\tL%03d,1\n", lel);
    printf("\tend\n");
    printf("L%03d:\n", lbx);
}
char *nameOverload(char* funname, nodeType *paras)
{
   char name[1024];
   nodeType *tmp;
   tmp = paras;
   strcpy(name, funname);
   while(tmp){
   strcat(name, "_");
   if(tmp->para.type == INTTYPE)
       strcat(name, "INT");
   else if(tmp->para.type == CHARTYPE)
       strcat(name, "CHAR");
   tmp = tmp->para.next;
   }
   return (strdup(name));
 }

char * nameOverloadwhencall(char* funname,nodeType* paras){
char name[1024];
char *paraname;
   nodeType *tmp;
   SymbolNode* symbol;
   tmp = paras;
   strcpy(name, funname);
 while(tmp!=NULL){
       int type = tmp->para.name->type;

	   if(type == typeId || type == typeIdR || type == typeIdP || type == typeArr || type == typeArrR || type == typeArrP){
		   if(type == typeArr || type == typeArrR || type == typeArrP)
			   paraname = tmp->para.name->arr.id;
		   else if(type == typeId || type == typeIdR || type == typeIdP)
			   paraname = tmp->para.name->id.i;
		   else{
			   printf("Error in nameoverloadwhencall 1056\n");
			   exit(-1);

		   }
		   strcat(name, "_");
		   if((getSymbol(paraname,thisFunc))!=NULL){
			   symbol = getSymbol(paraname,thisFunc);

		   }
		   else{
			   printf("Error: varable %s not declared\n",paraname);
			   exit(-1);


		   }
		   if(symbol->type == INTTYPE)
			   strcat(name, "INT");
		   else if(symbol->type == CHARTYPE)
			   strcat(name, "CHAR");
		   else{
		       printf("type =  %d \n",symbol->type);
		        }
		   }
	   else if(tmp->para.name->type == typeOpr){
	       strcat(name,"_INT");
	   }
	   else if(tmp->para.name->type == typeCon){
	       strcat(name,"_INT");
	   }
	   else{
	       printf("Error in overridenamewhencall() Unknow parameter type?\n");
	   }
	   tmp = tmp->para.next;
   }
   return (strdup(name));
 }
void c8c_init(){
    lel = lbl++;//label for runtime error exception
}
/**
 * Setup inbu2ild function for exception handler
 * It should be executed for one time at the end of main function
 */
 void setupInbuildFunc(){   
    printf("\tend\n");//end main function.
    /*RuntimeException(char * msg)*/
    printf("L%03d:\n",lel);
    printf("\tpush\tfp[-4]\n");
    printf("\tputs\n");
    printf("\tret\n");
    /*to be continued*/
}
/**
 * Interprete Expression
 */
 int inter(nodeType *p) {
    if (!p) {printf("Compile Error(1010): Unknown Type\n");exit(-1);return 0;}
    switch(p->type) {
        case typeCon:       return p->con.value;
        case typeOpr:
        switch(p->opr.oper) {
            case UMINUS:    return -inter(p->opr.op[0]);
            case '+':       return inter(p->opr.op[0]) + inter(p->opr.op[1]);
            case '-':       return inter(p->opr.op[0]) - inter(p->opr.op[1]);
            case '*':       return inter(p->opr.op[0]) * inter(p->opr.op[1]);
            case '/':       return inter(p->opr.op[0]) / inter(p->opr.op[1]);
            case '%':       return inter(p->opr.op[0]) % inter(p->opr.op[1]);
            case '<':       return inter(p->opr.op[0]) < inter(p->opr.op[1]);
            case '>':       return inter(p->opr.op[0]) > inter(p->opr.op[1]);
            case GE:        return inter(p->opr.op[0]) >= inter(p->opr.op[1]);
            case LE:        return inter(p->opr.op[0]) <= inter(p->opr.op[1]);
            case NE:        return inter(p->opr.op[0]) != inter(p->opr.op[1]);
            case EQ:        return inter(p->opr.op[0]) == inter(p->opr.op[1]);
            case AND:   return inter(p->opr.op[0]) && inter(p->opr.op[1]);
            case OR:    return inter(p->opr.op[0]) || inter(p->opr.op[1]);
        }
    }
    return 0;
}
