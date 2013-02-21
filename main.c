
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


VECTOR_SET:
PUSH(FP);
MOV(FP,SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R2, FPARG(5));
MOV(R1,(INDD(R1, IMM(1))));
ADD(R1, IMM(2));
MOV(INDD(R0,R1),R2);
MOV(R0, IMM(10));
POP(FP);
RETURN;
L_END_DEFS:

 // definition of constants 

PUSH(R0);
POP(R0);
int arr[1754] = {937610, 722689, 741553, 0, 741553, 1, 799345, 3, 110, 111, 116, 368031, 0, 799345, 4, 108, 105, 115, 116, 368031, 0, 799345, 4, 97, 100, 100, 49, 368031, 0, 799345, 4, 115, 117, 98, 49, 368031, 0, 799345, 5, 98, 105, 110, 62, 63, 368031, 0, 799345, 6, 98, 105, 110, 60, 61, 63, 368031, 0, 799345, 6, 98, 105, 110, 62, 61, 63, 368031, 0, 799345, 5, 122, 101, 114, 111, 63, 368031, 0, 799345, 6, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 5, 108, 105, 115, 116, 63, 368031, 0, 799345, 5, 101, 118, 101, 110, 63, 368031, 0, 799345, 4, 111, 100, 100, 63, 368031, 0, 799345, 3, 109, 97, 112, 368031, 0, 799345, 2, 121, 110, 368031, 0, 799345, 1, 43, 368031, 0, 799345, 1, 42, 368031, 0, 799345, 1, 45, 368031, 0, 799345, 1, 47, 368031, 0, 799345, 5, 111, 114, 100, 101, 114, 368031, 0, 799345, 6, 97, 110, 100, 109, 97, 112, 368031, 0, 799345, 2, 60, 62, 368031, 0, 799345, 5, 102, 111, 108, 100, 114, 368031, 0, 799345, 7, 99, 111, 109, 112, 111, 115, 101, 368031, 0, 799345, 4, 99, 97, 97, 114, 368031, 0, 799345, 4, 99, 97, 100, 114, 368031, 0, 799345, 4, 99, 100, 97, 114, 368031, 0, 799345, 4, 99, 100, 100, 114, 368031, 0, 799345, 5, 99, 97, 97, 97, 114, 368031, 0, 799345, 5, 99, 97, 97, 100, 114, 368031, 0, 799345, 5, 99, 97, 100, 97, 114, 368031, 0, 799345, 5, 99, 97, 100, 100, 114, 368031, 0, 799345, 5, 99, 100, 97, 97, 114, 368031, 0, 799345, 5, 99, 100, 97, 100, 114, 368031, 0, 799345, 5, 99, 100, 100, 97, 114, 368031, 0, 799345, 5, 99, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 100, 114, 368031, 0, 799345, 27, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 114, 105, 103, 104, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 26, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 108, 101, 102, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 8, 94, 99, 104, 97, 114, 45, 111, 112, 368031, 0, 799345, 1, 60, 368031, 0, 799345, 2, 60, 61, 368031, 0, 799345, 1, 62, 368031, 0, 799345, 2, 62, 61, 368031, 0, 799345, 6, 99, 104, 97, 114, 61, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 60, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 60, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 62, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 62, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 117, 112, 112, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 108, 111, 119, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 11, 99, 104, 97, 114, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 60, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 60, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 62, 63, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 62, 61, 63, 368031, 0, 799345, 12, 115, 116, 114, 105, 110, 103, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 15, 115, 116, 114, 105, 110, 103, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 8, 108, 105, 115, 116, 45, 114, 101, 102, 368031, 0, 799345, 5, 111, 114, 109, 97, 112, 368031, 0, 799345, 7, 109, 101, 109, 98, 101, 114, 63, 368031, 0, 799345, 9, 110, 101, 103, 97, 116, 105, 118, 101, 63, 368031, 0, 799345, 9, 112, 111, 115, 105, 116, 105, 118, 101, 63, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 6, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 12, 118, 101, 99, 116, 111, 114, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 9, 98, 111, 111, 108, 101, 97, 110, 61, 63, 368031, 0, 799345, 6, 101, 113, 117, 97, 108, 63, 368031, 0, 799345, 10, 94, 97, 115, 115, 111, 99, 105, 97, 116, 101, 368031, 0, 799345, 5, 97, 115, 115, 111, 99, 368031, 0, 799345, 4, 97, 115, 115, 113, 368031, 0, 799345, 4, 118, 111, 105, 100, 368031, 0, 799345, 5, 118, 111, 105, 100, 63, 368031, 0, 799345, 6, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 7, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 9, 108, 105, 115, 116, 45, 115, 101, 116, 33, 368031, 0, 799345, 3, 109, 97, 120, 368031, 0, 799345, 3, 109, 105, 110, 368031, 0, 799345, 5, 101, 114, 114, 111, 114, 368031, 0, 799345, 14, 110, 117, 109, 98, 101, 114, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 1, 61, 368031, 0, 799345, 1, 115, 368031, 0, 799345, 4, 98, 105, 110, 43, 368031, 0, 945311, 1, 799345, 4, 98, 105, 110, 45, 368031, 0, 799345, 5, 98, 105, 110, 60, 63, 368031, 0, 945311, 0, 799345, 5, 110, 117, 108, 108, 63, 368031, 0, 799345, 3, 99, 100, 114, 368031, 0, 799345, 5, 112, 97, 105, 114, 63, 368031, 0, 799345, 9, 114, 101, 109, 97, 105, 110, 100, 101, 114, 368031, 0, 945311, 2, 799345, 3, 99, 97, 114, 368031, 0, 799345, 4, 99, 111, 110, 115, 368031, 0, 799345, 5, 97, 112, 112, 108, 121, 368031, 0, 799345, 4, 98, 105, 110, 42, 368031, 0, 799345, 4, 98, 105, 110, 47, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 62, 105, 110, 116, 101, 103, 101, 114, 368031, 0, 799345, 5, 98, 105, 110, 61, 63, 368031, 0, 181048, 65, 181048, 90, 181048, 97, 181048, 122, 799345, 13, 105, 110, 116, 101, 103, 101, 114, 45, 62, 99, 104, 97, 114, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 945311, -1, 799345, 10, 115, 116, 114, 105, 110, 103, 45, 114, 101, 102, 368031, 0, 799345, 11, 115, 116, 114, 105, 110, 103, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 3, 101, 113, 63, 368031, 0, 799345, 11, 118, 101, 99, 116, 111, 114, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 10, 118, 101, 99, 116, 111, 114, 45, 114, 101, 102, 368031, 0, 799345, 8, 98, 111, 111, 108, 101, 97, 110, 63, 368031, 0, 799345, 5, 99, 104, 97, 114, 63, 368031, 0, 799345, 7, 110, 117, 109, 98, 101, 114, 63, 368031, 0, 799345, 7, 115, 116, 114, 105, 110, 103, 63, 368031, 0, 799345, 7, 115, 121, 109, 98, 111, 108, 63, 368031, 0, 799345, 7, 118, 101, 99, 116, 111, 114, 63, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 97, 114, 33, 368031, 0, 945311, 48, 945311, 123, 945311, 10, 945311, 97, 945311, 98, 799345, 2, 97, 98, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 62, 115, 121, 109, 98, 111, 108, 368031, 0, 799345, 4, 98, 105, 110, 60, 368031, 0, 799345, 4, 98, 105, 110, 61, 368031, 0, 799345, 4, 98, 105, 110, 62, 368031, 0, 799345, 10, 112, 114, 111, 99, 101, 100, 117, 114, 101, 63, 368031, 0, 799345, 14, 115, 121, 109, 98, 111, 108, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 100, 114, 33, 368031, 0, 799345, 8, 105, 110, 116, 101, 103, 101, 114, 63, 368031, 0};
void *a = &arr;
memcpy(&((*machine).mem[10]), a, 1754*4);
MOV(ADDR(0), IMM(1764));

 //generating symbol table
MOV(R1, IMM(1));
//creating bucket for not
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(16)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(22), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
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
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for sub1
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(39)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(46), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(47)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(55), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(56)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(65), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(66)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(75), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for zero?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(76)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(84), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(85)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(94), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(95)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(103), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for even?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(104)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(112), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for odd?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(113)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(120), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for map
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(121)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(127), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for yn
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(128)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(133), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for +
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(134)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(138), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for *
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(139)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(143), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for -
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(144)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(148), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for /
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(149)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(153), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for order
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(154)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
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
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(173)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(178), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for foldr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(179)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
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
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(223)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(230), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(231)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(239), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(240)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(248), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(249)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(257), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(258)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(266), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(267)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(275), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(276)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(284), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(285)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(293), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(294)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(302), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(303)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(312), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(313)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
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
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(333)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(342), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(343)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(352), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(353)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(362), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caddar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(363)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
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
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdadar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(403)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(412), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(413)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(422), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(423)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(432), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(433)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(442), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdddar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(443)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(452), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(453)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(462), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
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
//creating bucket for s
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1277)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1281), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin+
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1282)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1289), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin-
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1292)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1299), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1300)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1308), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for null?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1311)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1319), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1320)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1326), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for pair?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1327)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1335), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for remainder
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1336)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1348), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for car
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1351)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1357), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cons
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1358)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1365), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for apply
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1366)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1374), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin*
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1375)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1382), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin/
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1383)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1390), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char->integer
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1391)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1407), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1408)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1416), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for integer->char
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1425)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1441), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1442)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1458), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1461)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1474), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1475)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1489), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1490)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1504), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for eq?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1505)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1511), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1512)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1526), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1527)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1541), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1542)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1558), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1559)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1572), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for boolean?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1573)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1584), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1585)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1593), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for number?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1594)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1604), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1605)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1615), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1616)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1626), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1627)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1637), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-car!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1638)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1649), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ab
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1660)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1665), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->symbol
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1666)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1683), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1684)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1691), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1692)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1699), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1700)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1707), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for procedure?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1708)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1721), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1722)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1739), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-cdr!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1740)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1751), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for integer?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1752)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1763), R0); // save the bucket address in the symbol object 
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
MOV(R1, INDD(IMM(1325),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_PLUS
PUSH(LABEL(BIN_PLUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1288),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for APPLY
PUSH(LABEL(APPLY));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1373),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MINUS
PUSH(LABEL(BIN_MINUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1298),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MUL
PUSH(LABEL(BIN_MUL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1381),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_DIV
PUSH(LABEL(BIN_DIV));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1389),1));
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
MOV(R1, INDD(IMM(1706),1));
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
MOV(R1, INDD(IMM(1415),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1307),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1698),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1690),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CAR
PUSH(LABEL(CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1356),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1325),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_TO_SYMBOL
PUSH(LABEL(STRING_TO_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1682),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for REMAINDER
PUSH(LABEL(REMAINDER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1347),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PROCEDURE
PUSH(LABEL(IS_PROCEDURE));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1720),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_VECTOR
PUSH(LABEL(IS_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1636),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_SYMBOL
PUSH(LABEL(IS_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1625),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_STRING
PUSH(LABEL(IS_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1614),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_CHAR
PUSH(LABEL(IS_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1592),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NUMBER
PUSH(LABEL(IS_NUMBER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1603),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NUMBER
PUSH(LABEL(IS_NUMBER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1762),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_BOOLEAN
PUSH(LABEL(IS_BOOLEAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1583),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PAIR
PUSH(LABEL(IS_PAIR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1334),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NULL
PUSH(LABEL(IS_NULL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1318),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for INTEGER_TO_CHAR
PUSH(LABEL(INTEGER_TO_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1440),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CHAR_TO_INTEGER
PUSH(LABEL(CHAR_TO_INTEGER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1406),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1457),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1557),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_STRING
PUSH(LABEL(MAKE_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1503),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_VECTOR
PUSH(LABEL(MAKE_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1540),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SYMBOL_TO_STRING
PUSH(LABEL(SYMBOL_TO_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1738),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CAR
PUSH(LABEL(SET_CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1648),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CDR
PUSH(LABEL(SET_CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1750),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CONS
PUSH(LABEL(CONS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1364),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_SET
PUSH(LABEL(STRING_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1488),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for VECTOR_SET
PUSH(LABEL(VECTOR_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1525),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_REF
PUSH(LABEL(STRING_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1473),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for VECTOR_REF
PUSH(LABEL(VECTOR_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1571),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQ2
PUSH(LABEL(EQ2));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1510),1));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1825));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1823:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1822);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1823);

L_FOR1_END_1822:
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

L_FOR2_START_1821:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1820);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1821);

L_FOR2_END_1820:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1824);
L_CLOS_CODE_1825: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1819);
printf("at JUMP_SIMPLE_1819");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1819:
//IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1828); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1827);
		    L_THEN_1828:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_1826);
		    L_ELSE_1827:
		    MOV(R0,IMM(14));

		    L_IF_EXIT_1826:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1824:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1818));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1816:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1815);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1816);

L_FOR1_END_1815:
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

L_FOR2_START_1814:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1813);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1814);

L_FOR2_END_1813:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1817);
L_CLOS_CODE_1818: 

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
L_CLOS_EXIT_1817:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1811));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1809:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1808);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1809);

L_FOR1_END_1808:
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

L_FOR2_START_1807:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1806);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1807);

L_FOR2_END_1806:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1810);
L_CLOS_CODE_1811: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1805);
printf("at JUMP_SIMPLE_1805");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1805:

 //TC-APPLIC 

MOV(R0,IMM(1290));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1288));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1812);
printf("At L_JUMP_1812\n");
fflush(stdout);
L_JUMP_1812:
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
L_CLOS_EXIT_1810:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1803));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1801:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1800);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1801);

L_FOR1_END_1800:
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

L_FOR2_START_1799:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1798);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1799);

L_FOR2_END_1798:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1802);
L_CLOS_CODE_1803: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1797);
printf("at JUMP_SIMPLE_1797");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1797:

 //TC-APPLIC 

MOV(R0,IMM(1290));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1298));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1804);
printf("At L_JUMP_1804\n");
fflush(stdout);
L_JUMP_1804:
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
L_CLOS_EXIT_1802:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1795));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1793:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1792);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1793);

L_FOR1_END_1792:
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

L_FOR2_START_1791:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1790);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1791);

L_FOR2_END_1790:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1794);
L_CLOS_CODE_1795: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1789);
printf("at JUMP_SIMPLE_1789");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1789:

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
MOV(R0, IMM(1307));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1796);
printf("At L_JUMP_1796\n");
fflush(stdout);
L_JUMP_1796:
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
L_CLOS_EXIT_1794:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1786));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1784:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1783);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1784);

L_FOR1_END_1783:
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

L_FOR2_START_1782:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1781);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1782);

L_FOR2_END_1781:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1785);
L_CLOS_CODE_1786: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1780);
printf("at JUMP_SIMPLE_1780");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1780:

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
printf("At L_JUMP_1788\n");
JUMP_EQ(L_JUMP_1788);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1788:
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
JUMP_EQ(L_JUMP_1787);
printf("At L_JUMP_1787\n");
fflush(stdout);
L_JUMP_1787:
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
L_CLOS_EXIT_1785:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1777));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1775:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1774);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1775);

L_FOR1_END_1774:
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

L_FOR2_START_1773:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1772);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1773);

L_FOR2_END_1772:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1776);
L_CLOS_CODE_1777: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1771);
printf("at JUMP_SIMPLE_1771");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1771:

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
MOV(R0, IMM(1307));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1779\n");
JUMP_EQ(L_JUMP_1779);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1779:
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
JUMP_EQ(L_JUMP_1778);
printf("At L_JUMP_1778\n");
fflush(stdout);
L_JUMP_1778:
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
L_CLOS_EXIT_1776:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1769));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1767:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1766);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1767);

L_FOR1_END_1766:
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

L_FOR2_START_1765:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1764);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1765);

L_FOR2_END_1764:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1768);
L_CLOS_CODE_1769: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1763);
printf("at JUMP_SIMPLE_1763");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1763:

 //TC-APPLIC 

MOV(R0,IMM(1309));
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
JUMP_EQ(L_JUMP_1770);
printf("At L_JUMP_1770\n");
fflush(stdout);
L_JUMP_1770:
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
L_CLOS_EXIT_1768:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1755));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1753:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1752);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1753);

L_FOR1_END_1752:
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

L_FOR2_START_1751:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1750);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1751);

L_FOR2_END_1750:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1754);
L_CLOS_CODE_1755: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1749);
printf("at JUMP_SIMPLE_1749");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1749:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1762\n");
JUMP_EQ(L_JUMP_1762);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1762:
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
		    JUMP_NE(L_THEN_1758); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1757);
		    L_THEN_1758:MOV(R0,IMM(1309));

		    JUMP(L_IF_EXIT_1756);
		    L_ELSE_1757:
		    
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1761\n");
JUMP_EQ(L_JUMP_1761);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1761:
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
printf("At L_JUMP_1760\n");
JUMP_EQ(L_JUMP_1760);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1760:
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
JUMP_EQ(L_JUMP_1759);
printf("At L_JUMP_1759\n");
fflush(stdout);
L_JUMP_1759:
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

		    L_IF_EXIT_1756:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1754:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1740));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1738:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1737);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1738);

L_FOR1_END_1737:
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

L_FOR2_START_1736:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1735);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1736);

L_FOR2_END_1735:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1739);
L_CLOS_CODE_1740: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1734);
printf("at JUMP_SIMPLE_1734");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1734:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1748\n");
JUMP_EQ(L_JUMP_1748);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1748:
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
JUMP_NE(L_OR_END_1741);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1741);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1334));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1747\n");
JUMP_EQ(L_JUMP_1747);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1747:
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
		    JUMP_NE(L_THEN_1744); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1743);
		    L_THEN_1744:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1746\n");
JUMP_EQ(L_JUMP_1746);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1746:
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
JUMP_EQ(L_JUMP_1745);
printf("At L_JUMP_1745\n");
fflush(stdout);
L_JUMP_1745:
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

		    JUMP(L_IF_EXIT_1742);
		    L_ELSE_1743:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1742:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1741);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1741);
L_OR_END_1741:

POP(FP);
RETURN;
L_CLOS_EXIT_1739:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1731));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1729:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1728);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1729);

L_FOR1_END_1728:
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

L_FOR2_START_1727:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1726);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1727);

L_FOR2_END_1726:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1730);
L_CLOS_CODE_1731: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1725);
printf("at JUMP_SIMPLE_1725");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1725:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1349));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1347));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1733\n");
JUMP_EQ(L_JUMP_1733);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1733:
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
JUMP_EQ(L_JUMP_1732);
printf("At L_JUMP_1732\n");
fflush(stdout);
L_JUMP_1732:
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
L_CLOS_EXIT_1730:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1721));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1719:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1718);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1719);

L_FOR1_END_1718:
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

L_FOR2_START_1717:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1716);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1717);

L_FOR2_END_1716:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1720);
L_CLOS_CODE_1721: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1715);
printf("at JUMP_SIMPLE_1715");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1715:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1349));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1347));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1724\n");
JUMP_EQ(L_JUMP_1724);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1724:
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
printf("At L_JUMP_1723\n");
JUMP_EQ(L_JUMP_1723);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1723:
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
JUMP_EQ(L_JUMP_1722);
printf("At L_JUMP_1722\n");
fflush(stdout);
L_JUMP_1722:
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
L_CLOS_EXIT_1720:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1681));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1679:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1678);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1679);

L_FOR1_END_1678:
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

L_FOR2_START_1677:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1676);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1677);

L_FOR2_END_1676:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1680);
L_CLOS_CODE_1681: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1675);
printf("at JUMP_SIMPLE_1675");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1675:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1705));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1703:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1702);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1703);

L_FOR1_END_1702:
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

L_FOR2_START_1701:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1700);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1701);

L_FOR2_END_1700:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1704);
L_CLOS_CODE_1705: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1699);
printf("at JUMP_SIMPLE_1699");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1699:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1712));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1710:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1709);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1710);

L_FOR1_END_1709:
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

L_FOR2_START_1708:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1707);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1708);

L_FOR2_END_1707:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1711);
L_CLOS_CODE_1712: 

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
printf("At L_JUMP_1714\n");
JUMP_EQ(L_JUMP_1714);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1714:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1713);
printf("At L_JUMP_1713\n");
fflush(stdout);
L_JUMP_1713:
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
L_CLOS_EXIT_1711:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1706);
printf("At L_JUMP_1706\n");
fflush(stdout);
L_JUMP_1706:
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
L_CLOS_EXIT_1704:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1689));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1687:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1686);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1687);

L_FOR1_END_1686:
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

L_FOR2_START_1685:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1684);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1685);

L_FOR2_END_1684:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1688);
L_CLOS_CODE_1689: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1683);
printf("at JUMP_SIMPLE_1683");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1683:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1696));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1694:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1693);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1694);

L_FOR1_END_1693:
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

L_FOR2_START_1692:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1691);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1692);

L_FOR2_END_1691:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1695);
L_CLOS_CODE_1696: 

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
printf("At L_JUMP_1698\n");
JUMP_EQ(L_JUMP_1698);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1698:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1697);
printf("At L_JUMP_1697\n");
fflush(stdout);
L_JUMP_1697:
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
L_CLOS_EXIT_1695:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1690);
printf("At L_JUMP_1690\n");
fflush(stdout);
L_JUMP_1690:
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
L_CLOS_EXIT_1688:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1682);
printf("At L_JUMP_1682\n");
fflush(stdout);
L_JUMP_1682:
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
L_CLOS_EXIT_1680:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1602));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1600:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1599);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1600);

L_FOR1_END_1599:
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

L_FOR2_START_1598:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1597);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1598);

L_FOR2_END_1597:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1601);
L_CLOS_CODE_1602: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1596);
printf("at JUMP_SIMPLE_1596");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1596:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1658));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1656:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1655);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1656);

L_FOR1_END_1655:
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

L_FOR2_START_1654:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1653);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1654);

L_FOR2_END_1653:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1657);
L_CLOS_CODE_1658: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1652);
printf("at JUMP_SIMPLE_1652");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1652:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1665));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1663:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1662);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1663);

L_FOR1_END_1662:
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

L_FOR2_START_1661:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1660);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1661);

L_FOR2_END_1660:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1664);
L_CLOS_CODE_1665: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1659);
printf("at JUMP_SIMPLE_1659");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1659:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1674\n");
JUMP_EQ(L_JUMP_1674);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1674:
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
		    JUMP_NE(L_THEN_1668); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1667);
		    L_THEN_1668:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_1666);
		    L_ELSE_1667:
		    
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1673\n");
JUMP_EQ(L_JUMP_1673);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1673:
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
printf("At L_JUMP_1672\n");
JUMP_EQ(L_JUMP_1672);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1672:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1671\n");
JUMP_EQ(L_JUMP_1671);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1671:
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
printf("At L_JUMP_1670\n");
JUMP_EQ(L_JUMP_1670);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1670:
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
MOV(R0, IMM(1364));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1669);
printf("At L_JUMP_1669\n");
fflush(stdout);
L_JUMP_1669:
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

		    L_IF_EXIT_1666:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1664:

POP(FP);
RETURN;
L_CLOS_EXIT_1657:
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1651\n");
JUMP_EQ(L_JUMP_1651);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1651:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1610));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1608:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1607);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1608);

L_FOR1_END_1607:
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

L_FOR2_START_1606:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1605);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1606);

L_FOR2_END_1605:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1609);
L_CLOS_CODE_1610: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1604);
printf("at JUMP_SIMPLE_1604");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1604:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1633));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1631:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1630);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1631);

L_FOR1_END_1630:
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

L_FOR2_START_1629:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1628);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1629);

L_FOR2_END_1628:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1632);
L_CLOS_CODE_1633: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1627);
printf("at JUMP_SIMPLE_1627");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1627:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1640));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1638:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1637);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1638);

L_FOR1_END_1637:
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

L_FOR2_START_1636:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1635);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1636);

L_FOR2_END_1635:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1639);
L_CLOS_CODE_1640: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1634);
printf("at JUMP_SIMPLE_1634");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1634:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1650\n");
JUMP_EQ(L_JUMP_1650);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1650:
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
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1649\n");
JUMP_EQ(L_JUMP_1649);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1649:
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
		    JUMP_NE(L_THEN_1643); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1642);
		    L_THEN_1643:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_1641);
		    L_ELSE_1642:
		    
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
MOV(R0, IMM(1325));
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
printf("At L_JUMP_1648\n");
JUMP_EQ(L_JUMP_1648);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1648:
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
printf("At L_JUMP_1647\n");
JUMP_EQ(L_JUMP_1647);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1647:
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
MOV(R0, IMM(1356));
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
printf("At L_JUMP_1646\n");
JUMP_EQ(L_JUMP_1646);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1646:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1645\n");
JUMP_EQ(L_JUMP_1645);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1645:
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
MOV(R0, IMM(1364));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1644);
printf("At L_JUMP_1644\n");
fflush(stdout);
L_JUMP_1644:
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

		    L_IF_EXIT_1641:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1639:

POP(FP);
RETURN;
L_CLOS_EXIT_1632:
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1626\n");
JUMP_EQ(L_JUMP_1626);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1626:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1618));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1616:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1615);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1616);

L_FOR1_END_1615:
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

L_FOR2_START_1614:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1613);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1614);

L_FOR2_END_1613:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1617);
L_CLOS_CODE_1618: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1612);
printf("at JUMP_SIMPLE_1612");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1612:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1620));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1622:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1623);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1622);

L_FOR1_END_1623:
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

L_FOR2_START_1624:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1625);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1624);

L_FOR2_END_1625:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1621);
L_CLOS_CODE_1620: 
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
JUMP_EQ(L_JUMP_1619);
printf("At L_JUMP_1619\n");
fflush(stdout);
L_JUMP_1619:
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
L_CLOS_EXIT_1621:

POP(FP);
RETURN;
L_CLOS_EXIT_1617:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1611);
printf("At L_JUMP_1611\n");
fflush(stdout);
L_JUMP_1611:
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
L_CLOS_EXIT_1609:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1603);
printf("At L_JUMP_1603\n");
fflush(stdout);
L_JUMP_1603:
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
L_CLOS_EXIT_1601:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1595\n");
JUMP_EQ(L_JUMP_1595);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1595:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1553));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1551:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1550);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1551);

L_FOR1_END_1550:
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

L_FOR2_START_1549:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1548);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1549);

L_FOR2_END_1548:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1552);
L_CLOS_CODE_1553: 

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1571));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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

L_FOR2_START_1567:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1566);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1565);
printf("at JUMP_SIMPLE_1565");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1565:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1577));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1575:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1574);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1575);

L_FOR1_END_1574:
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

L_FOR2_START_1573:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1572);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1573);

L_FOR2_END_1572:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1576);
L_CLOS_CODE_1577: 

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1586));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

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

L_FOR2_START_1582:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1581);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1580);
printf("at JUMP_SIMPLE_1580");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1580:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1592));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_1590:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1589);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1590);

L_FOR1_END_1589:
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

L_FOR2_START_1588:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1587);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1588);

L_FOR2_END_1587:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1591);
L_CLOS_CODE_1592: 

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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1594\n");
JUMP_EQ(L_JUMP_1594);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1594:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1593);
printf("At L_JUMP_1593\n");
fflush(stdout);
L_JUMP_1593:
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
L_CLOS_EXIT_1591:

POP(FP);
RETURN;
L_CLOS_EXIT_1585:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1579\n");
JUMP_EQ(L_JUMP_1579);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1579:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1578);
printf("At L_JUMP_1578\n");
fflush(stdout);
L_JUMP_1578:
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
L_CLOS_EXIT_1576:

POP(FP);
RETURN;
L_CLOS_EXIT_1570:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1564\n");
JUMP_EQ(L_JUMP_1564);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1564:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1561));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1559:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1558);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1559);

L_FOR1_END_1558:
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

L_FOR2_START_1557:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1556);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1557);

L_FOR2_END_1556:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1560);
L_CLOS_CODE_1561: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1555);
printf("at JUMP_SIMPLE_1555");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1555:

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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1563\n");
JUMP_EQ(L_JUMP_1563);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1563:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1562);
printf("At L_JUMP_1562\n");
fflush(stdout);
L_JUMP_1562:
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
L_CLOS_EXIT_1560:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1554);
printf("At L_JUMP_1554\n");
fflush(stdout);
L_JUMP_1554:
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
L_CLOS_EXIT_1552:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1532));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1530:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1529);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1530);

L_FOR1_END_1529:
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

L_FOR2_START_1528:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1527);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1528);

L_FOR2_END_1527:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1531);
L_CLOS_CODE_1532: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1526);
printf("at JUMP_SIMPLE_1526");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1526:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1539));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1537:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1536);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1537);

L_FOR1_END_1536:
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

L_FOR2_START_1535:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1534);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1535);

L_FOR2_END_1534:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1538);
L_CLOS_CODE_1539: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1533);
printf("at JUMP_SIMPLE_1533");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1533:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1547\n");
JUMP_EQ(L_JUMP_1547);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1547:
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
		    JUMP_NE(L_THEN_1542); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1541);
		    L_THEN_1542:MOV(R0,IMM(1309));

		    JUMP(L_IF_EXIT_1540);
		    L_ELSE_1541:
		    
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1546\n");
JUMP_EQ(L_JUMP_1546);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1546:
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
printf("At L_JUMP_1545\n");
JUMP_EQ(L_JUMP_1545);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1545:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1544\n");
JUMP_EQ(L_JUMP_1544);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1544:
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
MOV(R0, IMM(1288));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1543);
printf("At L_JUMP_1543\n");
fflush(stdout);
L_JUMP_1543:
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

		    L_IF_EXIT_1540:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1538:

POP(FP);
RETURN;
L_CLOS_EXIT_1531:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1518));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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

L_FOR2_START_1514:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1513);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1512);
printf("at JUMP_SIMPLE_1512");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1512:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1524));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1522:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1521);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1522);

L_FOR1_END_1521:
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

L_FOR2_START_1520:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1519);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1520);

L_FOR2_END_1519:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1523);
L_CLOS_CODE_1524: 

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
JUMP_EQ(L_JUMP_1525);
printf("At L_JUMP_1525\n");
fflush(stdout);
L_JUMP_1525:
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
L_CLOS_EXIT_1523:

POP(FP);
RETURN;
L_CLOS_EXIT_1517:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1511\n");
JUMP_EQ(L_JUMP_1511);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1511:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1495));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1493:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1492);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1493);

L_FOR1_END_1492:
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

L_FOR2_START_1491:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1490);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1491);

L_FOR2_END_1490:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1494);
L_CLOS_CODE_1495: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1489);
printf("at JUMP_SIMPLE_1489");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1489:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1502));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1500:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1499);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1500);

L_FOR1_END_1499:
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

L_FOR2_START_1498:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1497);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1498);

L_FOR2_END_1497:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1501);
L_CLOS_CODE_1502: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1496);
printf("at JUMP_SIMPLE_1496");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1496:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1510\n");
JUMP_EQ(L_JUMP_1510);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1510:
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
		    JUMP_NE(L_THEN_1505); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1504);
		    L_THEN_1505:MOV(R0,IMM(1290));

		    JUMP(L_IF_EXIT_1503);
		    L_ELSE_1504:
		    
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1509\n");
JUMP_EQ(L_JUMP_1509);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1509:
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
printf("At L_JUMP_1508\n");
JUMP_EQ(L_JUMP_1508);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1508:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1507\n");
JUMP_EQ(L_JUMP_1507);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1507:
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
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1506);
printf("At L_JUMP_1506\n");
fflush(stdout);
L_JUMP_1506:
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

		    L_IF_EXIT_1503:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1501:

POP(FP);
RETURN;
L_CLOS_EXIT_1494:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1481));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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

L_FOR2_START_1477:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1476);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1475);
printf("at JUMP_SIMPLE_1475");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1475:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1487));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1485:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1484);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1485);

L_FOR1_END_1484:
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

L_FOR2_START_1483:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1482);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1483);

L_FOR2_END_1482:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1486);
L_CLOS_CODE_1487: 

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
JUMP_EQ(L_JUMP_1488);
printf("At L_JUMP_1488\n");
fflush(stdout);
L_JUMP_1488:
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
L_CLOS_EXIT_1486:

POP(FP);
RETURN;
L_CLOS_EXIT_1480:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1474\n");
JUMP_EQ(L_JUMP_1474);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1474:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1468));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1470:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1471);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1470);

L_FOR1_END_1471:
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

L_FOR2_START_1472:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1473);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1472);

L_FOR2_END_1473:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1469);
L_CLOS_CODE_1468: 
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
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1467\n");
JUMP_EQ(L_JUMP_1467);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1467:
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
		    JUMP_NE(L_THEN_1463); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1462);
		    L_THEN_1463:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1309));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1298));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1466);
printf("At L_JUMP_1466\n");
fflush(stdout);
L_JUMP_1466:
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

		    JUMP(L_IF_EXIT_1461);
		    L_ELSE_1462:
		    
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1465\n");
JUMP_EQ(L_JUMP_1465);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1465:
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
MOV(R0, IMM(1298));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1464);
printf("At L_JUMP_1464\n");
fflush(stdout);
L_JUMP_1464:
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

		    L_IF_EXIT_1461:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1469:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1455));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1457:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1458);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1457);

L_FOR1_END_1458:
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

L_FOR2_START_1459:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1460);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1459);

L_FOR2_END_1460:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1456);
L_CLOS_CODE_1455: 
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
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1454\n");
JUMP_EQ(L_JUMP_1454);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1454:
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
		    JUMP_NE(L_THEN_1450); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1449);
		    L_THEN_1450:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1290));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1389));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1453);
printf("At L_JUMP_1453\n");
fflush(stdout);
L_JUMP_1453:
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

		    JUMP(L_IF_EXIT_1448);
		    L_ELSE_1449:
		    
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1452\n");
JUMP_EQ(L_JUMP_1452);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1452:
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
MOV(R0, IMM(1389));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1451);
printf("At L_JUMP_1451\n");
fflush(stdout);
L_JUMP_1451:
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

		    L_IF_EXIT_1448:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1456:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1408));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1406:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1405);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1406);

L_FOR1_END_1405:
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

L_FOR2_START_1404:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1403);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1404);

L_FOR2_END_1403:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1407);
L_CLOS_CODE_1408: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1402);
printf("at JUMP_SIMPLE_1402");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1402:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1430));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1428:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1427);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1428);

L_FOR1_END_1427:
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

L_FOR2_START_1426:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1425);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1426);

L_FOR2_END_1425:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1429);
L_CLOS_CODE_1430: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1424);
printf("at JUMP_SIMPLE_1424");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1424:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1437));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1435:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1434);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1435);

L_FOR1_END_1434:
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

L_FOR2_START_1433:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1432);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1433);

L_FOR2_END_1432:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1436);
L_CLOS_CODE_1437: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1431);
printf("at JUMP_SIMPLE_1431");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1431:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1447\n");
JUMP_EQ(L_JUMP_1447);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1447:
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
JUMP_NE(L_OR_END_1438);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1438);
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
MOV(R0, IMM(1356));
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
printf("At L_JUMP_1445\n");
JUMP_EQ(L_JUMP_1445);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1445:
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
		    JUMP_NE(L_THEN_1441); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1440);
		    L_THEN_1441:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1444\n");
JUMP_EQ(L_JUMP_1444);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1444:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1443\n");
JUMP_EQ(L_JUMP_1443);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1443:
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
JUMP_EQ(L_JUMP_1442);
printf("At L_JUMP_1442\n");
fflush(stdout);
L_JUMP_1442:
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

		    JUMP(L_IF_EXIT_1439);
		    L_ELSE_1440:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1439:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1438);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1438);
L_OR_END_1438:

POP(FP);
RETURN;
L_CLOS_EXIT_1436:

POP(FP);
RETURN;
L_CLOS_EXIT_1429:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1416));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1414:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1413);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1414);

L_FOR1_END_1413:
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

L_FOR2_START_1412:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1411);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1412);

L_FOR2_END_1411:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1415);
L_CLOS_CODE_1416: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1410);
printf("at JUMP_SIMPLE_1410");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1410:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1418));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1420:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1421);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1420);

L_FOR1_END_1421:
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

L_FOR2_START_1422:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1423);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1422);

L_FOR2_END_1423:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1419);
L_CLOS_CODE_1418: 
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
JUMP_EQ(L_JUMP_1417);
printf("At L_JUMP_1417\n");
fflush(stdout);
L_JUMP_1417:
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
L_CLOS_EXIT_1419:

POP(FP);
RETURN;
L_CLOS_EXIT_1415:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1409);
printf("At L_JUMP_1409\n");
fflush(stdout);
L_JUMP_1409:
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
L_CLOS_EXIT_1407:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1396));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1398:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1399);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1398);

L_FOR1_END_1399:
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

L_FOR2_START_1400:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1401);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1400);

L_FOR2_END_1401:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1397);
L_CLOS_CODE_1396: 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1378));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1376:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1375);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1376);

L_FOR1_END_1375:
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

L_FOR2_START_1374:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1373);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1374);

L_FOR2_END_1373:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1377);
L_CLOS_CODE_1378: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1372);
printf("at JUMP_SIMPLE_1372");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1372:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1385));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1383:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1382);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1383);

L_FOR1_END_1382:
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

L_FOR2_START_1381:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1380);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1381);

L_FOR2_END_1380:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1384);
L_CLOS_CODE_1385: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1379);
printf("at JUMP_SIMPLE_1379");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1379:

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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1395\n");
JUMP_EQ(L_JUMP_1395);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1395:
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
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1394\n");
JUMP_EQ(L_JUMP_1394);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1394:
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
JUMP_NE(L_OR_END_1386);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1386);
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
MOV(R0, IMM(1356));
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
printf("At L_JUMP_1393\n");
JUMP_EQ(L_JUMP_1393);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1393:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1392\n");
JUMP_EQ(L_JUMP_1392);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1392:
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
		    JUMP_NE(L_THEN_1389); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1388);
		    L_THEN_1389:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
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
printf("At L_JUMP_1391\n");
JUMP_EQ(L_JUMP_1391);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1391:
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
JUMP_EQ(L_JUMP_1390);
printf("At L_JUMP_1390\n");
fflush(stdout);
L_JUMP_1390:
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

		    JUMP(L_IF_EXIT_1387);
		    L_ELSE_1388:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1387:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1386);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1386);
L_OR_END_1386:

POP(FP);
RETURN;
L_CLOS_EXIT_1384:

POP(FP);
RETURN;
L_CLOS_EXIT_1377:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1370));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1368:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1367);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1368);

L_FOR1_END_1367:
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

L_FOR2_START_1366:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1365);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1366);

L_FOR2_END_1365:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1369);
L_CLOS_CODE_1370: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1364);
printf("at JUMP_SIMPLE_1364");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1364:

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
JUMP_EQ(L_JUMP_1371);
printf("At L_JUMP_1371\n");
fflush(stdout);
L_JUMP_1371:
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
L_CLOS_EXIT_1369:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1363);
printf("At L_JUMP_1363\n");
fflush(stdout);
L_JUMP_1363:
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
L_CLOS_EXIT_1397:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1337));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1335:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1334);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1335);

L_FOR1_END_1334:
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

L_FOR2_START_1333:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1332);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1333);

L_FOR2_END_1332:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1336);
L_CLOS_CODE_1337: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1331);
printf("at JUMP_SIMPLE_1331");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1331:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1344));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1342:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1341);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1342);

L_FOR1_END_1341:
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

L_FOR2_START_1340:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1339);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1340);

L_FOR2_END_1339:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1343);
L_CLOS_CODE_1344: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1338);
printf("at JUMP_SIMPLE_1338");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1338:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1362\n");
JUMP_EQ(L_JUMP_1362);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1362:
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
JUMP_NE(L_OR_END_1345);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1345);
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1359));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1357:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1356);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1357);

L_FOR1_END_1356:
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

L_FOR2_START_1355:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1354);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1355);

L_FOR2_END_1354:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1358);
L_CLOS_CODE_1359: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1353);
printf("at JUMP_SIMPLE_1353");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1353:

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
printf("At L_JUMP_1361\n");
JUMP_EQ(L_JUMP_1361);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1361:
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
JUMP_EQ(L_JUMP_1360);
printf("At L_JUMP_1360\n");
fflush(stdout);
L_JUMP_1360:
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
L_CLOS_EXIT_1358:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(171));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1352\n");
JUMP_EQ(L_JUMP_1352);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1352:
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
		    JUMP_NE(L_THEN_1348); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1347);
		    L_THEN_1348:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1351\n");
JUMP_EQ(L_JUMP_1351);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1351:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1350\n");
JUMP_EQ(L_JUMP_1350);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1350:
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
JUMP_EQ(L_JUMP_1349);
printf("At L_JUMP_1349\n");
fflush(stdout);
L_JUMP_1349:
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

		    JUMP(L_IF_EXIT_1346);
		    L_ELSE_1347:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1346:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1345);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1345);
L_OR_END_1345:

POP(FP);
RETURN;
L_CLOS_EXIT_1343:

POP(FP);
RETURN;
L_CLOS_EXIT_1336:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1321));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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

L_FOR2_START_1317:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1316);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1315);
printf("at JUMP_SIMPLE_1315");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1315:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1327));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1325:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1324);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1325);

L_FOR1_END_1324:
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

L_FOR2_START_1323:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1322);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1323);

L_FOR2_END_1322:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1326);
L_CLOS_CODE_1327: 

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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1330\n");
JUMP_EQ(L_JUMP_1330);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1330:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1329\n");
JUMP_EQ(L_JUMP_1329);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1329:
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
JUMP_EQ(L_JUMP_1328);
printf("At L_JUMP_1328\n");
fflush(stdout);
L_JUMP_1328:
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
L_CLOS_EXIT_1326:

POP(FP);
RETURN;
L_CLOS_EXIT_1320:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1314\n");
JUMP_EQ(L_JUMP_1314);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1314:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1282));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1280:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1279);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1280);

L_FOR1_END_1279:
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

L_FOR2_START_1278:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1277);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1278);

L_FOR2_END_1277:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1281);
L_CLOS_CODE_1282: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_1276);
printf("at JUMP_SIMPLE_1276");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1276:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1298));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1296:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1295);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1296);

L_FOR1_END_1295:
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

L_FOR2_START_1294:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1293);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1294);

L_FOR2_END_1293:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1297);
L_CLOS_CODE_1298: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1292);
printf("at JUMP_SIMPLE_1292");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1292:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1305));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1303:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1302);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1303);

L_FOR1_END_1302:
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

L_FOR2_START_1301:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1300);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1301);

L_FOR2_END_1300:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1304);
L_CLOS_CODE_1305: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1299);
printf("at JUMP_SIMPLE_1299");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1299:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1313\n");
JUMP_EQ(L_JUMP_1313);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1313:
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
		    JUMP_NE(L_THEN_1308); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1307);
		    L_THEN_1308:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1306);
		    L_ELSE_1307:
		    
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1312\n");
JUMP_EQ(L_JUMP_1312);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1312:
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
printf("At L_JUMP_1311\n");
JUMP_EQ(L_JUMP_1311);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1311:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1310\n");
JUMP_EQ(L_JUMP_1310);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1310:
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
JUMP_EQ(L_JUMP_1309);
printf("At L_JUMP_1309\n");
fflush(stdout);
L_JUMP_1309:
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

		    L_IF_EXIT_1306:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1304:

POP(FP);
RETURN;
L_CLOS_EXIT_1297:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1290));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1288:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1287);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1288);

L_FOR1_END_1287:
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

L_FOR2_START_1286:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1285);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1286);

L_FOR2_END_1285:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1289);
L_CLOS_CODE_1290: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1284);
printf("at JUMP_SIMPLE_1284");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1284:

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
JUMP_EQ(L_JUMP_1291);
printf("At L_JUMP_1291\n");
fflush(stdout);
L_JUMP_1291:
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
L_CLOS_EXIT_1289:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1283);
printf("At L_JUMP_1283\n");
fflush(stdout);
L_JUMP_1283:
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
L_CLOS_EXIT_1281:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1266));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1264:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1263);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1264);

L_FOR1_END_1263:
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

L_FOR2_START_1262:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1261);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1262);

L_FOR2_END_1261:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1265);
L_CLOS_CODE_1266: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1260);
printf("at JUMP_SIMPLE_1260");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1260:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1273));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1271:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1270);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1271);

L_FOR1_END_1270:
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

L_FOR2_START_1269:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1268);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1269);

L_FOR2_END_1268:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1272);
L_CLOS_CODE_1273: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1267);
printf("at JUMP_SIMPLE_1267");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1267:

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
printf("At L_JUMP_1275\n");
JUMP_EQ(L_JUMP_1275);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1275:
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
JUMP_EQ(L_JUMP_1274);
printf("At L_JUMP_1274\n");
fflush(stdout);
L_JUMP_1274:
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
L_CLOS_EXIT_1272:

POP(FP);
RETURN;
L_CLOS_EXIT_1265:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1245));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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

L_FOR2_START_1241:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1240);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1239);
printf("at JUMP_SIMPLE_1239");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1239:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1251));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1249:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1248);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1249);

L_FOR1_END_1248:
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

L_FOR2_START_1247:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1246);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1247);

L_FOR2_END_1246:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1250);
L_CLOS_CODE_1251: 

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1259));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1257:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1256);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1257);

L_FOR1_END_1256:
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

L_FOR2_START_1255:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1254);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1255);

L_FOR2_END_1254:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1258);
L_CLOS_CODE_1259: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1253);
printf("at JUMP_SIMPLE_1253");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1253:
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_1258:
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
JUMP_EQ(L_JUMP_1252);
printf("At L_JUMP_1252\n");
fflush(stdout);
L_JUMP_1252:
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
L_CLOS_EXIT_1250:

POP(FP);
RETURN;
L_CLOS_EXIT_1244:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1238\n");
JUMP_EQ(L_JUMP_1238);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1238:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1356));
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
printf("At L_JUMP_1237\n");
JUMP_EQ(L_JUMP_1237);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1237:
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1356));
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
printf("At L_JUMP_1236\n");
JUMP_EQ(L_JUMP_1236);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1236:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
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
printf("At L_JUMP_1235\n");
JUMP_EQ(L_JUMP_1235);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1235:
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
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
printf("At L_JUMP_1234\n");
JUMP_EQ(L_JUMP_1234);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1234:
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
MOV(R0, IMM(1356));
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
printf("At L_JUMP_1233\n");
JUMP_EQ(L_JUMP_1233);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1233:
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
MOV(R0, IMM(1356));
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
printf("At L_JUMP_1232\n");
JUMP_EQ(L_JUMP_1232);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1232:
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1356));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(R0, IMM(1325));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1172));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1170:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1169);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1170);

L_FOR1_END_1169:
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

L_FOR2_START_1168:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1167);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1168);

L_FOR2_END_1167:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1171);
L_CLOS_CODE_1172: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1166);
printf("at JUMP_SIMPLE_1166");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1166:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1194));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1192:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1191);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1192);

L_FOR1_END_1191:
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

L_FOR2_START_1190:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1189);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1190);

L_FOR2_END_1189:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1193);
L_CLOS_CODE_1194: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1188);
printf("at JUMP_SIMPLE_1188");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1188:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1201));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1199:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1198);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1199);

L_FOR1_END_1198:
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

L_FOR2_START_1197:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1196);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1197);

L_FOR2_END_1196:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1200);
L_CLOS_CODE_1201: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1195);
printf("at JUMP_SIMPLE_1195");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1195:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
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
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1204); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1203);
		    L_THEN_1204:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1202);
		    L_ELSE_1203:
		    
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
MOV(R0, IMM(1325));
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
printf("At L_JUMP_1207\n");
JUMP_EQ(L_JUMP_1207);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1207:
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
MOV(R0, IMM(1356));
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
JUMP_EQ(L_JUMP_1205);
printf("At L_JUMP_1205\n");
fflush(stdout);
L_JUMP_1205:
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

		    L_IF_EXIT_1202:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1200:

POP(FP);
RETURN;
L_CLOS_EXIT_1193:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1180));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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

L_FOR2_START_1176:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1175);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1174);
printf("at JUMP_SIMPLE_1174");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1174:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1186));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1184:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1183);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1184);

L_FOR1_END_1183:
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

L_FOR2_START_1182:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1181);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1182);

L_FOR2_END_1181:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1185);
L_CLOS_CODE_1186: 

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
JUMP_EQ(L_JUMP_1187);
printf("At L_JUMP_1187\n");
fflush(stdout);
L_JUMP_1187:
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
L_CLOS_EXIT_1185:

POP(FP);
RETURN;
L_CLOS_EXIT_1179:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1173);
printf("At L_JUMP_1173\n");
fflush(stdout);
L_JUMP_1173:
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
L_CLOS_EXIT_1171:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1122));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1120:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1119);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1120);

L_FOR1_END_1119:
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

L_FOR2_START_1118:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1117);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1118);

L_FOR2_END_1117:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1121);
L_CLOS_CODE_1122: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1116);
printf("at JUMP_SIMPLE_1116");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1116:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1150));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1148:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1147);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1148);

L_FOR1_END_1147:
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

L_FOR2_START_1146:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1145);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1146);

L_FOR2_END_1145:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1149);
L_CLOS_CODE_1150: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1144);
printf("at JUMP_SIMPLE_1144");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1144:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1157));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1155:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1154);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1155);

L_FOR1_END_1154:
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

L_FOR2_START_1153:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1152);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1153);

L_FOR2_END_1152:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1156);
L_CLOS_CODE_1157: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1151);
printf("at JUMP_SIMPLE_1151");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1151:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1165\n");
JUMP_EQ(L_JUMP_1165);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1165:
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
		    JUMP_NE(L_THEN_1160); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1159);
		    L_THEN_1160:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_1158);
		    L_ELSE_1159:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1164\n");
JUMP_EQ(L_JUMP_1164);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1164:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1163\n");
JUMP_EQ(L_JUMP_1163);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1163:
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
printf("At L_JUMP_1162\n");
JUMP_EQ(L_JUMP_1162);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1162:
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
JUMP_EQ(L_JUMP_1161);
printf("At L_JUMP_1161\n");
fflush(stdout);
L_JUMP_1161:
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

		    L_IF_EXIT_1158:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1156:

POP(FP);
RETURN;
L_CLOS_EXIT_1149:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1130));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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

L_FOR2_START_1126:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1125);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1124);
printf("at JUMP_SIMPLE_1124");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1124:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1136));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1134:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1133);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1134);

L_FOR1_END_1133:
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

L_FOR2_START_1132:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1131);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1132);

L_FOR2_END_1131:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1135);
L_CLOS_CODE_1136: 

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
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1143\n");
JUMP_EQ(L_JUMP_1143);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1143:
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
		    JUMP_NE(L_THEN_1139); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1138);
		    L_THEN_1139:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1137);
		    L_ELSE_1138:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1142\n");
JUMP_EQ(L_JUMP_1142);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1142:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1141\n");
JUMP_EQ(L_JUMP_1141);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1141:
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
JUMP_EQ(L_JUMP_1140);
printf("At L_JUMP_1140\n");
fflush(stdout);
L_JUMP_1140:
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

		    L_IF_EXIT_1137:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1135:

POP(FP);
RETURN;
L_CLOS_EXIT_1129:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1123);
printf("At L_JUMP_1123\n");
fflush(stdout);
L_JUMP_1123:
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
L_CLOS_EXIT_1121:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1105));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1103:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1102);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1103);

L_FOR1_END_1102:
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

L_FOR2_START_1101:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1100);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1101);

L_FOR2_END_1100:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1104);
L_CLOS_CODE_1105: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1099);
printf("at JUMP_SIMPLE_1099");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1099:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1112));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1110:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1109);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1110);

L_FOR1_END_1109:
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

L_FOR2_START_1108:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1107);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1108);

L_FOR2_END_1107:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1111);
L_CLOS_CODE_1112: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1106);
printf("at JUMP_SIMPLE_1106");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1106:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1115\n");
JUMP_EQ(L_JUMP_1115);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1115:
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
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1114\n");
JUMP_EQ(L_JUMP_1114);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1114:
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
JUMP_EQ(L_JUMP_1113);
printf("At L_JUMP_1113\n");
fflush(stdout);
L_JUMP_1113:
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
L_CLOS_EXIT_1111:

POP(FP);
RETURN;
L_CLOS_EXIT_1104:
MOV(R1, INDD(IMM(534), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE<

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1307));
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
printf("At L_JUMP_1098\n");
JUMP_EQ(L_JUMP_1098);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1098:
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
printf("At L_JUMP_1097\n");
JUMP_EQ(L_JUMP_1097);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1097:
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
printf("At L_JUMP_1096\n");
JUMP_EQ(L_JUMP_1096);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1096:
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
printf("At L_JUMP_1095\n");
JUMP_EQ(L_JUMP_1095);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1095:
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
MOV(R0, IMM(1415));
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
printf("At L_JUMP_1094\n");
JUMP_EQ(L_JUMP_1094);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1094:
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
printf("At L_JUMP_1093\n");
JUMP_EQ(L_JUMP_1093);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1093:
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
MOV(R1, INDD(IMM(577), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar<?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1307));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1079));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1077:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1076);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1077);

L_FOR1_END_1076:
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

L_FOR2_START_1075:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1074);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1075);

L_FOR2_END_1074:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1078);
L_CLOS_CODE_1079: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1073);
printf("at JUMP_SIMPLE_1073");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1073:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
MOV(R0,IMM(1417));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(577));
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
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1082); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1081);
		    L_THEN_1082:
 //TC-APPLIC 

MOV(R0,IMM(1419));
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
JUMP_EQ(L_JUMP_1083);
printf("At L_JUMP_1083\n");
fflush(stdout);
L_JUMP_1083:
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

		    JUMP(L_IF_EXIT_1080);
		    L_ELSE_1081:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1080:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1078:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1067));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1065:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1064);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1065);

L_FOR1_END_1064:
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

L_FOR2_START_1063:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1062);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1063);

L_FOR2_END_1062:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1066);
L_CLOS_CODE_1067: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1061);
printf("at JUMP_SIMPLE_1061");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1061:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
MOV(R0,IMM(1421));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1072\n");
JUMP_EQ(L_JUMP_1072);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1072:
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
		    JUMP_NE(L_THEN_1070); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1069);
		    L_THEN_1070:
 //TC-APPLIC 

MOV(R0,IMM(1423));
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
JUMP_EQ(L_JUMP_1071);
printf("At L_JUMP_1071\n");
fflush(stdout);
L_JUMP_1071:
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

		    JUMP(L_IF_EXIT_1068);
		    L_ELSE_1069:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1068:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1066:
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
MOV(R0,IMM(1417));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1060\n");
JUMP_EQ(L_JUMP_1060);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1060:
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
MOV(R0,IMM(1421));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1059\n");
JUMP_EQ(L_JUMP_1059);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1059:
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
printf("At L_JUMP_1058\n");
JUMP_EQ(L_JUMP_1058);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1058:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1043));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1041:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1040);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1041);

L_FOR1_END_1040:
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

L_FOR2_START_1039:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1038);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1039);

L_FOR2_END_1038:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1042);
L_CLOS_CODE_1043: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1037);
printf("at JUMP_SIMPLE_1037");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1037:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1050));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1048:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1047);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1048);

L_FOR1_END_1047:
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

L_FOR2_START_1046:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1045);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1046);

L_FOR2_END_1045:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1049);
L_CLOS_CODE_1050: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1044);
printf("at JUMP_SIMPLE_1044");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1044:
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
printf("At L_JUMP_1057\n");
JUMP_EQ(L_JUMP_1057);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1057:
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
		    JUMP_NE(L_THEN_1053); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1052);
		    L_THEN_1053:
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
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1056\n");
JUMP_EQ(L_JUMP_1056);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1056:
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
printf("At L_JUMP_1055\n");
JUMP_EQ(L_JUMP_1055);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1055:
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
MOV(R0, IMM(1440));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1054);
printf("At L_JUMP_1054\n");
fflush(stdout);
L_JUMP_1054:
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

		    JUMP(L_IF_EXIT_1051);
		    L_ELSE_1052:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_1051:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1049:

POP(FP);
RETURN;
L_CLOS_EXIT_1042:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1036\n");
JUMP_EQ(L_JUMP_1036);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1036:
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
MOV(R0,IMM(1417));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1035\n");
JUMP_EQ(L_JUMP_1035);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1035:
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
MOV(R0,IMM(1421));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1034\n");
JUMP_EQ(L_JUMP_1034);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1034:
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
printf("At L_JUMP_1033\n");
JUMP_EQ(L_JUMP_1033);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1033:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1018));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1016:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1015);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1016);

L_FOR1_END_1015:
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

L_FOR2_START_1014:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1013);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1014);

L_FOR2_END_1013:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1017);
L_CLOS_CODE_1018: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1012);
printf("at JUMP_SIMPLE_1012");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1012:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1025));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1023:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1022);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1023);

L_FOR1_END_1022:
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

L_FOR2_START_1021:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1020);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1021);

L_FOR2_END_1020:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1024);
L_CLOS_CODE_1025: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1019);
printf("at JUMP_SIMPLE_1019");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1019:
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
printf("At L_JUMP_1032\n");
JUMP_EQ(L_JUMP_1032);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1032:
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
		    JUMP_NE(L_THEN_1028); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1027);
		    L_THEN_1028:
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
MOV(R0, IMM(1406));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1031\n");
JUMP_EQ(L_JUMP_1031);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1031:
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
printf("At L_JUMP_1030\n");
JUMP_EQ(L_JUMP_1030);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1030:
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
MOV(R0, IMM(1440));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1029);
printf("At L_JUMP_1029\n");
fflush(stdout);
L_JUMP_1029:
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

		    JUMP(L_IF_EXIT_1026);
		    L_ELSE_1027:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_1026:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1024:

POP(FP);
RETURN;
L_CLOS_EXIT_1017:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1011\n");
JUMP_EQ(L_JUMP_1011);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1011:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1007));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1005:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1004);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1005);

L_FOR1_END_1004:
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

L_FOR2_START_1003:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1002);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1003);

L_FOR2_END_1002:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1006);
L_CLOS_CODE_1007: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1001);
printf("at JUMP_SIMPLE_1001");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1001:

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
printf("At L_JUMP_1010\n");
JUMP_EQ(L_JUMP_1010);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1010:
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
printf("At L_JUMP_1009\n");
JUMP_EQ(L_JUMP_1009);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1009:
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
JUMP_EQ(L_JUMP_1008);
printf("At L_JUMP_1008\n");
fflush(stdout);
L_JUMP_1008:
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
L_CLOS_EXIT_1006:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1000\n");
JUMP_EQ(L_JUMP_1000);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1000:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_996));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_994:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_993);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_994);

L_FOR1_END_993:
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

L_FOR2_START_992:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_991);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_992);

L_FOR2_END_991:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_995);
L_CLOS_CODE_996: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_990);
printf("at JUMP_SIMPLE_990");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_990:

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
printf("At L_JUMP_999\n");
JUMP_EQ(L_JUMP_999);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_999:
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
printf("At L_JUMP_998\n");
JUMP_EQ(L_JUMP_998);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_998:
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
JUMP_EQ(L_JUMP_997);
printf("At L_JUMP_997\n");
fflush(stdout);
L_JUMP_997:
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
L_CLOS_EXIT_995:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_989\n");
JUMP_EQ(L_JUMP_989);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_989:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_985));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_983:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_982);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_983);

L_FOR1_END_982:
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

L_FOR2_START_981:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_980);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_981);

L_FOR2_END_980:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_984);
L_CLOS_CODE_985: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_979);
printf("at JUMP_SIMPLE_979");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_979:

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
printf("At L_JUMP_988\n");
JUMP_EQ(L_JUMP_988);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_988:
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
printf("At L_JUMP_987\n");
JUMP_EQ(L_JUMP_987);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_987:
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
JUMP_EQ(L_JUMP_986);
printf("At L_JUMP_986\n");
fflush(stdout);
L_JUMP_986:
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
L_CLOS_EXIT_984:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_978\n");
JUMP_EQ(L_JUMP_978);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_978:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_974));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_972:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_971);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_972);

L_FOR1_END_971:
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

L_FOR2_START_970:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_969);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_970);

L_FOR2_END_969:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_973);
L_CLOS_CODE_974: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_968);
printf("at JUMP_SIMPLE_968");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_968:

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
printf("At L_JUMP_977\n");
JUMP_EQ(L_JUMP_977);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_977:
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
printf("At L_JUMP_976\n");
JUMP_EQ(L_JUMP_976);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_976:
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
JUMP_EQ(L_JUMP_975);
printf("At L_JUMP_975\n");
fflush(stdout);
L_JUMP_975:
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
L_CLOS_EXIT_973:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_967\n");
JUMP_EQ(L_JUMP_967);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_967:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_963));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_961:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_960);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_961);

L_FOR1_END_960:
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

L_FOR2_START_959:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_958);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_959);

L_FOR2_END_958:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_962);
L_CLOS_CODE_963: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_957);
printf("at JUMP_SIMPLE_957");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_957:

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
printf("At L_JUMP_966\n");
JUMP_EQ(L_JUMP_966);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_966:
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
printf("At L_JUMP_965\n");
JUMP_EQ(L_JUMP_965);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_965:
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
JUMP_EQ(L_JUMP_964);
printf("At L_JUMP_964\n");
fflush(stdout);
L_JUMP_964:
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
L_CLOS_EXIT_962:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_956\n");
JUMP_EQ(L_JUMP_956);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_956:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_940));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_938:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_937);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_938);

L_FOR1_END_937:
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

L_FOR2_START_936:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_935);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_936);

L_FOR2_END_935:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_939);
L_CLOS_CODE_940: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_934);
printf("at JUMP_SIMPLE_934");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_934:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_947));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_945:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_944);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_945);

L_FOR1_END_944:
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

L_FOR2_START_943:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_942);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_943);

L_FOR2_END_942:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_946);
L_CLOS_CODE_947: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_941);
printf("at JUMP_SIMPLE_941");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_941:
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1459));
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
printf("At L_JUMP_955\n");
JUMP_EQ(L_JUMP_955);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_955:
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
		    JUMP_NE(L_THEN_950); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_949);
		    L_THEN_950:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_948);
		    L_ELSE_949:
		    
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
MOV(R0, IMM(1473));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_954\n");
JUMP_EQ(L_JUMP_954);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_954:
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
MOV(R0, IMM(1364));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_953\n");
JUMP_EQ(L_JUMP_953);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_953:
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
MOV(R0,IMM(1290));
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
printf("At L_JUMP_952\n");
JUMP_EQ(L_JUMP_952);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_952:
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
JUMP_EQ(L_JUMP_951);
printf("At L_JUMP_951\n");
fflush(stdout);
L_JUMP_951:
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

		    L_IF_EXIT_948:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_946:

POP(FP);
RETURN;
L_CLOS_EXIT_939:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_923));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_921:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_920);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_921);

L_FOR1_END_920:
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

L_FOR2_START_919:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_918);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_919);

L_FOR2_END_918:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_922);
L_CLOS_CODE_923: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_917);
printf("at JUMP_SIMPLE_917");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_917:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_930));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_928:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_927);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_928);

L_FOR1_END_927:
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

L_FOR2_START_926:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_925);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_926);

L_FOR2_END_925:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_929);
L_CLOS_CODE_930: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_924);
printf("at JUMP_SIMPLE_924");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_924:

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1290));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1457));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_933\n");
JUMP_EQ(L_JUMP_933);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_933:
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
printf("At L_JUMP_932\n");
JUMP_EQ(L_JUMP_932);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_932:
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
JUMP_EQ(L_JUMP_931);
printf("At L_JUMP_931\n");
fflush(stdout);
L_JUMP_931:
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
L_CLOS_EXIT_929:

POP(FP);
RETURN;
L_CLOS_EXIT_922:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_916\n");
JUMP_EQ(L_JUMP_916);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_916:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_865));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_863:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_862);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_863);

L_FOR1_END_862:
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

L_FOR2_START_861:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_860);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_861);

L_FOR2_END_860:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_864);
L_CLOS_CODE_865: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_859);
printf("at JUMP_SIMPLE_859");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_859:

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
printf("At L_JUMP_915\n");
JUMP_EQ(L_JUMP_915);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_915:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_873));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_871:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_870);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_871);

L_FOR1_END_870:
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

L_FOR2_START_869:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_868);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_869);

L_FOR2_END_868:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_872);
L_CLOS_CODE_873: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_867);
printf("at JUMP_SIMPLE_867");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_867:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1503));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_914\n");
JUMP_EQ(L_JUMP_914);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_914:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_881));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_879:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_878);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_879);

L_FOR1_END_878:
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

L_FOR2_START_877:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_876);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_877);

L_FOR2_END_876:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_880);
L_CLOS_CODE_881: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_875);
printf("at JUMP_SIMPLE_875");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_875:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_897));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_895:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_894);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_895);

L_FOR1_END_894:
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

L_FOR2_START_893:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_892);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_893);

L_FOR2_END_892:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_896);
L_CLOS_CODE_897: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_891);
printf("at JUMP_SIMPLE_891");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_891:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_904));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_902:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_901);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_902);

L_FOR1_END_901:
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

L_FOR2_START_900:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_899);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_900);

L_FOR2_END_899:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_903);
L_CLOS_CODE_904: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_898);
printf("at JUMP_SIMPLE_898");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_898:
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
printf("At L_JUMP_913\n");
JUMP_EQ(L_JUMP_913);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_913:
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
		    JUMP_NE(L_THEN_907); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_906);
		    L_THEN_907:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_905);
		    L_ELSE_906:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_912\n");
JUMP_EQ(L_JUMP_912);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_912:
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
MOV(R0, IMM(1488));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_911\n");
JUMP_EQ(L_JUMP_911);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_911:
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
MOV(R0,IMM(1290));
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
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_908);
printf("At L_JUMP_908\n");
fflush(stdout);
L_JUMP_908:
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

		    L_IF_EXIT_905:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_903:

POP(FP);
RETURN;
L_CLOS_EXIT_896:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_889));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_887:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_886);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_887);

L_FOR1_END_886:
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

L_FOR2_START_885:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_884);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_885);

L_FOR2_END_884:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_888);
L_CLOS_CODE_889: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_883);
printf("at JUMP_SIMPLE_883");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_883:

 //TC-APPLIC 

MOV(R0,IMM(1309));
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
JUMP_EQ(L_JUMP_890);
printf("At L_JUMP_890\n");
fflush(stdout);
L_JUMP_890:
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
L_CLOS_EXIT_888:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_882);
printf("At L_JUMP_882\n");
fflush(stdout);
L_JUMP_882:
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
L_CLOS_EXIT_880:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_874);
printf("At L_JUMP_874\n");
fflush(stdout);
L_JUMP_874:
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
L_CLOS_EXIT_872:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_866);
printf("At L_JUMP_866\n");
fflush(stdout);
L_JUMP_866:
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
L_CLOS_EXIT_864:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_855));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_853:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_852);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_853);

L_FOR1_END_852:
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

L_FOR2_START_851:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_850);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_851);

L_FOR2_END_850:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_854);
L_CLOS_CODE_855: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_849);
printf("at JUMP_SIMPLE_849");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_849:

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
printf("At L_JUMP_858\n");
JUMP_EQ(L_JUMP_858);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_858:
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
printf("At L_JUMP_857\n");
JUMP_EQ(L_JUMP_857);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_857:
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
JUMP_EQ(L_JUMP_856);
printf("At L_JUMP_856\n");
fflush(stdout);
L_JUMP_856:
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
L_CLOS_EXIT_854:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_845));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_843:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_842);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_843);

L_FOR1_END_842:
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

L_FOR2_START_841:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_840);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_841);

L_FOR2_END_840:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_844);
L_CLOS_CODE_845: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_839);
printf("at JUMP_SIMPLE_839");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_839:

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
printf("At L_JUMP_848\n");
JUMP_EQ(L_JUMP_848);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_848:
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
printf("At L_JUMP_847\n");
JUMP_EQ(L_JUMP_847);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_847:
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
JUMP_EQ(L_JUMP_846);
printf("At L_JUMP_846\n");
fflush(stdout);
L_JUMP_846:
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
L_CLOS_EXIT_844:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_830));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_828:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_827);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_828);

L_FOR1_END_827:
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

L_FOR2_START_826:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_825);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_826);

L_FOR2_END_825:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_829);
L_CLOS_CODE_830: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_824);
printf("at JUMP_SIMPLE_824");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_824:
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
printf("At L_JUMP_838\n");
JUMP_EQ(L_JUMP_838);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_838:
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
		    JUMP_NE(L_THEN_833); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_832);
		    L_THEN_833:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_837);
printf("At L_JUMP_837\n");
fflush(stdout);
L_JUMP_837:
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

		    JUMP(L_IF_EXIT_831);
		    L_ELSE_832:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1290));
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
printf("At L_JUMP_836\n");
JUMP_EQ(L_JUMP_836);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_836:
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_835\n");
JUMP_EQ(L_JUMP_835);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_835:
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
JUMP_EQ(L_JUMP_834);
printf("At L_JUMP_834\n");
fflush(stdout);
L_JUMP_834:
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

		    L_IF_EXIT_831:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_829:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_815));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_813:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_812);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_813);

L_FOR1_END_812:
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

L_FOR2_START_811:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_810);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_811);

L_FOR2_END_810:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_814);
L_CLOS_CODE_815: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_809);
printf("at JUMP_SIMPLE_809");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_809:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_823\n");
JUMP_EQ(L_JUMP_823);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_823:
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
JUMP_NE(L_OR_END_816);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_816);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1334));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_822\n");
JUMP_EQ(L_JUMP_822);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_822:
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
		    JUMP_NE(L_THEN_819); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_818);
		    L_THEN_819:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_821\n");
JUMP_EQ(L_JUMP_821);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_821:
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
JUMP_EQ(L_JUMP_820);
printf("At L_JUMP_820\n");
fflush(stdout);
L_JUMP_820:
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

		    JUMP(L_IF_EXIT_817);
		    L_ELSE_818:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_817:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_816);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_816);
L_OR_END_816:

POP(FP);
RETURN;
L_CLOS_EXIT_814:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_803));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_805:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_806);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_805);

L_FOR1_END_806:
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

L_FOR2_START_807:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_808);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_807);

L_FOR2_END_808:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_804);
L_CLOS_CODE_803: 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_785));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_783:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_782);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_783);

L_FOR1_END_782:
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

L_FOR2_START_781:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_780);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_781);

L_FOR2_END_780:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_784);
L_CLOS_CODE_785: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_779);
printf("at JUMP_SIMPLE_779");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_779:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_792));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_790:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_789);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_790);

L_FOR1_END_789:
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

L_FOR2_START_788:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_787);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_788);

L_FOR2_END_787:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_791);
L_CLOS_CODE_792: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_786);
printf("at JUMP_SIMPLE_786");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_786:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_802\n");
JUMP_EQ(L_JUMP_802);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_802:
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
MOV(R0, IMM(1334));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_801\n");
JUMP_EQ(L_JUMP_801);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_801:
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
		    JUMP_NE(L_THEN_795); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_794);
		    L_THEN_795:
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
MOV(R0, IMM(1356));
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
printf("At L_JUMP_800\n");
JUMP_EQ(L_JUMP_800);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_800:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_799\n");
JUMP_EQ(L_JUMP_799);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_799:
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
JUMP_NE(L_OR_END_796);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_796);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
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
printf("At L_JUMP_798\n");
JUMP_EQ(L_JUMP_798);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_798:
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
JUMP_EQ(L_JUMP_797);
printf("At L_JUMP_797\n");
fflush(stdout);
L_JUMP_797:
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
JUMP_NE(L_OR_END_796);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_796);
L_OR_END_796:

		    JUMP(L_IF_EXIT_793);
		    L_ELSE_794:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_793:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_791:

POP(FP);
RETURN;
L_CLOS_EXIT_784:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_777));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_775:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_774);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_775);

L_FOR1_END_774:
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

L_FOR2_START_773:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_772);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_773);

L_FOR2_END_772:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_776);
L_CLOS_CODE_777: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_771);
printf("at JUMP_SIMPLE_771");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_771:

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
JUMP_EQ(L_JUMP_778);
printf("At L_JUMP_778\n");
fflush(stdout);
L_JUMP_778:
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
L_CLOS_EXIT_776:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_770);
printf("At L_JUMP_770\n");
fflush(stdout);
L_JUMP_770:
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
L_CLOS_EXIT_804:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_760));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_758:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_757);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_758);

L_FOR1_END_757:
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

L_FOR2_START_756:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_755);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_756);

L_FOR2_END_755:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_759);
L_CLOS_CODE_760: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_754);
printf("at JUMP_SIMPLE_754");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_754:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_768));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_766:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_765);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_766);

L_FOR1_END_765:
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

L_FOR2_START_764:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_763);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_764);

L_FOR2_END_763:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_767);
L_CLOS_CODE_768: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_762);
printf("at JUMP_SIMPLE_762");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_762:

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
MOV(R0, IMM(1510));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_769);
printf("At L_JUMP_769\n");
fflush(stdout);
L_JUMP_769:
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
L_CLOS_EXIT_767:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(834));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_761);
printf("At L_JUMP_761\n");
fflush(stdout);
L_JUMP_761:
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
L_CLOS_EXIT_759:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_752));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_750:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_749);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_750);

L_FOR1_END_749:
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

L_FOR2_START_748:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_747);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_748);

L_FOR2_END_747:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_751);
L_CLOS_CODE_752: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_746);
printf("at JUMP_SIMPLE_746");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_746:

 //TC-APPLIC 

MOV(R0,IMM(1309));
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
JUMP_EQ(L_JUMP_753);
printf("At L_JUMP_753\n");
fflush(stdout);
L_JUMP_753:
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
L_CLOS_EXIT_751:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_744));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_742:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_741);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_742);

L_FOR1_END_741:
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

L_FOR2_START_740:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_739);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_740);

L_FOR2_END_739:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_743);
L_CLOS_CODE_744: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_738);
printf("at JUMP_SIMPLE_738");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_738:

 //TC-APPLIC 

MOV(R0,IMM(1309));
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
JUMP_EQ(L_JUMP_745);
printf("At L_JUMP_745\n");
fflush(stdout);
L_JUMP_745:
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
L_CLOS_EXIT_743:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_687));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_685:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_684);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_685);

L_FOR1_END_684:
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

L_FOR2_START_683:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_682);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_683);

L_FOR2_END_682:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_686);
L_CLOS_CODE_687: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_681);
printf("at JUMP_SIMPLE_681");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_681:

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
printf("At L_JUMP_737\n");
JUMP_EQ(L_JUMP_737);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_737:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_695));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_693:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_692);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_693);

L_FOR1_END_692:
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

L_FOR2_START_691:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_690);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_691);

L_FOR2_END_690:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_694);
L_CLOS_CODE_695: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_689);
printf("at JUMP_SIMPLE_689");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_689:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1540));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_736\n");
JUMP_EQ(L_JUMP_736);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_736:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_703));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_701:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_700);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_701);

L_FOR1_END_700:
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

L_FOR2_START_699:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_698);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_699);

L_FOR2_END_698:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_702);
L_CLOS_CODE_703: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_697);
printf("at JUMP_SIMPLE_697");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_697:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_719));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_717:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_716);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_717);

L_FOR1_END_716:
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

L_FOR2_START_715:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_714);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_715);

L_FOR2_END_714:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_718);
L_CLOS_CODE_719: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_713);
printf("at JUMP_SIMPLE_713");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_713:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_726));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_724:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_723);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_724);

L_FOR1_END_723:
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

L_FOR2_START_722:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_721);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_722);

L_FOR2_END_721:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_725);
L_CLOS_CODE_726: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_720);
printf("at JUMP_SIMPLE_720");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_720:
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
printf("At L_JUMP_735\n");
JUMP_EQ(L_JUMP_735);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_735:
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
		    JUMP_NE(L_THEN_729); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_728);
		    L_THEN_729:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_727);
		    L_ELSE_728:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_734\n");
JUMP_EQ(L_JUMP_734);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_734:
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
MOV(R0, IMM(1525));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_733\n");
JUMP_EQ(L_JUMP_733);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_733:
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
MOV(R0,IMM(1290));
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
printf("At L_JUMP_732\n");
JUMP_EQ(L_JUMP_732);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_732:
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
MOV(R0, IMM(1325));
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_730);
printf("At L_JUMP_730\n");
fflush(stdout);
L_JUMP_730:
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

		    L_IF_EXIT_727:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_725:

POP(FP);
RETURN;
L_CLOS_EXIT_718:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_711));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_709:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_708);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_709);

L_FOR1_END_708:
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

L_FOR2_START_707:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_706);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_707);

L_FOR2_END_706:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_710);
L_CLOS_CODE_711: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_705);
printf("at JUMP_SIMPLE_705");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_705:

 //TC-APPLIC 

MOV(R0,IMM(1309));
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
JUMP_EQ(L_JUMP_712);
printf("At L_JUMP_712\n");
fflush(stdout);
L_JUMP_712:
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
L_CLOS_EXIT_710:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_704);
printf("At L_JUMP_704\n");
fflush(stdout);
L_JUMP_704:
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
L_CLOS_EXIT_702:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_696);
printf("At L_JUMP_696\n");
fflush(stdout);
L_JUMP_696:
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
L_CLOS_EXIT_694:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_688);
printf("At L_JUMP_688\n");
fflush(stdout);
L_JUMP_688:
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
L_CLOS_EXIT_686:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_679));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_677:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_676);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_677);

L_FOR1_END_676:
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

L_FOR2_START_675:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_674);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_675);

L_FOR2_END_674:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_678);
L_CLOS_CODE_679: 

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
JUMP_EQ(L_JUMP_680);
printf("At L_JUMP_680\n");
fflush(stdout);
L_JUMP_680:
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
L_CLOS_EXIT_678:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_648));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_646:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_645);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_646);

L_FOR1_END_645:
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

L_FOR2_START_644:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_643);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_644);

L_FOR2_END_643:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_647);
L_CLOS_CODE_648: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_642);
printf("at JUMP_SIMPLE_642");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_642:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1457));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_673\n");
JUMP_EQ(L_JUMP_673);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_673:
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
MOV(R0, IMM(1457));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_672\n");
JUMP_EQ(L_JUMP_672);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_672:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_656));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_654:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_653);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_654);

L_FOR1_END_653:
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

L_FOR2_START_652:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_651);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_652);

L_FOR2_END_651:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_655);
L_CLOS_CODE_656: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_650);
printf("at JUMP_SIMPLE_650");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_650:
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
printf("At L_JUMP_671\n");
JUMP_EQ(L_JUMP_671);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_671:
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
		    JUMP_NE(L_THEN_659); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_658);
		    L_THEN_659:
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
printf("At L_JUMP_670\n");
JUMP_EQ(L_JUMP_670);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_670:
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
printf("At L_JUMP_669\n");
JUMP_EQ(L_JUMP_669);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_669:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_667));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_665:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_664);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_665);

L_FOR1_END_664:
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

L_FOR2_START_663:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_662);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_663);

L_FOR2_END_662:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_666);
L_CLOS_CODE_667: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_661);
printf("at JUMP_SIMPLE_661");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_661:

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
JUMP_EQ(L_JUMP_668);
printf("At L_JUMP_668\n");
fflush(stdout);
L_JUMP_668:
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
L_CLOS_EXIT_666:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_660);
printf("At L_JUMP_660\n");
fflush(stdout);
L_JUMP_660:
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

		    JUMP(L_IF_EXIT_657);
		    L_ELSE_658:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_657:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_655:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_649);
printf("At L_JUMP_649\n");
fflush(stdout);
L_JUMP_649:
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
L_CLOS_EXIT_647:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_595));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_593:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_592);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_593);

L_FOR1_END_592:
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

L_FOR2_START_591:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_590);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_591);

L_FOR2_END_590:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_594);
L_CLOS_CODE_595: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_589);
printf("at JUMP_SIMPLE_589");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_589:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_613));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_611:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_610);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_611);

L_FOR1_END_610:
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

L_FOR2_START_609:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_608);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_609);

L_FOR2_END_608:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_612);
L_CLOS_CODE_613: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_607);
printf("at JUMP_SIMPLE_607");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_607:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_620));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_618:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_617);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_618);

L_FOR1_END_617:
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

L_FOR2_START_616:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_615);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_616);

L_FOR2_END_615:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_619);
L_CLOS_CODE_620: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_614);
printf("at JUMP_SIMPLE_614");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_614:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_641\n");
JUMP_EQ(L_JUMP_641);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_641:
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
		    JUMP_NE(L_THEN_623); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_622);
		    L_THEN_623:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1334));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_640);
printf("At L_JUMP_640\n");
fflush(stdout);
L_JUMP_640:
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

		    JUMP(L_IF_EXIT_621);
		    L_ELSE_622:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_639\n");
JUMP_EQ(L_JUMP_639);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_639:
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
		    JUMP_NE(L_THEN_626); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_625);
		    L_THEN_626:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_624);
		    L_ELSE_625:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_638\n");
JUMP_EQ(L_JUMP_638);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_638:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_637\n");
JUMP_EQ(L_JUMP_637);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_637:
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
printf("At L_JUMP_636\n");
JUMP_EQ(L_JUMP_636);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_636:
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
		    JUMP_NE(L_THEN_629); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_628);
		    L_THEN_629:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
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
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_634\n");
JUMP_EQ(L_JUMP_634);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_634:
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
JUMP_EQ(L_JUMP_633);
printf("At L_JUMP_633\n");
fflush(stdout);
L_JUMP_633:
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

		    JUMP(L_IF_EXIT_627);
		    L_ELSE_628:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1356));
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
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1356));
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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(587));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_630);
printf("At L_JUMP_630\n");
fflush(stdout);
L_JUMP_630:
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

		    L_IF_EXIT_627:
		    
		    L_IF_EXIT_624:
		    
		    L_IF_EXIT_621:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_619:

POP(FP);
RETURN;
L_CLOS_EXIT_612:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_603));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_601:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_600);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_601);

L_FOR1_END_600:
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

L_FOR2_START_599:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_598);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_599);

L_FOR2_END_598:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_602);
L_CLOS_CODE_603: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_597);
printf("at JUMP_SIMPLE_597");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_597:

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
printf("At L_JUMP_606\n");
JUMP_EQ(L_JUMP_606);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_606:
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
printf("At L_JUMP_605\n");
JUMP_EQ(L_JUMP_605);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_605:
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
JUMP_EQ(L_JUMP_604);
printf("At L_JUMP_604\n");
fflush(stdout);
L_JUMP_604:
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
L_CLOS_EXIT_602:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_596);
printf("At L_JUMP_596\n");
fflush(stdout);
L_JUMP_596:
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
L_CLOS_EXIT_594:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_587));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_585:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_584);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_585);

L_FOR1_END_584:
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

L_FOR2_START_583:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_582);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_583);

L_FOR2_END_582:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_586);
L_CLOS_CODE_587: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_581);
printf("at JUMP_SIMPLE_581");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_581:

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
JUMP_EQ(L_JUMP_588);
printf("At L_JUMP_588\n");
fflush(stdout);
L_JUMP_588:
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
L_CLOS_EXIT_586:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_578));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_576:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_575);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_576);

L_FOR1_END_575:
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

L_FOR2_START_574:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_573);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_574);

L_FOR2_END_573:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_577);
L_CLOS_CODE_578: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_572);
printf("at JUMP_SIMPLE_572");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_572:

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
printf("At L_JUMP_580\n");
JUMP_EQ(L_JUMP_580);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_580:
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
JUMP_EQ(L_JUMP_579);
printf("At L_JUMP_579\n");
fflush(stdout);
L_JUMP_579:
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
L_CLOS_EXIT_577:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_569));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_567:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_566);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_567);

L_FOR1_END_566:
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

L_FOR2_START_565:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_564);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_565);

L_FOR2_END_564:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_568);
L_CLOS_CODE_569: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_563);
printf("at JUMP_SIMPLE_563");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_563:

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
printf("At L_JUMP_571\n");
JUMP_EQ(L_JUMP_571);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_571:
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
JUMP_EQ(L_JUMP_570);
printf("At L_JUMP_570\n");
fflush(stdout);
L_JUMP_570:
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
L_CLOS_EXIT_568:
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
printf("At L_JUMP_562\n");
JUMP_EQ(L_JUMP_562);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_562:
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
printf("At L_JUMP_561\n");
JUMP_EQ(L_JUMP_561);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_561:
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
printf("At L_JUMP_560\n");
JUMP_EQ(L_JUMP_560);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_560:
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
printf("At L_JUMP_559\n");
JUMP_EQ(L_JUMP_559);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_559:
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
printf("At L_JUMP_558\n");
JUMP_EQ(L_JUMP_558);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_558:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_542));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_540:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_539);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_540);

L_FOR1_END_539:
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

L_FOR2_START_538:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_537);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_538);

L_FOR2_END_537:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_541);
L_CLOS_CODE_542: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_536);
printf("at JUMP_SIMPLE_536");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_536:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_549));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_547:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_546);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_547);

L_FOR1_END_546:
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

L_FOR2_START_545:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_544);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_545);

L_FOR2_END_544:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_548);
L_CLOS_CODE_549: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_543);
printf("at JUMP_SIMPLE_543");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_543:
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1459));
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
printf("At L_JUMP_557\n");
JUMP_EQ(L_JUMP_557);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_557:
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
		    JUMP_NE(L_THEN_552); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_551);
		    L_THEN_552:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_550);
		    L_ELSE_551:
		    
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
MOV(R0, IMM(1571));
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
MOV(R0, IMM(1364));
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
MOV(R0,IMM(1290));
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
printf("At L_JUMP_554\n");
JUMP_EQ(L_JUMP_554);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_554:
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
JUMP_EQ(L_JUMP_553);
printf("At L_JUMP_553\n");
fflush(stdout);
L_JUMP_553:
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

		    L_IF_EXIT_550:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_548:

POP(FP);
RETURN;
L_CLOS_EXIT_541:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_525));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_523:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_522);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_523);

L_FOR1_END_522:
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

L_FOR2_START_521:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_520);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_521);

L_FOR2_END_520:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_524);
L_CLOS_CODE_525: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_519);
printf("at JUMP_SIMPLE_519");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_519:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_532));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_530:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_529);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_530);

L_FOR1_END_529:
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

L_FOR2_START_528:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_527);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_528);

L_FOR2_END_527:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_531);
L_CLOS_CODE_532: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_526);
printf("at JUMP_SIMPLE_526");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_526:

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1290));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_535\n");
JUMP_EQ(L_JUMP_535);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_535:
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
printf("At L_JUMP_534\n");
JUMP_EQ(L_JUMP_534);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_534:
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
JUMP_EQ(L_JUMP_533);
printf("At L_JUMP_533\n");
fflush(stdout);
L_JUMP_533:
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
L_CLOS_EXIT_531:

POP(FP);
RETURN;
L_CLOS_EXIT_524:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_518\n");
JUMP_EQ(L_JUMP_518);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_518:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_505));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_503:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_502);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_503);

L_FOR1_END_502:
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

L_FOR2_START_501:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_500);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_501);

L_FOR2_END_500:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_504);
L_CLOS_CODE_505: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_499);
printf("at JUMP_SIMPLE_499");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_499:
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
MOV(R0, IMM(1583));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_517\n");
JUMP_EQ(L_JUMP_517);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_517:
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
		    JUMP_NE(L_THEN_515); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_514);
		    L_THEN_515:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1583));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_516\n");
JUMP_EQ(L_JUMP_516);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_516:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_513);
		    L_ELSE_514:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_513:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_508); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_507);
		    L_THEN_508://IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_511); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_510);
		    L_THEN_511:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_509);
		    L_ELSE_510:
		    
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
JUMP_EQ(L_JUMP_512);
printf("At L_JUMP_512\n");
fflush(stdout);
L_JUMP_512:
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

		    L_IF_EXIT_509:
		    
		    JUMP(L_IF_EXIT_506);
		    L_ELSE_507:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_506:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_504:
MOV(R1, INDD(IMM(1085), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEequal?

 //APPLIC 

PUSH(IMM(0));
//IF 
MOV(R0,IMM(12));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_498); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_497);
		    L_THEN_498:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_496);
		    L_ELSE_497:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_496:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_384));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_382:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_381);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_382);

L_FOR1_END_381:
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

L_FOR2_START_380:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_379);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_380);

L_FOR2_END_379:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_383);
L_CLOS_CODE_384: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_378);
printf("at JUMP_SIMPLE_378");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_378:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_399));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_397:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_396);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_397);

L_FOR1_END_396:
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

L_FOR2_START_395:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_394);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_395);

L_FOR2_END_394:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_398);
L_CLOS_CODE_399: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_393);
printf("at JUMP_SIMPLE_393");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_393:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_406));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_404:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_403);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_404);

L_FOR1_END_403:
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

L_FOR2_START_402:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_401);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_402);

L_FOR2_END_401:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_405);
L_CLOS_CODE_406: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_400);
printf("at JUMP_SIMPLE_400");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_400:
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
MOV(R0, IMM(1583));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_495\n");
JUMP_EQ(L_JUMP_495);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_495:
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
		    JUMP_NE(L_THEN_493); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_492);
		    L_THEN_493:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1583));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_494\n");
JUMP_EQ(L_JUMP_494);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_494:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_491);
		    L_ELSE_492:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_491:
		    
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
JUMP_EQ(L_JUMP_490);
printf("At L_JUMP_490\n");
fflush(stdout);
L_JUMP_490:
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
MOV(R0, IMM(1592));
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
MOV(R0, IMM(1592));
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
MOV(R0, IMM(566));
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

		    JUMP(L_IF_EXIT_410);
		    L_ELSE_411:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
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
		    JUMP_NE(L_THEN_415); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_414);
		    L_THEN_415:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_482);
printf("At L_JUMP_482\n");
fflush(stdout);
L_JUMP_482:
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
MOV(R0, IMM(1603));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_481\n");
JUMP_EQ(L_JUMP_481);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_481:
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
		    JUMP_NE(L_THEN_479); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_478);
		    L_THEN_479:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1603));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_480\n");
JUMP_EQ(L_JUMP_480);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_480:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_477);
		    L_ELSE_478:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_477:
		    
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
MOV(R0, IMM(1275));
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
MOV(R0, IMM(1334));
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
MOV(R0, IMM(1334));
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
		    JUMP_NE(L_THEN_421); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_420);
		    L_THEN_421://IF 

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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_470\n");
JUMP_EQ(L_JUMP_470);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_470:
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
MOV(R0, IMM(1356));
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
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_468\n");
JUMP_EQ(L_JUMP_468);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_468:
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
		    JUMP_NE(L_THEN_464); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_463);
		    L_THEN_464:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_467\n");
JUMP_EQ(L_JUMP_467);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_467:
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_466\n");
JUMP_EQ(L_JUMP_466);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_466:
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
JUMP_EQ(L_JUMP_465);
printf("At L_JUMP_465\n");
fflush(stdout);
L_JUMP_465:
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

		    JUMP(L_IF_EXIT_462);
		    L_ELSE_463:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_462:
		    
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
MOV(R0, IMM(1614));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_459); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_458);
		    L_THEN_459:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1614));
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

		    JUMP(L_IF_EXIT_457);
		    L_ELSE_458:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_457:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_424); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_423);
		    L_THEN_424:
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
JUMP_EQ(L_JUMP_456);
printf("At L_JUMP_456\n");
fflush(stdout);
L_JUMP_456:
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
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1625));
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
MOV(R0, IMM(1625));
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
		    JUMP_NE(L_THEN_427); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_426);
		    L_THEN_427:
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
MOV(R0, IMM(1510));
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

		    JUMP(L_IF_EXIT_425);
		    L_ELSE_426:
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
MOV(R0, IMM(1636));
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
		    JUMP_NE(L_THEN_441); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_440);
		    L_THEN_441://IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1636));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_444); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_443);
		    L_THEN_444:
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
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_447\n");
JUMP_EQ(L_JUMP_447);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_447:
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
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_446\n");
JUMP_EQ(L_JUMP_446);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_446:
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
printf("At L_JUMP_445\n");
JUMP_EQ(L_JUMP_445);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_445:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_442);
		    L_ELSE_443:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_442:
		    
		    JUMP(L_IF_EXIT_439);
		    L_ELSE_440:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_439:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_430); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_429);
		    L_THEN_430:
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
printf("At L_JUMP_438\n");
JUMP_EQ(L_JUMP_438);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_438:
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
printf("At L_JUMP_437\n");
JUMP_EQ(L_JUMP_437);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_437:
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
JUMP_EQ(L_JUMP_436);
printf("At L_JUMP_436\n");
fflush(stdout);
L_JUMP_436:
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

		    JUMP(L_IF_EXIT_428);
		    L_ELSE_429:
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
MOV(R0, IMM(1510));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_435\n");
JUMP_EQ(L_JUMP_435);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_435:
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
		    JUMP_NE(L_THEN_433); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_432);
		    L_THEN_433:
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
MOV(R0, IMM(1510));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_434);
printf("At L_JUMP_434\n");
fflush(stdout);
L_JUMP_434:
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

		    JUMP(L_IF_EXIT_431);
		    L_ELSE_432:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_431:
		    
		    L_IF_EXIT_428:
		    
		    L_IF_EXIT_425:
		    
		    L_IF_EXIT_422:
		    
		    L_IF_EXIT_419:
		    
		    L_IF_EXIT_416:
		    
		    L_IF_EXIT_413:
		    
		    L_IF_EXIT_410:
		    
		    L_IF_EXIT_407:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_405:

POP(FP);
RETURN;
L_CLOS_EXIT_398:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_392));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_390:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_389);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_390);

L_FOR1_END_389:
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

L_FOR2_START_388:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_387);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_388);

L_FOR2_END_387:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_391);
L_CLOS_CODE_392: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_386);
printf("at JUMP_SIMPLE_386");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_386:
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_391:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_385);
printf("At L_JUMP_385\n");
fflush(stdout);
L_JUMP_385:
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
L_CLOS_EXIT_383:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_377\n");
JUMP_EQ(L_JUMP_377);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_377:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_342));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_340:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_339);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_340);

L_FOR1_END_339:
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

L_FOR2_START_338:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_337);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_338);

L_FOR2_END_337:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_341);
L_CLOS_CODE_342: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_336);
printf("at JUMP_SIMPLE_336");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_336:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_357));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_355:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_354);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_355);

L_FOR1_END_354:
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

L_FOR2_START_353:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_352);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_353);

L_FOR2_END_352:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_356);
L_CLOS_CODE_357: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_351);
printf("at JUMP_SIMPLE_351");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_351:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_364));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_362:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_361);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_362);

L_FOR1_END_361:
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

L_FOR2_START_360:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_359);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_360);

L_FOR2_END_359:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_363);
L_CLOS_CODE_364: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_358);
printf("at JUMP_SIMPLE_358");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_358:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_376\n");
JUMP_EQ(L_JUMP_376);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_376:
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
		    JUMP_NE(L_THEN_367); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_366);
		    L_THEN_367:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_365);
		    L_ELSE_366:
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
printf("At L_JUMP_375\n");
JUMP_EQ(L_JUMP_375);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_375:
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
printf("At L_JUMP_374\n");
JUMP_EQ(L_JUMP_374);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_374:
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
		    JUMP_NE(L_THEN_370); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_369);
		    L_THEN_370:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_373);
printf("At L_JUMP_373\n");
fflush(stdout);
L_JUMP_373:
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

		    JUMP(L_IF_EXIT_368);
		    L_ELSE_369:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_372\n");
JUMP_EQ(L_JUMP_372);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_372:
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
JUMP_EQ(L_JUMP_371);
printf("At L_JUMP_371\n");
fflush(stdout);
L_JUMP_371:
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

		    L_IF_EXIT_368:
		    
		    L_IF_EXIT_365:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_363:

POP(FP);
RETURN;
L_CLOS_EXIT_356:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_350));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_348:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_347);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_348);

L_FOR1_END_347:
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

L_FOR2_START_346:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_345);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_346);

L_FOR2_END_345:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_349);
L_CLOS_CODE_350: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_344);
printf("at JUMP_SIMPLE_344");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_344:
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_349:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_343);
printf("At L_JUMP_343\n");
fflush(stdout);
L_JUMP_343:
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
L_CLOS_EXIT_341:
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
printf("At L_JUMP_335\n");
JUMP_EQ(L_JUMP_335);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_335:
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
MOV(R0, IMM(1510));
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
printf("At L_JUMP_334\n");
JUMP_EQ(L_JUMP_334);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_334:
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
		    JUMP_NE(L_THEN_333); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_332);
		    L_THEN_333:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_331);
		    L_ELSE_332:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_331:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_323));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_321:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_320);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_321);

L_FOR1_END_320:
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

L_FOR2_START_319:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_318);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_319);

L_FOR2_END_318:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_322);
L_CLOS_CODE_323: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_317);
printf("at JUMP_SIMPLE_317");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_317:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_330));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_328:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_327);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_328);

L_FOR1_END_327:
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

L_FOR2_START_326:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_325);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_326);

L_FOR2_END_325:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_329);
L_CLOS_CODE_330: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(0));
JUMP_EQ(JUMP_SIMPLE_324);
printf("at JUMP_SIMPLE_324");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_324:
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

POP(FP);
RETURN;
L_CLOS_EXIT_329:

POP(FP);
RETURN;
L_CLOS_EXIT_322:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_316\n");
JUMP_EQ(L_JUMP_316);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_316:
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
printf("At L_JUMP_315\n");
JUMP_EQ(L_JUMP_315);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_315:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_306));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_304:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_303);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_304);

L_FOR1_END_303:
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

L_FOR2_START_302:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_301);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_302);

L_FOR2_END_301:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_305);
L_CLOS_CODE_306: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_300);
printf("at JUMP_SIMPLE_300");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_300:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_313));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_311:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_310);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_311);

L_FOR1_END_310:
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

L_FOR2_START_309:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_308);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_309);

L_FOR2_END_308:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_312);
L_CLOS_CODE_313: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_307);
printf("at JUMP_SIMPLE_307");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_307:

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
MOV(R0, IMM(1510));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_314);
printf("At L_JUMP_314\n");
fflush(stdout);
L_JUMP_314:
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
L_CLOS_EXIT_312:

POP(FP);
RETURN;
L_CLOS_EXIT_305:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_299\n");
JUMP_EQ(L_JUMP_299);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_299:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_283));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_281:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_280);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_281);

L_FOR1_END_280:
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

L_FOR2_START_279:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_278);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_279);

L_FOR2_END_278:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_282);
L_CLOS_CODE_283: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_277);
printf("at JUMP_SIMPLE_277");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_277:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_290));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_288:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_287);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_288);

L_FOR1_END_287:
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

L_FOR2_START_286:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_285);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_286);

L_FOR2_END_285:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_289);
L_CLOS_CODE_290: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_284);
printf("at JUMP_SIMPLE_284");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_284:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_298\n");
JUMP_EQ(L_JUMP_298);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_298:
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
		    JUMP_NE(L_THEN_293); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_292);
		    L_THEN_293:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_291);
		    L_ELSE_292:
		    
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_297\n");
JUMP_EQ(L_JUMP_297);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_297:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_296\n");
JUMP_EQ(L_JUMP_296);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_296:
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
printf("At L_JUMP_295\n");
JUMP_EQ(L_JUMP_295);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_295:
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
JUMP_EQ(L_JUMP_294);
printf("At L_JUMP_294\n");
fflush(stdout);
L_JUMP_294:
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

		    L_IF_EXIT_291:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_289:

POP(FP);
RETURN;
L_CLOS_EXIT_282:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_261));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_259:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_258);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_259);

L_FOR1_END_258:
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

L_FOR2_START_257:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_256);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_257);

L_FOR2_END_256:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_260);
L_CLOS_CODE_261: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_255);
printf("at JUMP_SIMPLE_255");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_255:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_268));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_266:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_265);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_266);

L_FOR1_END_265:
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

L_FOR2_START_264:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_263);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_264);

L_FOR2_END_263:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_267);
L_CLOS_CODE_268: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_262);
printf("at JUMP_SIMPLE_262");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_262:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_276\n");
JUMP_EQ(L_JUMP_276);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_276:
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
		    JUMP_NE(L_THEN_271); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_270);
		    L_THEN_271:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_269);
		    L_ELSE_270:
		    
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_275\n");
JUMP_EQ(L_JUMP_275);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_275:
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
printf("At L_JUMP_274\n");
JUMP_EQ(L_JUMP_274);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_274:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_273\n");
JUMP_EQ(L_JUMP_273);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_273:
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
MOV(R0, IMM(1364));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_272);
printf("At L_JUMP_272\n");
fflush(stdout);
L_JUMP_272:
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

		    L_IF_EXIT_269:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_267:

POP(FP);
RETURN;
L_CLOS_EXIT_260:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_241));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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

L_FOR2_START_237:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_236);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_235);
printf("at JUMP_SIMPLE_235");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_235:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_247));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_245:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_244);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_245);

L_FOR1_END_244:
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

L_FOR2_START_243:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_242);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_243);

L_FOR2_END_242:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_246);
L_CLOS_CODE_247: 

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
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_254\n");
JUMP_EQ(L_JUMP_254);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_254:
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
		    JUMP_NE(L_THEN_250); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_249);
		    L_THEN_250:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_248);
		    L_ELSE_249:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_253\n");
JUMP_EQ(L_JUMP_253);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_253:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_252\n");
JUMP_EQ(L_JUMP_252);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_252:
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
JUMP_EQ(L_JUMP_251);
printf("At L_JUMP_251\n");
fflush(stdout);
L_JUMP_251:
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

		    L_IF_EXIT_248:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_246:

POP(FP);
RETURN;
L_CLOS_EXIT_240:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_234\n");
JUMP_EQ(L_JUMP_234);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_234:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_230));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_228:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_227);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_228);

L_FOR1_END_227:
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

L_FOR2_START_226:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_225);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_226);

L_FOR2_END_225:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_229);
L_CLOS_CODE_230: 

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
printf("At L_JUMP_233\n");
JUMP_EQ(L_JUMP_233);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_233:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_232\n");
JUMP_EQ(L_JUMP_232);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_232:
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
JUMP_EQ(L_JUMP_231);
printf("At L_JUMP_231\n");
fflush(stdout);
L_JUMP_231:
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
L_CLOS_EXIT_229:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_221));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_219:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_218);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_219);

L_FOR1_END_218:
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

L_FOR2_START_217:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_216);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_217);

L_FOR2_END_216:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_220);
L_CLOS_CODE_221: 

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
printf("At L_JUMP_224\n");
JUMP_EQ(L_JUMP_224);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_224:
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
MOV(R0, IMM(1373));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_223\n");
JUMP_EQ(L_JUMP_223);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_223:
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
JUMP_EQ(L_JUMP_222);
printf("At L_JUMP_222\n");
fflush(stdout);
L_JUMP_222:
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
L_CLOS_EXIT_220:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_200));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_198:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_197);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_198);

L_FOR1_END_197:
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

L_FOR2_START_196:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_195);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_196);

L_FOR2_END_195:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_199);
L_CLOS_CODE_200: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_194);
printf("at JUMP_SIMPLE_194");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_194:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_207));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_205:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_204);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_205);

L_FOR1_END_204:
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

L_FOR2_START_203:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_202);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_203);

L_FOR2_END_202:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_206);
L_CLOS_CODE_207: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_201);
printf("at JUMP_SIMPLE_201");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_201:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_215\n");
JUMP_EQ(L_JUMP_215);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_215:
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
		    JUMP_NE(L_THEN_210); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_209);
		    L_THEN_210:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_208);
		    L_ELSE_209:
		    
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_214\n");
JUMP_EQ(L_JUMP_214);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_214:
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
MOV(R0, IMM(1364));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_213\n");
JUMP_EQ(L_JUMP_213);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_213:
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_212\n");
JUMP_EQ(L_JUMP_212);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_212:
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
JUMP_EQ(L_JUMP_211);
printf("At L_JUMP_211\n");
fflush(stdout);
L_JUMP_211:
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

		    L_IF_EXIT_208:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_206:

POP(FP);
RETURN;
L_CLOS_EXIT_199:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_185));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_183:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_182);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_183);

L_FOR1_END_182:
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

L_FOR2_START_181:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_180);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_181);

L_FOR2_END_180:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_184);
L_CLOS_CODE_185: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_179);
printf("at JUMP_SIMPLE_179");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_179:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_192));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_190:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_189);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_190);

L_FOR1_END_189:
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

L_FOR2_START_188:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_187);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_188);

L_FOR2_END_187:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_191);
L_CLOS_CODE_192: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_186);
printf("at JUMP_SIMPLE_186");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_186:

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
JUMP_EQ(L_JUMP_193);
printf("At L_JUMP_193\n");
fflush(stdout);
L_JUMP_193:
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
L_CLOS_EXIT_191:

POP(FP);
RETURN;
L_CLOS_EXIT_184:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_178\n");
JUMP_EQ(L_JUMP_178);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_178:
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
printf("At L_JUMP_177\n");
JUMP_EQ(L_JUMP_177);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_177:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_168));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_166:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_165);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_166);

L_FOR1_END_165:
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

L_FOR2_START_164:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_163);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_164);

L_FOR2_END_163:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_167);
L_CLOS_CODE_168: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_162);
printf("at JUMP_SIMPLE_162");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_162:
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
printf("At L_JUMP_176\n");
JUMP_EQ(L_JUMP_176);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_176:
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
		    JUMP_NE(L_THEN_171); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_170);
		    L_THEN_171:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_175);
printf("At L_JUMP_175\n");
fflush(stdout);
L_JUMP_175:
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

		    JUMP(L_IF_EXIT_169);
		    L_ELSE_170:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1290));
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
printf("At L_JUMP_174\n");
JUMP_EQ(L_JUMP_174);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_174:
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_173\n");
JUMP_EQ(L_JUMP_173);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_173:
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
JUMP_EQ(L_JUMP_172);
printf("At L_JUMP_172\n");
fflush(stdout);
L_JUMP_172:
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

		    L_IF_EXIT_169:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_167:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_153));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_151:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_150);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_151);

L_FOR1_END_150:
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

L_FOR2_START_149:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_148);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_149);

L_FOR2_END_148:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_152);
L_CLOS_CODE_153: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_147);
printf("at JUMP_SIMPLE_147");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_147:
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
printf("At L_JUMP_161\n");
JUMP_EQ(L_JUMP_161);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_161:
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
		    JUMP_NE(L_THEN_156); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_155);
		    L_THEN_156:
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
MOV(R0, IMM(1648));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_160);
printf("At L_JUMP_160\n");
fflush(stdout);
L_JUMP_160:
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

		    JUMP(L_IF_EXIT_154);
		    L_ELSE_155:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1290));
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
printf("At L_JUMP_159\n");
JUMP_EQ(L_JUMP_159);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_159:
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
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_158\n");
JUMP_EQ(L_JUMP_158);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_158:
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
JUMP_EQ(L_JUMP_157);
printf("At L_JUMP_157\n");
fflush(stdout);
L_JUMP_157:
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

		    L_IF_EXIT_154:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_152:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_142));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_140:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_139);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_140);

L_FOR1_END_139:
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

L_FOR2_START_138:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_137);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_138);

L_FOR2_END_137:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_141);
L_CLOS_CODE_142: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_136);
printf("at JUMP_SIMPLE_136");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_136:
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
printf("At L_JUMP_146\n");
JUMP_EQ(L_JUMP_146);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_146:
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
		    JUMP_NE(L_THEN_145); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_144);
		    L_THEN_145:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_143);
		    L_ELSE_144:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_143:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_141:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_128));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_126:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_125);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_126);

L_FOR1_END_125:
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

L_FOR2_START_124:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_123);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_124);

L_FOR2_END_123:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_127);
L_CLOS_CODE_128: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_122);
printf("at JUMP_SIMPLE_122");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_122:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_130));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_132:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_133);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_132);

L_FOR1_END_133:
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

L_FOR2_START_134:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_135);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_134);

L_FOR2_END_135:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_131);
L_CLOS_CODE_130: 
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
JUMP_EQ(L_JUMP_129);
printf("At L_JUMP_129\n");
fflush(stdout);
L_JUMP_129:
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
L_CLOS_EXIT_131:

POP(FP);
RETURN;
L_CLOS_EXIT_127:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_121\n");
JUMP_EQ(L_JUMP_121);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_121:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_116));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_114:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_113);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_114);

L_FOR1_END_113:
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

L_FOR2_START_112:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_111);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_112);

L_FOR2_END_111:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_115);
L_CLOS_CODE_116: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_110);
printf("at JUMP_SIMPLE_110");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_110:
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
printf("At L_JUMP_120\n");
JUMP_EQ(L_JUMP_120);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_120:
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
		    JUMP_NE(L_THEN_119); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_118);
		    L_THEN_119:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_117);
		    L_ELSE_118:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_117:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_115:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_102));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_100:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_99);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_100);

L_FOR1_END_99:
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

L_FOR2_START_98:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_97);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_98);

L_FOR2_END_97:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_101);
L_CLOS_CODE_102: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_96);
printf("at JUMP_SIMPLE_96");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_96:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_104));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_106:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_107);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_106);

L_FOR1_END_107:
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

L_FOR2_START_108:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_109);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_108);

L_FOR2_END_109:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_105);
L_CLOS_CODE_104: 
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
JUMP_EQ(L_JUMP_103);
printf("At L_JUMP_103\n");
fflush(stdout);
L_JUMP_103:
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
L_CLOS_EXIT_105:

POP(FP);
RETURN;
L_CLOS_EXIT_101:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_95\n");
JUMP_EQ(L_JUMP_95);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_95:
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
MOV(R0,IMM(1290));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_75));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_73:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_72);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_73);

L_FOR1_END_72:
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

L_FOR2_START_71:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_70);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_71);

L_FOR2_END_70:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_74);
L_CLOS_CODE_75: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_69);
printf("at JUMP_SIMPLE_69");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_69:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_82));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_80:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_79);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_80);

L_FOR1_END_79:
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

L_FOR2_START_78:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_77);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_78);

L_FOR2_END_77:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_81);
L_CLOS_CODE_82: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_76);
printf("at JUMP_SIMPLE_76");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_76:
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1654));
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
printf("At L_JUMP_94\n");
JUMP_EQ(L_JUMP_94);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_94:
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
		    JUMP_NE(L_THEN_85); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_84);
		    L_THEN_85:
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
JUMP_EQ(L_JUMP_93);
printf("At L_JUMP_93\n");
fflush(stdout);
L_JUMP_93:
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

		    JUMP(L_IF_EXIT_83);
		    L_ELSE_84:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1654));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1347));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_92\n");
JUMP_EQ(L_JUMP_92);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_92:
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
printf("At L_JUMP_91\n");
JUMP_EQ(L_JUMP_91);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_91:
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
MOV(R0,IMM(1654));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1654));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1347));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_90\n");
JUMP_EQ(L_JUMP_90);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_90:
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
printf("At L_JUMP_89\n");
JUMP_EQ(L_JUMP_89);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_89:
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
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_87\n");
JUMP_EQ(L_JUMP_87);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_87:
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
JUMP_EQ(L_JUMP_86);
printf("At L_JUMP_86\n");
fflush(stdout);
L_JUMP_86:
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

		    L_IF_EXIT_83:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_81:

POP(FP);
RETURN;
L_CLOS_EXIT_74:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_56));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_54:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_53);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_54);

L_FOR1_END_53:
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

L_FOR2_START_52:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_51);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_52);

L_FOR2_END_51:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_55);
L_CLOS_CODE_56: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_50);
printf("at JUMP_SIMPLE_50");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_50:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1652));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_68\n");
JUMP_EQ(L_JUMP_68);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_68:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_65));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_63:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_62);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_63);

L_FOR1_END_62:
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

L_FOR2_START_61:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_60);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_61);

L_FOR2_END_60:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_64);
L_CLOS_CODE_65: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_59);
printf("at JUMP_SIMPLE_59");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_59:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1650));
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
printf("At L_JUMP_67\n");
JUMP_EQ(L_JUMP_67);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_67:
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
MOV(R0, IMM(1440));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_66);
printf("At L_JUMP_66\n");
fflush(stdout);
L_JUMP_66:
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
L_CLOS_EXIT_64:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_58\n");
JUMP_EQ(L_JUMP_58);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_58:
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
JUMP_EQ(L_JUMP_57);
printf("At L_JUMP_57\n");
fflush(stdout);
L_JUMP_57:
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
L_CLOS_EXIT_55:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_49\n");
JUMP_EQ(L_JUMP_49);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_49:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_29));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_27:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_26);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_27);

L_FOR1_END_26:
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

L_FOR2_START_25:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_24);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_25);

L_FOR2_END_24:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_28);
L_CLOS_CODE_29: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_23);
printf("at JUMP_SIMPLE_23");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_23:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_36));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_34:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_33);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_34);

L_FOR1_END_33:
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

L_FOR2_START_32:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_31);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_32);

L_FOR2_END_31:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_35);
L_CLOS_CODE_36: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_30);
printf("at JUMP_SIMPLE_30");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_30:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1318));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_48\n");
JUMP_EQ(L_JUMP_48);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_48:
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
		    JUMP_NE(L_THEN_39); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_38);
		    L_THEN_39:MOV(R0,IMM(14));

		    JUMP(L_IF_EXIT_37);
		    L_ELSE_38:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_47\n");
JUMP_EQ(L_JUMP_47);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_47:
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
MOV(R0, IMM(1415));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_46\n");
JUMP_EQ(L_JUMP_46);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_46:
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
		    JUMP_NE(L_THEN_42); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_41);
		    L_THEN_42:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1325));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_45\n");
JUMP_EQ(L_JUMP_45);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_45:
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
MOV(R0, IMM(1356));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_44\n");
JUMP_EQ(L_JUMP_44);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_44:
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
JUMP_EQ(L_JUMP_43);
printf("At L_JUMP_43\n");
fflush(stdout);
L_JUMP_43:
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

		    JUMP(L_IF_EXIT_40);
		    L_ELSE_41:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_40:
		    
		    L_IF_EXIT_37:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_35:

POP(FP);
RETURN;
L_CLOS_EXIT_28:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_15));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_13:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_12);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_13);

L_FOR1_END_12:
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

L_FOR2_START_11:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_10);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_11);

L_FOR2_END_10:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_14);
L_CLOS_CODE_15: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_9);
printf("at JUMP_SIMPLE_9");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_9:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_17));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_19:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_20);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_19);

L_FOR1_END_20:
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

L_FOR2_START_21:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_22);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_21);

L_FOR2_END_22:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_18);
L_CLOS_CODE_17: 
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
JUMP_EQ(L_JUMP_16);
printf("At L_JUMP_16\n");
fflush(stdout);
L_JUMP_16:
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
L_CLOS_EXIT_18:

POP(FP);
RETURN;
L_CLOS_EXIT_14:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_8\n");
JUMP_EQ(L_JUMP_8);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_8:
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

 // DEFINEs

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1349));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1503));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_7\n");
JUMP_EQ(L_JUMP_7);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_7:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1280), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1656));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1440));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_6\n");
JUMP_EQ(L_JUMP_6);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_6:
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
MOV(R0,IMM(1309));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1280));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1488));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_5\n");
JUMP_EQ(L_JUMP_5);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_5:
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
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

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1658));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1440));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_4\n");
JUMP_EQ(L_JUMP_4);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_4:
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
MOV(R0,IMM(1290));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1280));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1488));
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
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
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

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1280));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1682));
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
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
PUSH(R0);
MOV(R0,IMM(1664));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1510));
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
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
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
