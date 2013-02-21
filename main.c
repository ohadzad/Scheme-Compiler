
 // starting code 
#include <stdio.h>
#include <stdlib.h>

#include "cisc.h"

/* change to 0 for no debug info to be printed: */
#define DO_SHOW 1

/* for debugging only, use SHOW("<some message>, <arg> */
#if DO_SHOW
#define SHOW(msg, x) { printf("%s %s = %ld\n", (msg), (#x), (x)); }
#else
#define SHOW(msg, x) {}
#endif

int main()
{
  START_MACHINE;

  JUMP(CONTINUE);

#include "char.lib"
#include "io.lib"
#include "math.lib"
#include "string.lib"
#include "system.lib"
#include "scheme.lib"

CONTINUE:
 JUMP(L_END_DEFS);

 // methods for error handling 
ERROR_NOT_A_CLOSURE:
printf("Not a closure.\n");
JUMP(L_END_OF_PROGRAM);
ERROR_WRONG_ARGUMENT_NUMBER:
printf("Wrong number of parameters.");
JUMP(L_END_OF_PROGRAM);
ERROR_UNDEFINED:
printf("Variable undefined.");
JUMP(L_END_OF_PROGRAM);

 // support methods 

MAKE_LIST: 
CMP(FPARG(2), R1);
JUMP_NE(MAKE_LIST_CONT);
MOV(R0,IMM(11));
RETURN;

MAKE_LIST_CONT:
INCR(R1);
CALL(MAKE_LIST); // recursive call
INCR(R1); //increase for correct extraction of argument
INCR(R1);
PUSH(R0);
PUSH(FPARG(R1));
DECR(R1);
DECR(R1);
DECR(R1);
CALL(MAKE_SOB_PAIR); //create the pair
DROP(IMM(2));
RETURN;

void print_memory(int from, int to){
int i;
printf("printing memory, FP: %d SP: %d R0: %d\n", (int)(FP), (int)(SP), (int)R0);
for(i=from;i<to; ++i){
printf("%d ", i);
SHOW("", M(mem)[i]);
}
}
void print_stack(char* comment){
        int i;
        printf("printing stack, FP: %d SP: %d %s\n", (int)(FP), (int)(SP), comment);
        for(i=SP+5; i>=0; --i){
        if(SP == i){
                printf("SP ");
        }
        if(FP == i){
                printf("FP");
        }
        printf("	element %d: ", i);
        SHOW(" ", STACK(i));
        }
}GET_LAST_BUCKET:
PUSH(FP);
MOV(FP,SP);
MOV(R0, ADDR(IMM(1)));

GET_LAST_BUCKET_LOOP:
CMP(INDD(R0, IMM(2)), IMM(0));
JUMP_EQ(GET_LAST_BUCKET_END);
MOV(R0,INDD(R0,IMM(2)));
JUMP(GET_LAST_BUCKET_LOOP);

GET_LAST_BUCKET_END:
POP(FP);
RETURN;COMPARE_STRINGS:
PUSH(FP);
MOV(FP,SP);
MOV(R0,FPARG(0));
MOV(R1,FPARG(1));
CMP(INDD(R0,1), INDD(R1,1));
JUMP_NE(COMPARE_STRINGS_END_FALSE);
MOV(R2, INDD(R0,1));
ADD(R0, IMM(2));
ADD(R1, IMM(2));

COMPARE_STRINGS_LOOP:
CMP(R2, IMM(0));
JUMP_EQ(COMPARE_STRINGS_END_TRUE);
CMP(ADDR(R0),ADDR(R1));
JUMP_NE(COMPARE_STRINGS_END_FALSE);
INCR(R0);
INCR(R1);
DECR(R2);
JUMP(COMPARE_STRINGS_LOOP);

COMPARE_STRINGS_END_FALSE:
MOV(R0,0);
POP(FP);
RETURN;

COMPARE_STRINGS_END_TRUE:
MOV(R0,1);
POP(FP);
RETURN;

COPY_STRING:
PUSH(FP);
MOV(FP,SP);
MOV(R1,FPARG(0));
MOV(R2, INDD(R1,IMM(1)));
ADD(R2, IMM(2));
PUSH(R2);
CALL(MALLOC);
DROP(IMM(1));
DECR(R2);

COPY_STRING_LOOP:
CMP(R2,IMM(-1));
JUMP_EQ(COPY_STRING_END);
MOV(INDD(R0,R2),INDD(R1,R2));
DECR(R2);
JUMP(COPY_STRING_LOOP)

COPY_STRING_END:
POP(FP);
RETURN;

 //Primiteves Procedures
STRING_TO_SYMBOL:
PUSH(FP);
MOV(FP,SP);
MOV(R1, FPARG(IMM(3)));
MOV(R2,IND(IMM(1)));
PUSH(R1);

STRING_TO_SYMBOL_COMPARE_LOOP:
CMP(R2, IMM(0));
JUMP_EQ(STRING_TO_SYMBOL_COMPARE_NOT_FOUND);
PUSH(R2);
PUSH(R1);
PUSH(IND(R2));
CALL(COMPARE_STRINGS);
DROP(IMM(1));
POP(R1);
POP(R2);
CMP(R0,IMM(1));
JUMP_EQ(STRING_TO_SYMBOL_COMPARE_FOUND);
MOV(R2, INDD(R2,IMM(2)));
JUMP(STRING_TO_SYMBOL_COMPARE_LOOP);

STRING_TO_SYMBOL_COMPARE_FOUND:
DROP(IMM(1));
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(IND(R0), T_SYMBOL);
MOV(INDD(R0,IMM(1)), R2);
POP(FP);
RETURN;

STRING_TO_SYMBOL_COMPARE_NOT_FOUND:
PUSH(R1);
CALL(GET_LAST_BUCKET);
POP(R1);
MOV(R2,R0);
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(IND(R0), R1);
MOV(INDD(R0, IMM(1)),IMM(0)); 
MOV(INDD(R0, IMM(2)),IMM(0));
MOV(INDD(R2,IMM(2)),R0);
MOV(R2,R0);
JUMP(STRING_TO_SYMBOL_COMPARE_FOUND);

CAR:
PUSH(FP);
MOV(FP,SP);
MOV(R0,FPARG(IMM(3)));
MOV(R0,INDD(R0,IMM(1)));
POP(FP);
RETURN;

CDR:
PUSH(FP);
MOV(FP,SP);
MOV(R0,FPARG(IMM(3)));
MOV(R0,INDD(R0,IMM(2)));
POP(FP);
RETURN;

APPLY:
PUSH(FP);
MOV(FP,SP);
PUSH(R1);
PUSH(R2);
PUSH(R3);
MOV(R0,FPARG(IMM(4)));
PUSH(IMM(0)); //magic number

PARAMS_LOOP:
CMP(IND(R0), T_NIL);
JUMP_EQ(END_PARAMS);
PUSH(INDD(R0,IMM(1))); //pushing the car
MOV(R0, INDD(R0, IMM(2))); //moving cdr to R0
JUMP(PARAMS_LOOP);

END_PARAMS:

MOV(R1,SP);
DECR(R1);
MOV(R2,FP);
ADD(R2,IMM(4));

REVERSE_LOOP:
CMP(R1,R2);
JUMP_LE(END_REVERSE);
MOV(R3, STACK(R1));
MOV(STACK(R1), STACK(R2));
MOV(STACK(R2), R3);
DECR(R1);
INCR(R2);
JUMP(REVERSE_LOOP);

END_REVERSE:
MOV(R1,SP);
SUB(R1,FP);
SUB(R1,IMM(4)); //calculate number of args
PUSH(R1);
PUSH(R1);
MOV(R2,FPARG(IMM(3)));
PUSH(INDD(R2,IMM(1)));
CALLA(INDD(R2,IMM(2)));
DROP(IMM(1));
POP(R1);
DROP(R1);
DROP(IMM(2));
POP(R3);
POP(R2);
POP(R1);
POP(FP);
RETURN;

BIN_PLUS:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
ADD(R0, R1);
PUSH(R0);
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
POP(FP);
RETURN;

BIN_MINUS:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
SUB(R0, R1);
PUSH(R0);
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
POP(FP);
RETURN;

BIN_MUL:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
MUL(R0, R1);
PUSH(R0);
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
POP(FP);
RETURN;

BIN_DIV:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
DIV(R0, R1);
PUSH(R0);
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
POP(FP);
RETURN;

GRATER_THAN:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
CMP(R0, R1);
JUMP_GT(GRATER);
//less then
MOV(R0,IMM(0));
JUMP(END_OF_GRATER_THAN);
GRATER:
MOV(R0,IMM(1));
END_OF_GRATER_THAN:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

LOWER_THAN:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
CMP(R0, R1);
JUMP_LT(LOWER);
//more then
MOV(R0,IMM(0));
JUMP(END_OF_LOWER_THAN);
LOWER:
MOV(R0,IMM(1));
END_OF_LOWER_THAN:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

EQUAL_TO:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
CMP(R0, R1);
JUMP_EQ(EQUAL_TO_EQ);
//not equal
MOV(R0,IMM(0));
JUMP(END_OF_EQUAL_TO);
EQUAL_TO_EQ:
MOV(R0,IMM(1));
END_OF_EQUAL_TO:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

REMAINDER:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R0, INDD(R0, 1));
MOV(R1, INDD(R1, 1));
REMAINDER_CMP:
CMP(R0, R1);
JUMP_LT(REMAINDER_LOWER_OR_EQUAL);
SUB(R0, R1);
JUMP(REMAINDER_CMP);
REMAINDER_LOWER_OR_EQUAL:

PUSH(R0);
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
POP(FP);
RETURN;

IS_PROCEDURE:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(276405));
JUMP_EQ(IS_PROCEDURE_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_PROCEDURE_END);
IS_PROCEDURE_EQ:
MOV(R0, IMM(1));
IS_PROCEDURE_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_VECTOR:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(335728));
JUMP_EQ(IS_VECTOR_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_VECTOR_END);
IS_VECTOR_EQ:
MOV(R0, IMM(1));
IS_VECTOR_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_SYMBOL:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(368031));
JUMP_EQ(IS_SYMBOL_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_SYMBOL_END);
IS_SYMBOL_EQ:
MOV(R0, IMM(1));
IS_SYMBOL_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_STRING:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(799345));
JUMP_EQ(IS_STRING_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_STRING_END);
IS_STRING_EQ:
MOV(R0, IMM(1));
IS_STRING_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_CHAR:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(181048));
JUMP_EQ(IS_CHAR_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_CHAR_END);
IS_CHAR_EQ:
MOV(R0, IMM(1));
IS_CHAR_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_NUMBER:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(945311));
JUMP_EQ(IS_NUMBER_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_NUMBER_END);
IS_NUMBER_EQ:
MOV(R0, IMM(1));
IS_NUMBER_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_BOOLEAN:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(741553));
JUMP_EQ(IS_BOOLEAN_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_BOOLEAN_END);
IS_BOOLEAN_EQ:
MOV(R0, IMM(1));
IS_BOOLEAN_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_PAIR:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(885397));
JUMP_EQ(IS_PAIR_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_PAIR_END);
IS_PAIR_EQ:
MOV(R0, IMM(1));
IS_PAIR_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

IS_NULL:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(722689));
JUMP_EQ(IS_NULL_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_NULL_END);
IS_NULL_EQ:
MOV(R0, IMM(1));
IS_NULL_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

INTEGER_TO_CHAR:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
PUSH(R0);
CALL(MAKE_SOB_CHAR);
DROP(IMM(1));
POP(FP);
RETURN;

CHAR_TO_INTEGER:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
PUSH(R0);
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
POP(FP);
RETURN;

STRING_LENGTH:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
PUSH(R0);
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
POP(FP);
RETURN;

MAKE_STRING:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(1));	//R0 has number of arguments
CMP(R0, IMM(2));
JUMP_EQ(MAKE_STRING_TWO_VAR);

MAKE_STRING_ONE_VAR:
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
MOV(R1, IMM(0));
MOV(R3, R0);
JUMP(MAKE_STRING_MAKE_STRING);

MAKE_STRING_TWO_VAR:
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
MOV(R1, FPARG(4));
MOV(R1, INDD(R1, 1));
MOV(R3, R0);

MAKE_STRING_MAKE_STRING:
CMP(R0, IMM(0));
JUMP_EQ(MAKE_STRING_END);
PUSH(R1);
SUB(R0, 1);
JUMP(MAKE_STRING_MAKE_STRING);

MAKE_STRING_END:
PUSH(R3);
CALL(MAKE_SOB_STRING);
ADD(R3, IMM(1));
DROP(R3);
POP(FP);
RETURN;

MAKE_VECTOR:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(1));	//R0 has number of arguments
CMP(R0, IMM(2));
JUMP_EQ(MAKE_VECTOR_TWO_VAR);

MAKE_VECTOR_ONE_VAR:
PUSH(IMM(0));
CALL(MAKE_SOB_INTEGER);
DROP(IMM(1));
MOV(R1, R0);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
MOV(R3, R0);
JUMP(MAKE_VECTOR_MAKE_VECTOR);

MAKE_VECTOR_TWO_VAR:
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
MOV(R1, FPARG(4));
//MOV(R1, INDD(R1, 1));
MOV(R3, R0);

MAKE_VECTOR_MAKE_VECTOR:
CMP(R0, IMM(0));
JUMP_EQ(MAKE_VECTOR_END);
PUSH(R1);
SUB(R0, 1);
JUMP(MAKE_VECTOR_MAKE_VECTOR);

MAKE_VECTOR_END:
PUSH(R3);
CALL(MAKE_SOB_VECTOR);
ADD(R3, IMM(1));
DROP(R3);
POP(FP);
RETURN;

SYMBOL_TO_STRING:
PUSH(FP);
MOV(FP,SP);
MOV(R0,FPARG(3));
MOV(R0,INDD(R0,IMM(1)));
MOV(R0,IND(R0));
PUSH(R0);
CALL(COPY_STRING);
DROP(IMM(1));
POP(FP);
RETURN;

SET_CAR:
PUSH(FP);
MOV(FP,SP);
MOV(R0, FPARG(IMM(3)));
MOV(R1, FPARG(IMM(4)));
MOV(INDD(R0,IMM(1)), R1);
MOV(R0, IMM(10));
POP(FP);
RETURN;

SET_CDR:
PUSH(FP);
MOV(FP,SP);
MOV(R0, FPARG(IMM(3)));
MOV(R1, FPARG(IMM(4)));
MOV(INDD(R0,IMM(2)), R1);
MOV(R0, IMM(10));
POP(FP);
RETURN;

CONS:
PUSH(FP);
MOV(FP,SP);
PUSH(FPARG(4));
PUSH(FPARG(3));
CALL(MAKE_SOB_PAIR);
DROP(IMM(2));
POP(FP);
RETURN;

STRING_SET:
PUSH(FP);
MOV(FP,SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R2, FPARG(5));
MOV(R1,(INDD(R1, IMM(1))));
ADD(R1, IMM(2));
MOV(R2,(INDD(R2, IMM(1))));
MOV(INDD(R0,R1),R2);
MOV(R0, IMM(10));
POP(FP);
RETURN;

STRING_REF:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R1, INDD(R1, 1));
ADD(R1, IMM(2));
MOV(R0, INDD(R0, R1));
PUSH(R0);
CALL(MAKE_SOB_CHAR);
DROP(IMM(1));
POP(FP);
RETURN;

VECTOR_REF:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R1, INDD(R1, 1));
ADD(R1, IMM(2));
MOV(R0, INDD(R0, R1));
POP(FP);
RETURN;

EQ:
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
CMP(R0, R1);
JUMP_NE(EQ_NOT_EQUAL);
//equalls
MOV(R0, IMM(1));
JUMP(EQ_END);
EQ_NOT_EQUAL:
MOV(R0, IMM(0));
EQ_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

EQ2:
  //SHOW("Equal?", R0);
  PUSH(FP);
  MOV(FP, SP);
  MOV(R0, FPARG(3));
  MOV(R1, FPARG(4));
  //SHOW("R0", R0);
  //SHOW("R1", R1);
  MOV(R2, INDD(R0, 0));
  MOV(R3, INDD(R1, 0));
  //First compare data types, not equal here is enough.
  CMP(R2, R3);
  JUMP_NE(EQ2_NOT_EQUAL);
  //check if it's T_CHAR
  CMP(R2, IMM(181048));
  JUMP_EQ(EQ2_T_BOOL_CHAR_INTEGER);
  //check if it's T_INTEGER
  CMP(R2, IMM(945311));
  JUMP_EQ(EQ2_T_BOOL_CHAR_INTEGER);
  JUMP(EQ2_ELSE_TYPE);
EQ2_T_BOOL_CHAR_INTEGER:
  MOV(R0, INDD(R0, 1));
  MOV(R1, INDD(R1, 1));
  CMP(R0, R1);
  JUMP_EQ(EQ2_EQUAL);
  JUMP(EQ2_NOT_EQUAL);
  
EQ2_ELSE_TYPE:
  CMP(R0, R1);
  JUMP_NE(EQ2_NOT_EQUAL);
  JUMP(EQ2_EQUAL);


EQ2_EQUAL:
MOV(R0, IMM(1));
JUMP(EQ2_END);
EQ2_NOT_EQUAL:
MOV(R0, IMM(0));
EQ2_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;


EQUAL:
  //SHOW("Equal?", R0);
  PUSH(FP);
  MOV(FP, SP);
  MOV(R0, FPARG(3));
  MOV(R1, FPARG(4));
  //SHOW("R0", R0);
  //SHOW("R1", R1);
  MOV(R2, INDD(R0, 0));
  MOV(R3, INDD(R1, 0));
  //First compare data types, not equal here is enough.
  CMP(R2, R3);
  JUMP_NE(EQUAL_NOT_EQUAL);
  //check if it's void
  CMP(R2, IMM(937610));
  JUMP_EQ(EQUAL_EQUAL);
  //checks if it's NILL
  CMP(R2, IMM(722689));
  JUMP_EQ(EQUAL_EQUAL);
  //check if it's T_BOOL
  CMP(R2, IMM(741553));
  JUMP_EQ(EQUAL_T_BOOL_CHAR_INTEGER);
  //check if it's T_CHAR
  CMP(R2, IMM(181048));
  JUMP_EQ(EQUAL_T_BOOL_CHAR_INTEGER);
  //check if it's T_INTEGER
  CMP(R2, IMM(945311));
  JUMP_EQ(EQUAL_T_BOOL_CHAR_INTEGER);
  //check if it's T_STRING
  CMP(R2, IMM(799345));
  JUMP_EQ(EQUAL_T_STRING);
  //check if it's T_SYMBOL
  CMP(R2, IMM(368031));
  JUMP_EQ(EQUAL_T_BOOL_CHAR_INTEGER);
  //check if it's T_PAIR
  CMP(R2, IMM(885397));
  JUMP_EQ(EQUAL_T_PAIR);
  //check if it's T_VECTOR
  CMP(R2, IMM(335728));
  JUMP_EQ(EQUAL_T_VECTOR);
  //check if it's T_CLOSURE
  CMP(R2, IMM(276405));
  JUMP_EQ(EQUAL_T_CLOSURE);
  JUMP(EQUAL_NOT_EQUAL);
EQUAL_T_BOOL_CHAR_INTEGER:
  MOV(R0, INDD(R0, 1));
  MOV(R1, INDD(R1, 1));
  CMP(R0, R1);
  JUMP_EQ(EQUAL_EQUAL);
  JUMP(EQUAL_NOT_EQUAL);
  
EQUAL_T_STRING:
  MOV(R5, INDD(R0, 1));
  //SHOW("Length str1: ", R5);
  MOV(R6, INDD(R1, 1));
  //SHOW("Length str2: ", R6);
  CMP(R5, R6);
  JUMP_NE(EQUAL_NOT_EQUAL); //length do not match
  ADD(R5, IMM(1));
EQUAL_T_STRING_LOOP:
  CMP(R5, IMM(1)); //empty string
  JUMP_EQ(EQUAL_EQUAL);
  MOV(R3, INDD(R0, R5));
  MOV(R2, INDD(R1, R5));
  CMP(R3, R2);
  JUMP_NE(EQUAL_NOT_EQUAL);
  SUB(R5, IMM(1));
  JUMP(EQUAL_T_STRING_LOOP);
EQUAL_T_PAIR:
  MOV(R2, INDD(R0, 1));
  MOV(R3, INDD(R1, 1));
  PUSH(R0); //To be used later
  PUSH(R1); //To be used later
    //call recursively to Equal
  PUSH(IMM(0)); //magic number
  PUSH(R2);
  PUSH(R3);
  PUSH(IMM(2));		//param 1
  PUSH(IMM(2));		//param 2
  PUSH(IMM(0)); 	// ENV
  CALL(EQUAL);
  DROP(IMM(6));
  MOV(R0, INDD(R0, 1));
  CMP(R0, IMM(1));
  JUMP_NE(EQUAL_T_PAIR_NOT_EQUAL_DROP_2);
  POP(R1);
  POP(R0);
  MOV(R2, INDD(R0, 2));
  MOV(R3, INDD(R1, 2));
   //call recursively to Equal
  PUSH(IMM(0)); //magic number
  PUSH(R2);
  PUSH(R3);
  PUSH(IMM(2));		//param 1
  PUSH(IMM(2));		//param 2
  PUSH(IMM(0)); 	// ENV
  CALL(EQUAL);
  DROP(IMM(6));
  MOV(R0, INDD(R0, 1));
  CMP(R0, IMM(1));
  JUMP_NE(EQUAL_NOT_EQUAL);
  JUMP(EQUAL_EQUAL);
EQUAL_T_PAIR_NOT_EQUAL_DROP_2:
  DROP(IMM(2));
  JUMP(EQUAL_NOT_EQUAL);
  
EQUAL_T_VECTOR:
  MOV(R5, INDD(R0, 1));
  //SHOW("Length vec1: ", R5);
  MOV(R6, INDD(R1, 1));
  //SHOW("Length vec2: ", R6);
  CMP(R5, R6);
  JUMP_NE(EQUAL_NOT_EQUAL); //length do not match
  ADD(R5, IMM(1));
EQUAL_T_VECTOR_LOOP:
  CMP(R5, IMM(1)); //empty vector
  JUMP_EQ(EQUAL_EQUAL);
  //print_memory(0, 51);
  //SHOW("R0", R0);
  //SHOW("R1", R1);
  MOV(R2, R0);
  ADD(R2, R5);
  MOV(R2, ADDR(R2));
  MOV(R3, R1);
  ADD(R3, R5);
  MOV(R3, ADDR(R3));
  
    //call recursively to Equal
  //SHOW("GO_REC_NUM:", R5);
 // SHOW("R2:", R2);
  //SHOW("R3:", R3);
  
  PUSH(R0);
  PUSH(R1);
  PUSH(R5);
  
  PUSH(IMM(0)); //magic number
  PUSH(R2);
  PUSH(R3);
  PUSH(IMM(2));		//param 1
  PUSH(IMM(2));		//param 2
  PUSH(IMM(0)); 	// ENV
  CALL(EQUAL);
  DROP(IMM(6));
  //SHOW("After call R0:", R0);
 //print_memory(R0, R0+2);
  MOV(R0, INDD(R0, 1));
  //SHOW("R0:", R0);
  CMP(R0, IMM(1));
  JUMP_NE(EQUAL_T_VECTOR_NOT_EQUAL_DROP_3);

  POP(R5);
  POP(R1);
  POP(R0);  
  SUB(R5, IMM(1));
  JUMP(EQUAL_T_VECTOR_LOOP);
  
EQUAL_T_VECTOR_NOT_EQUAL_DROP_3:
  DROP(IMM(3));
  JUMP(EQUAL_NOT_EQUAL);
  
  
EQUAL_T_CLOSURE:
//SHOW("T_CLS R0:", R0);
//SHOW("R1: ", R1);
  CMP(R0, R1);
  JUMP_NE(EQUAL_NOT_EQUAL);
  JUMP(EQUAL_EQUAL);


EQUAL_EQUAL:
MOV(R0, IMM(1));
JUMP(EQUAL_END);
EQUAL_NOT_EQUAL:
MOV(R0, IMM(0));
EQUAL_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;

L_END_DEFS:

 // definition of constants 

PUSH(R0);
POP(R0);
<<<<<<< HEAD
int arr[458] = {937610, 722689, 741553, 0, 741553, 1, 799345, 3, 101, 113, 63, 368031, 0, 945311, 2, 799345, 4, 98, 105, 110, 43, 368031, 0, 945311, 1, 799345, 4, 98, 105, 110, 45, 368031, 0, 799345, 4, 98, 105, 110, 42, 368031, 0, 799345, 4, 98, 105, 110, 47, 368031, 0, 799345, 1, 62, 368031, 0, 799345, 1, 60, 368031, 0, 799345, 1, 61, 368031, 0, 799345, 5, 98, 105, 110, 60, 63, 368031, 0, 799345, 5, 98, 105, 110, 61, 63, 368031, 0, 799345, 4, 98, 105, 110, 60, 368031, 0, 799345, 4, 98, 105, 110, 61, 368031, 0, 799345, 4, 98, 105, 110, 62, 368031, 0, 799345, 5, 97, 112, 112, 108, 121, 368031, 0, 799345, 9, 114, 101, 109, 97, 105, 110, 100, 101, 114, 368031, 0, 799345, 10, 112, 114, 111, 99, 101, 100, 117, 114, 101, 63, 368031, 0, 799345, 3, 99, 97, 114, 368031, 0, 799345, 3, 99, 100, 114, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 62, 115, 121, 109, 98, 111, 108, 368031, 0, 799345, 7, 118, 101, 99, 116, 111, 114, 63, 368031, 0, 799345, 7, 115, 121, 109, 98, 111, 108, 63, 368031, 0, 799345, 7, 115, 116, 114, 105, 110, 103, 63, 368031, 0, 799345, 5, 99, 104, 97, 114, 63, 368031, 0, 799345, 7, 110, 117, 109, 98, 101, 114, 63, 368031, 0, 799345, 8, 98, 111, 111, 108, 101, 97, 110, 63, 368031, 0, 799345, 5, 112, 97, 105, 114, 63, 368031, 0, 799345, 5, 110, 117, 108, 108, 63, 368031, 0, 799345, 13, 105, 110, 116, 101, 103, 101, 114, 45, 62, 99, 104, 97, 114, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 62, 105, 110, 116, 101, 103, 101, 114, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 14, 115, 121, 109, 98, 111, 108, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 97, 114, 33, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 100, 114, 33, 368031, 0, 799345, 4, 99, 111, 110, 115, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 11, 115, 116, 114, 105, 110, 103, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 10, 115, 116, 114, 105, 110, 103, 45, 114, 101, 102, 368031, 0, 799345, 10, 118, 101, 99, 116, 111, 114, 45, 114, 101, 102, 368031, 0, 799345, 6, 101, 113, 117, 97, 108, 63, 368031, 0};
void *a = &arr;
memcpy(&((*machine).mem[10]), a, 458*4);
MOV(ADDR(0), IMM(468));

 //generating symbol table
MOV(R1, IMM(1));
//creating bucket for eq?
=======
int arr[1727] = {937610, 722689, 741553, 0, 741553, 1, 799345, 3, 110, 111, 116, 368031, 0, 799345, 4, 108, 105, 115, 116, 368031, 0, 799345, 4, 97, 100, 100, 49, 368031, 0, 799345, 4, 115, 117, 98, 49, 368031, 0, 799345, 5, 98, 105, 110, 62, 63, 368031, 0, 799345, 6, 98, 105, 110, 60, 61, 63, 368031, 0, 799345, 6, 98, 105, 110, 62, 61, 63, 368031, 0, 799345, 5, 122, 101, 114, 111, 63, 368031, 0, 799345, 6, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 5, 108, 105, 115, 116, 63, 368031, 0, 799345, 5, 101, 118, 101, 110, 63, 368031, 0, 799345, 4, 111, 100, 100, 63, 368031, 0, 799345, 3, 109, 97, 112, 368031, 0, 799345, 2, 121, 110, 368031, 0, 799345, 1, 43, 368031, 0, 799345, 1, 42, 368031, 0, 799345, 1, 45, 368031, 0, 799345, 1, 47, 368031, 0, 799345, 5, 111, 114, 100, 101, 114, 368031, 0, 799345, 6, 97, 110, 100, 109, 97, 112, 368031, 0, 799345, 2, 60, 62, 368031, 0, 799345, 5, 102, 111, 108, 100, 114, 368031, 0, 799345, 7, 99, 111, 109, 112, 111, 115, 101, 368031, 0, 799345, 4, 99, 97, 97, 114, 368031, 0, 799345, 4, 99, 97, 100, 114, 368031, 0, 799345, 4, 99, 100, 97, 114, 368031, 0, 799345, 4, 99, 100, 100, 114, 368031, 0, 799345, 5, 99, 97, 97, 97, 114, 368031, 0, 799345, 5, 99, 97, 97, 100, 114, 368031, 0, 799345, 5, 99, 97, 100, 97, 114, 368031, 0, 799345, 5, 99, 97, 100, 100, 114, 368031, 0, 799345, 5, 99, 100, 97, 97, 114, 368031, 0, 799345, 5, 99, 100, 97, 100, 114, 368031, 0, 799345, 5, 99, 100, 100, 97, 114, 368031, 0, 799345, 5, 99, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 100, 114, 368031, 0, 799345, 27, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 114, 105, 103, 104, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 26, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 108, 101, 102, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 8, 94, 99, 104, 97, 114, 45, 111, 112, 368031, 0, 799345, 1, 60, 368031, 0, 799345, 2, 60, 61, 368031, 0, 799345, 1, 62, 368031, 0, 799345, 2, 62, 61, 368031, 0, 799345, 6, 99, 104, 97, 114, 61, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 60, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 60, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 62, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 62, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 117, 112, 112, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 108, 111, 119, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 11, 99, 104, 97, 114, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 60, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 60, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 62, 63, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 62, 61, 63, 368031, 0, 799345, 12, 115, 116, 114, 105, 110, 103, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 15, 115, 116, 114, 105, 110, 103, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 8, 108, 105, 115, 116, 45, 114, 101, 102, 368031, 0, 799345, 5, 111, 114, 109, 97, 112, 368031, 0, 799345, 7, 109, 101, 109, 98, 101, 114, 63, 368031, 0, 799345, 9, 110, 101, 103, 97, 116, 105, 118, 101, 63, 368031, 0, 799345, 9, 112, 111, 115, 105, 116, 105, 118, 101, 63, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 6, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 12, 118, 101, 99, 116, 111, 114, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 9, 98, 111, 111, 108, 101, 97, 110, 61, 63, 368031, 0, 799345, 6, 101, 113, 117, 97, 108, 63, 368031, 0, 799345, 10, 94, 97, 115, 115, 111, 99, 105, 97, 116, 101, 368031, 0, 799345, 5, 97, 115, 115, 111, 99, 368031, 0, 799345, 4, 97, 115, 115, 113, 368031, 0, 799345, 4, 118, 111, 105, 100, 368031, 0, 799345, 5, 118, 111, 105, 100, 63, 368031, 0, 799345, 6, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 7, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 9, 108, 105, 115, 116, 45, 115, 101, 116, 33, 368031, 0, 799345, 3, 109, 97, 120, 368031, 0, 799345, 3, 109, 105, 110, 368031, 0, 799345, 5, 101, 114, 114, 111, 114, 368031, 0, 799345, 14, 110, 117, 109, 98, 101, 114, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 1, 61, 368031, 0, 799345, 4, 98, 105, 110, 43, 368031, 0, 945311, 1, 799345, 4, 98, 105, 110, 45, 368031, 0, 799345, 5, 98, 105, 110, 60, 63, 368031, 0, 945311, 0, 799345, 5, 110, 117, 108, 108, 63, 368031, 0, 799345, 3, 99, 100, 114, 368031, 0, 799345, 5, 112, 97, 105, 114, 63, 368031, 0, 799345, 9, 114, 101, 109, 97, 105, 110, 100, 101, 114, 368031, 0, 945311, 2, 799345, 3, 99, 97, 114, 368031, 0, 799345, 4, 99, 111, 110, 115, 368031, 0, 799345, 5, 97, 112, 112, 108, 121, 368031, 0, 799345, 4, 98, 105, 110, 42, 368031, 0, 799345, 4, 98, 105, 110, 47, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 62, 105, 110, 116, 101, 103, 101, 114, 368031, 0, 799345, 5, 98, 105, 110, 61, 63, 368031, 0, 181048, 65, 181048, 90, 181048, 97, 181048, 122, 799345, 13, 105, 110, 116, 101, 103, 101, 114, 45, 62, 99, 104, 97, 114, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 945311, -1, 799345, 10, 115, 116, 114, 105, 110, 103, 45, 114, 101, 102, 368031, 0, 799345, 11, 115, 116, 114, 105, 110, 103, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 3, 101, 113, 63, 368031, 0, 799345, 11, 118, 101, 99, 116, 111, 114, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 10, 118, 101, 99, 116, 111, 114, 45, 114, 101, 102, 368031, 0, 799345, 8, 98, 111, 111, 108, 101, 97, 110, 63, 368031, 0, 799345, 5, 99, 104, 97, 114, 63, 368031, 0, 799345, 7, 110, 117, 109, 98, 101, 114, 63, 368031, 0, 799345, 7, 115, 116, 114, 105, 110, 103, 63, 368031, 0, 799345, 7, 115, 121, 109, 98, 111, 108, 63, 368031, 0, 799345, 7, 118, 101, 99, 116, 111, 114, 63, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 97, 114, 33, 368031, 0, 945311, 48, 945311, 123, 945311, 10, 799345, 4, 98, 105, 110, 60, 368031, 0, 799345, 4, 98, 105, 110, 61, 368031, 0, 799345, 4, 98, 105, 110, 62, 368031, 0, 799345, 10, 112, 114, 111, 99, 101, 100, 117, 114, 101, 63, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 62, 115, 121, 109, 98, 111, 108, 368031, 0, 799345, 14, 115, 121, 109, 98, 111, 108, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 100, 114, 33, 368031, 0};
void *a = &arr;
memcpy(&((*machine).mem[10]), a, 1727*4);
MOV(ADDR(0), IMM(1737));

 //generating symbol table
MOV(R1, IMM(1));
//creating bucket for not
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(16)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(22), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
<<<<<<< HEAD
//creating bucket for bin+
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(25)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(32), R0); // save the bucket address in the symbol object 
=======
//creating bucket for list
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(23)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(30), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for add1
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(31)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(38), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin-
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(35)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(42), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(39)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(46), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin*
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(43)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(50), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(47)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(55), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin/
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(51)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(58), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(56)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(65), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for >
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(59)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(63), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(66)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(75), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(64)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(68), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(76)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(84), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for =
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(69)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(73), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(85)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(94), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(74)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(82), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(95)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(103), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(83)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(91), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(104)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(112), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(92)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(99), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(113)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(120), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(100)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(107), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(121)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(127), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(108)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(115), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(128)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(133), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for apply
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(116)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(124), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(134)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(138), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for remainder
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(125)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(137), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(139)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(143), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for procedure?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(138)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(151), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(144)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(148), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for car
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(152)); // add pointer to the string 
=======
MOV(INDD(R0, IMM(0)), IMM(149)); // add pointer to the string 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(153), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(154)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
<<<<<<< HEAD
MOV(ADDR(165), R0); // save the bucket address in the symbol object 
=======
MOV(ADDR(162), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for andmap
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(163)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(172), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->symbol
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(166)); // add pointer to the string 
=======
MOV(INDD(R0, IMM(0)), IMM(173)); // add pointer to the string 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(178), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(179)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
<<<<<<< HEAD
MOV(ADDR(194), R0); // save the bucket address in the symbol object 
=======
MOV(ADDR(187), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for compose
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(188)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(198), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(199)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(206), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(207)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(214), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(215)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(222), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(195)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(205), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(223)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(230), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(206)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(216), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(231)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(239), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(217)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(225), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(240)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(248), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for number?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(226)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(236), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(249)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(257), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for boolean?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(237)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(248), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(258)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(266), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for pair?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(249)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(257), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(267)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(275), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for null?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(258)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(266), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(276)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(284), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for integer->char
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(267)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(283), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(285)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(293), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char->integer
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(284)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(300), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(294)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(302), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(301)); // add pointer to the string 
=======
MOV(INDD(R0, IMM(0)), IMM(303)); // add pointer to the string 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(312), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(313)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
<<<<<<< HEAD
MOV(ADDR(335), R0); // save the bucket address in the symbol object 
=======
MOV(ADDR(322), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caadar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(323)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(332), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-car!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(336)); // add pointer to the string 
=======
MOV(INDD(R0, IMM(0)), IMM(333)); // add pointer to the string 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(342), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-cdr!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(343)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
<<<<<<< HEAD
MOV(ADDR(359), R0); // save the bucket address in the symbol object 
=======
MOV(ADDR(352), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cons
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(360)); // add pointer to the string 
=======
MOV(INDD(R0, IMM(0)), IMM(353)); // add pointer to the string 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(362), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(363)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
<<<<<<< HEAD
MOV(ADDR(382), R0); // save the bucket address in the symbol object 
=======
MOV(ADDR(372), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(373)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(382), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(383)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(392), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(393)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(402), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(383)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(399), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(403)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(412), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(400)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(414), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(413)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(422), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(415)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(429), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(423)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(432), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(430)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(443), R0); // save the bucket address in the symbol object 
=======
MOV(INDD(R0, IMM(0)), IMM(433)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(442), R0); // save the bucket address in the symbol object 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
<<<<<<< HEAD
MOV(INDD(R0, IMM(0)), IMM(444)); // add pointer to the string 
=======
MOV(INDD(R0, IMM(0)), IMM(443)); // add pointer to the string 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(452), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for equal?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(453)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(462), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
<<<<<<< HEAD
MOV(ADDR(R1), IMM(0));

 // bindings primitive procedures to their symbols

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(164),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_PLUS
PUSH(LABEL(BIN_PLUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(31),1));
MOV(INDD(R1, 1), R0);
=======
//creating bucket for ^variadic-right-from-binary
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(463)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(493), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ^variadic-left-from-binary
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(494)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(523), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ^char-op
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(524)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(535), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(536)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(540), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(541)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(546), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for >
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(547)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(551), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for >=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(552)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(557), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(558)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(567), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(568)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(578), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(579)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(588), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(589)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(599), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(600)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(609), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-uppercase?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(610)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(628), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-lowercase?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(629)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(647), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-upcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(648)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(662), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-downcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(663)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(679), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(680)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(693), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(694)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(706), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(707)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(719), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(720)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(732), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(733)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(746), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->list
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(747)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(762), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(763)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(778), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-upcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(779)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(795), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-downcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(796)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(814), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(815)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(826), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ormap
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(827)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(835), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for member?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(836)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(846), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for negative?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(847)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(859), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for positive?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(860)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(872), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list->vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(873)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(888), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(889)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(898), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(899)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(917), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(918)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(936), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(937)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(955), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(956)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(975), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(976)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(995), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(996)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1007), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1008)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1019), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1020)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1031), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1032)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1044), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1045)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1057), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector->list
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1058)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1073), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for boolean=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1074)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1086), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for equal?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1087)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1096), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ^associate
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1097)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1110), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for assoc
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1111)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1119), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for assq
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1120)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1127), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for void
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1128)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1135), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for void?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1136)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1144), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for append
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1145)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1154), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-append
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1155)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1171), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-append
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1172)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1188), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for reverse
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1189)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1199), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-reverse
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1200)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1217), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1218)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1230), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for max
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1231)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1237), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for min
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1238)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1244), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for error
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1245)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1253), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for number->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1254)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1271), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for =
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1272)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1276), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin+
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1277)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1284), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin-
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1287)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1294), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1295)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1303), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for null?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1306)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1314), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1315)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1321), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for pair?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1322)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1330), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for remainder
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1331)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1343), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for car
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1346)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1352), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cons
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1353)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1360), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for apply
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1361)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1369), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin*
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1370)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1377), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin/
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1378)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1385), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char->integer
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1386)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1402), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1403)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1411), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for integer->char
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1420)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1436), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1437)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1453), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1456)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1469), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1470)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1484), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1485)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1499), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for eq?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1500)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1506), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1507)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1521), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1522)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1536), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1537)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1553), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1554)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1567), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for boolean?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1568)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1579), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1580)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1588), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for number?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1589)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1599), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1600)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1610), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1611)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1621), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1622)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1632), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-car!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1633)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1644), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1651)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1658), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1659)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1666), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1667)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1674), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for procedure?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1675)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1688), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->symbol
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1689)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1706), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1707)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1724), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-cdr!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1725)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1736), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
MOV(ADDR(R1), IMM(0));

 // bindings primitive procedures to their symbols

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1320),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_PLUS
PUSH(LABEL(BIN_PLUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1283),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for APPLY
PUSH(LABEL(APPLY));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1368),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MINUS
PUSH(LABEL(BIN_MINUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1293),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MUL
PUSH(LABEL(BIN_MUL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1376),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_DIV
PUSH(LABEL(BIN_DIV));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1384),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(550),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1673),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(539),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1275),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1410),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1302),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1665),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1657),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CAR
PUSH(LABEL(CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1351),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1320),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_TO_SYMBOL
PUSH(LABEL(STRING_TO_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1705),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for REMAINDER
PUSH(LABEL(REMAINDER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1342),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PROCEDURE
PUSH(LABEL(IS_PROCEDURE));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1687),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_VECTOR
PUSH(LABEL(IS_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1631),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_SYMBOL
PUSH(LABEL(IS_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1620),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_STRING
PUSH(LABEL(IS_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1609),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_CHAR
PUSH(LABEL(IS_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1587),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NUMBER
PUSH(LABEL(IS_NUMBER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1598),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_BOOLEAN
PUSH(LABEL(IS_BOOLEAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1578),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PAIR
PUSH(LABEL(IS_PAIR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1329),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NULL
PUSH(LABEL(IS_NULL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1313),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for INTEGER_TO_CHAR
PUSH(LABEL(INTEGER_TO_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1435),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CHAR_TO_INTEGER
PUSH(LABEL(CHAR_TO_INTEGER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1401),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1452),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1552),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_STRING
PUSH(LABEL(MAKE_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1498),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_VECTOR
PUSH(LABEL(MAKE_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1535),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SYMBOL_TO_STRING
PUSH(LABEL(SYMBOL_TO_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1723),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CAR
PUSH(LABEL(SET_CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1643),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CDR
PUSH(LABEL(SET_CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1735),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CONS
PUSH(LABEL(CONS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1359),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_SET
PUSH(LABEL(STRING_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1483),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_REF
PUSH(LABEL(STRING_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1468),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for VECTOR_REF
PUSH(LABEL(VECTOR_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1566),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQ
PUSH(LABEL(EQ));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1505),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL
PUSH(LABEL(EQUAL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1095),1));
MOV(INDD(R1, 1), R0);

 // fixing the stack before executing the actual code
PUSH(IMM(0)); //magic number 
PUSH(IMM(0)); //0 arguments 
PUSH(IMM(0)); //0 arguments indicator 
PUSH(IMM(1)); //empty environment 
CALL(MALLOC);
DROP(IMM(1));
PUSH(R0);
PUSH(IMM(9999)); // dummy return address 
PUSH(IMM(9999)); //just like the beginning of a function body 
MOV(FP,SP);

 // actual code

 // DEFINEnot

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1819));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1817:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1816);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1817);

L_FOR1_END_1816:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1815:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1814);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1815);

L_FOR2_END_1814:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1818);
L_CLOS_CODE_1819: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1813);
printf("at JUMP_SIMPLE_1813");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1813:
//IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1822); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1821);
		    L_THEN_1822:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_1820);
		    L_ELSE_1821:
		    MOV(R0,IMM(14));

		    L_IF_EXIT_1820:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1818:
MOV(R1, INDD(IMM(21), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1812));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1810:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1809);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1810);

L_FOR1_END_1809:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1808:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1807);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1808);

L_FOR2_END_1807:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1811);
L_CLOS_CODE_1812: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_1811:
MOV(R1, INDD(IMM(29), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEadd1

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1805));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1803:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1802);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1803);

L_FOR1_END_1802:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1801:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1800);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1801);

L_FOR2_END_1800:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1804);
L_CLOS_CODE_1805: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1799);
printf("at JUMP_SIMPLE_1799");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1799:

 //TC-APPLIC 

MOV(R0,IMM(1285));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1283));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1806);
printf("At L_JUMP_1806\n");
fflush(stdout);
L_JUMP_1806:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1804:
MOV(R1, INDD(IMM(37), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEsub1

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1797));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1795:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1794);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1795);

L_FOR1_END_1794:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1793:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1792);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1793);

L_FOR2_END_1792:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1796);
L_CLOS_CODE_1797: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1791);
printf("at JUMP_SIMPLE_1791");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1791:

 //TC-APPLIC 

MOV(R0,IMM(1285));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1293));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1798);
printf("At L_JUMP_1798\n");
fflush(stdout);
L_JUMP_1798:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1796:
MOV(R1, INDD(IMM(45), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbin>?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1789));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1787:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1786);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1787);

L_FOR1_END_1786:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1785:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1784);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1785);

L_FOR2_END_1784:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1788);
L_CLOS_CODE_1789: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1783);
printf("at JUMP_SIMPLE_1783");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1783:

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1302));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1790);
printf("At L_JUMP_1790\n");
fflush(stdout);
L_JUMP_1790:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1788:
MOV(R1, INDD(IMM(54), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbin<=?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1780));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1778:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1777);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1778);

L_FOR1_END_1777:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1776:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1775);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1776);

L_FOR2_END_1775:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1779);
L_CLOS_CODE_1780: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1774);
printf("at JUMP_SIMPLE_1774");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1774:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(54));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1782\n");
JUMP_EQ(L_JUMP_1782);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1782:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(21));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1781);
printf("At L_JUMP_1781\n");
fflush(stdout);
L_JUMP_1781:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1779:
MOV(R1, INDD(IMM(64), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbin>=?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1771));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1769:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1768);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1769);

L_FOR1_END_1768:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1767:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1766);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1767);

L_FOR2_END_1766:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1770);
L_CLOS_CODE_1771: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1765);
printf("at JUMP_SIMPLE_1765");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1765:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1302));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1773\n");
JUMP_EQ(L_JUMP_1773);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1773:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(21));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1772);
printf("At L_JUMP_1772\n");
fflush(stdout);
L_JUMP_1772:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1770:
MOV(R1, INDD(IMM(74), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEzero?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1763));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1761:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1760);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1761);

L_FOR1_END_1760:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1759:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1758);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1759);

L_FOR2_END_1758:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1762);
L_CLOS_CODE_1763: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1757);
printf("at JUMP_SIMPLE_1757");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1757:

 //TC-APPLIC 

MOV(R0,IMM(1304));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1764);
printf("At L_JUMP_1764\n");
fflush(stdout);
L_JUMP_1764:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1762:
MOV(R1, INDD(IMM(83), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElength

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1749));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1747:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1746);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1747);

L_FOR1_END_1746:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1745:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1744);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1745);

L_FOR2_END_1744:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1748);
L_CLOS_CODE_1749: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1743);
printf("at JUMP_SIMPLE_1743");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1743:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1756\n");
JUMP_EQ(L_JUMP_1756);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1756:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1752); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1751);
		    L_THEN_1752:MOV(R0,IMM(1304));

		    JUMP(L_IF_EXIT_1750);
		    L_ELSE_1751:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1755\n");
JUMP_EQ(L_JUMP_1755);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1755:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(93));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1754\n");
JUMP_EQ(L_JUMP_1754);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1754:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(37));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1753);
printf("At L_JUMP_1753\n");
fflush(stdout);
L_JUMP_1753:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1750:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1748:
MOV(R1, INDD(IMM(93), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1734));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1732:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1731);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1732);

L_FOR1_END_1731:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1730:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1729);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1730);

L_FOR2_END_1729:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1733);
L_CLOS_CODE_1734: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1728);
printf("at JUMP_SIMPLE_1728");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1728:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1742\n");
JUMP_EQ(L_JUMP_1742);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1742:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
// OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1735);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1735);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1329));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1741\n");
JUMP_EQ(L_JUMP_1741);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1741:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1738); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1737);
		    L_THEN_1738:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1740\n");
JUMP_EQ(L_JUMP_1740);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1740:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(102));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1739);
printf("At L_JUMP_1739\n");
fflush(stdout);
L_JUMP_1739:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1736);
		    L_ELSE_1737:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1736:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1735);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1735);
L_OR_END_1735:

POP(FP);
RETURN;
L_CLOS_EXIT_1733:
MOV(R1, INDD(IMM(102), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEeven?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1725));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1723:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1722);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1723);

L_FOR1_END_1722:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1721:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1720);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1721);

L_FOR2_END_1720:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1724);
L_CLOS_CODE_1725: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1719);
printf("at JUMP_SIMPLE_1719");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1719:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1344));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1342));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1727\n");
JUMP_EQ(L_JUMP_1727);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1727:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(83));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1726);
printf("At L_JUMP_1726\n");
fflush(stdout);
L_JUMP_1726:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1724:
MOV(R1, INDD(IMM(111), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEodd?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1715));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1713:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1712);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1713);

L_FOR1_END_1712:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1711:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1710);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1711);

L_FOR2_END_1710:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1714);
L_CLOS_CODE_1715: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1709);
printf("at JUMP_SIMPLE_1709");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1709:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1344));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1342));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1718\n");
JUMP_EQ(L_JUMP_1718);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1718:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(83));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1717\n");
JUMP_EQ(L_JUMP_1717);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1717:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(21));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1716);
printf("At L_JUMP_1716\n");
fflush(stdout);
L_JUMP_1716:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1714:
MOV(R1, INDD(IMM(119), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEmap

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1675));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1673:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1672);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1673);

L_FOR1_END_1672:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1671:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1670);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1671);

L_FOR2_END_1670:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1674);
L_CLOS_CODE_1675: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1669);
printf("at JUMP_SIMPLE_1669");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1669:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1699));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1697:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1696);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1697);

L_FOR1_END_1696:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1695:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1694);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1695);

L_FOR2_END_1694:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1698);
L_CLOS_CODE_1699: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1693);
printf("at JUMP_SIMPLE_1693");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1693:

 //TC-APPLIC 


//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1706));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1704:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1703);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1704);

L_FOR1_END_1703:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1702:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1701);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1702);

L_FOR2_END_1701:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1705);
L_CLOS_CODE_1706: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1708\n");
JUMP_EQ(L_JUMP_1708);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1708:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1707);
printf("At L_JUMP_1707\n");
fflush(stdout);
L_JUMP_1707:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1705:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1700);
printf("At L_JUMP_1700\n");
fflush(stdout);
L_JUMP_1700:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1698:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1683));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1681:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1680);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1681);

L_FOR1_END_1680:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1679:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1678);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1679);

L_FOR2_END_1678:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1682);
L_CLOS_CODE_1683: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1677);
printf("at JUMP_SIMPLE_1677");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1677:

 //TC-APPLIC 


//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1690));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1688:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1687);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1688);

L_FOR1_END_1687:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1686:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1685);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1686);

L_FOR2_END_1685:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1689);
L_CLOS_CODE_1690: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1692\n");
JUMP_EQ(L_JUMP_1692);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1692:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1691);
printf("At L_JUMP_1691\n");
fflush(stdout);
L_JUMP_1691:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1689:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1684);
printf("At L_JUMP_1684\n");
fflush(stdout);
L_JUMP_1684:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1682:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1676);
printf("At L_JUMP_1676\n");
fflush(stdout);
L_JUMP_1676:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1674:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1596));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1594:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1593);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1594);

L_FOR1_END_1593:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1592:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1591);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1592);

L_FOR2_END_1591:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1595);
L_CLOS_CODE_1596: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1590);
printf("at JUMP_SIMPLE_1590");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1590:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1652));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1650:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1649);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1650);

L_FOR1_END_1649:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1648:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1647);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1648);

L_FOR2_END_1647:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1651);
L_CLOS_CODE_1652: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1646);
printf("at JUMP_SIMPLE_1646");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1646:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1659));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1657:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1656);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1657);

L_FOR1_END_1656:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1655:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1654);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1655);

L_FOR2_END_1654:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1658);
L_CLOS_CODE_1659: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1653);
printf("at JUMP_SIMPLE_1653");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1653:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1668\n");
JUMP_EQ(L_JUMP_1668);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1668:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1662); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1661);
		    L_THEN_1662:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_1660);
		    L_ELSE_1661:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1667\n");
JUMP_EQ(L_JUMP_1667);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1667:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1666\n");
JUMP_EQ(L_JUMP_1666);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1666:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1665\n");
JUMP_EQ(L_JUMP_1665);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1665:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1664\n");
JUMP_EQ(L_JUMP_1664);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1664:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1359));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1663);
printf("At L_JUMP_1663\n");
fflush(stdout);
L_JUMP_1663:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1660:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1658:

POP(FP);
RETURN;
L_CLOS_EXIT_1651:
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1645\n");
JUMP_EQ(L_JUMP_1645);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1645:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1604));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1602:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1601);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1602);

L_FOR1_END_1601:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1600:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1599);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1600);

L_FOR2_END_1599:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1603);
L_CLOS_CODE_1604: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1598);
printf("at JUMP_SIMPLE_1598");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1598:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1627));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1625:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1624);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1625);

L_FOR1_END_1624:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1623:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1622);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1623);

L_FOR2_END_1622:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1626);
L_CLOS_CODE_1627: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1621);
printf("at JUMP_SIMPLE_1621");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1621:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1634));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1632:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1631);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1632);

L_FOR1_END_1631:
//for (i = 0; i < 4; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1630:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1629);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1630);

L_FOR2_END_1629:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1633);
L_CLOS_CODE_1634: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1628);
printf("at JUMP_SIMPLE_1628");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1628:
//IF 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1644\n");
JUMP_EQ(L_JUMP_1644);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1644:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1643\n");
JUMP_EQ(L_JUMP_1643);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1643:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1637); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1636);
		    L_THEN_1637:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_1635);
		    L_ELSE_1636:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1642\n");
JUMP_EQ(L_JUMP_1642);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1642:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1641\n");
JUMP_EQ(L_JUMP_1641);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1641:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1640\n");
JUMP_EQ(L_JUMP_1640);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1640:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1639\n");
JUMP_EQ(L_JUMP_1639);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1639:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1359));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1638);
printf("At L_JUMP_1638\n");
fflush(stdout);
L_JUMP_1638:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1635:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1633:

POP(FP);
RETURN;
L_CLOS_EXIT_1626:
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1620\n");
JUMP_EQ(L_JUMP_1620);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1620:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1612));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1610:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1609);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1610);

L_FOR1_END_1609:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1608:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1607);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1608);

L_FOR2_END_1607:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1611);
L_CLOS_CODE_1612: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1606);
printf("at JUMP_SIMPLE_1606");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1606:

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1614));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1616:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1617);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1616);

L_FOR1_END_1617:
//for (i = 0; i < 4; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1618:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1619);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1618);

L_FOR2_END_1619:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1615);
L_CLOS_CODE_1614: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1613);
printf("At L_JUMP_1613\n");
fflush(stdout);
L_JUMP_1613:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1615:

POP(FP);
RETURN;
L_CLOS_EXIT_1611:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1605);
printf("At L_JUMP_1605\n");
fflush(stdout);
L_JUMP_1605:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1603:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1597);
printf("At L_JUMP_1597\n");
fflush(stdout);
L_JUMP_1597:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1595:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1589\n");
JUMP_EQ(L_JUMP_1589);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1589:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(126), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEyn

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1547));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1545:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1544);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1545);

L_FOR1_END_1544:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1543:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1542);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1543);

L_FOR2_END_1542:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1546);
L_CLOS_CODE_1547: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1565));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1563:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1562);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1563);

L_FOR1_END_1562:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1561:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1560);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1561);

L_FOR2_END_1560:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1564);
L_CLOS_CODE_1565: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1559);
printf("at JUMP_SIMPLE_1559");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1559:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1571));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1569:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1568);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1569);

L_FOR1_END_1568:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1567:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1566);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1567);

L_FOR2_END_1566:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1570);
L_CLOS_CODE_1571: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1580));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1578:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1577);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1578);

L_FOR1_END_1577:
//for (i = 0; i < 4; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1576:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1575);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1576);

L_FOR2_END_1575:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1579);
L_CLOS_CODE_1580: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1574);
printf("at JUMP_SIMPLE_1574");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1574:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(6));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1586));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_1584:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1583);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1584);

L_FOR1_END_1583:
//for (i = 0; i < 5; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1582:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1581);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1582);

L_FOR2_END_1581:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1585);
L_CLOS_CODE_1586: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1588\n");
JUMP_EQ(L_JUMP_1588);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1588:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1587);
printf("At L_JUMP_1587\n");
fflush(stdout);
L_JUMP_1587:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1585:

POP(FP);
RETURN;
L_CLOS_EXIT_1579:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1573\n");
JUMP_EQ(L_JUMP_1573);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1573:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1572);
printf("At L_JUMP_1572\n");
fflush(stdout);
L_JUMP_1572:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1570:

POP(FP);
RETURN;
L_CLOS_EXIT_1564:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1558\n");
JUMP_EQ(L_JUMP_1558);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1558:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1555));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1553:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1552);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1553);

L_FOR1_END_1552:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1551:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1550);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1551);

L_FOR2_END_1550:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1554);
L_CLOS_CODE_1555: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1549);
printf("at JUMP_SIMPLE_1549");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1549:

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1557\n");
JUMP_EQ(L_JUMP_1557);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1557:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1556);
printf("At L_JUMP_1556\n");
fflush(stdout);
L_JUMP_1556:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1554:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1548);
printf("At L_JUMP_1548\n");
fflush(stdout);
L_JUMP_1548:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1546:
MOV(R1, INDD(IMM(132), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE+

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1526));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1524:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1523);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1524);

L_FOR1_END_1523:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1522:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1521);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1522);

L_FOR2_END_1521:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1525);
L_CLOS_CODE_1526: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1520);
printf("at JUMP_SIMPLE_1520");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1520:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1533));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1531:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1530);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1531);

L_FOR1_END_1530:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1529:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1528);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1529);

L_FOR2_END_1528:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1532);
L_CLOS_CODE_1533: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1527);
printf("at JUMP_SIMPLE_1527");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1527:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1541\n");
JUMP_EQ(L_JUMP_1541);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1541:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1536); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1535);
		    L_THEN_1536:MOV(R0,IMM(1304));

		    JUMP(L_IF_EXIT_1534);
		    L_ELSE_1535:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1540\n");
JUMP_EQ(L_JUMP_1540);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1540:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1539\n");
JUMP_EQ(L_JUMP_1539);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1539:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1538\n");
JUMP_EQ(L_JUMP_1538);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1538:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1283));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1537);
printf("At L_JUMP_1537\n");
fflush(stdout);
L_JUMP_1537:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1534:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1532:

POP(FP);
RETURN;
L_CLOS_EXIT_1525:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1512));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1510:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1509);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1510);

L_FOR1_END_1509:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1508:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1507);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1508);

L_FOR2_END_1507:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1511);
L_CLOS_CODE_1512: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1506);
printf("at JUMP_SIMPLE_1506");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1506:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1518));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1516:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1515);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1516);

L_FOR1_END_1515:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1514:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1513);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1514);

L_FOR2_END_1513:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1517);
L_CLOS_CODE_1518: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1519);
printf("At L_JUMP_1519\n");
fflush(stdout);
L_JUMP_1519:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1517:

POP(FP);
RETURN;
L_CLOS_EXIT_1511:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1505\n");
JUMP_EQ(L_JUMP_1505);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1505:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(137), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE*

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1489));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1487:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1486);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1487);

L_FOR1_END_1486:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1485:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1484);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1485);

L_FOR2_END_1484:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1488);
L_CLOS_CODE_1489: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1483);
printf("at JUMP_SIMPLE_1483");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1483:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1496));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1494:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1493);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1494);

L_FOR1_END_1493:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1492:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1491);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1492);

L_FOR2_END_1491:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1495);
L_CLOS_CODE_1496: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1490);
printf("at JUMP_SIMPLE_1490");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1490:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1504\n");
JUMP_EQ(L_JUMP_1504);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1504:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1499); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1498);
		    L_THEN_1499:MOV(R0,IMM(1285));

		    JUMP(L_IF_EXIT_1497);
		    L_ELSE_1498:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1503\n");
JUMP_EQ(L_JUMP_1503);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1503:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1502\n");
JUMP_EQ(L_JUMP_1502);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1502:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1501\n");
JUMP_EQ(L_JUMP_1501);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1501:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1376));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1500);
printf("At L_JUMP_1500\n");
fflush(stdout);
L_JUMP_1500:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1497:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1495:

POP(FP);
RETURN;
L_CLOS_EXIT_1488:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1475));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1473:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1472);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1473);

L_FOR1_END_1472:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1471:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1470);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1471);

L_FOR2_END_1470:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1474);
L_CLOS_CODE_1475: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1469);
printf("at JUMP_SIMPLE_1469");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1469:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1481));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1479:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1478);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1479);

L_FOR1_END_1478:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1477:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1476);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1477);

L_FOR2_END_1476:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1480);
L_CLOS_CODE_1481: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1482);
printf("At L_JUMP_1482\n");
fflush(stdout);
L_JUMP_1482:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1480:

POP(FP);
RETURN;
L_CLOS_EXIT_1474:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1468\n");
JUMP_EQ(L_JUMP_1468);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1468:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(142), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE-

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1462));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1464:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1465);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1464);

L_FOR1_END_1465:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1466:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1467);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1466);

L_FOR2_END_1467:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1463);
L_CLOS_CODE_1462: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1461\n");
JUMP_EQ(L_JUMP_1461);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1461:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1457); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1456);
		    L_THEN_1457:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1304));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1293));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1460);
printf("At L_JUMP_1460\n");
fflush(stdout);
L_JUMP_1460:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1455);
		    L_ELSE_1456:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1459\n");
JUMP_EQ(L_JUMP_1459);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1459:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1293));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1458);
printf("At L_JUMP_1458\n");
fflush(stdout);
L_JUMP_1458:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1455:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1463:
MOV(R1, INDD(IMM(147), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE/

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1449));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1451:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1452);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1451);

L_FOR1_END_1452:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1453:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1454);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1453);

L_FOR2_END_1454:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1450);
L_CLOS_CODE_1449: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1448\n");
JUMP_EQ(L_JUMP_1448);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1448:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1444); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1443);
		    L_THEN_1444:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1285));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1384));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1447);
printf("At L_JUMP_1447\n");
fflush(stdout);
L_JUMP_1447:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1442);
		    L_ELSE_1443:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(142));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1446\n");
JUMP_EQ(L_JUMP_1446);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1446:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1384));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1445);
printf("At L_JUMP_1445\n");
fflush(stdout);
L_JUMP_1445:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1442:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1450:
MOV(R1, INDD(IMM(152), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEorder

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1402));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1400:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1399);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1400);

L_FOR1_END_1399:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1398:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1397);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1398);

L_FOR2_END_1397:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1401);
L_CLOS_CODE_1402: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1396);
printf("at JUMP_SIMPLE_1396");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1396:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1424));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1422:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1421);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1422);

L_FOR1_END_1421:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1420:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1419);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1420);

L_FOR2_END_1419:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1423);
L_CLOS_CODE_1424: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1418);
printf("at JUMP_SIMPLE_1418");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1418:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1431));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1429:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1428);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1429);

L_FOR1_END_1428:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1427:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1426);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1427);

L_FOR2_END_1426:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1430);
L_CLOS_CODE_1431: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1425);
printf("at JUMP_SIMPLE_1425");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1425:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1441\n");
JUMP_EQ(L_JUMP_1441);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1441:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
// OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1432);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1432);
//IF 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1440\n");
JUMP_EQ(L_JUMP_1440);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1440:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1439\n");
JUMP_EQ(L_JUMP_1439);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1439:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1435); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1434);
		    L_THEN_1435:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1438\n");
JUMP_EQ(L_JUMP_1438);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1438:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1437\n");
JUMP_EQ(L_JUMP_1437);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1437:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1436);
printf("At L_JUMP_1436\n");
fflush(stdout);
L_JUMP_1436:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1433);
		    L_ELSE_1434:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1433:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1432);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1432);
L_OR_END_1432:

POP(FP);
RETURN;
L_CLOS_EXIT_1430:

POP(FP);
RETURN;
L_CLOS_EXIT_1423:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1410));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1408:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1407);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1408);

L_FOR1_END_1407:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1406:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1405);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1406);

L_FOR2_END_1405:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1409);
L_CLOS_CODE_1410: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1404);
printf("at JUMP_SIMPLE_1404");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1404:

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1412));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1414:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1415);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1414);

L_FOR1_END_1415:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1416:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1417);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1416);

L_FOR2_END_1417:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1413);
L_CLOS_CODE_1412: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1411);
printf("At L_JUMP_1411\n");
fflush(stdout);
L_JUMP_1411:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1413:

POP(FP);
RETURN;
L_CLOS_EXIT_1409:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1403);
printf("At L_JUMP_1403\n");
fflush(stdout);
L_JUMP_1403:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1401:
MOV(R1, INDD(IMM(161), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEandmap

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1390));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1392:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1393);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1392);

L_FOR1_END_1393:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1394:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1395);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1394);

L_FOR2_END_1395:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1391);
L_CLOS_CODE_1390: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1372));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1370:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1369);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1370);

L_FOR1_END_1369:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1368:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1367);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1368);

L_FOR2_END_1367:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1371);
L_CLOS_CODE_1372: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1366);
printf("at JUMP_SIMPLE_1366");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1366:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1379));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1377:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1376);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1377);

L_FOR1_END_1376:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1375:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1374);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1375);

L_FOR2_END_1374:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1378);
L_CLOS_CODE_1379: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1373);
printf("at JUMP_SIMPLE_1373");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1373:

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1389\n");
JUMP_EQ(L_JUMP_1389);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1389:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1388\n");
JUMP_EQ(L_JUMP_1388);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1388:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
// OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1380);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1380);
//IF 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1387\n");
JUMP_EQ(L_JUMP_1387);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1387:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1386\n");
JUMP_EQ(L_JUMP_1386);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1386:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1383); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1382);
		    L_THEN_1383:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1385\n");
JUMP_EQ(L_JUMP_1385);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1385:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1384);
printf("At L_JUMP_1384\n");
fflush(stdout);
L_JUMP_1384:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1381);
		    L_ELSE_1382:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1381:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1380);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1380);
L_OR_END_1380:

POP(FP);
RETURN;
L_CLOS_EXIT_1378:

POP(FP);
RETURN;
L_CLOS_EXIT_1371:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1364));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1362:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1361);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1362);

L_FOR1_END_1361:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1360:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1359);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1360);

L_FOR2_END_1359:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1363);
L_CLOS_CODE_1364: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1358);
printf("at JUMP_SIMPLE_1358");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1358:

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1365);
printf("At L_JUMP_1365\n");
fflush(stdout);
L_JUMP_1365:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1363:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1357);
printf("At L_JUMP_1357\n");
fflush(stdout);
L_JUMP_1357:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1391:
MOV(R1, INDD(IMM(171), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE<>

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1331));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1329:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1328);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1329);

L_FOR1_END_1328:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1327:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1326);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1327);

L_FOR2_END_1326:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1330);
L_CLOS_CODE_1331: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1325);
printf("at JUMP_SIMPLE_1325");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1325:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1338));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1336:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1335);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1336);

L_FOR1_END_1335:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1334:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1333);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1334);

L_FOR2_END_1333:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1337);
L_CLOS_CODE_1338: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1332);
printf("at JUMP_SIMPLE_1332");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1332:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1356\n");
JUMP_EQ(L_JUMP_1356);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1356:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
// OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1339);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1339);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1353));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1351:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1350);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1351);

L_FOR1_END_1350:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1349:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1348);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1349);

L_FOR2_END_1348:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1352);
L_CLOS_CODE_1353: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1347);
printf("at JUMP_SIMPLE_1347");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1347:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1355\n");
JUMP_EQ(L_JUMP_1355);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1355:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(21));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1354);
printf("At L_JUMP_1354\n");
fflush(stdout);
L_JUMP_1354:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1352:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(171));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1346\n");
JUMP_EQ(L_JUMP_1346);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1346:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1342); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1341);
		    L_THEN_1342:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1345\n");
JUMP_EQ(L_JUMP_1345);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1345:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1344\n");
JUMP_EQ(L_JUMP_1344);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1344:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1343);
printf("At L_JUMP_1343\n");
fflush(stdout);
L_JUMP_1343:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1340);
		    L_ELSE_1341:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1340:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1339);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1339);
L_OR_END_1339:

POP(FP);
RETURN;
L_CLOS_EXIT_1337:

POP(FP);
RETURN;
L_CLOS_EXIT_1330:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1315));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1313:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1312);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1313);

L_FOR1_END_1312:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1311:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1310);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1311);

L_FOR2_END_1310:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1314);
L_CLOS_CODE_1315: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1309);
printf("at JUMP_SIMPLE_1309");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1309:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1321));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1319:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1318);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1319);

L_FOR1_END_1318:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1317:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1316);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1317);

L_FOR2_END_1316:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1320);
L_CLOS_CODE_1321: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1324\n");
JUMP_EQ(L_JUMP_1324);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1324:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1323\n");
JUMP_EQ(L_JUMP_1323);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1323:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1322);
printf("At L_JUMP_1322\n");
fflush(stdout);
L_JUMP_1322:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1320:

POP(FP);
RETURN;
L_CLOS_EXIT_1314:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1308\n");
JUMP_EQ(L_JUMP_1308);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1308:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(177), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEfoldr

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1276));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1274:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1273);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1274);

L_FOR1_END_1273:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1272:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1271);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1272);

L_FOR2_END_1271:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1275);
L_CLOS_CODE_1276: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_1270);
printf("at JUMP_SIMPLE_1270");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1270:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1292));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1290:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1289);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1290);

L_FOR1_END_1289:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1288:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1287);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1288);

L_FOR2_END_1287:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1291);
L_CLOS_CODE_1292: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1286);
printf("at JUMP_SIMPLE_1286");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1286:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1299));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1297:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1296);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1297);

L_FOR1_END_1296:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1295:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1294);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1295);

L_FOR2_END_1294:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1298);
L_CLOS_CODE_1299: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1293);
printf("at JUMP_SIMPLE_1293");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1293:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1307\n");
JUMP_EQ(L_JUMP_1307);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1307:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1302); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1301);
		    L_THEN_1302:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1300);
		    L_ELSE_1301:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1306\n");
JUMP_EQ(L_JUMP_1306);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1306:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1305\n");
JUMP_EQ(L_JUMP_1305);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1305:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1304\n");
JUMP_EQ(L_JUMP_1304);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1304:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1303);
printf("At L_JUMP_1303\n");
fflush(stdout);
L_JUMP_1303:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1300:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1298:

POP(FP);
RETURN;
L_CLOS_EXIT_1291:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1284));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1282:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1281);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1282);

L_FOR1_END_1281:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1280:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1279);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1280);

L_FOR2_END_1279:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1283);
L_CLOS_CODE_1284: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1278);
printf("at JUMP_SIMPLE_1278");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1278:

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 2));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1285);
printf("At L_JUMP_1285\n");
fflush(stdout);
L_JUMP_1285:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1283:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1277);
printf("At L_JUMP_1277\n");
fflush(stdout);
L_JUMP_1277:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1275:
MOV(R1, INDD(IMM(186), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcompose

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1260));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1258:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1257);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1258);

L_FOR1_END_1257:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1256:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1255);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1256);

L_FOR2_END_1255:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1259);
L_CLOS_CODE_1260: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1254);
printf("at JUMP_SIMPLE_1254");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1254:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1267));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1265:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1264);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1265);

L_FOR1_END_1264:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1263:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1262);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1263);

L_FOR2_END_1262:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1266);
L_CLOS_CODE_1267: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1261);
printf("at JUMP_SIMPLE_1261");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1261:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1269\n");
JUMP_EQ(L_JUMP_1269);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1269:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1268);
printf("At L_JUMP_1268\n");
fflush(stdout);
L_JUMP_1268:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1266:

POP(FP);
RETURN;
L_CLOS_EXIT_1259:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1239));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1237:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1236);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1237);

L_FOR1_END_1236:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1235:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1234);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1235);

L_FOR2_END_1234:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1238);
L_CLOS_CODE_1239: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1233);
printf("at JUMP_SIMPLE_1233");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1233:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1245));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1243:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1242);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1243);

L_FOR1_END_1242:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1241:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1240);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1241);

L_FOR2_END_1240:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1244);
L_CLOS_CODE_1245: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1253));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1251:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1250);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1251);

L_FOR1_END_1250:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1249:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1248);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1249);

L_FOR2_END_1248:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1252);
L_CLOS_CODE_1253: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1247);
printf("at JUMP_SIMPLE_1247");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1247:
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_1252:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(186));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1246);
printf("At L_JUMP_1246\n");
fflush(stdout);
L_JUMP_1246:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1244:

POP(FP);
RETURN;
L_CLOS_EXIT_1238:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1232\n");
JUMP_EQ(L_JUMP_1232);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1232:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(197), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1231\n");
JUMP_EQ(L_JUMP_1231);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1231:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(205), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1230\n");
JUMP_EQ(L_JUMP_1230);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1230:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(213), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1229\n");
JUMP_EQ(L_JUMP_1229);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1229:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(221), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1228\n");
JUMP_EQ(L_JUMP_1228);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1228:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(229), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(205));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1227\n");
JUMP_EQ(L_JUMP_1227);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1227:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(238), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(213));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1226\n");
JUMP_EQ(L_JUMP_1226);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1226:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(247), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(221));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1225\n");
JUMP_EQ(L_JUMP_1225);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1225:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(256), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(229));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1224\n");
JUMP_EQ(L_JUMP_1224);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1224:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(265), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(205));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1223\n");
JUMP_EQ(L_JUMP_1223);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1223:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(274), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(213));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1222\n");
JUMP_EQ(L_JUMP_1222);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1222:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(283), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(221));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1221\n");
JUMP_EQ(L_JUMP_1221);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1221:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(292), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(229));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1220\n");
JUMP_EQ(L_JUMP_1220);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1220:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(301), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(238));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1219\n");
JUMP_EQ(L_JUMP_1219);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1219:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(311), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(247));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1218\n");
JUMP_EQ(L_JUMP_1218);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1218:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(321), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(256));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1217\n");
JUMP_EQ(L_JUMP_1217);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1217:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(331), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(265));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1216\n");
JUMP_EQ(L_JUMP_1216);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1216:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(341), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(274));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1215\n");
JUMP_EQ(L_JUMP_1215);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1215:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(351), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(283));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1214\n");
JUMP_EQ(L_JUMP_1214);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1214:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(361), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(292));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1213\n");
JUMP_EQ(L_JUMP_1213);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1213:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(371), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(301));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1212\n");
JUMP_EQ(L_JUMP_1212);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1212:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(381), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(238));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1211\n");
JUMP_EQ(L_JUMP_1211);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1211:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(391), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(247));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1210\n");
JUMP_EQ(L_JUMP_1210);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1210:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(401), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(256));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1209\n");
JUMP_EQ(L_JUMP_1209);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1209:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(411), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(265));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1208\n");
JUMP_EQ(L_JUMP_1208);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1208:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(421), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(274));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1207\n");
JUMP_EQ(L_JUMP_1207);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1207:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(431), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(283));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1206\n");
JUMP_EQ(L_JUMP_1206);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1206:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(441), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(292));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1205\n");
JUMP_EQ(L_JUMP_1205);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1205:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(451), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(301));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1204\n");
JUMP_EQ(L_JUMP_1204);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1204:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(461), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE^variadic-right-from-binary

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1166));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1164:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1163);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1164);

L_FOR1_END_1163:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1162:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1161);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1162);

L_FOR2_END_1161:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1165);
L_CLOS_CODE_1166: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1160);
printf("at JUMP_SIMPLE_1160");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1160:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1188));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1186:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1185);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1186);

L_FOR1_END_1185:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1184:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1183);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1184);

L_FOR2_END_1183:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1187);
L_CLOS_CODE_1188: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1182);
printf("at JUMP_SIMPLE_1182");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1182:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1195));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1193:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1192);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1193);

L_FOR1_END_1192:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1191:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1190);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1191);

L_FOR2_END_1190:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1194);
L_CLOS_CODE_1195: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1189);
printf("at JUMP_SIMPLE_1189");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1189:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1203\n");
JUMP_EQ(L_JUMP_1203);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1203:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1198); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1197);
		    L_THEN_1198:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1196);
		    L_ELSE_1197:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1202\n");
JUMP_EQ(L_JUMP_1202);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1202:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1201\n");
JUMP_EQ(L_JUMP_1201);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1201:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1200\n");
JUMP_EQ(L_JUMP_1200);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1200:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1199);
printf("At L_JUMP_1199\n");
fflush(stdout);
L_JUMP_1199:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1196:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1194:

POP(FP);
RETURN;
L_CLOS_EXIT_1187:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1174));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1172:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1171);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1172);

L_FOR1_END_1171:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1170:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1169);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1170);

L_FOR2_END_1169:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1173);
L_CLOS_CODE_1174: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1168);
printf("at JUMP_SIMPLE_1168");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1168:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1180));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1178:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1177);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1178);

L_FOR1_END_1177:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1176:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1175);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1176);

L_FOR2_END_1175:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1179);
L_CLOS_CODE_1180: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1181);
printf("At L_JUMP_1181\n");
fflush(stdout);
L_JUMP_1181:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1179:

POP(FP);
RETURN;
L_CLOS_EXIT_1173:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1167);
printf("At L_JUMP_1167\n");
fflush(stdout);
L_JUMP_1167:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1165:
MOV(R1, INDD(IMM(492), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE^variadic-left-from-binary

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1116));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1114:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1113);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1114);

L_FOR1_END_1113:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1112:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1111);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1112);

L_FOR2_END_1111:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1115);
L_CLOS_CODE_1116: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1110);
printf("at JUMP_SIMPLE_1110");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1110:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1144));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1142:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1141);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1142);

L_FOR1_END_1141:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1140:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1139);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1140);

L_FOR2_END_1139:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1143);
L_CLOS_CODE_1144: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1138);
printf("at JUMP_SIMPLE_1138");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1138:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1151));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1149:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1148);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1149);

L_FOR1_END_1148:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1147:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1146);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1147);

L_FOR2_END_1146:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1150);
L_CLOS_CODE_1151: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1145);
printf("at JUMP_SIMPLE_1145");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1145:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1159\n");
JUMP_EQ(L_JUMP_1159);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1159:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1154); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1153);
		    L_THEN_1154:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_1152);
		    L_ELSE_1153:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1158\n");
JUMP_EQ(L_JUMP_1158);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1158:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1157\n");
JUMP_EQ(L_JUMP_1157);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1157:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1156\n");
JUMP_EQ(L_JUMP_1156);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1156:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1155);
printf("At L_JUMP_1155\n");
fflush(stdout);
L_JUMP_1155:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1152:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1150:

POP(FP);
RETURN;
L_CLOS_EXIT_1143:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1124));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1122:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1121);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1122);

L_FOR1_END_1121:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1120:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1119);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1120);

L_FOR2_END_1119:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1123);
L_CLOS_CODE_1124: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1118);
printf("at JUMP_SIMPLE_1118");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1118:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1130));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1128:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1127);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1128);

L_FOR1_END_1127:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1126:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1125);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1126);

L_FOR2_END_1125:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1129);
L_CLOS_CODE_1130: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1137\n");
JUMP_EQ(L_JUMP_1137);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1137:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1133); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1132);
		    L_THEN_1133:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1131);
		    L_ELSE_1132:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1136\n");
JUMP_EQ(L_JUMP_1136);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1136:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1135\n");
JUMP_EQ(L_JUMP_1135);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1135:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1134);
printf("At L_JUMP_1134\n");
fflush(stdout);
L_JUMP_1134:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_1131:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1129:

POP(FP);
RETURN;
L_CLOS_EXIT_1123:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1117);
printf("At L_JUMP_1117\n");
fflush(stdout);
L_JUMP_1117:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1115:
MOV(R1, INDD(IMM(522), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE^char-op

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1099));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1097:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1096);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1097);

L_FOR1_END_1096:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1095:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1094);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1095);

L_FOR2_END_1094:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1098);
L_CLOS_CODE_1099: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1093);
printf("at JUMP_SIMPLE_1093");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1093:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1106));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1104:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1103);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1104);

L_FOR1_END_1103:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1102:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1101);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1102);

L_FOR2_END_1101:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1105);
L_CLOS_CODE_1106: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1100);
printf("at JUMP_SIMPLE_1100");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1100:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1109\n");
JUMP_EQ(L_JUMP_1109);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1109:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1108\n");
JUMP_EQ(L_JUMP_1108);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1108:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1107);
printf("At L_JUMP_1107\n");
fflush(stdout);
L_JUMP_1107:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1105:

POP(FP);
RETURN;
L_CLOS_EXIT_1098:
MOV(R1, INDD(IMM(534), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE<

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1302));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1092\n");
JUMP_EQ(L_JUMP_1092);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1092:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(539), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE<=

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(64));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1091\n");
JUMP_EQ(L_JUMP_1091);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1091:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(545), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE>

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(54));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1090\n");
JUMP_EQ(L_JUMP_1090);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1090:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(550), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE>=

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(74));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1089\n");
JUMP_EQ(L_JUMP_1089);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1089:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(556), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1410));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1088\n");
JUMP_EQ(L_JUMP_1088);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1088:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1087\n");
JUMP_EQ(L_JUMP_1087);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1087:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(566), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar<=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(64));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1086\n");
JUMP_EQ(L_JUMP_1086);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1086:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1085\n");
JUMP_EQ(L_JUMP_1085);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1085:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(577), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar<?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1302));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1084\n");
JUMP_EQ(L_JUMP_1084);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1084:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1083\n");
JUMP_EQ(L_JUMP_1083);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1083:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(587), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar>=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(74));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1082\n");
JUMP_EQ(L_JUMP_1082);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1082:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1081\n");
JUMP_EQ(L_JUMP_1081);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1081:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(598), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar>?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(54));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1080\n");
JUMP_EQ(L_JUMP_1080);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1080:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1079\n");
JUMP_EQ(L_JUMP_1079);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1079:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(608), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-uppercase?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1073));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1071:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1070);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1071);

L_FOR1_END_1070:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1069:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1068);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1069);

L_FOR2_END_1068:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1072);
L_CLOS_CODE_1073: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1067);
printf("at JUMP_SIMPLE_1067");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1067:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
MOV(R0,IMM(1412));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1078\n");
JUMP_EQ(L_JUMP_1078);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1078:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1076); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1075);
		    L_THEN_1076:
 //TC-APPLIC 

MOV(R0,IMM(1414));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1077);
printf("At L_JUMP_1077\n");
fflush(stdout);
L_JUMP_1077:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1074);
		    L_ELSE_1075:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1074:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1072:
MOV(R1, INDD(IMM(627), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-lowercase?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1061));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1059:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1058);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1059);

L_FOR1_END_1058:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1057:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1056);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1057);

L_FOR2_END_1056:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1060);
L_CLOS_CODE_1061: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1055);
printf("at JUMP_SIMPLE_1055");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1055:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
MOV(R0,IMM(1416));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1066\n");
JUMP_EQ(L_JUMP_1066);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1066:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1064); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1063);
		    L_THEN_1064:
 //TC-APPLIC 

MOV(R0,IMM(1418));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1065);
printf("At L_JUMP_1065\n");
fflush(stdout);
L_JUMP_1065:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1062);
		    L_ELSE_1063:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1062:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1060:
MOV(R1, INDD(IMM(646), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-upcase

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1412));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1054\n");
JUMP_EQ(L_JUMP_1054);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1054:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1416));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1053\n");
JUMP_EQ(L_JUMP_1053);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1053:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1052\n");
JUMP_EQ(L_JUMP_1052);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1052:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1037));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1035:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1034);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1035);

L_FOR1_END_1034:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1033:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1032);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1033);

L_FOR2_END_1032:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1036);
L_CLOS_CODE_1037: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1031);
printf("at JUMP_SIMPLE_1031");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1031:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1044));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1042:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1041);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1042);

L_FOR1_END_1041:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1040:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1039);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1040);

L_FOR2_END_1039:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1043);
L_CLOS_CODE_1044: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1038);
printf("at JUMP_SIMPLE_1038");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1038:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(646));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1051\n");
JUMP_EQ(L_JUMP_1051);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1051:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1047); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1046);
		    L_THEN_1047:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1050\n");
JUMP_EQ(L_JUMP_1050);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1050:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1049\n");
JUMP_EQ(L_JUMP_1049);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1049:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1435));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1048);
printf("At L_JUMP_1048\n");
fflush(stdout);
L_JUMP_1048:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1045);
		    L_ELSE_1046:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_1045:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1043:

POP(FP);
RETURN;
L_CLOS_EXIT_1036:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1030\n");
JUMP_EQ(L_JUMP_1030);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1030:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(661), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-downcase

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1412));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1029\n");
JUMP_EQ(L_JUMP_1029);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1029:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1416));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1028\n");
JUMP_EQ(L_JUMP_1028);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1028:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1027\n");
JUMP_EQ(L_JUMP_1027);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1027:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1012));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1010:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1009);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1010);

L_FOR1_END_1009:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1008:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1007);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1008);

L_FOR2_END_1007:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1011);
L_CLOS_CODE_1012: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1006);
printf("at JUMP_SIMPLE_1006");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1006:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1019));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1017:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1016);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1017);

L_FOR1_END_1016:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1015:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1014);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1015);

L_FOR2_END_1014:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1018);
L_CLOS_CODE_1019: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1013);
printf("at JUMP_SIMPLE_1013");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1013:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(627));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1026\n");
JUMP_EQ(L_JUMP_1026);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1026:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1022); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1021);
		    L_THEN_1022:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1401));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1025\n");
JUMP_EQ(L_JUMP_1025);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1025:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1024\n");
JUMP_EQ(L_JUMP_1024);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1024:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1435));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1023);
printf("At L_JUMP_1023\n");
fflush(stdout);
L_JUMP_1023:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_1020);
		    L_ELSE_1021:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_1020:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1018:

POP(FP);
RETURN;
L_CLOS_EXIT_1011:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1005\n");
JUMP_EQ(L_JUMP_1005);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1005:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(678), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-ci<=?

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1001));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_999:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_998);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_999);

L_FOR1_END_998:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_997:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_996);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_997);

L_FOR2_END_996:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1000);
L_CLOS_CODE_1001: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_995);
printf("at JUMP_SIMPLE_995");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_995:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1004\n");
JUMP_EQ(L_JUMP_1004);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1004:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1003\n");
JUMP_EQ(L_JUMP_1003);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1003:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1002);
printf("At L_JUMP_1002\n");
fflush(stdout);
L_JUMP_1002:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_1000:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_994\n");
JUMP_EQ(L_JUMP_994);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_994:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(692), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-ci<?

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_990));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_988:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_987);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_988);

L_FOR1_END_987:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_986:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_985);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_986);

L_FOR2_END_985:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_989);
L_CLOS_CODE_990: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_984);
printf("at JUMP_SIMPLE_984");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_984:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_993\n");
JUMP_EQ(L_JUMP_993);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_993:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_992\n");
JUMP_EQ(L_JUMP_992);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_992:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(587));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_991);
printf("At L_JUMP_991\n");
fflush(stdout);
L_JUMP_991:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_989:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_983\n");
JUMP_EQ(L_JUMP_983);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_983:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(705), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-ci=?

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_979));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_977:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_976);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_977);

L_FOR1_END_976:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_975:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_974);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_975);

L_FOR2_END_974:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_978);
L_CLOS_CODE_979: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_973);
printf("at JUMP_SIMPLE_973");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_973:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_982\n");
JUMP_EQ(L_JUMP_982);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_982:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_981\n");
JUMP_EQ(L_JUMP_981);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_981:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_980);
printf("At L_JUMP_980\n");
fflush(stdout);
L_JUMP_980:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_978:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_972\n");
JUMP_EQ(L_JUMP_972);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_972:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(718), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-ci>?

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_968));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_966:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_965);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_966);

L_FOR1_END_965:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_964:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_963);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_964);

L_FOR2_END_963:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_967);
L_CLOS_CODE_968: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_962);
printf("at JUMP_SIMPLE_962");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_962:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_971\n");
JUMP_EQ(L_JUMP_971);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_971:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_970\n");
JUMP_EQ(L_JUMP_970);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_970:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(608));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_969);
printf("At L_JUMP_969\n");
fflush(stdout);
L_JUMP_969:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_967:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_961\n");
JUMP_EQ(L_JUMP_961);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_961:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(731), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar-ci>=?

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_957));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_955:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_954);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_955);

L_FOR1_END_954:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_953:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_952);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_953);

L_FOR2_END_952:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_956);
L_CLOS_CODE_957: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_951);
printf("at JUMP_SIMPLE_951");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_951:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_960\n");
JUMP_EQ(L_JUMP_960);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_960:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_959\n");
JUMP_EQ(L_JUMP_959);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_959:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(598));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_958);
printf("At L_JUMP_958\n");
fflush(stdout);
L_JUMP_958:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_956:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_950\n");
JUMP_EQ(L_JUMP_950);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_950:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(745), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring->list

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_934));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_932:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_931);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_932);

L_FOR1_END_931:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_930:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_929);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_930);

L_FOR2_END_929:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_933);
L_CLOS_CODE_934: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_928);
printf("at JUMP_SIMPLE_928");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_928:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_941));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_939:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_938);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_939);

L_FOR1_END_938:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_937:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_936);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_937);

L_FOR2_END_936:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_940);
L_CLOS_CODE_941: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_935);
printf("at JUMP_SIMPLE_935");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_935:
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1454));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_949\n");
JUMP_EQ(L_JUMP_949);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_949:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_944); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_943);
		    L_THEN_944:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_942);
		    L_ELSE_943:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1468));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_948\n");
JUMP_EQ(L_JUMP_948);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_948:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1359));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_947\n");
JUMP_EQ(L_JUMP_947);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_947:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_946\n");
JUMP_EQ(L_JUMP_946);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_946:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_945);
printf("At L_JUMP_945\n");
fflush(stdout);
L_JUMP_945:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_942:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_940:

POP(FP);
RETURN;
L_CLOS_EXIT_933:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_917));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_915:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_914);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_915);

L_FOR1_END_914:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_913:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_912);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_913);

L_FOR2_END_912:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_916);
L_CLOS_CODE_917: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_911);
printf("at JUMP_SIMPLE_911");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_911:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_924));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_922:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_921);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_922);

L_FOR1_END_921:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_920:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_919);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_920);

L_FOR2_END_919:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_923);
L_CLOS_CODE_924: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_918);
printf("at JUMP_SIMPLE_918");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_918:

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1452));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_927\n");
JUMP_EQ(L_JUMP_927);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_927:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_926\n");
JUMP_EQ(L_JUMP_926);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_926:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_925);
printf("At L_JUMP_925\n");
fflush(stdout);
L_JUMP_925:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_923:

POP(FP);
RETURN;
L_CLOS_EXIT_916:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_910\n");
JUMP_EQ(L_JUMP_910);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_910:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(761), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist->string

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_859));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_857:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_856);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_857);

L_FOR1_END_856:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_855:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_854);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_855);

L_FOR2_END_854:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_858);
L_CLOS_CODE_859: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_853);
printf("at JUMP_SIMPLE_853");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_853:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(93));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_909\n");
JUMP_EQ(L_JUMP_909);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_909:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_867));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_865:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_864);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_865);

L_FOR1_END_864:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_863:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_862);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_863);

L_FOR2_END_862:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_866);
L_CLOS_CODE_867: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_861);
printf("at JUMP_SIMPLE_861");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_861:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1498));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_908\n");
JUMP_EQ(L_JUMP_908);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_908:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_875));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_873:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_872);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_873);

L_FOR1_END_872:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_871:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_870);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_871);

L_FOR2_END_870:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_874);
L_CLOS_CODE_875: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_869);
printf("at JUMP_SIMPLE_869");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_869:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_891));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_889:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_888);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_889);

L_FOR1_END_888:
//for (i = 0; i < 4; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_887:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_886);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_887);

L_FOR2_END_886:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_890);
L_CLOS_CODE_891: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_885);
printf("at JUMP_SIMPLE_885");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_885:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(6));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_898));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_896:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_895);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_896);

L_FOR1_END_895:
//for (i = 0; i < 5; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_894:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_893);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_894);

L_FOR2_END_893:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_897);
L_CLOS_CODE_898: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_892);
printf("at JUMP_SIMPLE_892");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_892:
//IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_907\n");
JUMP_EQ(L_JUMP_907);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_907:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_901); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_900);
		    L_THEN_901:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_899);
		    L_ELSE_900:
		    // SEQ 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_906\n");
JUMP_EQ(L_JUMP_906);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_906:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1483));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_905\n");
JUMP_EQ(L_JUMP_905);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_905:
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_904\n");
JUMP_EQ(L_JUMP_904);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_904:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_903\n");
JUMP_EQ(L_JUMP_903);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_903:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_902);
printf("At L_JUMP_902\n");
fflush(stdout);
L_JUMP_902:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_899:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_897:

POP(FP);
RETURN;
L_CLOS_EXIT_890:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_883));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_881:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_880);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_881);

L_FOR1_END_880:
//for (i = 0; i < 4; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_879:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_878);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_879);

L_FOR2_END_878:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_882);
L_CLOS_CODE_883: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_877);
printf("at JUMP_SIMPLE_877");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_877:

 //TC-APPLIC 

MOV(R0,IMM(1304));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_884);
printf("At L_JUMP_884\n");
fflush(stdout);
L_JUMP_884:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_882:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_876);
printf("At L_JUMP_876\n");
fflush(stdout);
L_JUMP_876:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_874:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_868);
printf("At L_JUMP_868\n");
fflush(stdout);
L_JUMP_868:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_866:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_860);
printf("At L_JUMP_860\n");
fflush(stdout);
L_JUMP_860:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_858:
MOV(R1, INDD(IMM(777), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring-upcase

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_849));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_847:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_846);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_847);

L_FOR1_END_846:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_845:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_844);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_845);

L_FOR2_END_844:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_848);
L_CLOS_CODE_849: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_843);
printf("at JUMP_SIMPLE_843");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_843:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_852\n");
JUMP_EQ(L_JUMP_852);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_852:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_851\n");
JUMP_EQ(L_JUMP_851);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_851:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(777));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_850);
printf("At L_JUMP_850\n");
fflush(stdout);
L_JUMP_850:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_848:
MOV(R1, INDD(IMM(794), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring-downcase

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_839));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_837:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_836);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_837);

L_FOR1_END_836:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_835:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_834);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_835);

L_FOR2_END_834:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_838);
L_CLOS_CODE_839: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_833);
printf("at JUMP_SIMPLE_833");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_833:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_842\n");
JUMP_EQ(L_JUMP_842);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_842:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(678));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_841\n");
JUMP_EQ(L_JUMP_841);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_841:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(777));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_840);
printf("At L_JUMP_840\n");
fflush(stdout);
L_JUMP_840:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_838:
MOV(R1, INDD(IMM(813), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist-ref

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_824));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_822:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_821);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_822);

L_FOR1_END_821:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_820:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_819);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_820);

L_FOR2_END_819:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_823);
L_CLOS_CODE_824: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_818);
printf("at JUMP_SIMPLE_818");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_818:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(83));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_832\n");
JUMP_EQ(L_JUMP_832);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_832:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_827); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_826);
		    L_THEN_827:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_831);
printf("At L_JUMP_831\n");
fflush(stdout);
L_JUMP_831:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_825);
		    L_ELSE_826:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_830\n");
JUMP_EQ(L_JUMP_830);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_830:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_829\n");
JUMP_EQ(L_JUMP_829);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_829:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(825));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_828);
printf("At L_JUMP_828\n");
fflush(stdout);
L_JUMP_828:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_825:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_823:
MOV(R1, INDD(IMM(825), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_809));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_807:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_806);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_807);

L_FOR1_END_806:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_805:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_804);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_805);

L_FOR2_END_804:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_808);
L_CLOS_CODE_809: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_803);
printf("at JUMP_SIMPLE_803");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_803:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_817\n");
JUMP_EQ(L_JUMP_817);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_817:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
// OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_810);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_810);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1329));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_816\n");
JUMP_EQ(L_JUMP_816);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_816:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_813); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_812);
		    L_THEN_813:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_815\n");
JUMP_EQ(L_JUMP_815);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_815:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(102));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_814);
printf("At L_JUMP_814\n");
fflush(stdout);
L_JUMP_814:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_811);
		    L_ELSE_812:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_811:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_810);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_810);
L_OR_END_810:

POP(FP);
RETURN;
L_CLOS_EXIT_808:
MOV(R1, INDD(IMM(102), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEormap

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_797));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_799:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_800);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_799);

L_FOR1_END_800:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_801:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_802);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_801);

L_FOR2_END_802:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_798);
L_CLOS_CODE_797: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_779));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_777:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_776);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_777);

L_FOR1_END_776:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_775:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_774);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_775);

L_FOR2_END_774:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_778);
L_CLOS_CODE_779: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_773);
printf("at JUMP_SIMPLE_773");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_773:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_786));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_784:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_783);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_784);

L_FOR1_END_783:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_782:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_781);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_782);

L_FOR2_END_781:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_785);
L_CLOS_CODE_786: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_780);
printf("at JUMP_SIMPLE_780");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_780:
//IF 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_796\n");
JUMP_EQ(L_JUMP_796);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_796:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1329));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_795\n");
JUMP_EQ(L_JUMP_795);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_795:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_789); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_788);
		    L_THEN_789:
 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_794\n");
JUMP_EQ(L_JUMP_794);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_794:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_793\n");
JUMP_EQ(L_JUMP_793);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_793:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
// OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_790);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_790);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_792\n");
JUMP_EQ(L_JUMP_792);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_792:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_791);
printf("At L_JUMP_791\n");
fflush(stdout);
L_JUMP_791:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place
// OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_790);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_790);
L_OR_END_790:

		    JUMP(L_IF_EXIT_787);
		    L_ELSE_788:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_787:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_785:

POP(FP);
RETURN;
L_CLOS_EXIT_778:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_771));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_769:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_768);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_769);

L_FOR1_END_768:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_767:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_766);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_767);

L_FOR2_END_766:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_770);
L_CLOS_CODE_771: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_765);
printf("at JUMP_SIMPLE_765");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_765:

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_772);
printf("At L_JUMP_772\n");
fflush(stdout);
L_JUMP_772:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_770:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_764);
printf("At L_JUMP_764\n");
fflush(stdout);
L_JUMP_764:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_798:
MOV(R1, INDD(IMM(834), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEmember?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_754));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_752:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_751);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_752);

L_FOR1_END_751:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_750:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_749);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_750);

L_FOR2_END_749:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_753);
L_CLOS_CODE_754: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_748);
printf("at JUMP_SIMPLE_748");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_748:

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_762));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_760:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_759);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_760);

L_FOR1_END_759:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_758:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_757);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_758);

L_FOR2_END_757:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_761);
L_CLOS_CODE_762: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_756);
printf("at JUMP_SIMPLE_756");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_756:

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1505));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_763);
printf("At L_JUMP_763\n");
fflush(stdout);
L_JUMP_763:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_761:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(834));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_755);
printf("At L_JUMP_755\n");
fflush(stdout);
L_JUMP_755:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_753:
MOV(R1, INDD(IMM(845), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEnegative?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_746));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_744:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_743);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_744);

L_FOR1_END_743:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_742:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_741);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_742);

L_FOR2_END_741:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_745);
L_CLOS_CODE_746: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_740);
printf("at JUMP_SIMPLE_740");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_740:

 //TC-APPLIC 

MOV(R0,IMM(1304));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(539));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_747);
printf("At L_JUMP_747\n");
fflush(stdout);
L_JUMP_747:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_745:
MOV(R1, INDD(IMM(858), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEpositive?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_738));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_736:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_735);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_736);

L_FOR1_END_735:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_734:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_733);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_734);

L_FOR2_END_733:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_737);
L_CLOS_CODE_738: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_732);
printf("at JUMP_SIMPLE_732");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_732:

 //TC-APPLIC 

MOV(R0,IMM(1304));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(550));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_739);
printf("At L_JUMP_739\n");
fflush(stdout);
L_JUMP_739:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_737:
MOV(R1, INDD(IMM(871), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist->vector

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_681));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_679:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_678);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_679);

L_FOR1_END_678:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_677:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_676);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_677);

L_FOR2_END_676:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_680);
L_CLOS_CODE_681: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_675);
printf("at JUMP_SIMPLE_675");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_675:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(93));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_731\n");
JUMP_EQ(L_JUMP_731);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_731:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_689));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_687:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_686);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_687);

L_FOR1_END_686:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_685:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_684);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_685);

L_FOR2_END_684:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_688);
L_CLOS_CODE_689: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_683);
printf("at JUMP_SIMPLE_683");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_683:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1535));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_730\n");
JUMP_EQ(L_JUMP_730);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_730:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_697));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_695:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_694);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_695);

L_FOR1_END_694:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_693:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_692);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_693);

L_FOR2_END_692:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_696);
L_CLOS_CODE_697: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_691);
printf("at JUMP_SIMPLE_691");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_691:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_713));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_711:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_710);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_711);

L_FOR1_END_710:
//for (i = 0; i < 4; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_709:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_708);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_709);

L_FOR2_END_708:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_712);
L_CLOS_CODE_713: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_707);
printf("at JUMP_SIMPLE_707");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_707:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(6));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_720));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_718:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_717);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_718);

L_FOR1_END_717:
//for (i = 0; i < 5; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_716:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_715);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_716);

L_FOR2_END_715:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_719);
L_CLOS_CODE_720: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_714);
printf("at JUMP_SIMPLE_714");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_714:
//IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_729\n");
JUMP_EQ(L_JUMP_729);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_729:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_723); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_722);
		    L_THEN_723:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_721);
		    L_ELSE_722:
		    // SEQ 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_728\n");
JUMP_EQ(L_JUMP_728);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_728:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1520));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_727\n");
JUMP_EQ(L_JUMP_727);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_727:
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_726\n");
JUMP_EQ(L_JUMP_726);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_726:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_725\n");
JUMP_EQ(L_JUMP_725);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_725:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_724);
printf("At L_JUMP_724\n");
fflush(stdout);
L_JUMP_724:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_721:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_719:

POP(FP);
RETURN;
L_CLOS_EXIT_712:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_705));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_703:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_702);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_703);

L_FOR1_END_702:
//for (i = 0; i < 4; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_701:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_700);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_701);

L_FOR2_END_700:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_704);
L_CLOS_CODE_705: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_699);
printf("at JUMP_SIMPLE_699");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_699:

 //TC-APPLIC 

MOV(R0,IMM(1304));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_706);
printf("At L_JUMP_706\n");
fflush(stdout);
L_JUMP_706:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_704:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_698);
printf("At L_JUMP_698\n");
fflush(stdout);
L_JUMP_698:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_696:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_690);
printf("At L_JUMP_690\n");
fflush(stdout);
L_JUMP_690:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_688:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_682);
printf("At L_JUMP_682\n");
fflush(stdout);
L_JUMP_682:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_680:
MOV(R1, INDD(IMM(887), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEvector

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_673));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_671:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_670);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_671);

L_FOR1_END_670:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_669:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_668);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_669);

L_FOR2_END_668:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_672);
L_CLOS_CODE_673: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(887));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_674);
printf("At L_JUMP_674\n");
fflush(stdout);
L_JUMP_674:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_672:
MOV(R1, INDD(IMM(897), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbinary-string=?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_642));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_640:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_639);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_640);

L_FOR1_END_639:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_638:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_637);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_638);

L_FOR2_END_637:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_641);
L_CLOS_CODE_642: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_636);
printf("at JUMP_SIMPLE_636");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_636:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1452));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_667\n");
JUMP_EQ(L_JUMP_667);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_667:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1452));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_666\n");
JUMP_EQ(L_JUMP_666);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_666:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_650));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_648:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_647);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_648);

L_FOR1_END_647:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_646:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_645);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_646);

L_FOR2_END_645:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_649);
L_CLOS_CODE_650: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_644);
printf("at JUMP_SIMPLE_644");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_644:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_665\n");
JUMP_EQ(L_JUMP_665);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_665:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_653); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_652);
		    L_THEN_653:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_664\n");
JUMP_EQ(L_JUMP_664);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_664:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_663\n");
JUMP_EQ(L_JUMP_663);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_663:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_661));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_659:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_658);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_659);

L_FOR1_END_658:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_657:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_656);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_657);

L_FOR2_END_656:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_660);
L_CLOS_CODE_661: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_655);
printf("at JUMP_SIMPLE_655");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_655:

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(171));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_662);
printf("At L_JUMP_662\n");
fflush(stdout);
L_JUMP_662:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_660:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_654);
printf("At L_JUMP_654\n");
fflush(stdout);
L_JUMP_654:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_651);
		    L_ELSE_652:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_651:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_649:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_643);
printf("At L_JUMP_643\n");
fflush(stdout);
L_JUMP_643:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_641:
MOV(R1, INDD(IMM(916), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbinary-string<?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_589));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_587:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_586);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_587);

L_FOR1_END_586:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_585:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_584);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_585);

L_FOR2_END_584:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_588);
L_CLOS_CODE_589: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_583);
printf("at JUMP_SIMPLE_583");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_583:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_607));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_605:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_604);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_605);

L_FOR1_END_604:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_603:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_602);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_603);

L_FOR2_END_602:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_606);
L_CLOS_CODE_607: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_601);
printf("at JUMP_SIMPLE_601");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_601:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_614));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_612:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_611);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_612);

L_FOR1_END_611:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_610:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_609);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_610);

L_FOR2_END_609:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_613);
L_CLOS_CODE_614: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_608);
printf("at JUMP_SIMPLE_608");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_608:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_635\n");
JUMP_EQ(L_JUMP_635);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_635:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_617); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_616);
		    L_THEN_617:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1329));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_634);
printf("At L_JUMP_634\n");
fflush(stdout);
L_JUMP_634:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_615);
		    L_ELSE_616:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_633\n");
JUMP_EQ(L_JUMP_633);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_633:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_620); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_619);
		    L_THEN_620:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_618);
		    L_ELSE_619:
		    //IF 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_632\n");
JUMP_EQ(L_JUMP_632);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_632:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_631\n");
JUMP_EQ(L_JUMP_631);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_631:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_630\n");
JUMP_EQ(L_JUMP_630);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_630:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_623); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_622);
		    L_THEN_623:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_629\n");
JUMP_EQ(L_JUMP_629);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_629:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_628\n");
JUMP_EQ(L_JUMP_628);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_628:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_627);
printf("At L_JUMP_627\n");
fflush(stdout);
L_JUMP_627:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_621);
		    L_ELSE_622:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_626\n");
JUMP_EQ(L_JUMP_626);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_626:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_625\n");
JUMP_EQ(L_JUMP_625);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_625:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(587));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_624);
printf("At L_JUMP_624\n");
fflush(stdout);
L_JUMP_624:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_621:
		    
		    L_IF_EXIT_618:
		    
		    L_IF_EXIT_615:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_613:

POP(FP);
RETURN;
L_CLOS_EXIT_606:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_597));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_595:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_594);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_595);

L_FOR1_END_594:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_593:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_592);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_593);

L_FOR2_END_592:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_596);
L_CLOS_CODE_597: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_591);
printf("at JUMP_SIMPLE_591");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_591:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_600\n");
JUMP_EQ(L_JUMP_600);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_600:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_599\n");
JUMP_EQ(L_JUMP_599);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_599:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_598);
printf("At L_JUMP_598\n");
fflush(stdout);
L_JUMP_598:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_596:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_590);
printf("At L_JUMP_590\n");
fflush(stdout);
L_JUMP_590:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_588:
MOV(R1, INDD(IMM(935), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbinary-string>?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_581));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_579:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_578);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_579);

L_FOR1_END_578:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_577:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_576);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_577);

L_FOR2_END_576:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_580);
L_CLOS_CODE_581: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_575);
printf("at JUMP_SIMPLE_575");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_575:

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(935));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_582);
printf("At L_JUMP_582\n");
fflush(stdout);
L_JUMP_582:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_580:
MOV(R1, INDD(IMM(954), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbinary-string<=?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_572));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_570:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_569);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_570);

L_FOR1_END_569:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_568:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_567);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_568);

L_FOR2_END_567:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_571);
L_CLOS_CODE_572: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_566);
printf("at JUMP_SIMPLE_566");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_566:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(954));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_574\n");
JUMP_EQ(L_JUMP_574);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_574:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(21));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_573);
printf("At L_JUMP_573\n");
fflush(stdout);
L_JUMP_573:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_571:
MOV(R1, INDD(IMM(974), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEbinary-string>=?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_563));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_561:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_560);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_561);

L_FOR1_END_560:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_559:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_558);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_559);

L_FOR2_END_558:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_562);
L_CLOS_CODE_563: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_557);
printf("at JUMP_SIMPLE_557");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_557:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(935));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_565\n");
JUMP_EQ(L_JUMP_565);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_565:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(21));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_564);
printf("At L_JUMP_564\n");
fflush(stdout);
L_JUMP_564:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_562:
MOV(R1, INDD(IMM(994), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(916));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_556\n");
JUMP_EQ(L_JUMP_556);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_556:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1006), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring<?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(935));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_555\n");
JUMP_EQ(L_JUMP_555);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_555:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1018), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring>?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(954));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_554\n");
JUMP_EQ(L_JUMP_554);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_554:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1030), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring<=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(974));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_553\n");
JUMP_EQ(L_JUMP_553);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_553:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1043), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring>=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(994));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_552\n");
JUMP_EQ(L_JUMP_552);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_552:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1056), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEvector->list

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_536));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_534:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_533);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_534);

L_FOR1_END_533:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_532:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_531);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_532);

L_FOR2_END_531:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_535);
L_CLOS_CODE_536: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_530);
printf("at JUMP_SIMPLE_530");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_530:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_543));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_541:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_540);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_541);

L_FOR1_END_540:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_539:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_538);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_539);

L_FOR2_END_538:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_542);
L_CLOS_CODE_543: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_537);
printf("at JUMP_SIMPLE_537");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_537:
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1454));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_551\n");
JUMP_EQ(L_JUMP_551);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_551:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_546); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_545);
		    L_THEN_546:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_544);
		    L_ELSE_545:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_550\n");
JUMP_EQ(L_JUMP_550);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_550:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1359));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_549\n");
JUMP_EQ(L_JUMP_549);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_549:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_548\n");
JUMP_EQ(L_JUMP_548);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_548:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_547);
printf("At L_JUMP_547\n");
fflush(stdout);
L_JUMP_547:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_544:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_542:

POP(FP);
RETURN;
L_CLOS_EXIT_535:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_519));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_517:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_516);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_517);

L_FOR1_END_516:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_515:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_514);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_515);

L_FOR2_END_514:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_518);
L_CLOS_CODE_519: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_513);
printf("at JUMP_SIMPLE_513");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_513:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_526));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_524:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_523);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_524);

L_FOR1_END_523:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_522:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_521);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_522);

L_FOR2_END_521:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_525);
L_CLOS_CODE_526: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_520);
printf("at JUMP_SIMPLE_520");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_520:

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1552));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_529\n");
JUMP_EQ(L_JUMP_529);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_529:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_528\n");
JUMP_EQ(L_JUMP_528);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_528:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_527);
printf("At L_JUMP_527\n");
fflush(stdout);
L_JUMP_527:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_525:

POP(FP);
RETURN;
L_CLOS_EXIT_518:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_512\n");
JUMP_EQ(L_JUMP_512);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_512:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1072), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEboolean=?

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_499));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_497:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_496);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_497);

L_FOR1_END_496:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_495:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_494);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_495);

L_FOR2_END_494:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_498);
L_CLOS_CODE_499: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_493);
printf("at JUMP_SIMPLE_493");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_493:
//IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1578));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_511\n");
JUMP_EQ(L_JUMP_511);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_511:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_509); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_508);
		    L_THEN_509:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1578));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_510\n");
JUMP_EQ(L_JUMP_510);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_510:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_507);
		    L_ELSE_508:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_507:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_502); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_501);
		    L_THEN_502://IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_505); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_504);
		    L_THEN_505:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_503);
		    L_ELSE_504:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(21));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_506);
printf("At L_JUMP_506\n");
fflush(stdout);
L_JUMP_506:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_503:
		    
		    JUMP(L_IF_EXIT_500);
		    L_ELSE_501:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_500:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_498:
MOV(R1, INDD(IMM(1085), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEequal?

 //APPLIC 

PUSH(IMM(0));
//IF 
MOV(R0,IMM(12));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_492); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_491);
		    L_THEN_492:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_490);
		    L_ELSE_491:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_490:
		    PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_378));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_376:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_375);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_376);

L_FOR1_END_375:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_374:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_373);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_374);

L_FOR2_END_373:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_377);
L_CLOS_CODE_378: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_372);
printf("at JUMP_SIMPLE_372");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_372:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_393));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_391:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_390);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_391);

L_FOR1_END_390:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_389:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_388);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_389);

L_FOR2_END_388:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_392);
L_CLOS_CODE_393: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_387);
printf("at JUMP_SIMPLE_387");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_387:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_400));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_398:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_397);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_398);

L_FOR1_END_397:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_396:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_395);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_396);

L_FOR2_END_395:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_399);
L_CLOS_CODE_400: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_394);
printf("at JUMP_SIMPLE_394");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_394:
//IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1578));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_489\n");
JUMP_EQ(L_JUMP_489);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_489:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_487); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_486);
		    L_THEN_487:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1578));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_488\n");
JUMP_EQ(L_JUMP_488);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_488:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_485);
		    L_ELSE_486:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_485:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_403); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_402);
		    L_THEN_403:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1085));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_484);
printf("At L_JUMP_484\n");
fflush(stdout);
L_JUMP_484:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_401);
		    L_ELSE_402:
		    //IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1587));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_483\n");
JUMP_EQ(L_JUMP_483);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_483:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_481); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_480);
		    L_THEN_481:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1587));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_482\n");
JUMP_EQ(L_JUMP_482);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_482:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_479);
		    L_ELSE_480:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_479:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_406); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_405);
		    L_THEN_406:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_478);
printf("At L_JUMP_478\n");
fflush(stdout);
L_JUMP_478:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_404);
		    L_ELSE_405:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_477\n");
JUMP_EQ(L_JUMP_477);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_477:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_409); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_408);
		    L_THEN_409:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_476);
printf("At L_JUMP_476\n");
fflush(stdout);
L_JUMP_476:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_407);
		    L_ELSE_408:
		    //IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1598));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_475\n");
JUMP_EQ(L_JUMP_475);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_475:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_473); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_472);
		    L_THEN_473:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1598));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_474\n");
JUMP_EQ(L_JUMP_474);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_474:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_471);
		    L_ELSE_472:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_471:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_412); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_411);
		    L_THEN_412:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_470);
printf("At L_JUMP_470\n");
fflush(stdout);
L_JUMP_470:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_410);
		    L_ELSE_411:
		    //IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1329));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_469\n");
JUMP_EQ(L_JUMP_469);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_469:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_467); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_466);
		    L_THEN_467:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1329));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_468\n");
JUMP_EQ(L_JUMP_468);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_468:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_465);
		    L_ELSE_466:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_465:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_415); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_414);
		    L_THEN_415://IF 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_464\n");
JUMP_EQ(L_JUMP_464);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_464:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_463\n");
JUMP_EQ(L_JUMP_463);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_463:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_462\n");
JUMP_EQ(L_JUMP_462);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_462:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_458); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_457);
		    L_THEN_458:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_461\n");
JUMP_EQ(L_JUMP_461);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_461:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_460\n");
JUMP_EQ(L_JUMP_460);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_460:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_459);
printf("At L_JUMP_459\n");
fflush(stdout);
L_JUMP_459:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_456);
		    L_ELSE_457:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_456:
		    
		    JUMP(L_IF_EXIT_413);
		    L_ELSE_414:
		    //IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1609));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_455\n");
JUMP_EQ(L_JUMP_455);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_455:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_453); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_452);
		    L_THEN_453:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1609));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_454\n");
JUMP_EQ(L_JUMP_454);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_454:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_451);
		    L_ELSE_452:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_451:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_418); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_417);
		    L_THEN_418:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1006));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_450);
printf("At L_JUMP_450\n");
fflush(stdout);
L_JUMP_450:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_416);
		    L_ELSE_417:
		    //IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1620));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_449\n");
JUMP_EQ(L_JUMP_449);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_449:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_447); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_446);
		    L_THEN_447:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1620));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_448\n");
JUMP_EQ(L_JUMP_448);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_448:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_445);
		    L_ELSE_446:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_445:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_421); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_420);
		    L_THEN_421:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1505));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_444);
printf("At L_JUMP_444\n");
fflush(stdout);
L_JUMP_444:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_419);
		    L_ELSE_420:
		    //IF 
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1631));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_443\n");
JUMP_EQ(L_JUMP_443);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_443:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_435); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_434);
		    L_THEN_435://IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1631));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_442\n");
JUMP_EQ(L_JUMP_442);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_442:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_438); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_437);
		    L_THEN_438:
 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1552));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_441\n");
JUMP_EQ(L_JUMP_441);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_441:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1552));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_440\n");
JUMP_EQ(L_JUMP_440);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_440:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_439\n");
JUMP_EQ(L_JUMP_439);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_439:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_436);
		    L_ELSE_437:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_436:
		    
		    JUMP(L_IF_EXIT_433);
		    L_ELSE_434:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_433:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_424); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_423);
		    L_THEN_424:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1072));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_432\n");
JUMP_EQ(L_JUMP_432);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_432:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1072));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_431\n");
JUMP_EQ(L_JUMP_431);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_431:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_430);
printf("At L_JUMP_430\n");
fflush(stdout);
L_JUMP_430:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_422);
		    L_ELSE_423:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1505));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_429\n");
JUMP_EQ(L_JUMP_429);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_429:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_427); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_426);
		    L_THEN_427:
 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1505));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_428);
printf("At L_JUMP_428\n");
fflush(stdout);
L_JUMP_428:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_425);
		    L_ELSE_426:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_425:
		    
		    L_IF_EXIT_422:
		    
		    L_IF_EXIT_419:
		    
		    L_IF_EXIT_416:
		    
		    L_IF_EXIT_413:
		    
		    L_IF_EXIT_410:
		    
		    L_IF_EXIT_407:
		    
		    L_IF_EXIT_404:
		    
		    L_IF_EXIT_401:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_399:

POP(FP);
RETURN;
L_CLOS_EXIT_392:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_386));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_384:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_383);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_384);

L_FOR1_END_383:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_382:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_381);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_382);

L_FOR2_END_381:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_385);
L_CLOS_CODE_386: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_380);
printf("at JUMP_SIMPLE_380");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_380:
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_385:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_379);
printf("At L_JUMP_379\n");
fflush(stdout);
L_JUMP_379:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_377:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_371\n");
JUMP_EQ(L_JUMP_371);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_371:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1095), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE^associate

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_336));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_334:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_333);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_334);

L_FOR1_END_333:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_332:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_331);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_332);

L_FOR2_END_331:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_335);
L_CLOS_CODE_336: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_330);
printf("at JUMP_SIMPLE_330");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_330:

 //TC-APPLIC 


//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_351));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_349:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_348);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_349);

L_FOR1_END_348:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_347:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_346);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_347);

L_FOR2_END_346:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_350);
L_CLOS_CODE_351: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_345);
printf("at JUMP_SIMPLE_345");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_345:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_358));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_356:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_355);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_356);

L_FOR1_END_355:
//for (i = 0; i < 3; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_354:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_353);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_354);

L_FOR2_END_353:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_357);
L_CLOS_CODE_358: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_352);
printf("at JUMP_SIMPLE_352");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_352:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_370\n");
JUMP_EQ(L_JUMP_370);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_370:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_361); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_360);
		    L_THEN_361:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_359);
		    L_ELSE_360:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(205));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_369\n");
JUMP_EQ(L_JUMP_369);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_369:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_368\n");
JUMP_EQ(L_JUMP_368);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_368:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_364); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_363);
		    L_THEN_364:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_367);
printf("At L_JUMP_367\n");
fflush(stdout);
L_JUMP_367:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_362);
		    L_ELSE_363:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_366\n");
JUMP_EQ(L_JUMP_366);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_366:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_365);
printf("At L_JUMP_365\n");
fflush(stdout);
L_JUMP_365:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_362:
		    
		    L_IF_EXIT_359:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_357:

POP(FP);
RETURN;
L_CLOS_EXIT_350:
PUSH(R0); // finished evaluating arg 

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_344));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_342:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_341);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_342);

L_FOR1_END_341:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_340:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_339);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_340);

L_FOR2_END_339:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_343);
L_CLOS_CODE_344: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_338);
printf("at JUMP_SIMPLE_338");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_338:
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_343:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_337);
printf("At L_JUMP_337\n");
fflush(stdout);
L_JUMP_337:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_335:
MOV(R1, INDD(IMM(1109), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEassoc

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1095));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1109));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_329\n");
JUMP_EQ(L_JUMP_329);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_329:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1118), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEassq

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1505));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1109));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_328\n");
JUMP_EQ(L_JUMP_328);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_328:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1126), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEvoid

 //APPLIC 

PUSH(IMM(0));
//IF 
MOV(R0,IMM(12));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_327); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_326);
		    L_THEN_327:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_325);
		    L_ELSE_326:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_325:
		    PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_317));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_315:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_314);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_315);

L_FOR1_END_314:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_313:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_312);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_313);

L_FOR2_END_312:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_316);
L_CLOS_CODE_317: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_311);
printf("at JUMP_SIMPLE_311");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_311:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_324));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_322:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_321);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_322);

L_FOR1_END_321:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_320:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_319);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_320);

L_FOR2_END_319:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_323);
L_CLOS_CODE_324: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(0));
JUMP_EQ(JUMP_SIMPLE_318);
printf("at JUMP_SIMPLE_318");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_318:
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

POP(FP);
RETURN;
L_CLOS_EXIT_323:

POP(FP);
RETURN;
L_CLOS_EXIT_316:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_310\n");
JUMP_EQ(L_JUMP_310);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_310:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1134), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEvoid?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1134));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_309\n");
JUMP_EQ(L_JUMP_309);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_309:
//APPLIC CONTINUE
PUSH(IMM(0)); // pushing number of arguments 
PUSH(IMM(0)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_300));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_298:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_297);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_298);

L_FOR1_END_297:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_296:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_295);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_296);

L_FOR2_END_295:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_299);
L_CLOS_CODE_300: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_294);
printf("at JUMP_SIMPLE_294");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_294:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_307));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_305:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_304);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_305);

L_FOR1_END_304:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_303:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_302);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_303);

L_FOR2_END_302:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_306);
L_CLOS_CODE_307: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_301);
printf("at JUMP_SIMPLE_301");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_301:

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1505));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_308);
printf("At L_JUMP_308\n");
fflush(stdout);
L_JUMP_308:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_306:

POP(FP);
RETURN;
L_CLOS_EXIT_299:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_293\n");
JUMP_EQ(L_JUMP_293);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_293:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1143), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEappend

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_277));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_275:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_274);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_275);

L_FOR1_END_274:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_273:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_272);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_273);

L_FOR2_END_272:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_276);
L_CLOS_CODE_277: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_271);
printf("at JUMP_SIMPLE_271");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_271:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_284));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_282:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_281);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_282);

L_FOR1_END_281:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_280:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_279);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_280);

L_FOR2_END_279:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_283);
L_CLOS_CODE_284: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_278);
printf("at JUMP_SIMPLE_278");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_278:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_292\n");
JUMP_EQ(L_JUMP_292);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_292:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_287); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_286);
		    L_THEN_287:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_285);
		    L_ELSE_286:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_291\n");
JUMP_EQ(L_JUMP_291);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_291:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_290\n");
JUMP_EQ(L_JUMP_290);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_290:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 2));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_289\n");
JUMP_EQ(L_JUMP_289);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_289:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_288);
printf("At L_JUMP_288\n");
fflush(stdout);
L_JUMP_288:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_285:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_283:

POP(FP);
RETURN;
L_CLOS_EXIT_276:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_255));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_253:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_252);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_253);

L_FOR1_END_252:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_251:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_250);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_251);

L_FOR2_END_250:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_254);
L_CLOS_CODE_255: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_249);
printf("at JUMP_SIMPLE_249");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_249:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_262));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_260:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_259);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_260);

L_FOR1_END_259:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_258:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_257);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_258);

L_FOR2_END_257:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_261);
L_CLOS_CODE_262: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_256);
printf("at JUMP_SIMPLE_256");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_256:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_270\n");
JUMP_EQ(L_JUMP_270);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_270:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_265); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_264);
		    L_THEN_265:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_263);
		    L_ELSE_264:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_269\n");
JUMP_EQ(L_JUMP_269);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_269:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_268\n");
JUMP_EQ(L_JUMP_268);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_268:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_267\n");
JUMP_EQ(L_JUMP_267);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_267:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1359));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_266);
printf("At L_JUMP_266\n");
fflush(stdout);
L_JUMP_266:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_263:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_261:

POP(FP);
RETURN;
L_CLOS_EXIT_254:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_235));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_233:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_232);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_233);

L_FOR1_END_232:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_231:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_230);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_231);

L_FOR2_END_230:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_234);
L_CLOS_CODE_235: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_229);
printf("at JUMP_SIMPLE_229");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_229:

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_241));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_239:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_238);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_239);

L_FOR1_END_238:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_237:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_236);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_237);

L_FOR2_END_236:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_240);
L_CLOS_CODE_241: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_248\n");
JUMP_EQ(L_JUMP_248);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_248:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_244); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_243);
		    L_THEN_244:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_242);
		    L_ELSE_243:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_247\n");
JUMP_EQ(L_JUMP_247);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_247:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_246\n");
JUMP_EQ(L_JUMP_246);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_246:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 2));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_245);
printf("At L_JUMP_245\n");
fflush(stdout);
L_JUMP_245:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_242:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_240:

POP(FP);
RETURN;
L_CLOS_EXIT_234:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_228\n");
JUMP_EQ(L_JUMP_228);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_228:
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1153), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring-append

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_224));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_222:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_221);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_222);

L_FOR1_END_221:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_220:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_219);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_220);

L_FOR2_END_219:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_223);
L_CLOS_CODE_224: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_227\n");
JUMP_EQ(L_JUMP_227);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_227:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_226\n");
JUMP_EQ(L_JUMP_226);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_226:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(777));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_225);
printf("At L_JUMP_225\n");
fflush(stdout);
L_JUMP_225:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_223:
MOV(R1, INDD(IMM(1170), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEvector-append

//LAMBDA VARIADIC
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_215));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_213:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_212);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_213);

L_FOR1_END_212:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_211:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_210);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_211);

L_FOR2_END_210:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_214);
L_CLOS_CODE_215: 

PUSH(FP);
MOV(FP, SP);
//print_stack("LAMBDA VAR before fixing the stack but after PUSH(FP)");
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//print_stack("LAMBDA VAR after fixing the stack");
//body of the lambda goes here

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1072));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_218\n");
JUMP_EQ(L_JUMP_218);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_218:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1368));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_217\n");
JUMP_EQ(L_JUMP_217);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_217:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(887));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_216);
printf("At L_JUMP_216\n");
fflush(stdout);
L_JUMP_216:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_214:
MOV(R1, INDD(IMM(1187), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEreverse

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_194));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_192:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_191);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_192);

L_FOR1_END_191:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_190:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_189);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_190);

L_FOR2_END_189:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_193);
L_CLOS_CODE_194: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_188);
printf("at JUMP_SIMPLE_188");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_188:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_201));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_199:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_198);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_199);

L_FOR1_END_198:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_197:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_196);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_197);

L_FOR2_END_196:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_200);
L_CLOS_CODE_201: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_195);
printf("at JUMP_SIMPLE_195");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_195:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_209\n");
JUMP_EQ(L_JUMP_209);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_209:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_204); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_203);
		    L_THEN_204:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_202);
		    L_ELSE_203:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_208\n");
JUMP_EQ(L_JUMP_208);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_208:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1359));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_207\n");
JUMP_EQ(L_JUMP_207);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_207:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_206\n");
JUMP_EQ(L_JUMP_206);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_206:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_205);
printf("At L_JUMP_205\n");
fflush(stdout);
L_JUMP_205:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_202:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_200:

POP(FP);
RETURN;
L_CLOS_EXIT_193:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_179));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_177:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_176);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_177);

L_FOR1_END_176:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_175:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_174);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_175);

L_FOR2_END_174:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_178);
L_CLOS_CODE_179: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_173);
printf("at JUMP_SIMPLE_173");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_173:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_186));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_184:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_183);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_184);

L_FOR1_END_183:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_182:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_181);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_182);

L_FOR2_END_181:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_185);
L_CLOS_CODE_186: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_180);
printf("at JUMP_SIMPLE_180");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_180:

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_187);
printf("At L_JUMP_187\n");
fflush(stdout);
L_JUMP_187:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_185:

POP(FP);
RETURN;
L_CLOS_EXIT_178:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_172\n");
JUMP_EQ(L_JUMP_172);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_172:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1198), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring-reverse

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1198));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(777));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_171\n");
JUMP_EQ(L_JUMP_171);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_171:
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1216), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist-ref

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_162));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_160:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_159);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_160);

L_FOR1_END_159:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_158:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_157);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_158);

L_FOR2_END_157:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_161);
L_CLOS_CODE_162: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_156);
printf("at JUMP_SIMPLE_156");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_156:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(83));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_170\n");
JUMP_EQ(L_JUMP_170);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_170:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_165); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_164);
		    L_THEN_165:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_169);
printf("At L_JUMP_169\n");
fflush(stdout);
L_JUMP_169:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_163);
		    L_ELSE_164:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_168\n");
JUMP_EQ(L_JUMP_168);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_168:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_167\n");
JUMP_EQ(L_JUMP_167);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_167:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(825));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_166);
printf("At L_JUMP_166\n");
fflush(stdout);
L_JUMP_166:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_163:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_161:
MOV(R1, INDD(IMM(825), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINElist-set!

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_147));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_145:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_144);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_145);

L_FOR1_END_144:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_143:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_142);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_143);

L_FOR2_END_142:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_146);
L_CLOS_CODE_147: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_141);
printf("at JUMP_SIMPLE_141");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_141:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(83));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_155\n");
JUMP_EQ(L_JUMP_155);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_155:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_150); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_149);
		    L_THEN_150:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1643));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_154);
printf("At L_JUMP_154\n");
fflush(stdout);
L_JUMP_154:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_148);
		    L_ELSE_149:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1285));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_153\n");
JUMP_EQ(L_JUMP_153);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_153:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_152\n");
JUMP_EQ(L_JUMP_152);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_152:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1229));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_151);
printf("At L_JUMP_151\n");
fflush(stdout);
L_JUMP_151:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_148:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_146:
MOV(R1, INDD(IMM(1229), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEmax

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_136));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_134:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_133);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_134);

L_FOR1_END_133:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_132:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_131);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_132);

L_FOR2_END_131:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_135);
L_CLOS_CODE_136: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_130);
printf("at JUMP_SIMPLE_130");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_130:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(550));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_140\n");
JUMP_EQ(L_JUMP_140);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_140:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_139); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_138);
		    L_THEN_139:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_137);
		    L_ELSE_138:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_137:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_135:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_122));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_120:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_119);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_120);

L_FOR1_END_119:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_118:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_117);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_118);

L_FOR2_END_117:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_121);
L_CLOS_CODE_122: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_116);
printf("at JUMP_SIMPLE_116");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_116:

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_124));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_126:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_127);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_126);

L_FOR1_END_127:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_128:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_129);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_128);

L_FOR2_END_129:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_125);
L_CLOS_CODE_124: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(186));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_123);
printf("At L_JUMP_123\n");
fflush(stdout);
L_JUMP_123:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_125:

POP(FP);
RETURN;
L_CLOS_EXIT_121:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_115\n");
JUMP_EQ(L_JUMP_115);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_115:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1236), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEmin

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_110));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_108:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_107);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_108);

L_FOR1_END_107:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_106:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_105);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_106);

L_FOR2_END_105:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_109);
L_CLOS_CODE_110: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_104);
printf("at JUMP_SIMPLE_104");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_104:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(539));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_114\n");
JUMP_EQ(L_JUMP_114);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_114:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_113); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_112);
		    L_THEN_113:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_111);
		    L_ELSE_112:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_111:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_109:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_96));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_94:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_93);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_94);

L_FOR1_END_93:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_92:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_91);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_92);

L_FOR2_END_91:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_95);
L_CLOS_CODE_96: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_90);
printf("at JUMP_SIMPLE_90");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_90:

//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_98));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_100:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_101);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_100);

L_FOR1_END_101:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_102:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_103);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_102);

L_FOR2_END_103:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_99);
L_CLOS_CODE_98: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(186));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_97);
printf("At L_JUMP_97\n");
fflush(stdout);
L_JUMP_97:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,FPARG(1)); // start copying arg2
ADD(R1, IMM(0));
MOV(FPARG(R1), LOCAL(IMM(2)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(2)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(3)); //push number of params 
PUSH(IMM(3)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_99:

POP(FP);
RETURN;
L_CLOS_EXIT_95:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_89\n");
JUMP_EQ(L_JUMP_89);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_89:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1243), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEerror
MOV(R0,IMM(1285));
MOV(R1, INDD(IMM(1252), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEnumber->string

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_69));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_67:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_66);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_67);

L_FOR1_END_66:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_65:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_64);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_65);

L_FOR2_END_64:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_68);
L_CLOS_CODE_69: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_63);
printf("at JUMP_SIMPLE_63");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_63:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_76));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_74:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_73);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_74);

L_FOR1_END_73:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_72:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_71);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_72);

L_FOR2_END_71:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_75);
L_CLOS_CODE_76: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_70);
printf("at JUMP_SIMPLE_70");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_70:
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1649));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(539));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_88\n");
JUMP_EQ(L_JUMP_88);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_88:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_79); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_78);
		    L_THEN_79:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(29));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_87);
printf("At L_JUMP_87\n");
fflush(stdout);
L_JUMP_87:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    JUMP(L_IF_EXIT_77);
		    L_ELSE_78:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1649));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1342));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_86\n");
JUMP_EQ(L_JUMP_86);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_86:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(29));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_85\n");
JUMP_EQ(L_JUMP_85);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_85:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1649));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1649));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1342));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_84\n");
JUMP_EQ(L_JUMP_84);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_84:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(147));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_83\n");
JUMP_EQ(L_JUMP_83);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_83:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_82\n");
JUMP_EQ(L_JUMP_82);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_82:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_81\n");
JUMP_EQ(L_JUMP_81);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_81:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_80);
printf("At L_JUMP_80\n");
fflush(stdout);
L_JUMP_80:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

		    L_IF_EXIT_77:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_75:

POP(FP);
RETURN;
L_CLOS_EXIT_68:
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_50));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_48:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_47);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_48);

L_FOR1_END_47:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_46:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_45);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_46);

L_FOR2_END_45:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_49);
L_CLOS_CODE_50: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_44);
printf("at JUMP_SIMPLE_44");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_44:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1647));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_62\n");
JUMP_EQ(L_JUMP_62);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_62:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_59));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_57:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_56);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_57);

L_FOR1_END_56:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_55:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_54);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_55);

L_FOR2_END_54:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_58);
L_CLOS_CODE_59: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_53);
printf("at JUMP_SIMPLE_53");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_53:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1645));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_61\n");
JUMP_EQ(L_JUMP_61);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_61:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1435));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_60);
printf("At L_JUMP_60\n");
fflush(stdout);
L_JUMP_60:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_58:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_52\n");
JUMP_EQ(L_JUMP_52);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_52:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(777));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_51);
printf("At L_JUMP_51\n");
fflush(stdout);
L_JUMP_51:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(4)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(1)); //push number of params 
PUSH(IMM(1)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_49:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_43\n");
JUMP_EQ(L_JUMP_43);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_43:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1270), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE=

 //APPLIC 

PUSH(IMM(0));

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(2));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_23));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_21:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_20);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_21);

L_FOR1_END_20:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_19:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_18);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_19);

L_FOR2_END_18:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_22);
L_CLOS_CODE_23: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_17);
printf("at JUMP_SIMPLE_17");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_17:

//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_30));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_28:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_27);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_28);

L_FOR1_END_27:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_26:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_25);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_26);

L_FOR2_END_25:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_29);
L_CLOS_CODE_30: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_24);
printf("at JUMP_SIMPLE_24");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_24:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1313));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_42\n");
JUMP_EQ(L_JUMP_42);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_42:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_33); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_32);
		    L_THEN_33:MOV(R0,IMM(14));

		    JUMP(L_IF_EXIT_31);
		    L_ELSE_32:
		    //IF 
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543

//This is the bindings for APPLY
PUSH(LABEL(APPLY));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(123),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MINUS
PUSH(LABEL(BIN_MINUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(41),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MUL
PUSH(LABEL(BIN_MUL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(49),1));
MOV(INDD(R1, 1), R0);

<<<<<<< HEAD
//This is the bindings for BIN_DIV
PUSH(LABEL(BIN_DIV));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(57),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(62),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(114),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(67),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(72),1));
MOV(INDD(R1, 1), R0);
=======
PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_41\n");
JUMP_EQ(L_JUMP_41);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_41:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1410));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_40\n");
JUMP_EQ(L_JUMP_40);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_40:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_36); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_35);
		    L_THEN_36:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_39\n");
JUMP_EQ(L_JUMP_39);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_39:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1351));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_38\n");
JUMP_EQ(L_JUMP_38);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_38:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_37);
printf("At L_JUMP_37\n");
fflush(stdout);
L_JUMP_37:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(90),1));
MOV(INDD(R1, 1), R0);

<<<<<<< HEAD
//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(81),1));
MOV(INDD(R1, 1), R0);

<<<<<<< HEAD
		    JUMP(L_IF_EXIT_34);
		    L_ELSE_35:
=======
//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(106),1));
MOV(INDD(R1, 1), R0);
=======
		    JUMP(L_IF_EXIT_36);
		    L_ELSE_37:
>>>>>>> f2a95e7b85ce50e04bc612d067a95c4263dd3add
		    MOV(R0,IMM(12));

		    L_IF_EXIT_34:
		    
		    L_IF_EXIT_31:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_29:

POP(FP);
RETURN;
L_CLOS_EXIT_22:
PUSH(R0);
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(98),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CAR
PUSH(LABEL(CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(157),1));
MOV(INDD(R1, 1), R0);
<<<<<<< HEAD
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_9));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_7:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_6);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_7);

L_FOR1_END_6:
//for (i = 0; i < 1; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_5:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_4);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_5);

L_FOR2_END_4:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_8);
L_CLOS_CODE_9: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_3);
printf("at JUMP_SIMPLE_3");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_3:
=======

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(164),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_TO_SYMBOL
PUSH(LABEL(STRING_TO_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(182),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for REMAINDER
PUSH(LABEL(REMAINDER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(136),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PROCEDURE
PUSH(LABEL(IS_PROCEDURE));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(150),1));
MOV(INDD(R1, 1), R0);

<<<<<<< HEAD
//This is the bindings for IS_VECTOR
PUSH(LABEL(IS_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(193),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_SYMBOL
PUSH(LABEL(IS_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(204),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_STRING
PUSH(LABEL(IS_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(215),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_CHAR
PUSH(LABEL(IS_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(224),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NUMBER
PUSH(LABEL(IS_NUMBER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(235),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_BOOLEAN
PUSH(LABEL(IS_BOOLEAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(247),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PAIR
PUSH(LABEL(IS_PAIR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(256),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NULL
PUSH(LABEL(IS_NULL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(265),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for INTEGER_TO_CHAR
PUSH(LABEL(INTEGER_TO_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(282),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CHAR_TO_INTEGER
PUSH(LABEL(CHAR_TO_INTEGER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(299),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(316),1));
MOV(INDD(R1, 1), R0);
>>>>>>> f2a95e7b85ce50e04bc612d067a95c4263dd3add

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(398),1));
MOV(INDD(R1, 1), R0);
=======
//LAMBDA OPTIONAL
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_11));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_13:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_14);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_13);

L_FOR1_END_14:
//for (i = 0; i < 2; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_15:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_16);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_15);

L_FOR2_END_16:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_12);
L_CLOS_CODE_11: 
PUSH(FP);
//print_stack("LAMBDA OPT before fixing the stack after PUSH(FP)");
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(1));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//print_stack("LAMBDA OPT after fixing the stack ");
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543

//This is the bindings for MAKE_STRING
PUSH(LABEL(MAKE_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(381),1));
MOV(INDD(R1, 1), R0);

<<<<<<< HEAD
//This is the bindings for MAKE_VECTOR
PUSH(LABEL(MAKE_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(428),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SYMBOL_TO_STRING
PUSH(LABEL(SYMBOL_TO_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(334),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CAR
PUSH(LABEL(SET_CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(346),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CDR
PUSH(LABEL(SET_CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(358),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CONS
PUSH(LABEL(CONS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(366),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_SET
PUSH(LABEL(STRING_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(413),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_REF
PUSH(LABEL(STRING_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(442),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for VECTOR_REF
PUSH(LABEL(VECTOR_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(456),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQ2
PUSH(LABEL(EQ2));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(21),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL
PUSH(LABEL(EQUAL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(466),1));
MOV(INDD(R1, 1), R0);

 // fixing the stack before executing the actual code
PUSH(IMM(0)); //magic number 
PUSH(IMM(0)); //0 arguments 
PUSH(IMM(0)); //0 arguments indicator 
PUSH(IMM(1)); //empty environment 
CALL(MALLOC);
DROP(IMM(1));
=======
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_10);
printf("At L_JUMP_10\n");
fflush(stdout);
L_JUMP_10:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,FPARG(1)); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,FPARG(1)); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));

 //done copying args 
MOV(R5,FP);
SUB(R5,R4); //R5 should point to the lowest arg 
SUB(R5, IMM(3)); // R5 should hold the correct place for SP 
MOV(SP,R5);
PUSH(IMM(2)); //push number of params 
PUSH(IMM(2)); //push indicator 
PUSH(INDD(R0,IMM(1))); //push the ENV 
PUSH(R3); //push the old RET 
MOV(FP,R2); //update FP to the old FP 
JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place

POP(FP);
RETURN;
L_CLOS_EXIT_12:

POP(FP);
RETURN;
<<<<<<< HEAD
L_CLOS_EXIT_8:
=======
L_CLOS_EXIT_10:
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
>>>>>>> f2a95e7b85ce50e04bc612d067a95c4263dd3add
PUSH(R0);
PUSH(IMM(9999)); // dummy return address 
PUSH(IMM(9999)); //just like the beginning of a function body 
MOV(FP,SP);

<<<<<<< HEAD
 // actual code
=======
 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_2\n");
JUMP_EQ(L_JUMP_2);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_2:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1275), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543

 //APPLIC 

PUSH(IMM(0));
<<<<<<< HEAD
=======
MOV(R0,IMM(1285));
PUSH(R0);
<<<<<<< HEAD
=======

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_3\n");
JUMP_EQ(L_JUMP_3);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_3:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
        CALL(WRITE_SOB);    
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543

 //APPLIC 

PUSH(IMM(0));
<<<<<<< HEAD
MOV(R0,IMM(23));
PUSH(R0);
MOV(R0,IMM(33));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(31));
=======
>>>>>>> f2a95e7b85ce50e04bc612d067a95c4263dd3add
MOV(R0,IMM(1285));
PUSH(R0);
MOV(R0,IMM(1285));
PUSH(R0);
MOV(R0,IMM(1285));
PUSH(R0);
MOV(R0,IMM(1285));
PUSH(R0);
<<<<<<< HEAD
MOV(R0,IMM(1285));
PUSH(R0);
MOV(R0,IMM(1285));
=======

 //FVAR 
MOV(R0, IMM(1275));
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_2\n");
JUMP_EQ(L_JUMP_2);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_2:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
<<<<<<< HEAD
MOV(R0,IMM(23));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(21));
=======
        CALL(WRITE_SOB);    
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1344));
>>>>>>> f2a95e7b85ce50e04bc612d067a95c4263dd3add
PUSH(R0);
MOV(R0,IMM(1285));
PUSH(R0);
MOV(R0,IMM(1285));
PUSH(R0);
MOV(R0,IMM(1285));
PUSH(R0);

 //FVAR 
<<<<<<< HEAD
MOV(R0, IMM(137));
=======
MOV(R0, IMM(1275));
>>>>>>> 6ad18c3c7580203ff94d4a2c8456575a13bf4543
>>>>>>> f2a95e7b85ce50e04bc612d067a95c4263dd3add
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1\n");
JUMP_EQ(L_JUMP_1);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1:
//APPLIC CONTINUE
<<<<<<< HEAD
PUSH(IMM(10)); // pushing number of arguments 
PUSH(IMM(10)); // pushing again for reference for frame removal 
=======
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
>>>>>>> f2a95e7b85ce50e04bc612d067a95c4263dd3add
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
        CALL(WRITE_SOB);    
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
L_END_OF_PROGRAM:
STOP_MACHINE;

return 0;
}
