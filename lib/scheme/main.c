
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
int arr[474] = {937610, 722689, 741553, 0, 741553, 1, 799345, 5, 111, 114, 100, 101, 114, 368031, 0, 799345, 1, 98, 368031, 0, 945311, 1, 945311, 2, 945311, 3, 799345, 4, 98, 105, 110, 43, 368031, 0, 799345, 4, 98, 105, 110, 45, 368031, 0, 799345, 4, 98, 105, 110, 42, 368031, 0, 799345, 4, 98, 105, 110, 47, 368031, 0, 799345, 1, 62, 368031, 0, 799345, 1, 60, 368031, 0, 799345, 1, 61, 368031, 0, 799345, 5, 98, 105, 110, 60, 63, 368031, 0, 799345, 5, 98, 105, 110, 61, 63, 368031, 0, 799345, 4, 98, 105, 110, 60, 368031, 0, 799345, 4, 98, 105, 110, 61, 368031, 0, 799345, 4, 98, 105, 110, 62, 368031, 0, 799345, 5, 97, 112, 112, 108, 121, 368031, 0, 799345, 9, 114, 101, 109, 97, 105, 110, 100, 101, 114, 368031, 0, 799345, 10, 112, 114, 111, 99, 101, 100, 117, 114, 101, 63, 368031, 0, 799345, 3, 99, 97, 114, 368031, 0, 799345, 3, 99, 100, 114, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 62, 115, 121, 109, 98, 111, 108, 368031, 0, 799345, 7, 118, 101, 99, 116, 111, 114, 63, 368031, 0, 799345, 7, 115, 
121, 109, 98, 111, 108, 63, 368031, 0, 799345, 7, 115, 116, 114, 105, 110, 103, 63, 368031, 0, 799345, 5, 99, 104, 97, 114, 63, 368031, 0, 799345, 7, 110, 117, 109, 98, 101, 114, 63, 368031, 0, 799345, 8, 98, 111, 111, 108, 101, 97, 110, 63, 368031, 0, 799345, 5, 112, 97, 105, 114, 63, 368031, 0, 799345, 5, 110, 117, 108, 108, 63, 368031, 0, 799345, 13, 105, 110, 116, 101, 103, 101, 114, 45, 62, 99, 104, 97, 114, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 62, 105, 110, 116, 101, 103, 101, 114, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 14, 115, 121, 109, 98, 111, 108, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 97, 114, 33, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 100, 114, 33, 368031, 0, 799345, 4, 99, 111, 110, 115, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 11, 
115, 116, 114, 105, 110, 103, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 10, 115, 116, 114, 105, 110, 103, 45, 114, 101, 102, 368031, 0, 799345, 10, 118, 101, 99, 116, 111, 114, 45, 114, 101, 102, 368031, 0, 799345, 3, 101, 113, 63, 368031, 0, 799345, 6, 101, 113, 117, 97, 108, 63, 368031, 0};
void *a = &arr;
memcpy(&((*machine).mem[10]), a, 474*4);
MOV(ADDR(0), IMM(484));

 //generating symbol table
MOV(R1, IMM(1));
//creating bucket for order
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(16)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(24), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for b
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(25)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(29), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin+
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(36)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(43), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin-
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(44)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(51), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin*
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(52)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(59), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin/
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(60)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(67), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for >
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(68)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(72), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(73)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(77), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for =
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(78)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(82), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(83)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(91), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(92)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(100), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(101)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(108), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(109)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(116), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(117)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(124), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for apply
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(125)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(133), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for remainder
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(134)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(146), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for procedure?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(147)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(160), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for car
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(161)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(167), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(168)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(174), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->symbol
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(175)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(192), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(193)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(203), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(204)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(214), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(215)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(225), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(226)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(234), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for number?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(235)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(245), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for boolean?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(246)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(257), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for pair?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(258)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(266), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for null?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(267)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(275), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for integer->char
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(276)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(292), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char->integer
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(293)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(309), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(310)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(326), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(327)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(344), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-car!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(345)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(356), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-cdr!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(357)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(368), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cons
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(369)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(376), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(377)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(391), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(392)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(408), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(409)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(423), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(424)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(438), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(439)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(452), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(453)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(466), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for eq?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(467)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(473), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for equal?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(474)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(483), R0); // save the bucket address in the symbol object 
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
MOV(R1, INDD(IMM(173),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_PLUS
PUSH(LABEL(BIN_PLUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(42),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for APPLY
PUSH(LABEL(APPLY));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(132),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MINUS
PUSH(LABEL(BIN_MINUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(50),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MUL
PUSH(LABEL(BIN_MUL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(58),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_DIV
PUSH(LABEL(BIN_DIV));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(66),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(71),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(123),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(76),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(81),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(99),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(90),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(115),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(107),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CAR
PUSH(LABEL(CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(166),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(173),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_TO_SYMBOL
PUSH(LABEL(STRING_TO_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(191),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for REMAINDER
PUSH(LABEL(REMAINDER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(145),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PROCEDURE
PUSH(LABEL(IS_PROCEDURE));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(159),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_VECTOR
PUSH(LABEL(IS_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(202),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_SYMBOL
PUSH(LABEL(IS_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(213),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_STRING
PUSH(LABEL(IS_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(224),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_CHAR
PUSH(LABEL(IS_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(233),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NUMBER
PUSH(LABEL(IS_NUMBER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(244),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_BOOLEAN
PUSH(LABEL(IS_BOOLEAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(256),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PAIR
PUSH(LABEL(IS_PAIR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(265),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NULL
PUSH(LABEL(IS_NULL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(274),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for INTEGER_TO_CHAR
PUSH(LABEL(INTEGER_TO_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(291),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CHAR_TO_INTEGER
PUSH(LABEL(CHAR_TO_INTEGER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(308),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(325),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(407),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_STRING
PUSH(LABEL(MAKE_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(390),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_VECTOR
PUSH(LABEL(MAKE_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(437),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SYMBOL_TO_STRING
PUSH(LABEL(SYMBOL_TO_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(343),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CAR
PUSH(LABEL(SET_CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(355),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CDR
PUSH(LABEL(SET_CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(367),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CONS
PUSH(LABEL(CONS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(375),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_SET
PUSH(LABEL(STRING_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(422),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_REF
PUSH(LABEL(STRING_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(451),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for VECTOR_REF
PUSH(LABEL(VECTOR_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(465),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQ
PUSH(LABEL(EQ));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(472),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL
PUSH(LABEL(EQUAL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(482),1));
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
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_3);
printf("at JUMP_SIMPLE_3");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_3:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_14));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_16:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_17);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_16);

L_FOR1_END_17:
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

L_FOR2_START_18:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_19);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_18);

L_FOR2_END_19:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_15);
L_CLOS_CODE_14: 
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
MOV(R0,IMM(14));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_12); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_11);
		    L_THEN_12:
 //TC-APPLIC 

MOV(R0,IMM(32));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(30));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);
CALL(WRITE_SOB);
DROP(IMM(1));
SHOW(" LABEL", LABEL(LOWER_THAN));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_13);
printf("At L_JUMP_13\n");
fflush(stdout);
L_JUMP_13:
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

		    JUMP(L_IF_EXIT_10);
		    L_ELSE_11:
		    MOV(R0,IMM(14));

		    L_IF_EXIT_10:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_15:

POP(FP);
RETURN;
L_CLOS_EXIT_8:
MOV(R1, INDD(IMM(23), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEb

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(107));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(23));
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
MOV(R1, INDD(IMM(28), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(34));
PUSH(R0);
MOV(R0,IMM(32));
PUSH(R0);
MOV(R0,IMM(30));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(28));
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
L_END_OF_PROGRAM:
STOP_MACHINE;

return 0;
}
