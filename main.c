
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
int arr[1732] = {937610, 722689, 741553, 0, 741553, 1, 799345, 1, 61, 368031, 0, 799345, 3, 110, 111, 116, 368031, 0, 799345, 4, 108, 105, 115, 116, 368031, 0, 799345, 4, 97, 100, 100, 49, 368031, 0, 799345, 4, 115, 117, 98, 49, 368031, 0, 799345, 5, 98, 105, 110, 62, 63, 368031, 0, 799345, 6, 98, 105, 110, 60, 61, 63, 368031, 0, 799345, 6, 98, 105, 110, 62, 61, 63, 368031, 0, 799345, 5, 122, 101, 114, 111, 63, 368031, 0, 799345, 6, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 5, 108, 105, 115, 116, 63, 368031, 0, 799345, 5, 101, 118, 101, 110, 63, 368031, 0, 799345, 4, 111, 100, 100, 63, 368031, 0, 799345, 3, 109, 97, 112, 368031, 0, 799345, 2, 121, 110, 368031, 0, 799345, 1, 43, 368031, 0, 799345, 1, 42, 368031, 0, 799345, 1, 45, 368031, 0, 799345, 1, 47, 368031, 0, 799345, 5, 111, 114, 100, 101, 114, 368031, 0, 799345, 6, 97, 110, 100, 109, 97, 112, 368031, 0, 799345, 2, 60, 62, 368031, 0, 799345, 5, 102, 111, 108, 100, 114, 368031, 0, 799345, 7, 99, 111, 109, 112, 111, 115, 101, 368031, 0, 799345, 4, 99, 97, 97, 114, 368031, 0, 799345, 4, 99, 97, 100, 114, 368031, 0, 799345, 4, 99, 100, 97, 114, 368031, 0, 799345, 4, 99, 100, 100, 114, 368031, 0, 799345, 5, 99, 97, 97, 97, 114, 368031, 0, 799345, 5, 99, 97, 97, 100, 114, 368031, 0, 799345, 5, 99, 97, 100, 97, 114, 368031, 0, 799345, 5, 99, 97, 100, 100, 114, 368031, 0, 799345, 5, 99, 100, 97, 97, 114, 368031, 0, 799345, 5, 99, 100, 97, 100, 114, 368031, 0, 799345, 5, 99, 100, 100, 97, 114, 368031, 0, 799345, 5, 99, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 100, 114, 368031, 0, 799345, 27, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 114, 105, 103, 104, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 26, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 108, 101, 102, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 8, 94, 99, 104, 97, 114, 45, 111, 112, 368031, 0, 799345, 1, 60, 368031, 0, 799345, 2, 60, 61, 368031, 0, 799345, 1, 62, 368031, 0, 799345, 2, 62, 61, 368031, 0, 799345, 6, 99, 104, 97, 114, 61, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 60, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 60, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 62, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 62, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 117, 112, 112, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 108, 111, 119, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 11, 99, 104, 97, 114, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 60, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 60, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 62, 63, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 62, 61, 63, 368031, 0, 799345, 12, 115, 116, 114, 105, 110, 103, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 15, 115, 116, 114, 105, 110, 103, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 8, 108, 105, 115, 116, 45, 114, 101, 102, 368031, 0, 799345, 5, 111, 114, 109, 97, 112, 368031, 0, 799345, 7, 109, 101, 109, 98, 101, 114, 63, 368031, 0, 799345, 9, 110, 101, 103, 97, 116, 105, 118, 101, 63, 368031, 0, 799345, 9, 112, 111, 115, 105, 116, 105, 118, 101, 63, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 6, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 12, 118, 101, 99, 116, 111, 114, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 9, 98, 111, 111, 108, 101, 97, 110, 61, 63, 368031, 0, 799345, 6, 101, 113, 117, 97, 108, 63, 368031, 0, 799345, 10, 94, 97, 115, 115, 111, 99, 105, 97, 116, 101, 368031, 0, 799345, 5, 97, 115, 115, 111, 99, 368031, 0, 799345, 4, 97, 115, 115, 113, 368031, 0, 799345, 4, 118, 111, 105, 100, 368031, 0, 799345, 5, 118, 111, 105, 100, 63, 368031, 0, 799345, 6, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 7, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 9, 108, 105, 115, 116, 45, 115, 101, 116, 33, 368031, 0, 799345, 3, 109, 97, 120, 368031, 0, 799345, 3, 109, 105, 110, 368031, 0, 799345, 5, 101, 114, 114, 111, 114, 368031, 0, 799345, 14, 110, 117, 109, 98, 101, 114, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 5, 110, 117, 108, 108, 63, 368031, 0, 799345, 5, 98, 105, 110, 61, 63, 368031, 0, 799345, 3, 99, 97, 114, 368031, 0, 799345, 1, 115, 368031, 0, 799345, 3, 99, 100, 114, 368031, 0, 799345, 4, 98, 105, 110, 43, 368031, 0, 945311, 1, 799345, 4, 98, 105, 110, 45, 368031, 0, 799345, 5, 98, 105, 110, 60, 63, 368031, 0, 945311, 0, 799345, 5, 112, 97, 105, 114, 63, 368031, 0, 799345, 9, 114, 101, 109, 97, 105, 110, 100, 101, 114, 368031, 0, 945311, 2, 799345, 4, 99, 111, 110, 115, 368031, 0, 799345, 5, 97, 112, 112, 108, 121, 368031, 0, 799345, 4, 98, 105, 110, 42, 368031, 0, 799345, 4, 98, 105, 110, 47, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 62, 105, 110, 116, 101, 103, 101, 114, 368031, 0, 181048, 65, 181048, 90, 181048, 97, 181048, 122, 799345, 13, 105, 110, 116, 101, 103, 101, 114, 45, 62, 99, 104, 97, 114, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 945311, -1, 799345, 10, 115, 116, 114, 105, 110, 103, 45, 114, 101, 102, 368031, 0, 799345, 11, 115, 116, 114, 105, 110, 103, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 3, 101, 113, 63, 368031, 0, 799345, 11, 118, 101, 99, 116, 111, 114, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 10, 118, 101, 99, 116, 111, 114, 45, 114, 101, 102, 368031, 0, 799345, 8, 98, 111, 111, 108, 101, 97, 110, 63, 368031, 0, 799345, 5, 99, 104, 97, 114, 63, 368031, 0, 799345, 7, 110, 117, 109, 98, 101, 114, 63, 368031, 0, 799345, 7, 115, 116, 114, 105, 110, 103, 63, 368031, 0, 799345, 7, 115, 121, 109, 98, 111, 108, 63, 368031, 0, 799345, 7, 118, 101, 99, 116, 111, 114, 63, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 97, 114, 33, 368031, 0, 945311, 48, 945311, 123, 945311, 10, 799345, 4, 98, 105, 110, 60, 368031, 0, 799345, 4, 98, 105, 110, 61, 368031, 0, 799345, 4, 98, 105, 110, 62, 368031, 0, 799345, 10, 112, 114, 111, 99, 101, 100, 117, 114, 101, 63, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 62, 115, 121, 109, 98, 111, 108, 368031, 0, 799345, 14, 115, 121, 109, 98, 111, 108, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 100, 114, 33, 368031, 0};
void *a = &arr;
memcpy(&((*machine).mem[10]), a, 1732*4);
MOV(ADDR(0), IMM(1742));

 //generating symbol table
MOV(R1, IMM(1));
//creating bucket for =
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(16)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(20), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for not
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(21)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(27), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(28)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(35), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for add1
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(36)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(43), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for sub1
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(44)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(51), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(52)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(60), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(61)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(70), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(71)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(80), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for zero?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(81)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(89), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(90)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(99), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(100)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(108), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for even?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(109)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(117), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for odd?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(118)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(125), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for map
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(126)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(132), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for yn
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(133)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(138), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for +
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(139)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(143), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for *
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(144)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(148), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for -
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(149)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(153), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for /
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(154)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(158), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for order
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(159)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(167), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for andmap
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(168)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(177), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(178)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(183), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for foldr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(184)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(192), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for compose
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(193)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(203), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(204)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(211), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(212)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(219), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(220)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(227), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(228)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(235), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(236)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(244), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(245)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(253), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(254)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(262), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(263)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(271), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(272)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(280), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(281)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(289), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(290)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(298), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(299)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(307), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(308)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(317), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(318)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(327), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caadar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(328)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(337), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caaddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(338)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(347), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(348)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(357), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(358)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(367), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for caddar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(368)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(377), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cadddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(378)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(387), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(388)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(397), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(398)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(407), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdadar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(408)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(417), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdaddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(418)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(427), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddaar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(428)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(437), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddadr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(438)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(447), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdddar
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(448)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(457), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cddddr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(458)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(467), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ^variadic-right-from-binary
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(468)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(498), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ^variadic-left-from-binary
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(499)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(528), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ^char-op
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(529)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(540), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(541)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(545), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for <=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(546)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(551), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for >
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(552)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(556), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for >=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(557)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(562), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(563)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(572), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(573)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(583), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(584)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(593), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(594)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(604), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(605)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(614), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-uppercase?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(615)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(633), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-lowercase?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(634)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(652), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-upcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(653)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(667), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-downcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(668)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(684), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(685)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(698), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(699)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(711), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(712)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(724), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(725)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(737), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char-ci>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(738)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(751), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->list
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(752)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(767), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(768)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(783), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-upcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(784)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(800), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-downcase
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(801)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(819), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(820)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(831), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ormap
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(832)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(840), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for member?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(841)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(851), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for negative?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(852)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(864), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for positive?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(865)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(877), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list->vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(878)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(893), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(894)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(903), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(904)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(922), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(923)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(941), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(942)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(960), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(961)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(980), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for binary-string>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(981)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1000), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1001)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1012), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1013)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1024), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string>?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1025)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1036), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string<=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1037)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1049), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string>=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1050)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1062), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector->list
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1063)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1078), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for boolean=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1079)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1091), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for equal?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1092)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1101), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for ^associate
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1102)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1115), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for assoc
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1116)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1124), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for assq
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1125)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1132), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for void
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1133)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1140), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for void?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1141)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1149), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for append
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1150)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1159), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-append
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1160)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1176), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-append
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1177)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1193), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for reverse
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1194)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1204), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-reverse
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1205)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1222), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for list-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1223)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1235), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for max
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1236)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1242), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for min
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1243)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1249), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for error
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1250)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1258), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for number->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1259)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1276), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for null?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1277)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1285), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1286)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1294), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for car
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1295)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1301), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for s
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1302)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1306), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1307)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1313), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin+
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1314)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1321), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin-
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1324)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1331), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1332)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1340), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for pair?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1343)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1351), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for remainder
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1352)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1364), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cons
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1367)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1374), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for apply
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1375)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1383), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin*
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1384)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1391), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin/
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1392)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1399), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char->integer
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1400)); // add pointer to the string 
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
//creating bucket for bin<
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1656)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1663), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1664)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1671), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1672)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1679), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for procedure?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1680)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1693), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->symbol
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1694)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1711), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1712)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1729), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-cdr!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1730)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1741), R0); // save the bucket address in the symbol object 
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
MOV(R1, INDD(IMM(1312),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_PLUS
PUSH(LABEL(BIN_PLUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1320),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for APPLY
PUSH(LABEL(APPLY));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1382),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MINUS
PUSH(LABEL(BIN_MINUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1330),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MUL
PUSH(LABEL(BIN_MUL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1390),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_DIV
PUSH(LABEL(BIN_DIV));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1398),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(555),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for GRATER_THAN
PUSH(LABEL(GRATER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1678),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(544),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(19),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1293),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1339),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1670),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1662),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CAR
PUSH(LABEL(CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1300),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1312),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_TO_SYMBOL
PUSH(LABEL(STRING_TO_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1710),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for REMAINDER
PUSH(LABEL(REMAINDER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1363),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PROCEDURE
PUSH(LABEL(IS_PROCEDURE));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1692),1));
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
MOV(R1, INDD(IMM(1350),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NULL
PUSH(LABEL(IS_NULL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1284),1));
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
MOV(R1, INDD(IMM(1415),1));
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
MOV(R1, INDD(IMM(1728),1));
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
MOV(R1, INDD(IMM(1740),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CONS
PUSH(LABEL(CONS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1373),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_SET
PUSH(LABEL(STRING_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1488),1));
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

//This is the bindings for EQ
PUSH(LABEL(EQ));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1510),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL
PUSH(LABEL(EQUAL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1100),1));
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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1799);
printf("at JUMP_SIMPLE_1799");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1799:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1812));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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

L_FOR2_START_1808:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1807);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1806);
printf("at JUMP_SIMPLE_1806");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1806:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1824\n");
JUMP_EQ(L_JUMP_1824);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1824:
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
		    JUMP_NE(L_THEN_1815); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1814);
		    L_THEN_1815:MOV(R0,IMM(14));

		    JUMP(L_IF_EXIT_1813);
		    L_ELSE_1814:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1823\n");
JUMP_EQ(L_JUMP_1823);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1823:
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
MOV(R0, IMM(1293));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1822\n");
JUMP_EQ(L_JUMP_1822);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1822:
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
		    JUMP_NE(L_THEN_1818); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1817);
		    L_THEN_1818:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1305));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1821\n");
JUMP_EQ(L_JUMP_1821);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1821:
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

 //FVAR 
MOV(R0, IMM(1305));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1820\n");
JUMP_EQ(L_JUMP_1820);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1820:
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
JUMP_EQ(L_JUMP_1819);
printf("At L_JUMP_1819\n");
fflush(stdout);
L_JUMP_1819:
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

		    JUMP(L_IF_EXIT_1816);
		    L_ELSE_1817:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1816:
		    
		    L_IF_EXIT_1813:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1811:

POP(FP);
RETURN;
L_CLOS_EXIT_1804:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1791));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1789:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1788);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1789);

L_FOR1_END_1788:
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

L_FOR2_START_1787:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1786);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1787);

L_FOR2_END_1786:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1790);
L_CLOS_CODE_1791: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1785);
printf("at JUMP_SIMPLE_1785");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1785:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1793));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1795:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1796);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1795);

L_FOR1_END_1796:
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

L_FOR2_START_1797:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1798);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1797);

L_FOR2_END_1798:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1794);
L_CLOS_CODE_1793: 
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
JUMP_EQ(L_JUMP_1792);
printf("At L_JUMP_1792\n");
fflush(stdout);
L_JUMP_1792:
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

POP(FP);
RETURN;
L_CLOS_EXIT_1790:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1784\n");
JUMP_EQ(L_JUMP_1784);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1784:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(19), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

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
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1774);
printf("at JUMP_SIMPLE_1774");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1774:
//IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1783); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1782);
		    L_THEN_1783:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_1781);
		    L_ELSE_1782:
		    MOV(R0,IMM(14));

		    L_IF_EXIT_1781:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1779:
MOV(R1, INDD(IMM(26), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1773));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1771:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1770);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1771);

L_FOR1_END_1770:
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

L_FOR2_START_1769:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1768);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1769);

L_FOR2_END_1768:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1772);
L_CLOS_CODE_1773: 

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
L_CLOS_EXIT_1772:
MOV(R1, INDD(IMM(34), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1766));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1764:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1763);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1764);

L_FOR1_END_1763:
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

L_FOR2_START_1762:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1761);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1762);

L_FOR2_END_1761:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1765);
L_CLOS_CODE_1766: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1760);
printf("at JUMP_SIMPLE_1760");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1760:

 //TC-APPLIC 

MOV(R0,IMM(1322));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1767);
printf("At L_JUMP_1767\n");
fflush(stdout);
L_JUMP_1767:
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
L_CLOS_EXIT_1765:
MOV(R1, INDD(IMM(42), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1758));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1756:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1755);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1756);

L_FOR1_END_1755:
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

L_FOR2_START_1754:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1753);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1754);

L_FOR2_END_1753:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1757);
L_CLOS_CODE_1758: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1752);
printf("at JUMP_SIMPLE_1752");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1752:

 //TC-APPLIC 

MOV(R0,IMM(1322));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1330));
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
L_CLOS_EXIT_1757:
MOV(R1, INDD(IMM(50), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1750));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1748:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1747);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1748);

L_FOR1_END_1747:
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

L_FOR2_START_1746:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1745);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1746);

L_FOR2_END_1745:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1749);
L_CLOS_CODE_1750: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1744);
printf("at JUMP_SIMPLE_1744");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1744:

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
MOV(R0, IMM(1339));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1751);
printf("At L_JUMP_1751\n");
fflush(stdout);
L_JUMP_1751:
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
L_CLOS_EXIT_1749:
MOV(R1, INDD(IMM(59), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1741));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1739:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1738);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1739);

L_FOR1_END_1738:
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

L_FOR2_START_1737:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1736);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1737);

L_FOR2_END_1736:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1740);
L_CLOS_CODE_1741: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1735);
printf("at JUMP_SIMPLE_1735");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1735:

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
MOV(R0, IMM(59));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1743\n");
JUMP_EQ(L_JUMP_1743);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1743:
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
MOV(R0, IMM(26));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1742);
printf("At L_JUMP_1742\n");
fflush(stdout);
L_JUMP_1742:
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
L_CLOS_EXIT_1740:
MOV(R1, INDD(IMM(69), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1732));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1730:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1729);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1730);

L_FOR1_END_1729:
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

L_FOR2_START_1728:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1727);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1728);

L_FOR2_END_1727:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1731);
L_CLOS_CODE_1732: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1726);
printf("at JUMP_SIMPLE_1726");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1726:

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
MOV(R0, IMM(1339));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1734\n");
JUMP_EQ(L_JUMP_1734);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1734:
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
MOV(R0, IMM(26));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1733);
printf("At L_JUMP_1733\n");
fflush(stdout);
L_JUMP_1733:
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
L_CLOS_EXIT_1731:
MOV(R1, INDD(IMM(79), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1724));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1722:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1721);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1722);

L_FOR1_END_1721:
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

L_FOR2_START_1720:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1719);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1720);

L_FOR2_END_1719:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1723);
L_CLOS_CODE_1724: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1718);
printf("at JUMP_SIMPLE_1718");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1718:

 //TC-APPLIC 

MOV(R0,IMM(1341));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(19));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1725);
printf("At L_JUMP_1725\n");
fflush(stdout);
L_JUMP_1725:
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
L_CLOS_EXIT_1723:
MOV(R1, INDD(IMM(88), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1710));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1708:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1707);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1708);

L_FOR1_END_1707:
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

L_FOR2_START_1706:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1705);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1706);

L_FOR2_END_1705:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1709);
L_CLOS_CODE_1710: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1704);
printf("at JUMP_SIMPLE_1704");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1704:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1713); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1712);
		    L_THEN_1713:MOV(R0,IMM(1341));

		    JUMP(L_IF_EXIT_1711);
		    L_ELSE_1712:
		    
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1716\n");
JUMP_EQ(L_JUMP_1716);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1716:
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
MOV(R0, IMM(98));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1715\n");
JUMP_EQ(L_JUMP_1715);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1715:
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
MOV(R0, IMM(42));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1714);
printf("At L_JUMP_1714\n");
fflush(stdout);
L_JUMP_1714:
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

		    L_IF_EXIT_1711:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1709:
MOV(R1, INDD(IMM(98), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1695));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1693:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1692);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1693);

L_FOR1_END_1692:
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

L_FOR2_START_1691:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1690);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1691);

L_FOR2_END_1690:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1694);
L_CLOS_CODE_1695: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1689);
printf("at JUMP_SIMPLE_1689");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1689:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1703\n");
JUMP_EQ(L_JUMP_1703);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1703:
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
JUMP_NE(L_OR_END_1696);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1696);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1350));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1702\n");
JUMP_EQ(L_JUMP_1702);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1702:
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
		    JUMP_NE(L_THEN_1699); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1698);
		    L_THEN_1699:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1701\n");
JUMP_EQ(L_JUMP_1701);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1701:
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
MOV(R0, IMM(107));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
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

		    JUMP(L_IF_EXIT_1697);
		    L_ELSE_1698:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1697:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1696);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1696);
L_OR_END_1696:

POP(FP);
RETURN;
L_CLOS_EXIT_1694:
MOV(R1, INDD(IMM(107), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1686));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1684:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1683);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1684);

L_FOR1_END_1683:
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

L_FOR2_START_1682:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1681);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1682);

L_FOR2_END_1681:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1685);
L_CLOS_CODE_1686: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1680);
printf("at JUMP_SIMPLE_1680");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1680:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1365));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1363));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1688\n");
JUMP_EQ(L_JUMP_1688);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1688:
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
MOV(R0, IMM(88));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1687);
printf("At L_JUMP_1687\n");
fflush(stdout);
L_JUMP_1687:
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
L_CLOS_EXIT_1685:
MOV(R1, INDD(IMM(116), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1676));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1674:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1673);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1674);

L_FOR1_END_1673:
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

L_FOR2_START_1672:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1671);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1672);

L_FOR2_END_1671:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1675);
L_CLOS_CODE_1676: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1670);
printf("at JUMP_SIMPLE_1670");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1670:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1365));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1363));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1679\n");
JUMP_EQ(L_JUMP_1679);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1679:
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
MOV(R0, IMM(88));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1678\n");
JUMP_EQ(L_JUMP_1678);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1678:
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
MOV(R0, IMM(26));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1677);
printf("At L_JUMP_1677\n");
fflush(stdout);
L_JUMP_1677:
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
L_CLOS_EXIT_1675:
MOV(R1, INDD(IMM(124), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1636));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1634:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1633);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1634);

L_FOR1_END_1633:
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

L_FOR2_START_1632:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1631);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1632);

L_FOR2_END_1631:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1635);
L_CLOS_CODE_1636: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1630);
printf("at JUMP_SIMPLE_1630");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1630:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1660));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1658:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1657);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1658);

L_FOR1_END_1657:
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

L_FOR2_START_1656:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1655);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1656);

L_FOR2_END_1655:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1659);
L_CLOS_CODE_1660: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1654);
printf("at JUMP_SIMPLE_1654");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1654:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1667));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1665:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1664);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1665);

L_FOR1_END_1664:
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

L_FOR2_START_1663:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1662);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1663);

L_FOR2_END_1662:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1666);
L_CLOS_CODE_1667: 

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
printf("At L_JUMP_1669\n");
JUMP_EQ(L_JUMP_1669);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1669:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1668);
printf("At L_JUMP_1668\n");
fflush(stdout);
L_JUMP_1668:
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
L_CLOS_EXIT_1666:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1661);
printf("At L_JUMP_1661\n");
fflush(stdout);
L_JUMP_1661:
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
L_CLOS_EXIT_1659:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1644));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1642:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1641);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1642);

L_FOR1_END_1641:
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

L_FOR2_START_1640:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1639);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1640);

L_FOR2_END_1639:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1643);
L_CLOS_CODE_1644: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1638);
printf("at JUMP_SIMPLE_1638");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1638:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1651));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1649:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1648);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1649);

L_FOR1_END_1648:
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

L_FOR2_START_1647:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1646);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1647);

L_FOR2_END_1646:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1650);
L_CLOS_CODE_1651: 

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
printf("At L_JUMP_1653\n");
JUMP_EQ(L_JUMP_1653);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1653:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1652);
printf("At L_JUMP_1652\n");
fflush(stdout);
L_JUMP_1652:
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
L_CLOS_EXIT_1650:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1645);
printf("At L_JUMP_1645\n");
fflush(stdout);
L_JUMP_1645:
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
L_CLOS_EXIT_1643:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1637);
printf("At L_JUMP_1637\n");
fflush(stdout);
L_JUMP_1637:
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
L_CLOS_EXIT_1635:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1557));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1555:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1554);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1555);

L_FOR1_END_1554:
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

L_FOR2_START_1553:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1552);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1553);

L_FOR2_END_1552:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1556);
L_CLOS_CODE_1557: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1551);
printf("at JUMP_SIMPLE_1551");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1551:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1613));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1611:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1610);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1611);

L_FOR1_END_1610:
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

L_FOR2_START_1609:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1608);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1609);

L_FOR2_END_1608:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1612);
L_CLOS_CODE_1613: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1607);
printf("at JUMP_SIMPLE_1607");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1607:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1620));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1618:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1617);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1618);

L_FOR1_END_1617:
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

L_FOR2_START_1616:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1615);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1616);

L_FOR2_END_1615:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1619);
L_CLOS_CODE_1620: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1614);
printf("at JUMP_SIMPLE_1614");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1614:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1629\n");
JUMP_EQ(L_JUMP_1629);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1629:
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
		    JUMP_NE(L_THEN_1623); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1622);
		    L_THEN_1623:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_1621);
		    L_ELSE_1622:
		    
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1628\n");
JUMP_EQ(L_JUMP_1628);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1628:
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
printf("At L_JUMP_1627\n");
JUMP_EQ(L_JUMP_1627);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1627:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
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
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1625\n");
JUMP_EQ(L_JUMP_1625);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1625:
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
JUMP_EQ(L_JUMP_1624);
printf("At L_JUMP_1624\n");
fflush(stdout);
L_JUMP_1624:
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

		    L_IF_EXIT_1621:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1619:

POP(FP);
RETURN;
L_CLOS_EXIT_1612:
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1606\n");
JUMP_EQ(L_JUMP_1606);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1606:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1588));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1586:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1585);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1586);

L_FOR1_END_1585:
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

L_FOR2_START_1584:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1583);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1584);

L_FOR2_END_1583:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1587);
L_CLOS_CODE_1588: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1582);
printf("at JUMP_SIMPLE_1582");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1582:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1595));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1593:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1592);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1593);

L_FOR1_END_1592:
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

L_FOR2_START_1591:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1590);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1591);

L_FOR2_END_1590:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1594);
L_CLOS_CODE_1595: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1589);
printf("at JUMP_SIMPLE_1589");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1589:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1605\n");
JUMP_EQ(L_JUMP_1605);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1605:
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
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1604\n");
JUMP_EQ(L_JUMP_1604);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1604:
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
		    JUMP_NE(L_THEN_1598); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1597);
		    L_THEN_1598:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_1596);
		    L_ELSE_1597:
		    
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
MOV(R0, IMM(1312));
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
printf("At L_JUMP_1603\n");
JUMP_EQ(L_JUMP_1603);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1603:
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
printf("At L_JUMP_1602\n");
JUMP_EQ(L_JUMP_1602);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1602:
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
MOV(R0, IMM(1300));
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
printf("At L_JUMP_1601\n");
JUMP_EQ(L_JUMP_1601);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1601:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1600\n");
JUMP_EQ(L_JUMP_1600);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1600:
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
JUMP_EQ(L_JUMP_1599);
printf("At L_JUMP_1599\n");
fflush(stdout);
L_JUMP_1599:
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

		    L_IF_EXIT_1596:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1594:

POP(FP);
RETURN;
L_CLOS_EXIT_1587:
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1581\n");
JUMP_EQ(L_JUMP_1581);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1581:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1573));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1571:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1570);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1571);

L_FOR1_END_1570:
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

L_FOR2_START_1569:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1568);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1569);

L_FOR2_END_1568:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1572);
L_CLOS_CODE_1573: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1567);
printf("at JUMP_SIMPLE_1567");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1567:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1575));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1577:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1578);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1577);

L_FOR1_END_1578:
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

L_FOR2_START_1579:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1580);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1579);

L_FOR2_END_1580:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1576);
L_CLOS_CODE_1575: 
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
JUMP_EQ(L_JUMP_1574);
printf("At L_JUMP_1574\n");
fflush(stdout);
L_JUMP_1574:
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
L_CLOS_EXIT_1572:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1566);
printf("At L_JUMP_1566\n");
fflush(stdout);
L_JUMP_1566:
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
L_CLOS_EXIT_1564:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1558);
printf("At L_JUMP_1558\n");
fflush(stdout);
L_JUMP_1558:
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
L_CLOS_EXIT_1556:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1550\n");
JUMP_EQ(L_JUMP_1550);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1550:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(131), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1508));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1506:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1505);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1506);

L_FOR1_END_1505:
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

L_FOR2_START_1504:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1503);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1504);

L_FOR2_END_1503:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1507);
L_CLOS_CODE_1508: 

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1526));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1520);
printf("at JUMP_SIMPLE_1520");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1520:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1532));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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

L_FOR2_START_1528:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1527);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1541));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1539:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1538);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1539);

L_FOR1_END_1538:
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

L_FOR2_START_1537:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1536);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1537);

L_FOR2_END_1536:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1540);
L_CLOS_CODE_1541: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1535);
printf("at JUMP_SIMPLE_1535");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1535:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1547));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1549\n");
JUMP_EQ(L_JUMP_1549);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1549:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
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
L_CLOS_EXIT_1546:

POP(FP);
RETURN;
L_CLOS_EXIT_1540:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1534\n");
JUMP_EQ(L_JUMP_1534);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1534:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1533);
printf("At L_JUMP_1533\n");
fflush(stdout);
L_JUMP_1533:
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
L_CLOS_EXIT_1531:

POP(FP);
RETURN;
L_CLOS_EXIT_1525:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1519\n");
JUMP_EQ(L_JUMP_1519);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1519:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1516));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1514:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1513);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1514);

L_FOR1_END_1513:
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

L_FOR2_START_1512:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1511);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1512);

L_FOR2_END_1511:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1515);
L_CLOS_CODE_1516: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1510);
printf("at JUMP_SIMPLE_1510");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1510:

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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1518\n");
JUMP_EQ(L_JUMP_1518);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1518:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1517);
printf("At L_JUMP_1517\n");
fflush(stdout);
L_JUMP_1517:
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
L_CLOS_EXIT_1515:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1509);
printf("At L_JUMP_1509\n");
fflush(stdout);
L_JUMP_1509:
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
L_CLOS_EXIT_1507:
MOV(R1, INDD(IMM(137), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1487));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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

L_FOR2_START_1483:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1482);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
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
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1481);
printf("at JUMP_SIMPLE_1481");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1481:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1494));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1492:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1491);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1492);

L_FOR1_END_1491:
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

L_FOR2_START_1490:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1489);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1490);

L_FOR2_END_1489:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1493);
L_CLOS_CODE_1494: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1488);
printf("at JUMP_SIMPLE_1488");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1488:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1497); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1496);
		    L_THEN_1497:MOV(R0,IMM(1341));

		    JUMP(L_IF_EXIT_1495);
		    L_ELSE_1496:
		    
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
MOV(R0, IMM(1312));
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
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1500\n");
JUMP_EQ(L_JUMP_1500);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1500:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1499\n");
JUMP_EQ(L_JUMP_1499);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1499:
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
MOV(R0, IMM(1320));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1498);
printf("At L_JUMP_1498\n");
fflush(stdout);
L_JUMP_1498:
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

		    L_IF_EXIT_1495:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1493:

POP(FP);
RETURN;
L_CLOS_EXIT_1486:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1473));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1471:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1470);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1471);

L_FOR1_END_1470:
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

L_FOR2_START_1469:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1468);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1469);

L_FOR2_END_1468:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1472);
L_CLOS_CODE_1473: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1467);
printf("at JUMP_SIMPLE_1467");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1467:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1479));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1477:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1476);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1477);

L_FOR1_END_1476:
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

L_FOR2_START_1475:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1474);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1475);

L_FOR2_END_1474:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1478);
L_CLOS_CODE_1479: 

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
JUMP_EQ(L_JUMP_1480);
printf("At L_JUMP_1480\n");
fflush(stdout);
L_JUMP_1480:
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
L_CLOS_EXIT_1478:

POP(FP);
RETURN;
L_CLOS_EXIT_1472:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1466\n");
JUMP_EQ(L_JUMP_1466);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1466:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1450));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1448:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1447);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1448);

L_FOR1_END_1447:
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

L_FOR2_START_1446:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1445);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1446);

L_FOR2_END_1445:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1449);
L_CLOS_CODE_1450: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1444);
printf("at JUMP_SIMPLE_1444");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1444:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1457));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1455:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1454);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1455);

L_FOR1_END_1454:
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

L_FOR2_START_1453:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1452);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1453);

L_FOR2_END_1452:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1456);
L_CLOS_CODE_1457: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1451);
printf("at JUMP_SIMPLE_1451");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1451:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
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
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1460); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1459);
		    L_THEN_1460:MOV(R0,IMM(1322));

		    JUMP(L_IF_EXIT_1458);
		    L_ELSE_1459:
		    
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1464\n");
JUMP_EQ(L_JUMP_1464);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1464:
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
printf("At L_JUMP_1463\n");
JUMP_EQ(L_JUMP_1463);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1463:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1462\n");
JUMP_EQ(L_JUMP_1462);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1462:
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
MOV(R0, IMM(1390));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1461);
printf("At L_JUMP_1461\n");
fflush(stdout);
L_JUMP_1461:
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

		    L_IF_EXIT_1458:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1456:

POP(FP);
RETURN;
L_CLOS_EXIT_1449:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1436));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1434:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1433);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1434);

L_FOR1_END_1433:
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

L_FOR2_START_1432:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1431);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1432);

L_FOR2_END_1431:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1435);
L_CLOS_CODE_1436: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1430);
printf("at JUMP_SIMPLE_1430");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1430:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1442));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1440:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1439);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1440);

L_FOR1_END_1439:
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

L_FOR2_START_1438:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1437);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1438);

L_FOR2_END_1437:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1441);
L_CLOS_CODE_1442: 

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
JUMP_EQ(L_JUMP_1443);
printf("At L_JUMP_1443\n");
fflush(stdout);
L_JUMP_1443:
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
L_CLOS_EXIT_1441:

POP(FP);
RETURN;
L_CLOS_EXIT_1435:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1429\n");
JUMP_EQ(L_JUMP_1429);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1429:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(147), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1423));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1425:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1426);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1425);

L_FOR1_END_1426:
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

L_FOR2_START_1427:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1428);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1427);

L_FOR2_END_1428:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1424);
L_CLOS_CODE_1423: 
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
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1422\n");
JUMP_EQ(L_JUMP_1422);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1422:
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
		    JUMP_NE(L_THEN_1418); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1417);
		    L_THEN_1418:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1341));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1330));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1421);
printf("At L_JUMP_1421\n");
fflush(stdout);
L_JUMP_1421:
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

		    JUMP(L_IF_EXIT_1416);
		    L_ELSE_1417:
		    
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1420\n");
JUMP_EQ(L_JUMP_1420);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1420:
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
MOV(R0, IMM(1330));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1419);
printf("At L_JUMP_1419\n");
fflush(stdout);
L_JUMP_1419:
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

		    L_IF_EXIT_1416:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1424:
MOV(R1, INDD(IMM(152), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1410));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1412:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1413);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1412);

L_FOR1_END_1413:
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

L_FOR2_START_1414:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1415);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1414);

L_FOR2_END_1415:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1411);
L_CLOS_CODE_1410: 
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
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1409\n");
JUMP_EQ(L_JUMP_1409);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1409:
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
		    JUMP_NE(L_THEN_1405); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1404);
		    L_THEN_1405:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1322));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1398));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1408);
printf("At L_JUMP_1408\n");
fflush(stdout);
L_JUMP_1408:
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

		    JUMP(L_IF_EXIT_1403);
		    L_ELSE_1404:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1407\n");
JUMP_EQ(L_JUMP_1407);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1407:
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
MOV(R0, IMM(1398));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1406);
printf("At L_JUMP_1406\n");
fflush(stdout);
L_JUMP_1406:
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

		    L_IF_EXIT_1403:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1411:
MOV(R1, INDD(IMM(157), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1363));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1361:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1360);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1361);

L_FOR1_END_1360:
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

L_FOR2_START_1359:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1358);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1359);

L_FOR2_END_1358:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1362);
L_CLOS_CODE_1363: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1357);
printf("at JUMP_SIMPLE_1357");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1357:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1385));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1379);
printf("at JUMP_SIMPLE_1379");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1379:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1392));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1390:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1389);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1390);

L_FOR1_END_1389:
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

L_FOR2_START_1388:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1387);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1388);

L_FOR2_END_1387:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1391);
L_CLOS_CODE_1392: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1386);
printf("at JUMP_SIMPLE_1386");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1386:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1402\n");
JUMP_EQ(L_JUMP_1402);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1402:
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
JUMP_NE(L_OR_END_1393);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1393);
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1401\n");
JUMP_EQ(L_JUMP_1401);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1401:
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
printf("At L_JUMP_1400\n");
JUMP_EQ(L_JUMP_1400);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1400:
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
		    JUMP_NE(L_THEN_1396); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1395);
		    L_THEN_1396:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1399\n");
JUMP_EQ(L_JUMP_1399);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1399:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1398\n");
JUMP_EQ(L_JUMP_1398);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1398:
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
JUMP_EQ(L_JUMP_1397);
printf("At L_JUMP_1397\n");
fflush(stdout);
L_JUMP_1397:
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

		    JUMP(L_IF_EXIT_1394);
		    L_ELSE_1395:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1394:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1393);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1393);
L_OR_END_1393:

POP(FP);
RETURN;
L_CLOS_EXIT_1391:

POP(FP);
RETURN;
L_CLOS_EXIT_1384:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1371));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1369:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1368);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1369);

L_FOR1_END_1368:
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

L_FOR2_START_1367:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1366);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1367);

L_FOR2_END_1366:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1370);
L_CLOS_CODE_1371: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1365);
printf("at JUMP_SIMPLE_1365");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1365:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1373));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1375:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1376);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1375);

L_FOR1_END_1376:
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

L_FOR2_START_1377:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1378);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1377);

L_FOR2_END_1378:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1374);
L_CLOS_CODE_1373: 
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
JUMP_EQ(L_JUMP_1372);
printf("At L_JUMP_1372\n");
fflush(stdout);
L_JUMP_1372:
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
L_CLOS_EXIT_1374:

POP(FP);
RETURN;
L_CLOS_EXIT_1370:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1364);
printf("At L_JUMP_1364\n");
fflush(stdout);
L_JUMP_1364:
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
L_CLOS_EXIT_1362:
MOV(R1, INDD(IMM(166), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1351));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1353:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1354);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1353);

L_FOR1_END_1354:
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

L_FOR2_START_1355:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1356);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1355);

L_FOR2_END_1356:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1352);
L_CLOS_CODE_1351: 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1333));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1331:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1330);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1331);

L_FOR1_END_1330:
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

L_FOR2_START_1329:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1328);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1329);

L_FOR2_END_1328:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1332);
L_CLOS_CODE_1333: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1327);
printf("at JUMP_SIMPLE_1327");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1327:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1340));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1338:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1337);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1338);

L_FOR1_END_1337:
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

L_FOR2_START_1336:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1335);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1336);

L_FOR2_END_1335:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1339);
L_CLOS_CODE_1340: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1334);
printf("at JUMP_SIMPLE_1334");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1334:

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
MOV(R0, IMM(1300));
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1349\n");
JUMP_EQ(L_JUMP_1349);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1349:
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
JUMP_NE(L_OR_END_1341);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1341);
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1348\n");
JUMP_EQ(L_JUMP_1348);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1348:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1347\n");
JUMP_EQ(L_JUMP_1347);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1347:
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
		    JUMP_NE(L_THEN_1344); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1343);
		    L_THEN_1344:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
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
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1345);
printf("At L_JUMP_1345\n");
fflush(stdout);
L_JUMP_1345:
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

		    JUMP(L_IF_EXIT_1342);
		    L_ELSE_1343:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1342:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1341);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1341);
L_OR_END_1341:

POP(FP);
RETURN;
L_CLOS_EXIT_1339:

POP(FP);
RETURN;
L_CLOS_EXIT_1332:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1325));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1323:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1322);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1323);

L_FOR1_END_1322:
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

L_FOR2_START_1321:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1320);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1321);

L_FOR2_END_1320:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1324);
L_CLOS_CODE_1325: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1319);
printf("at JUMP_SIMPLE_1319");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1319:

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
JUMP_EQ(L_JUMP_1326);
printf("At L_JUMP_1326\n");
fflush(stdout);
L_JUMP_1326:
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
L_CLOS_EXIT_1324:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1318);
printf("At L_JUMP_1318\n");
fflush(stdout);
L_JUMP_1318:
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
L_CLOS_EXIT_1352:
MOV(R1, INDD(IMM(176), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1292));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1299));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1293);
printf("at JUMP_SIMPLE_1293");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1293:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1317\n");
JUMP_EQ(L_JUMP_1317);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1317:
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
JUMP_NE(L_OR_END_1300);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1300);
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1314));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1312:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1311);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1312);

L_FOR1_END_1311:
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

L_FOR2_START_1310:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1309);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1310);

L_FOR2_END_1309:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1313);
L_CLOS_CODE_1314: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1308);
printf("at JUMP_SIMPLE_1308");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1308:

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
MOV(R0, IMM(19));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1316\n");
JUMP_EQ(L_JUMP_1316);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1316:
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
MOV(R0, IMM(26));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1315);
printf("At L_JUMP_1315\n");
fflush(stdout);
L_JUMP_1315:
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
L_CLOS_EXIT_1313:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(176));
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
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1303); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1302);
		    L_THEN_1303:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
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
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1304);
printf("At L_JUMP_1304\n");
fflush(stdout);
L_JUMP_1304:
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

		    JUMP(L_IF_EXIT_1301);
		    L_ELSE_1302:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1301:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1300);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1300);
L_OR_END_1300:

POP(FP);
RETURN;
L_CLOS_EXIT_1298:

POP(FP);
RETURN;
L_CLOS_EXIT_1291:
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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1270);
printf("at JUMP_SIMPLE_1270");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1270:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1282));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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

L_FOR2_START_1278:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1277);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1285\n");
JUMP_EQ(L_JUMP_1285);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1285:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1284\n");
JUMP_EQ(L_JUMP_1284);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1284:
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

POP(FP);
RETURN;
L_CLOS_EXIT_1275:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1269\n");
JUMP_EQ(L_JUMP_1269);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1269:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(182), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1237));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1235:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1234);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1235);

L_FOR1_END_1234:
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

L_FOR2_START_1233:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1232);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1233);

L_FOR2_END_1232:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1236);
L_CLOS_CODE_1237: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_1231);
printf("at JUMP_SIMPLE_1231");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1231:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1253));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1247);
printf("at JUMP_SIMPLE_1247");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1247:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1260));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1254);
printf("at JUMP_SIMPLE_1254");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1254:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1268\n");
JUMP_EQ(L_JUMP_1268);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1268:
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
		    JUMP_NE(L_THEN_1263); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1262);
		    L_THEN_1263:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1261);
		    L_ELSE_1262:
		    
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1267\n");
JUMP_EQ(L_JUMP_1267);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1267:
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
printf("At L_JUMP_1266\n");
JUMP_EQ(L_JUMP_1266);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1266:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1265\n");
JUMP_EQ(L_JUMP_1265);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1265:
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
JUMP_EQ(L_JUMP_1264);
printf("At L_JUMP_1264\n");
fflush(stdout);
L_JUMP_1264:
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

		    L_IF_EXIT_1261:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1259:

POP(FP);
RETURN;
L_CLOS_EXIT_1252:
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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1239);
printf("at JUMP_SIMPLE_1239");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1239:

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
L_CLOS_EXIT_1244:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1238);
printf("At L_JUMP_1238\n");
fflush(stdout);
L_JUMP_1238:
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
L_CLOS_EXIT_1236:
MOV(R1, INDD(IMM(191), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1221));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1219:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1218);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1219);

L_FOR1_END_1218:
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

L_FOR2_START_1217:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1216);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1217);

L_FOR2_END_1216:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1220);
L_CLOS_CODE_1221: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1215);
printf("at JUMP_SIMPLE_1215");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1215:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1228));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1226:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1225);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1226);

L_FOR1_END_1225:
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

L_FOR2_START_1224:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1223);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1224);

L_FOR2_END_1223:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1227);
L_CLOS_CODE_1228: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1222);
printf("at JUMP_SIMPLE_1222");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1222:

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
printf("At L_JUMP_1230\n");
JUMP_EQ(L_JUMP_1230);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1230:
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
JUMP_EQ(L_JUMP_1229);
printf("At L_JUMP_1229\n");
fflush(stdout);
L_JUMP_1229:
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
L_CLOS_EXIT_1227:

POP(FP);
RETURN;
L_CLOS_EXIT_1220:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1200));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1198:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1197);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1198);

L_FOR1_END_1197:
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

L_FOR2_START_1196:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1195);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1196);

L_FOR2_END_1195:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1199);
L_CLOS_CODE_1200: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1194);
printf("at JUMP_SIMPLE_1194");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1194:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1206));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1204:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1203);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1204);

L_FOR1_END_1203:
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

L_FOR2_START_1202:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1201);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1202);

L_FOR2_END_1201:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1205);
L_CLOS_CODE_1206: 

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1214));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1212:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1211);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1212);

L_FOR1_END_1211:
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

L_FOR2_START_1210:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1209);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1210);

L_FOR2_END_1209:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1213);
L_CLOS_CODE_1214: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1208);
printf("at JUMP_SIMPLE_1208");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1208:
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_1213:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(191));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1207);
printf("At L_JUMP_1207\n");
fflush(stdout);
L_JUMP_1207:
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
L_CLOS_EXIT_1205:

POP(FP);
RETURN;
L_CLOS_EXIT_1199:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1193\n");
JUMP_EQ(L_JUMP_1193);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1193:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(202), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1192\n");
JUMP_EQ(L_JUMP_1192);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1192:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(210), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1191\n");
JUMP_EQ(L_JUMP_1191);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1191:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(218), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1190\n");
JUMP_EQ(L_JUMP_1190);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1190:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(226), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1189\n");
JUMP_EQ(L_JUMP_1189);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1189:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(234), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(210));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1188\n");
JUMP_EQ(L_JUMP_1188);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1188:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(243), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(218));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1187\n");
JUMP_EQ(L_JUMP_1187);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1187:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(252), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(226));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1186\n");
JUMP_EQ(L_JUMP_1186);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1186:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(261), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(234));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1185\n");
JUMP_EQ(L_JUMP_1185);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1185:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(270), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(210));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1184\n");
JUMP_EQ(L_JUMP_1184);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1184:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(279), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(218));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1183\n");
JUMP_EQ(L_JUMP_1183);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1183:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(288), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(226));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1182\n");
JUMP_EQ(L_JUMP_1182);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1182:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(297), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(234));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1181\n");
JUMP_EQ(L_JUMP_1181);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1181:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(306), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(243));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1180\n");
JUMP_EQ(L_JUMP_1180);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1180:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(316), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(252));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1179\n");
JUMP_EQ(L_JUMP_1179);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1179:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(326), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(261));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1178\n");
JUMP_EQ(L_JUMP_1178);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1178:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(336), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(270));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1177\n");
JUMP_EQ(L_JUMP_1177);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1177:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(346), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(279));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1176\n");
JUMP_EQ(L_JUMP_1176);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1176:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(356), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(288));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1175\n");
JUMP_EQ(L_JUMP_1175);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1175:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(366), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcaddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(297));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1174\n");
JUMP_EQ(L_JUMP_1174);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1174:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(376), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcadddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(306));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1173\n");
JUMP_EQ(L_JUMP_1173);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1173:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(386), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(243));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1172\n");
JUMP_EQ(L_JUMP_1172);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1172:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(396), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(252));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1171\n");
JUMP_EQ(L_JUMP_1171);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1171:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(406), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(261));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1170\n");
JUMP_EQ(L_JUMP_1170);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1170:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(416), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(270));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1169\n");
JUMP_EQ(L_JUMP_1169);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1169:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(426), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(279));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1168\n");
JUMP_EQ(L_JUMP_1168);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1168:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(436), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(288));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1167\n");
JUMP_EQ(L_JUMP_1167);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1167:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(446), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcdddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(297));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1166\n");
JUMP_EQ(L_JUMP_1166);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1166:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(456), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEcddddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(306));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
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
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(466), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1127));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1125:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1124);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1125);

L_FOR1_END_1124:
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

L_FOR2_START_1123:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1122);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1123);

L_FOR2_END_1122:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1126);
L_CLOS_CODE_1127: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1121);
printf("at JUMP_SIMPLE_1121");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1121:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1149));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1147:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1146);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1147);

L_FOR1_END_1146:
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

L_FOR2_START_1145:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1144);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1145);

L_FOR2_END_1144:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1148);
L_CLOS_CODE_1149: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1143);
printf("at JUMP_SIMPLE_1143");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1143:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1156));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1154:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1153);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1154);

L_FOR1_END_1153:
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

L_FOR2_START_1152:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1151);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1152);

L_FOR2_END_1151:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1155);
L_CLOS_CODE_1156: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1150);
printf("at JUMP_SIMPLE_1150");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1150:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1159); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1158);
		    L_THEN_1159:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1157);
		    L_ELSE_1158:
		    
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
MOV(R0, IMM(1312));
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1162\n");
JUMP_EQ(L_JUMP_1162);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1162:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1161\n");
JUMP_EQ(L_JUMP_1161);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1161:
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
JUMP_EQ(L_JUMP_1160);
printf("At L_JUMP_1160\n");
fflush(stdout);
L_JUMP_1160:
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

		    L_IF_EXIT_1157:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1155:

POP(FP);
RETURN;
L_CLOS_EXIT_1148:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1135));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1133:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1132);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1133);

L_FOR1_END_1132:
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

L_FOR2_START_1131:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1130);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1131);

L_FOR2_END_1130:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1134);
L_CLOS_CODE_1135: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1129);
printf("at JUMP_SIMPLE_1129");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1129:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1141));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1139:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1138);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1139);

L_FOR1_END_1138:
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

L_FOR2_START_1137:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1136);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1137);

L_FOR2_END_1136:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1140);
L_CLOS_CODE_1141: 

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
JUMP_EQ(L_JUMP_1142);
printf("At L_JUMP_1142\n");
fflush(stdout);
L_JUMP_1142:
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
L_CLOS_EXIT_1140:

POP(FP);
RETURN;
L_CLOS_EXIT_1134:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1128);
printf("At L_JUMP_1128\n");
fflush(stdout);
L_JUMP_1128:
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
L_CLOS_EXIT_1126:
MOV(R1, INDD(IMM(497), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1077));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1075:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1074);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1075);

L_FOR1_END_1074:
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

L_FOR2_START_1073:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1072);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1073);

L_FOR2_END_1072:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1076);
L_CLOS_CODE_1077: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1071);
printf("at JUMP_SIMPLE_1071");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1071:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1105));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
CMP(FPARG(1), IMM(2));
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
PUSH(IMM(4));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1112));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1120\n");
JUMP_EQ(L_JUMP_1120);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1120:
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
		    JUMP_NE(L_THEN_1115); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1114);
		    L_THEN_1115:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_1113);
		    L_ELSE_1114:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1119\n");
JUMP_EQ(L_JUMP_1119);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1119:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1118\n");
JUMP_EQ(L_JUMP_1118);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1118:
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
printf("At L_JUMP_1117\n");
JUMP_EQ(L_JUMP_1117);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1117:
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
JUMP_EQ(L_JUMP_1116);
printf("At L_JUMP_1116\n");
fflush(stdout);
L_JUMP_1116:
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

		    L_IF_EXIT_1113:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1111:

POP(FP);
RETURN;
L_CLOS_EXIT_1104:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1085));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1083:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1082);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1083);

L_FOR1_END_1082:
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

L_FOR2_START_1081:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1080);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1081);

L_FOR2_END_1080:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1084);
L_CLOS_CODE_1085: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1079);
printf("at JUMP_SIMPLE_1079");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1079:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1091));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1089:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1088);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1089);

L_FOR1_END_1088:
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

L_FOR2_START_1087:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1086);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1087);

L_FOR2_END_1086:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1090);
L_CLOS_CODE_1091: 

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
MOV(R0, IMM(1284));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1094); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1093);
		    L_THEN_1094:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_1092);
		    L_ELSE_1093:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
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
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
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
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1095);
printf("At L_JUMP_1095\n");
fflush(stdout);
L_JUMP_1095:
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

		    L_IF_EXIT_1092:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1090:

POP(FP);
RETURN;
L_CLOS_EXIT_1084:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1078);
printf("At L_JUMP_1078\n");
fflush(stdout);
L_JUMP_1078:
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
L_CLOS_EXIT_1076:
MOV(R1, INDD(IMM(527), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1060));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1058:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1057);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1058);

L_FOR1_END_1057:
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

L_FOR2_START_1056:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1055);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1056);

L_FOR2_END_1055:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1059);
L_CLOS_CODE_1060: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1054);
printf("at JUMP_SIMPLE_1054");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1054:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1067));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_1061);
printf("at JUMP_SIMPLE_1061");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1061:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
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
printf("At L_JUMP_1070\n");
JUMP_EQ(L_JUMP_1070);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1070:
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
MOV(R0, IMM(1415));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1069\n");
JUMP_EQ(L_JUMP_1069);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1069:
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
JUMP_EQ(L_JUMP_1068);
printf("At L_JUMP_1068\n");
fflush(stdout);
L_JUMP_1068:
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
L_CLOS_EXIT_1066:

POP(FP);
RETURN;
L_CLOS_EXIT_1059:
MOV(R1, INDD(IMM(539), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE<

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1339));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
MOV(R1, INDD(IMM(544), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE<=

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(69));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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

 // DEFINE>

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(59));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
MOV(R1, INDD(IMM(555), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINE>=

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(79));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
MOV(R1, INDD(IMM(561), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1293));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(539));
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
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1048\n");
JUMP_EQ(L_JUMP_1048);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1048:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(571), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar<=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(69));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(539));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1047\n");
JUMP_EQ(L_JUMP_1047);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1047:
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
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1046\n");
JUMP_EQ(L_JUMP_1046);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1046:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(582), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar<?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1339));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(539));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1045\n");
JUMP_EQ(L_JUMP_1045);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1045:
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
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1044\n");
JUMP_EQ(L_JUMP_1044);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1044:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(592), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar>=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(79));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(539));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1043\n");
JUMP_EQ(L_JUMP_1043);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1043:
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
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1042\n");
JUMP_EQ(L_JUMP_1042);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1042:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(603), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEchar>?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(59));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(539));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1041\n");
JUMP_EQ(L_JUMP_1041);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1041:
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
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1040\n");
JUMP_EQ(L_JUMP_1040);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1040:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(613), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1034));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1032:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1031);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1032);

L_FOR1_END_1031:
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

L_FOR2_START_1030:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1029);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1030);

L_FOR2_END_1029:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1033);
L_CLOS_CODE_1034: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1028);
printf("at JUMP_SIMPLE_1028");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1028:
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
MOV(R0, IMM(582));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1039\n");
JUMP_EQ(L_JUMP_1039);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1039:
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
		    JUMP_NE(L_THEN_1037); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1036);
		    L_THEN_1037:
 //TC-APPLIC 

MOV(R0,IMM(1419));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(582));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1038);
printf("At L_JUMP_1038\n");
fflush(stdout);
L_JUMP_1038:
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

		    JUMP(L_IF_EXIT_1035);
		    L_ELSE_1036:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1035:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1033:
MOV(R1, INDD(IMM(632), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1022));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1020:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1019);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1020);

L_FOR1_END_1019:
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

L_FOR2_START_1018:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1017);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1018);

L_FOR2_END_1017:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1021);
L_CLOS_CODE_1022: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_1016);
printf("at JUMP_SIMPLE_1016");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_1016:
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
MOV(R0, IMM(582));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1025); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1024);
		    L_THEN_1025:
 //TC-APPLIC 

MOV(R0,IMM(1423));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(582));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_1026);
printf("At L_JUMP_1026\n");
fflush(stdout);
L_JUMP_1026:
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

		    JUMP(L_IF_EXIT_1023);
		    L_ELSE_1024:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1023:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1021:
MOV(R1, INDD(IMM(651), IMM(1)));
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
MOV(R0, IMM(1415));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1015\n");
JUMP_EQ(L_JUMP_1015);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1015:
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
MOV(R0, IMM(1415));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1014\n");
JUMP_EQ(L_JUMP_1014);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1014:
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
MOV(R0, IMM(152));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1013\n");
JUMP_EQ(L_JUMP_1013);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1013:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_998));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_996:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_995);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_996);

L_FOR1_END_995:
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

L_FOR2_START_994:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_993);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_994);

L_FOR2_END_993:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_997);
L_CLOS_CODE_998: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_992);
printf("at JUMP_SIMPLE_992");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_992:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1005));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1003:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1002);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1003);

L_FOR1_END_1002:
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

L_FOR2_START_1001:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1000);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1001);

L_FOR2_END_1000:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1004);
L_CLOS_CODE_1005: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_999);
printf("at JUMP_SIMPLE_999");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_999:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(651));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_1012\n");
JUMP_EQ(L_JUMP_1012);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_1012:
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
		    JUMP_NE(L_THEN_1008); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1007);
		    L_THEN_1008:
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
MOV(R0, IMM(1415));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
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
JUMP_EQ(L_JUMP_1009);
printf("At L_JUMP_1009\n");
fflush(stdout);
L_JUMP_1009:
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

		    JUMP(L_IF_EXIT_1006);
		    L_ELSE_1007:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_1006:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1004:

POP(FP);
RETURN;
L_CLOS_EXIT_997:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_991\n");
JUMP_EQ(L_JUMP_991);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_991:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(666), IMM(1)));
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
MOV(R0, IMM(1415));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_990\n");
JUMP_EQ(L_JUMP_990);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_990:
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
MOV(R0, IMM(1415));
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_973));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_971:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_970);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_971);

L_FOR1_END_970:
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

L_FOR2_START_969:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_968);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_969);

L_FOR2_END_968:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_972);
L_CLOS_CODE_973: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_967);
printf("at JUMP_SIMPLE_967");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_967:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_980));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_978:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_977);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_978);

L_FOR1_END_977:
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

L_FOR2_START_976:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_975);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_976);

L_FOR2_END_975:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_979);
L_CLOS_CODE_980: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_974);
printf("at JUMP_SIMPLE_974");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_974:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(632));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_983); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_982);
		    L_THEN_983:
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
MOV(R0, IMM(1415));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_986\n");
JUMP_EQ(L_JUMP_986);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_986:
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
MOV(R0, IMM(142));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_985\n");
JUMP_EQ(L_JUMP_985);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_985:
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
JUMP_EQ(L_JUMP_984);
printf("At L_JUMP_984\n");
fflush(stdout);
L_JUMP_984:
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

		    JUMP(L_IF_EXIT_981);
		    L_ELSE_982:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_981:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_979:

POP(FP);
RETURN;
L_CLOS_EXIT_972:
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
MOV(R1, INDD(IMM(683), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_962));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_960:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_959);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_960);

L_FOR1_END_959:
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

L_FOR2_START_958:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_957);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_958);

L_FOR2_END_957:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_961);
L_CLOS_CODE_962: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_956);
printf("at JUMP_SIMPLE_956");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_956:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(666));
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

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(666));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_964\n");
JUMP_EQ(L_JUMP_964);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_964:
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
MOV(R0, IMM(582));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_963);
printf("At L_JUMP_963\n");
fflush(stdout);
L_JUMP_963:
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
L_CLOS_EXIT_961:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(697), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_951));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_949:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_948);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_949);

L_FOR1_END_948:
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

L_FOR2_START_947:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_946);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_947);

L_FOR2_END_946:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_950);
L_CLOS_CODE_951: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_945);
printf("at JUMP_SIMPLE_945");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_945:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(666));
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
MOV(R0, IMM(666));
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
MOV(R0, IMM(592));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_952);
printf("At L_JUMP_952\n");
fflush(stdout);
L_JUMP_952:
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
L_CLOS_EXIT_950:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_944\n");
JUMP_EQ(L_JUMP_944);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_944:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(710), IMM(1)));
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

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(666));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_943\n");
JUMP_EQ(L_JUMP_943);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_943:
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
MOV(R0, IMM(666));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_942\n");
JUMP_EQ(L_JUMP_942);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_942:
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
MOV(R0, IMM(571));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_941);
printf("At L_JUMP_941\n");
fflush(stdout);
L_JUMP_941:
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
L_CLOS_EXIT_939:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
MOV(R1, INDD(IMM(723), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_929));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_927:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_926);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_927);

L_FOR1_END_926:
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

L_FOR2_START_925:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_924);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_925);

L_FOR2_END_924:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_928);
L_CLOS_CODE_929: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_923);
printf("at JUMP_SIMPLE_923");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_923:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(666));
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
MOV(R0, IMM(666));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_931\n");
JUMP_EQ(L_JUMP_931);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_931:
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
MOV(R0, IMM(613));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_930);
printf("At L_JUMP_930\n");
fflush(stdout);
L_JUMP_930:
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
L_CLOS_EXIT_928:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_922\n");
JUMP_EQ(L_JUMP_922);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_922:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(736), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_918));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_916:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_915);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_916);

L_FOR1_END_915:
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

L_FOR2_START_914:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_913);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_914);

L_FOR2_END_913:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_917);
L_CLOS_CODE_918: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_912);
printf("at JUMP_SIMPLE_912");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_912:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(666));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_921\n");
JUMP_EQ(L_JUMP_921);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_921:
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
MOV(R0, IMM(666));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_920\n");
JUMP_EQ(L_JUMP_920);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_920:
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
MOV(R0, IMM(603));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_919);
printf("At L_JUMP_919\n");
fflush(stdout);
L_JUMP_919:
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
L_CLOS_EXIT_917:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(750), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_895));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_893:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_892);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_893);

L_FOR1_END_892:
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

L_FOR2_START_891:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_890);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_891);

L_FOR2_END_890:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_894);
L_CLOS_CODE_895: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_889);
printf("at JUMP_SIMPLE_889");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_889:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_902));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_900:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_899);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_900);

L_FOR1_END_899:
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

L_FOR2_START_898:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_897);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_898);

L_FOR2_END_897:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_901);
L_CLOS_CODE_902: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_896);
printf("at JUMP_SIMPLE_896");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_896:
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
MOV(R0, IMM(19));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_905); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_904);
		    L_THEN_905:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_903);
		    L_ELSE_904:
		    
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
printf("At L_JUMP_909\n");
JUMP_EQ(L_JUMP_909);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_909:
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
MOV(R0, IMM(1373));
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
MOV(R0,IMM(1322));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
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
JUMP_EQ(L_JUMP_906);
printf("At L_JUMP_906\n");
fflush(stdout);
L_JUMP_906:
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

		    L_IF_EXIT_903:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_901:

POP(FP);
RETURN;
L_CLOS_EXIT_894:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_878));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_876:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_875);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_876);

L_FOR1_END_875:
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

L_FOR2_START_874:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_873);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_874);

L_FOR2_END_873:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_877);
L_CLOS_CODE_878: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_872);
printf("at JUMP_SIMPLE_872");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_872:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_885));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_883:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_882);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_883);

L_FOR1_END_882:
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

L_FOR2_START_881:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_880);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_881);

L_FOR2_END_880:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_884);
L_CLOS_CODE_885: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_879);
printf("at JUMP_SIMPLE_879");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_879:

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1322));
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
printf("At L_JUMP_888\n");
JUMP_EQ(L_JUMP_888);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_888:
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
MOV(R0, IMM(152));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_887\n");
JUMP_EQ(L_JUMP_887);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_887:
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
JUMP_EQ(L_JUMP_886);
printf("At L_JUMP_886\n");
fflush(stdout);
L_JUMP_886:
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
L_CLOS_EXIT_884:

POP(FP);
RETURN;
L_CLOS_EXIT_877:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_871\n");
JUMP_EQ(L_JUMP_871);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_871:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(766), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_820));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_818:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_817);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_818);

L_FOR1_END_817:
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

L_FOR2_START_816:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_815);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_816);

L_FOR2_END_815:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_819);
L_CLOS_CODE_820: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_814);
printf("at JUMP_SIMPLE_814");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_814:

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(98));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_870\n");
JUMP_EQ(L_JUMP_870);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_870:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_828));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_826:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_825);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_826);

L_FOR1_END_825:
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

L_FOR2_START_824:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_823);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_824);

L_FOR2_END_823:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_827);
L_CLOS_CODE_828: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_822);
printf("at JUMP_SIMPLE_822");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_822:

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
printf("At L_JUMP_869\n");
JUMP_EQ(L_JUMP_869);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_869:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_836));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_834:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_833);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_834);

L_FOR1_END_833:
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

L_FOR2_START_832:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_831);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_832);

L_FOR2_END_831:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_835);
L_CLOS_CODE_836: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_830);
printf("at JUMP_SIMPLE_830");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_830:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_852));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_850:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_849);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_850);

L_FOR1_END_849:
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

L_FOR2_START_848:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_847);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_848);

L_FOR2_END_847:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_851);
L_CLOS_CODE_852: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_846);
printf("at JUMP_SIMPLE_846");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_846:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_859));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_853);
printf("at JUMP_SIMPLE_853");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_853:
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
MOV(R0, IMM(19));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_868\n");
JUMP_EQ(L_JUMP_868);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_868:
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
		    JUMP_NE(L_THEN_862); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_861);
		    L_THEN_862:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_860);
		    L_ELSE_861:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_867\n");
JUMP_EQ(L_JUMP_867);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_867:
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
printf("At L_JUMP_866\n");
JUMP_EQ(L_JUMP_866);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_866:
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
MOV(R0,IMM(1322));
PUSH(R0);
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
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_865\n");
JUMP_EQ(L_JUMP_865);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_865:
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_864\n");
JUMP_EQ(L_JUMP_864);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_864:
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
JUMP_EQ(L_JUMP_863);
printf("At L_JUMP_863\n");
fflush(stdout);
L_JUMP_863:
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

		    L_IF_EXIT_860:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_858:

POP(FP);
RETURN;
L_CLOS_EXIT_851:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_844));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_842:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_841);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_842);

L_FOR1_END_841:
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

L_FOR2_START_840:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_839);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_840);

L_FOR2_END_839:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_843);
L_CLOS_CODE_844: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_838);
printf("at JUMP_SIMPLE_838");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_838:

 //TC-APPLIC 

MOV(R0,IMM(1341));
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
JUMP_EQ(L_JUMP_845);
printf("At L_JUMP_845\n");
fflush(stdout);
L_JUMP_845:
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
L_CLOS_EXIT_843:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
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
L_CLOS_EXIT_835:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_829);
printf("At L_JUMP_829\n");
fflush(stdout);
L_JUMP_829:
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
L_CLOS_EXIT_827:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_821);
printf("At L_JUMP_821\n");
fflush(stdout);
L_JUMP_821:
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
L_CLOS_EXIT_819:
MOV(R1, INDD(IMM(782), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_810));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_808:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_807);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_808);

L_FOR1_END_807:
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

L_FOR2_START_806:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_805);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_806);

L_FOR2_END_805:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_809);
L_CLOS_CODE_810: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_804);
printf("at JUMP_SIMPLE_804");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_804:

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
MOV(R0, IMM(766));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_813\n");
JUMP_EQ(L_JUMP_813);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_813:
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
MOV(R0, IMM(666));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_812\n");
JUMP_EQ(L_JUMP_812);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_812:
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
MOV(R0, IMM(782));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_811);
printf("At L_JUMP_811\n");
fflush(stdout);
L_JUMP_811:
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
L_CLOS_EXIT_809:
MOV(R1, INDD(IMM(799), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_800));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_798:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_797);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_798);

L_FOR1_END_797:
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

L_FOR2_START_796:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_795);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_796);

L_FOR2_END_795:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_799);
L_CLOS_CODE_800: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_794);
printf("at JUMP_SIMPLE_794");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_794:

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
MOV(R0, IMM(766));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_803\n");
JUMP_EQ(L_JUMP_803);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_803:
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
MOV(R0, IMM(683));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
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
MOV(R0, IMM(782));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_801);
printf("At L_JUMP_801\n");
fflush(stdout);
L_JUMP_801:
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
L_CLOS_EXIT_799:
MOV(R1, INDD(IMM(818), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_785));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(88));
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
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_788); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_787);
		    L_THEN_788:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_792);
printf("At L_JUMP_792\n");
fflush(stdout);
L_JUMP_792:
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

		    JUMP(L_IF_EXIT_786);
		    L_ELSE_787:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1322));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_791\n");
JUMP_EQ(L_JUMP_791);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_791:
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_790\n");
JUMP_EQ(L_JUMP_790);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_790:
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
MOV(R0, IMM(830));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_789);
printf("At L_JUMP_789\n");
fflush(stdout);
L_JUMP_789:
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

		    L_IF_EXIT_786:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_784:
MOV(R1, INDD(IMM(830), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_770));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_768:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_767);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_768);

L_FOR1_END_767:
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

L_FOR2_START_766:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_765);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_766);

L_FOR2_END_765:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_769);
L_CLOS_CODE_770: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_764);
printf("at JUMP_SIMPLE_764");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_764:

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_778\n");
JUMP_EQ(L_JUMP_778);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_778:
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
JUMP_NE(L_OR_END_771);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_771);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1350));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_777\n");
JUMP_EQ(L_JUMP_777);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_777:
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
		    JUMP_NE(L_THEN_774); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_773);
		    L_THEN_774:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_776\n");
JUMP_EQ(L_JUMP_776);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_776:
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
MOV(R0, IMM(107));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_775);
printf("At L_JUMP_775\n");
fflush(stdout);
L_JUMP_775:
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

		    JUMP(L_IF_EXIT_772);
		    L_ELSE_773:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_772:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_771);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_771);
L_OR_END_771:

POP(FP);
RETURN;
L_CLOS_EXIT_769:
MOV(R1, INDD(IMM(107), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_758));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_760:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_761);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_760);

L_FOR1_END_761:
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

L_FOR2_START_762:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_763);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_762);

L_FOR2_END_763:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_759);
L_CLOS_CODE_758: 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_740));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_738:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_737);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_738);

L_FOR1_END_737:
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

L_FOR2_START_736:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_735);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_736);

L_FOR2_END_735:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_739);
L_CLOS_CODE_740: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_734);
printf("at JUMP_SIMPLE_734");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_734:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_747));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_745:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_744);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_745);

L_FOR1_END_744:
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

L_FOR2_START_743:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_742);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_743);

L_FOR2_END_742:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_746);
L_CLOS_CODE_747: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_741);
printf("at JUMP_SIMPLE_741");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_741:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_757\n");
JUMP_EQ(L_JUMP_757);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_757:
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
MOV(R0, IMM(1350));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_756\n");
JUMP_EQ(L_JUMP_756);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_756:
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
		    JUMP_NE(L_THEN_750); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_749);
		    L_THEN_750:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_755\n");
JUMP_EQ(L_JUMP_755);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_755:
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
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_754\n");
JUMP_EQ(L_JUMP_754);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_754:
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
JUMP_NE(L_OR_END_751);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_751);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_753\n");
JUMP_EQ(L_JUMP_753);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_753:
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
JUMP_EQ(L_JUMP_752);
printf("At L_JUMP_752\n");
fflush(stdout);
L_JUMP_752:
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
JUMP_NE(L_OR_END_751);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_751);
L_OR_END_751:

		    JUMP(L_IF_EXIT_748);
		    L_ELSE_749:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_748:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_746:

POP(FP);
RETURN;
L_CLOS_EXIT_739:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_732));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_730:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_729);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_730);

L_FOR1_END_729:
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

L_FOR2_START_728:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_727);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_728);

L_FOR2_END_727:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_731);
L_CLOS_CODE_732: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_726);
printf("at JUMP_SIMPLE_726");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_726:

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
JUMP_EQ(L_JUMP_733);
printf("At L_JUMP_733\n");
fflush(stdout);
L_JUMP_733:
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
L_CLOS_EXIT_731:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_725);
printf("At L_JUMP_725\n");
fflush(stdout);
L_JUMP_725:
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
MOV(R1, INDD(IMM(839), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_715));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_713:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_712);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_713);

L_FOR1_END_712:
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

L_FOR2_START_711:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_710);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_711);

L_FOR2_END_710:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_714);
L_CLOS_CODE_715: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_709);
printf("at JUMP_SIMPLE_709");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_709:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_723));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_721:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_720);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_721);

L_FOR1_END_720:
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

L_FOR2_START_719:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_718);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_719);

L_FOR2_END_718:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_722);
L_CLOS_CODE_723: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_717);
printf("at JUMP_SIMPLE_717");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_717:

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

POP(FP);
RETURN;
L_CLOS_EXIT_722:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(839));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_716);
printf("At L_JUMP_716\n");
fflush(stdout);
L_JUMP_716:
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
L_CLOS_EXIT_714:
MOV(R1, INDD(IMM(850), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_707));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_705:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_704);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_705);

L_FOR1_END_704:
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

L_FOR2_START_703:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_702);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_703);

L_FOR2_END_702:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_706);
L_CLOS_CODE_707: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_701);
printf("at JUMP_SIMPLE_701");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_701:

 //TC-APPLIC 

MOV(R0,IMM(1341));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(544));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_708);
printf("At L_JUMP_708\n");
fflush(stdout);
L_JUMP_708:
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
L_CLOS_EXIT_706:
MOV(R1, INDD(IMM(863), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_699));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_697:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_696);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_697);

L_FOR1_END_696:
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

L_FOR2_START_695:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_694);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_695);

L_FOR2_END_694:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_698);
L_CLOS_CODE_699: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_693);
printf("at JUMP_SIMPLE_693");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_693:

 //TC-APPLIC 

MOV(R0,IMM(1341));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(555));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_700);
printf("At L_JUMP_700\n");
fflush(stdout);
L_JUMP_700:
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
L_CLOS_EXIT_698:
MOV(R1, INDD(IMM(876), IMM(1)));
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
CMP(FPARG(1), IMM(1));
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
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(98));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_692\n");
JUMP_EQ(L_JUMP_692);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_692:
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
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_644);
printf("at JUMP_SIMPLE_644");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_644:

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
printf("At L_JUMP_691\n");
JUMP_EQ(L_JUMP_691);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_691:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_658));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_656:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_655);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_656);

L_FOR1_END_655:
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

L_FOR2_START_654:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_653);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_654);

L_FOR2_END_653:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_657);
L_CLOS_CODE_658: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_652);
printf("at JUMP_SIMPLE_652");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_652:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_674));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_672:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_671);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_672);

L_FOR1_END_671:
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

L_FOR2_START_670:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_669);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_670);

L_FOR2_END_669:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_673);
L_CLOS_CODE_674: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_668);
printf("at JUMP_SIMPLE_668");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_668:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_681));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

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
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_675);
printf("at JUMP_SIMPLE_675");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_675:
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
MOV(R0, IMM(19));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_690\n");
JUMP_EQ(L_JUMP_690);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_690:
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
		    JUMP_NE(L_THEN_684); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_683);
		    L_THEN_684:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

		    JUMP(L_IF_EXIT_682);
		    L_ELSE_683:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_689\n");
JUMP_EQ(L_JUMP_689);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_689:
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
printf("At L_JUMP_688\n");
JUMP_EQ(L_JUMP_688);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_688:
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
MOV(R0,IMM(1322));
PUSH(R0);
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
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_687\n");
JUMP_EQ(L_JUMP_687);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_687:
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_686\n");
JUMP_EQ(L_JUMP_686);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_686:
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
JUMP_EQ(L_JUMP_685);
printf("At L_JUMP_685\n");
fflush(stdout);
L_JUMP_685:
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

		    L_IF_EXIT_682:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_680:

POP(FP);
RETURN;
L_CLOS_EXIT_673:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_666));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_664:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_663);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_664);

L_FOR1_END_663:
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

L_FOR2_START_662:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_661);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_662);

L_FOR2_END_661:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_665);
L_CLOS_CODE_666: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_660);
printf("at JUMP_SIMPLE_660");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_660:

 //TC-APPLIC 

MOV(R0,IMM(1341));
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
JUMP_EQ(L_JUMP_667);
printf("At L_JUMP_667\n");
fflush(stdout);
L_JUMP_667:
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
L_CLOS_EXIT_665:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_659);
printf("At L_JUMP_659\n");
fflush(stdout);
L_JUMP_659:
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
L_CLOS_EXIT_657:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_651);
printf("At L_JUMP_651\n");
fflush(stdout);
L_JUMP_651:
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
L_CLOS_EXIT_641:
MOV(R1, INDD(IMM(892), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_634));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_632:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_631);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_632);

L_FOR1_END_631:
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

L_FOR2_START_630:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_629);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_630);

L_FOR2_END_629:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_633);
L_CLOS_CODE_634: 

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
MOV(R0, IMM(892));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_635);
printf("At L_JUMP_635\n");
fflush(stdout);
L_JUMP_635:
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
L_CLOS_EXIT_633:
MOV(R1, INDD(IMM(902), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_603));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
printf("At L_JUMP_627\n");
JUMP_EQ(L_JUMP_627);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_627:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_611));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_609:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_608);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_609);

L_FOR1_END_608:
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

L_FOR2_START_607:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_606);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_607);

L_FOR2_END_606:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_610);
L_CLOS_CODE_611: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_605);
printf("at JUMP_SIMPLE_605");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_605:
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
MOV(R0, IMM(19));
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
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_614); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_613);
		    L_THEN_614:
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
MOV(R0, IMM(766));
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

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(766));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_624\n");
JUMP_EQ(L_JUMP_624);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_624:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_622));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_620:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_619);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_620);

L_FOR1_END_619:
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

L_FOR2_START_618:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_617);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_618);

L_FOR2_END_617:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_621);
L_CLOS_CODE_622: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_616);
printf("at JUMP_SIMPLE_616");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_616:

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
MOV(R0, IMM(571));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(176));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_623);
printf("At L_JUMP_623\n");
fflush(stdout);
L_JUMP_623:
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
L_CLOS_EXIT_621:
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_615);
printf("At L_JUMP_615\n");
fflush(stdout);
L_JUMP_615:
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

		    JUMP(L_IF_EXIT_612);
		    L_ELSE_613:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_612:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_610:
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
MOV(R1, INDD(IMM(921), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_550));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_548:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_547);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_548);

L_FOR1_END_547:
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

L_FOR2_START_546:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_545);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_546);

L_FOR2_END_545:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_549);
L_CLOS_CODE_550: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_544);
printf("at JUMP_SIMPLE_544");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_544:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_568));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_566:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_565);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_566);

L_FOR1_END_565:
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

L_FOR2_START_564:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_563);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_564);

L_FOR2_END_563:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_567);
L_CLOS_CODE_568: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_562);
printf("at JUMP_SIMPLE_562");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_562:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_575));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_573:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_572);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_573);

L_FOR1_END_572:
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

L_FOR2_START_571:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_570);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_571);

L_FOR2_END_570:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_574);
L_CLOS_CODE_575: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_569);
printf("at JUMP_SIMPLE_569");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_569:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_596\n");
JUMP_EQ(L_JUMP_596);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_596:
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
		    JUMP_NE(L_THEN_578); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_577);
		    L_THEN_578:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1350));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_595);
printf("At L_JUMP_595\n");
fflush(stdout);
L_JUMP_595:
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

		    JUMP(L_IF_EXIT_576);
		    L_ELSE_577:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_594\n");
JUMP_EQ(L_JUMP_594);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_594:
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
		    JUMP_NE(L_THEN_581); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_580);
		    L_THEN_581:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_579);
		    L_ELSE_580:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_593\n");
JUMP_EQ(L_JUMP_593);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_593:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_592\n");
JUMP_EQ(L_JUMP_592);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_592:
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
MOV(R0, IMM(571));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_591\n");
JUMP_EQ(L_JUMP_591);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_591:
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
		    JUMP_NE(L_THEN_584); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_583);
		    L_THEN_584:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_590\n");
JUMP_EQ(L_JUMP_590);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_590:
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_589\n");
JUMP_EQ(L_JUMP_589);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_589:
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

		    JUMP(L_IF_EXIT_582);
		    L_ELSE_583:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_587\n");
JUMP_EQ(L_JUMP_587);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_587:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_586\n");
JUMP_EQ(L_JUMP_586);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_586:
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
MOV(R0, IMM(592));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_585);
printf("At L_JUMP_585\n");
fflush(stdout);
L_JUMP_585:
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

		    L_IF_EXIT_582:
		    
		    L_IF_EXIT_579:
		    
		    L_IF_EXIT_576:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_574:

POP(FP);
RETURN;
L_CLOS_EXIT_567:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_558));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_556:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_555);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_556);

L_FOR1_END_555:
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

L_FOR2_START_554:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_553);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_554);

L_FOR2_END_553:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_557);
L_CLOS_CODE_558: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_552);
printf("at JUMP_SIMPLE_552");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_552:

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
MOV(R0, IMM(766));
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
MOV(R0, IMM(766));
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
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_559);
printf("At L_JUMP_559\n");
fflush(stdout);
L_JUMP_559:
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
L_CLOS_EXIT_557:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_551);
printf("At L_JUMP_551\n");
fflush(stdout);
L_JUMP_551:
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
L_CLOS_EXIT_549:
MOV(R1, INDD(IMM(940), IMM(1)));
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
MOV(R0, IMM(940));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_543);
printf("At L_JUMP_543\n");
fflush(stdout);
L_JUMP_543:
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
L_CLOS_EXIT_541:
MOV(R1, INDD(IMM(959), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_533));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_531:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_530);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_531);

L_FOR1_END_530:
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

L_FOR2_START_529:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_528);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_529);

L_FOR2_END_528:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_532);
L_CLOS_CODE_533: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_527);
printf("at JUMP_SIMPLE_527");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_527:

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
MOV(R0, IMM(959));
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
MOV(R0, IMM(26));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_534);
printf("At L_JUMP_534\n");
fflush(stdout);
L_JUMP_534:
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
L_CLOS_EXIT_532:
MOV(R1, INDD(IMM(979), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_524));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_522:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_521);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_522);

L_FOR1_END_521:
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

L_FOR2_START_520:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_519);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_520);

L_FOR2_END_519:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_523);
L_CLOS_CODE_524: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_518);
printf("at JUMP_SIMPLE_518");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_518:

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
MOV(R0, IMM(940));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_526\n");
JUMP_EQ(L_JUMP_526);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_526:
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
MOV(R0, IMM(26));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_525);
printf("At L_JUMP_525\n");
fflush(stdout);
L_JUMP_525:
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
L_CLOS_EXIT_523:
MOV(R1, INDD(IMM(999), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(921));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
MOV(R1, INDD(IMM(1011), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring<?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(940));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
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
MOV(R1, INDD(IMM(1023), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring>?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(959));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_515\n");
JUMP_EQ(L_JUMP_515);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_515:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1035), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring<=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(979));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_514\n");
JUMP_EQ(L_JUMP_514);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_514:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1048), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring>=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(999));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(166));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_513\n");
JUMP_EQ(L_JUMP_513);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_513:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1061), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_497));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_495:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_494);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_495);

L_FOR1_END_494:
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

L_FOR2_START_493:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_492);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_493);

L_FOR2_END_492:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_496);
L_CLOS_CODE_497: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_491);
printf("at JUMP_SIMPLE_491");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_491:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_504));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_502:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_501);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_502);

L_FOR1_END_501:
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

L_FOR2_START_500:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_499);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_500);

L_FOR2_END_499:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_503);
L_CLOS_CODE_504: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_498);
printf("at JUMP_SIMPLE_498");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_498:
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
MOV(R0, IMM(19));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_507); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_506);
		    L_THEN_507:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_505);
		    L_ELSE_506:
		    
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
printf("At L_JUMP_511\n");
JUMP_EQ(L_JUMP_511);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_511:
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
MOV(R0, IMM(1373));
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
MOV(R0,IMM(1322));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_509\n");
JUMP_EQ(L_JUMP_509);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_509:
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
JUMP_EQ(L_JUMP_508);
printf("At L_JUMP_508\n");
fflush(stdout);
L_JUMP_508:
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

		    L_IF_EXIT_505:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_503:

POP(FP);
RETURN;
L_CLOS_EXIT_496:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_480));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_478:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_477);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_478);

L_FOR1_END_477:
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

L_FOR2_START_476:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_475);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_476);

L_FOR2_END_475:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_479);
L_CLOS_CODE_480: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_474);
printf("at JUMP_SIMPLE_474");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_474:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_487));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_485:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_484);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_485);

L_FOR1_END_484:
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

L_FOR2_START_483:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_482);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_483);

L_FOR2_END_482:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_486);
L_CLOS_CODE_487: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_481);
printf("at JUMP_SIMPLE_481");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_481:

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1322));
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
printf("At L_JUMP_490\n");
JUMP_EQ(L_JUMP_490);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_490:
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
MOV(R0, IMM(152));
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
JUMP_EQ(L_JUMP_488);
printf("At L_JUMP_488\n");
fflush(stdout);
L_JUMP_488:
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
L_CLOS_EXIT_486:

POP(FP);
RETURN;
L_CLOS_EXIT_479:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_473\n");
JUMP_EQ(L_JUMP_473);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_473:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1077), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_460));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_458:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_457);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_458);

L_FOR1_END_457:
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

L_FOR2_START_456:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_455);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_456);

L_FOR2_END_455:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_459);
L_CLOS_CODE_460: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_454);
printf("at JUMP_SIMPLE_454");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_454:
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
printf("At L_JUMP_472\n");
JUMP_EQ(L_JUMP_472);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_472:
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
		    JUMP_NE(L_THEN_470); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_469);
		    L_THEN_470:
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
printf("At L_JUMP_471\n");
JUMP_EQ(L_JUMP_471);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_471:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_468);
		    L_ELSE_469:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_468:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_463); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_462);
		    L_THEN_463://IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_466); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_465);
		    L_THEN_466:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_464);
		    L_ELSE_465:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(26));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_467);
printf("At L_JUMP_467\n");
fflush(stdout);
L_JUMP_467:
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

		    L_IF_EXIT_464:
		    
		    JUMP(L_IF_EXIT_461);
		    L_ELSE_462:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_461:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_459:
MOV(R1, INDD(IMM(1090), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEequal?

 //APPLIC 

PUSH(IMM(0));
//IF 
MOV(R0,IMM(12));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_453); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_452);
		    L_THEN_453:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_451);
		    L_ELSE_452:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_451:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_339));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_337:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_336);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_337);

L_FOR1_END_336:
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

L_FOR2_START_335:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_334);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_335);

L_FOR2_END_334:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_338);
L_CLOS_CODE_339: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_333);
printf("at JUMP_SIMPLE_333");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_333:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_354));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_352:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_351);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_352);

L_FOR1_END_351:
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

L_FOR2_START_350:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_349);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_350);

L_FOR2_END_349:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_353);
L_CLOS_CODE_354: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_348);
printf("at JUMP_SIMPLE_348");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_348:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_361));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_359:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_358);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_359);

L_FOR1_END_358:
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

L_FOR2_START_357:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_356);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_357);

L_FOR2_END_356:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_360);
L_CLOS_CODE_361: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_355);
printf("at JUMP_SIMPLE_355");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_355:
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
printf("At L_JUMP_450\n");
JUMP_EQ(L_JUMP_450);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_450:
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
		    JUMP_NE(L_THEN_448); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_447);
		    L_THEN_448:
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

		    JUMP(L_IF_EXIT_446);
		    L_ELSE_447:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_446:
		    
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
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1090));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_445);
printf("At L_JUMP_445\n");
fflush(stdout);
L_JUMP_445:
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

		    JUMP(L_IF_EXIT_362);
		    L_ELSE_363:
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
printf("At L_JUMP_444\n");
JUMP_EQ(L_JUMP_444);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_444:
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
		    JUMP_NE(L_THEN_442); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_441);
		    L_THEN_442:
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

		    JUMP(L_IF_EXIT_440);
		    L_ELSE_441:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_440:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_367); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_366);
		    L_THEN_367:
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
MOV(R0, IMM(571));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_439);
printf("At L_JUMP_439\n");
fflush(stdout);
L_JUMP_439:
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

 //FVAR 
MOV(R0, IMM(1284));
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
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_437);
printf("At L_JUMP_437\n");
fflush(stdout);
L_JUMP_437:
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
printf("At L_JUMP_436\n");
JUMP_EQ(L_JUMP_436);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_436:
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
		    JUMP_NE(L_THEN_434); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_433);
		    L_THEN_434:
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
printf("At L_JUMP_435\n");
JUMP_EQ(L_JUMP_435);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_435:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_432);
		    L_ELSE_433:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_432:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_373); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_372);
		    L_THEN_373:
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
MOV(R0, IMM(19));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_431);
printf("At L_JUMP_431\n");
fflush(stdout);
L_JUMP_431:
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

		    JUMP(L_IF_EXIT_371);
		    L_ELSE_372:
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
MOV(R0, IMM(1350));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_430\n");
JUMP_EQ(L_JUMP_430);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_430:
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
		    JUMP_NE(L_THEN_428); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_427);
		    L_THEN_428:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1350));
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
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_426);
		    L_ELSE_427:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_426:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_376); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_375);
		    L_THEN_376://IF 

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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_425\n");
JUMP_EQ(L_JUMP_425);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_425:
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_424\n");
JUMP_EQ(L_JUMP_424);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_424:
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
printf("At L_JUMP_423\n");
JUMP_EQ(L_JUMP_423);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_423:
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
		    JUMP_NE(L_THEN_419); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_418);
		    L_THEN_419:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_422\n");
JUMP_EQ(L_JUMP_422);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_422:
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_421\n");
JUMP_EQ(L_JUMP_421);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_421:
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
JUMP_EQ(L_JUMP_420);
printf("At L_JUMP_420\n");
fflush(stdout);
L_JUMP_420:
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

		    JUMP(L_IF_EXIT_417);
		    L_ELSE_418:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_417:
		    
		    JUMP(L_IF_EXIT_374);
		    L_ELSE_375:
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
printf("At L_JUMP_416\n");
JUMP_EQ(L_JUMP_416);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_416:
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
		    JUMP_NE(L_THEN_414); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_413);
		    L_THEN_414:
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
printf("At L_JUMP_415\n");
JUMP_EQ(L_JUMP_415);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_415:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_412);
		    L_ELSE_413:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_412:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_379); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_378);
		    L_THEN_379:
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
MOV(R0, IMM(1011));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_411);
printf("At L_JUMP_411\n");
fflush(stdout);
L_JUMP_411:
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

		    JUMP(L_IF_EXIT_377);
		    L_ELSE_378:
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
printf("At L_JUMP_410\n");
JUMP_EQ(L_JUMP_410);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_410:
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
		    JUMP_NE(L_THEN_408); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_407);
		    L_THEN_408:
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
printf("At L_JUMP_409\n");
JUMP_EQ(L_JUMP_409);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_409:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_406);
		    L_ELSE_407:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_406:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_382); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_381);
		    L_THEN_382:
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
JUMP_EQ(L_JUMP_405);
printf("At L_JUMP_405\n");
fflush(stdout);
L_JUMP_405:
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

		    JUMP(L_IF_EXIT_380);
		    L_ELSE_381:
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
printf("At L_JUMP_404\n");
JUMP_EQ(L_JUMP_404);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_404:
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
		    JUMP_NE(L_THEN_396); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_395);
		    L_THEN_396://IF 

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
printf("At L_JUMP_403\n");
JUMP_EQ(L_JUMP_403);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_403:
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
		    JUMP_NE(L_THEN_399); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_398);
		    L_THEN_399:
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
printf("At L_JUMP_402\n");
JUMP_EQ(L_JUMP_402);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_402:
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
printf("At L_JUMP_401\n");
JUMP_EQ(L_JUMP_401);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_401:
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
MOV(R0, IMM(19));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_400\n");
JUMP_EQ(L_JUMP_400);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_400:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_397);
		    L_ELSE_398:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_397:
		    
		    JUMP(L_IF_EXIT_394);
		    L_ELSE_395:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_394:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_385); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_384);
		    L_THEN_385:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1077));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_393\n");
JUMP_EQ(L_JUMP_393);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_393:
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
MOV(R0, IMM(1077));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_392\n");
JUMP_EQ(L_JUMP_392);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_392:
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
JUMP_EQ(L_JUMP_391);
printf("At L_JUMP_391\n");
fflush(stdout);
L_JUMP_391:
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

		    JUMP(L_IF_EXIT_383);
		    L_ELSE_384:
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
printf("At L_JUMP_390\n");
JUMP_EQ(L_JUMP_390);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_390:
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
		    JUMP_NE(L_THEN_388); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_387);
		    L_THEN_388:
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
JUMP_EQ(L_JUMP_389);
printf("At L_JUMP_389\n");
fflush(stdout);
L_JUMP_389:
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

		    JUMP(L_IF_EXIT_386);
		    L_ELSE_387:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_386:
		    
		    L_IF_EXIT_383:
		    
		    L_IF_EXIT_380:
		    
		    L_IF_EXIT_377:
		    
		    L_IF_EXIT_374:
		    
		    L_IF_EXIT_371:
		    
		    L_IF_EXIT_368:
		    
		    L_IF_EXIT_365:
		    
		    L_IF_EXIT_362:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_360:

POP(FP);
RETURN;
L_CLOS_EXIT_353:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_347));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_345:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_344);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_345);

L_FOR1_END_344:
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

L_FOR2_START_343:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_342);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_343);

L_FOR2_END_342:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_346);
L_CLOS_CODE_347: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_341);
printf("at JUMP_SIMPLE_341");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_341:
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_346:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_340);
printf("At L_JUMP_340\n");
fflush(stdout);
L_JUMP_340:
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
L_CLOS_EXIT_338:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_332\n");
JUMP_EQ(L_JUMP_332);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_332:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1100), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_297));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_295:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_294);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_295);

L_FOR1_END_294:
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

L_FOR2_START_293:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_292);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_293);

L_FOR2_END_292:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_296);
L_CLOS_CODE_297: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_291);
printf("at JUMP_SIMPLE_291");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_291:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_312));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_310:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_309);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_310);

L_FOR1_END_309:
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

L_FOR2_START_308:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_307);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_308);

L_FOR2_END_307:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_311);
L_CLOS_CODE_312: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_306);
printf("at JUMP_SIMPLE_306");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_306:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_319));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_317:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_316);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_317);

L_FOR1_END_316:
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

L_FOR2_START_315:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_314);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_315);

L_FOR2_END_314:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_318);
L_CLOS_CODE_319: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_313);
printf("at JUMP_SIMPLE_313");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_313:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_331\n");
JUMP_EQ(L_JUMP_331);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_331:
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
		    JUMP_NE(L_THEN_322); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_321);
		    L_THEN_322:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_320);
		    L_ELSE_321:
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
MOV(R0, IMM(210));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_330\n");
JUMP_EQ(L_JUMP_330);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_330:
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
printf("At L_JUMP_329\n");
JUMP_EQ(L_JUMP_329);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_329:
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
		    JUMP_NE(L_THEN_325); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_324);
		    L_THEN_325:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_328);
printf("At L_JUMP_328\n");
fflush(stdout);
L_JUMP_328:
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

		    JUMP(L_IF_EXIT_323);
		    L_ELSE_324:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_327\n");
JUMP_EQ(L_JUMP_327);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_327:
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
JUMP_EQ(L_JUMP_326);
printf("At L_JUMP_326\n");
fflush(stdout);
L_JUMP_326:
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

		    L_IF_EXIT_323:
		    
		    L_IF_EXIT_320:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_318:

POP(FP);
RETURN;
L_CLOS_EXIT_311:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_305));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_303:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_302);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_303);

L_FOR1_END_302:
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

L_FOR2_START_301:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_300);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_301);

L_FOR2_END_300:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_304);
L_CLOS_CODE_305: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_299);
printf("at JUMP_SIMPLE_299");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_299:
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_304:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_298);
printf("At L_JUMP_298\n");
fflush(stdout);
L_JUMP_298:
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
L_CLOS_EXIT_296:
MOV(R1, INDD(IMM(1114), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEassoc

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1100));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1114));
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
MOV(R1, INDD(IMM(1123), IMM(1)));
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
MOV(R0, IMM(1114));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_289\n");
JUMP_EQ(L_JUMP_289);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_289:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1131), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEvoid

 //APPLIC 

PUSH(IMM(0));
//IF 
MOV(R0,IMM(12));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_288); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_287);
		    L_THEN_288:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_286);
		    L_ELSE_287:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_286:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_278));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_276:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_275);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_276);

L_FOR1_END_275:
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

L_FOR2_START_274:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_273);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_274);

L_FOR2_END_273:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_277);
L_CLOS_CODE_278: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_272);
printf("at JUMP_SIMPLE_272");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_272:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_285));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_283:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_282);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_283);

L_FOR1_END_282:
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

L_FOR2_START_281:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_280);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_281);

L_FOR2_END_280:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_284);
L_CLOS_CODE_285: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(0));
JUMP_EQ(JUMP_SIMPLE_279);
printf("at JUMP_SIMPLE_279");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_279:
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
SHOW("bvar", R0);

POP(FP);
RETURN;
L_CLOS_EXIT_284:

POP(FP);
RETURN;
L_CLOS_EXIT_277:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_271\n");
JUMP_EQ(L_JUMP_271);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_271:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1139), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEvoid?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1139));
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
CMP(FPARG(1), IMM(1));
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
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_262);
printf("at JUMP_SIMPLE_262");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_262:

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
JUMP_EQ(L_JUMP_269);
printf("At L_JUMP_269\n");
fflush(stdout);
L_JUMP_269:
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
L_CLOS_EXIT_267:

POP(FP);
RETURN;
L_CLOS_EXIT_260:
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
MOV(R1, INDD(IMM(1148), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_238));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_236:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_235);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_236);

L_FOR1_END_235:
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

L_FOR2_START_234:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_233);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_234);

L_FOR2_END_233:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_237);
L_CLOS_CODE_238: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_232);
printf("at JUMP_SIMPLE_232");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_232:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_245));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_243:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_242);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_243);

L_FOR1_END_242:
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

L_FOR2_START_241:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_240);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_241);

L_FOR2_END_240:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_244);
L_CLOS_CODE_245: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_239);
printf("at JUMP_SIMPLE_239");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_239:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_248); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_247);
		    L_THEN_248:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_246);
		    L_ELSE_247:
		    
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
MOV(R0, IMM(1312));
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
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_251\n");
JUMP_EQ(L_JUMP_251);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_251:
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
printf("At L_JUMP_250\n");
JUMP_EQ(L_JUMP_250);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_250:
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
JUMP_EQ(L_JUMP_249);
printf("At L_JUMP_249\n");
fflush(stdout);
L_JUMP_249:
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

		    L_IF_EXIT_246:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_244:

POP(FP);
RETURN;
L_CLOS_EXIT_237:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_216));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_214:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_213);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_214);

L_FOR1_END_213:
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

L_FOR2_START_212:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_211);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_212);

L_FOR2_END_211:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_215);
L_CLOS_CODE_216: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_210);
printf("at JUMP_SIMPLE_210");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_210:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_223));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_221:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_220);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_221);

L_FOR1_END_220:
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

L_FOR2_START_219:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_218);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_219);

L_FOR2_END_218:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_222);
L_CLOS_CODE_223: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_217);
printf("at JUMP_SIMPLE_217");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_217:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_231\n");
JUMP_EQ(L_JUMP_231);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_231:
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
		    JUMP_NE(L_THEN_226); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_225);
		    L_THEN_226:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_224);
		    L_ELSE_225:
		    
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_230\n");
JUMP_EQ(L_JUMP_230);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_230:
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
printf("At L_JUMP_229\n");
JUMP_EQ(L_JUMP_229);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_229:
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
MOV(R0, IMM(1300));
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
JUMP_EQ(L_JUMP_227);
printf("At L_JUMP_227\n");
fflush(stdout);
L_JUMP_227:
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

		    L_IF_EXIT_224:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_222:

POP(FP);
RETURN;
L_CLOS_EXIT_215:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_196));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_194:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_193);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_194);

L_FOR1_END_193:
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

L_FOR2_START_192:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_191);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_192);

L_FOR2_END_191:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_195);
L_CLOS_CODE_196: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_190);
printf("at JUMP_SIMPLE_190");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_190:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_202));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_200:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_199);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_200);

L_FOR1_END_199:
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

L_FOR2_START_198:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_197);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_198);

L_FOR2_END_197:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_201);
L_CLOS_CODE_202: 

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
MOV(R0, IMM(1284));
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
		    JUMP_NE(L_THEN_205); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_204);
		    L_THEN_205:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_203);
		    L_ELSE_204:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
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
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1300));
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
JUMP_EQ(L_JUMP_206);
printf("At L_JUMP_206\n");
fflush(stdout);
L_JUMP_206:
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

		    L_IF_EXIT_203:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_201:

POP(FP);
RETURN;
L_CLOS_EXIT_195:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_189\n");
JUMP_EQ(L_JUMP_189);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_189:
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1158), IMM(1)));
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
PUSH(FPARG(2));
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
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
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
MOV(R0, IMM(766));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_188\n");
JUMP_EQ(L_JUMP_188);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_188:
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
MOV(R0, IMM(1158));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1382));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_187\n");
JUMP_EQ(L_JUMP_187);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_187:
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
MOV(R0, IMM(782));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_186);
printf("At L_JUMP_186\n");
fflush(stdout);
L_JUMP_186:
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
L_CLOS_EXIT_184:
MOV(R1, INDD(IMM(1175), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_176));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_174:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_173);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_174);

L_FOR1_END_173:
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

L_FOR2_START_172:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_171);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_172);

L_FOR2_END_171:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_175);
L_CLOS_CODE_176: 

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
MOV(R0, IMM(1077));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_179\n");
JUMP_EQ(L_JUMP_179);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_179:
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
MOV(R0, IMM(1158));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1382));
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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(892));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_177);
printf("At L_JUMP_177\n");
fflush(stdout);
L_JUMP_177:
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
L_CLOS_EXIT_175:
MOV(R1, INDD(IMM(1192), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_155));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_153:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_152);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_153);

L_FOR1_END_152:
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

L_FOR2_START_151:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_150);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_151);

L_FOR2_END_150:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_154);
L_CLOS_CODE_155: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_149);
printf("at JUMP_SIMPLE_149");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_149:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_162));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1284));
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
		    L_THEN_165:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_163);
		    L_ELSE_164:
		    
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
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_169\n");
JUMP_EQ(L_JUMP_169);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_169:
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
MOV(R0, IMM(1373));
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
MOV(R0, IMM(1312));
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
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

POP(FP);
RETURN;
L_CLOS_EXIT_154:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_140));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_138:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_137);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_138);

L_FOR1_END_137:
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

L_FOR2_START_136:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_135);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_136);

L_FOR2_END_135:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_139);
L_CLOS_CODE_140: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_134);
printf("at JUMP_SIMPLE_134");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_134:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_147));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_141);
printf("at JUMP_SIMPLE_141");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_141:

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
JUMP_EQ(L_JUMP_148);
printf("At L_JUMP_148\n");
fflush(stdout);
L_JUMP_148:
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
L_CLOS_EXIT_146:

POP(FP);
RETURN;
L_CLOS_EXIT_139:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_133\n");
JUMP_EQ(L_JUMP_133);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_133:
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1203), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEstring-reverse

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(766));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1203));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(782));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
PUSH(R0);

 //FVAR 
MOV(R0, IMM(202));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_132\n");
JUMP_EQ(L_JUMP_132);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_132:
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1221), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_123));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_121:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_120);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_121);

L_FOR1_END_120:
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

L_FOR2_START_119:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_118);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_119);

L_FOR2_END_118:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_122);
L_CLOS_CODE_123: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_117);
printf("at JUMP_SIMPLE_117");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_117:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(88));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_131\n");
JUMP_EQ(L_JUMP_131);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_131:
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
		    JUMP_NE(L_THEN_126); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_125);
		    L_THEN_126:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1300));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_130);
printf("At L_JUMP_130\n");
fflush(stdout);
L_JUMP_130:
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

		    JUMP(L_IF_EXIT_124);
		    L_ELSE_125:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1322));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_129\n");
JUMP_EQ(L_JUMP_129);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_129:
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
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_128\n");
JUMP_EQ(L_JUMP_128);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_128:
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
MOV(R0, IMM(830));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_127);
printf("At L_JUMP_127\n");
fflush(stdout);
L_JUMP_127:
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

		    L_IF_EXIT_124:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_122:
MOV(R1, INDD(IMM(830), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_108));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_106:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_105);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_106);

L_FOR1_END_105:
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

L_FOR2_START_104:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_103);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_104);

L_FOR2_END_103:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_107);
L_CLOS_CODE_108: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(3));
JUMP_EQ(JUMP_SIMPLE_102);
printf("at JUMP_SIMPLE_102");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_102:
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(88));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_116\n");
JUMP_EQ(L_JUMP_116);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_116:
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
		    JUMP_NE(L_THEN_111); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_110);
		    L_THEN_111:
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
JUMP_EQ(L_JUMP_115);
printf("At L_JUMP_115\n");
fflush(stdout);
L_JUMP_115:
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

		    JUMP(L_IF_EXIT_109);
		    L_ELSE_110:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1322));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(152));
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
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1312));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_113\n");
JUMP_EQ(L_JUMP_113);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_113:
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
MOV(R0, IMM(1234));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_112);
printf("At L_JUMP_112\n");
fflush(stdout);
L_JUMP_112:
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

		    L_IF_EXIT_109:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_107:
MOV(R1, INDD(IMM(1234), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_97));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_95:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_94);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_95);

L_FOR1_END_94:
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

L_FOR2_START_93:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_92);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_93);

L_FOR2_END_92:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_96);
L_CLOS_CODE_97: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_91);
printf("at JUMP_SIMPLE_91");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_91:
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
MOV(R0, IMM(555));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_101\n");
JUMP_EQ(L_JUMP_101);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_101:
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
		    JUMP_NE(L_THEN_100); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_99);
		    L_THEN_100:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_98);
		    L_ELSE_99:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_98:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_96:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_83));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_81:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_80);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_81);

L_FOR1_END_80:
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

L_FOR2_START_79:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_78);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_79);

L_FOR2_END_78:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_82);
L_CLOS_CODE_83: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_77);
printf("at JUMP_SIMPLE_77");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_77:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_85));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_87:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_88);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_87);

L_FOR1_END_88:
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

L_FOR2_START_89:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_90);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_89);

L_FOR2_END_90:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_86);
L_CLOS_CODE_85: 
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
MOV(R0, IMM(191));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_84);
printf("At L_JUMP_84\n");
fflush(stdout);
L_JUMP_84:
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
L_CLOS_EXIT_86:

POP(FP);
RETURN;
L_CLOS_EXIT_82:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_76\n");
JUMP_EQ(L_JUMP_76);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_76:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1241), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_71));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_69:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_68);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_69);

L_FOR1_END_68:
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

L_FOR2_START_67:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_66);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_67);

L_FOR2_END_66:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_70);
L_CLOS_CODE_71: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_65);
printf("at JUMP_SIMPLE_65");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_65:
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
MOV(R0, IMM(544));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_75\n");
JUMP_EQ(L_JUMP_75);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_75:
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
		    JUMP_NE(L_THEN_74); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_73);
		    L_THEN_74:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_72);
		    L_ELSE_73:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_72:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_70:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_57));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_55:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_54);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_55);

L_FOR1_END_54:
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

L_FOR2_START_53:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_52);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_53);

L_FOR2_END_52:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_56);
L_CLOS_CODE_57: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_51);
printf("at JUMP_SIMPLE_51");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_51:

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_59));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_61:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_62);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_61);

L_FOR1_END_62:
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

L_FOR2_START_63:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_64);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_63);

L_FOR2_END_64:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_60);
L_CLOS_CODE_59: 
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
MOV(R0, IMM(191));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_58);
printf("At L_JUMP_58\n");
fflush(stdout);
L_JUMP_58:
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
L_CLOS_EXIT_60:

POP(FP);
RETURN;
L_CLOS_EXIT_56:
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_50\n");
JUMP_EQ(L_JUMP_50);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_50:
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1248), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));

 // DEFINEerror
MOV(R0,IMM(1322));
MOV(R1, INDD(IMM(1257), IMM(1)));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_30));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_37));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_35:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_34);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_35);

L_FOR1_END_34:
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

L_FOR2_START_33:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_32);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_33);

L_FOR2_END_32:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_36);
L_CLOS_CODE_37: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_31);
printf("at JUMP_SIMPLE_31");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_31:
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
MOV(R0, IMM(544));
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

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_40); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_39);
		    L_THEN_40:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(34));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_48);
printf("At L_JUMP_48\n");
fflush(stdout);
L_JUMP_48:
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

		    JUMP(L_IF_EXIT_38);
		    L_ELSE_39:
		    
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
MOV(R0, IMM(1363));
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
MOV(R0, IMM(34));
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
MOV(R0, IMM(1363));
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
MOV(R0, IMM(152));
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
MOV(R0, IMM(157));
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
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
SHOW("bvar", R0);
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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1158));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_41);
printf("At L_JUMP_41\n");
fflush(stdout);
L_JUMP_41:
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

		    L_IF_EXIT_38:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_36:

POP(FP);
RETURN;
L_CLOS_EXIT_29:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_11));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_9:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_8);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_9);

L_FOR1_END_8:
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

L_FOR2_START_7:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_6);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_7);

L_FOR2_END_6:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_10);
L_CLOS_CODE_11: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(2));
JUMP_EQ(JUMP_SIMPLE_5);
printf("at JUMP_SIMPLE_5");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_5:

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
printf("At L_JUMP_23\n");
JUMP_EQ(L_JUMP_23);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_23:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_20));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_18:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_17);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_18);

L_FOR1_END_17:
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

L_FOR2_START_16:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_15);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_16);

L_FOR2_END_15:
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_19);
L_CLOS_CODE_20: 
PUSH(FP);
MOV(FP, SP);
//print_stack("THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP");
CMP(FPARG(1), IMM(1));
JUMP_EQ(JUMP_SIMPLE_14);
printf("at JUMP_SIMPLE_14");
SHOW("expected number of args:", FPARG(1));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
//checking number of arguments match the length of the lambda
JUMP_SIMPLE_14:

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
MOV(R0, IMM(142));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_22\n");
JUMP_EQ(L_JUMP_22);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_22:
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
JUMP_EQ(L_JUMP_21);
printf("At L_JUMP_21\n");
fflush(stdout);
L_JUMP_21:
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
L_CLOS_EXIT_19:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(131));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
printf("At L_JUMP_13\n");
JUMP_EQ(L_JUMP_13);
JUMP(ERROR_NOT_A_CLOSURE);
fflush(stdout);
L_JUMP_13:
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
MOV(R0, IMM(782));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
CMP(R0, IMM(0));
JUMP_EQ(ERROR_UNDEFINED);
CMP(ADDR(R0), IMM(T_CLOSURE));
JUMP_EQ(L_JUMP_12);
printf("At L_JUMP_12\n");
fflush(stdout);
L_JUMP_12:
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
L_CLOS_EXIT_10:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
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

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1322));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(19));
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

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1322));
PUSH(R0);
MOV(R0,IMM(1322));
PUSH(R0);
MOV(R0,IMM(1322));
PUSH(R0);
MOV(R0,IMM(1322));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(19));
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
PUSH(IMM(4)); // pushing number of arguments 
PUSH(IMM(4)); // pushing again for reference for frame removal 
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
MOV(R0,IMM(1365));
PUSH(R0);
MOV(R0,IMM(1322));
PUSH(R0);
MOV(R0,IMM(1322));
PUSH(R0);
MOV(R0,IMM(1322));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(19));
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
PUSH(IMM(4)); // pushing number of arguments 
PUSH(IMM(4)); // pushing again for reference for frame removal 
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
