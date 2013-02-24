
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
GET_LAST_BUCKET:
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
  //check if it's T_SYMBOL
  CMP(R2, IMM(368031));
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
int arr[2046] = {937610, 722689, 741553, 0, 741553, 1, 799345, 3, 110, 111, 116, 368031, 0, 799345, 4, 108, 105, 115, 116, 368031, 0, 799345, 4, 97, 100, 100, 49, 368031, 0, 799345, 4, 115, 117, 98, 49, 368031, 0, 799345, 5, 98, 105, 110, 62, 63, 368031, 0, 799345, 6, 98, 105, 110, 60, 61, 63, 368031, 0, 799345, 6, 98, 105, 110, 62, 61, 63, 368031, 0, 799345, 5, 122, 101, 114, 111, 63, 368031, 0, 799345, 6, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 5, 108, 105, 115, 116, 63, 368031, 0, 799345, 5, 101, 118, 101, 110, 63, 368031, 0, 799345, 4, 111, 100, 100, 63, 368031, 0, 799345, 3, 109, 97, 112, 368031, 0, 799345, 2, 121, 110, 368031, 0, 799345, 1, 43, 368031, 0, 799345, 1, 42, 368031, 0, 799345, 1, 45, 368031, 0, 799345, 1, 47, 368031, 0, 799345, 5, 111, 114, 100, 101, 114, 368031, 0, 799345, 6, 97, 110, 100, 109, 97, 112, 368031, 0, 799345, 2, 60, 62, 368031, 0, 799345, 5, 102, 111, 108, 100, 114, 368031, 0, 799345, 7, 99, 111, 109, 112, 111, 115, 101, 368031, 0, 799345, 4, 99, 97, 97, 114, 368031, 0, 799345, 4, 99, 97, 100, 114, 368031, 0, 799345, 4, 99, 100, 97, 114, 368031, 0, 799345, 4, 99, 100, 100, 114, 368031, 0, 799345, 5, 99, 97, 97, 97, 114, 368031, 0, 799345, 5, 99, 97, 97, 100, 114, 368031, 0, 799345, 5, 99, 97, 100, 97, 114, 368031, 0, 799345, 5, 99, 97, 100, 100, 114, 368031, 0, 799345, 5, 99, 100, 97, 97, 114, 368031, 0, 799345, 5, 99, 100, 97, 100, 114, 368031, 0, 799345, 5, 99, 100, 100, 97, 114, 368031, 0, 799345, 5, 99, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 97, 100, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 97, 100, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 97, 100, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 97, 114, 368031, 0, 799345, 6, 99, 100, 100, 100, 100, 114, 368031, 0, 799345, 27, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 114, 105, 103, 104, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 26, 94, 118, 97, 114, 105, 97, 100, 105, 99, 45, 108, 101, 102, 116, 45, 102, 114, 111, 109, 45, 98, 105, 110, 97, 114, 121, 368031, 0, 799345, 8, 94, 99, 104, 97, 114, 45, 111, 112, 368031, 0, 799345, 1, 60, 368031, 0, 799345, 2, 60, 61, 368031, 0, 799345, 1, 62, 368031, 0, 799345, 2, 62, 61, 368031, 0, 799345, 6, 99, 104, 97, 114, 61, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 60, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 60, 63, 368031, 0, 799345, 7, 99, 104, 97, 114, 62, 61, 63, 368031, 0, 799345, 6, 99, 104, 97, 114, 62, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 117, 112, 112, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 15, 99, 104, 97, 114, 45, 108, 111, 119, 101, 114, 99, 97, 115, 101, 63, 368031, 0, 799345, 11, 99, 104, 97, 114, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 60, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 60, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 61, 63, 368031, 0, 799345, 9, 99, 104, 97, 114, 45, 99, 105, 62, 63, 368031, 0, 799345, 10, 99, 104, 97, 114, 45, 99, 105, 62, 61, 63, 368031, 0, 799345, 12, 115, 116, 114, 105, 110, 103, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 117, 112, 99, 97, 115, 101, 368031, 0, 799345, 15, 115, 116, 114, 105, 110, 103, 45, 100, 111, 119, 110, 99, 97, 115, 101, 368031, 0, 799345, 8, 108, 105, 115, 116, 45, 114, 101, 102, 368031, 0, 799345, 5, 111, 114, 109, 97, 112, 368031, 0, 799345, 7, 109, 101, 109, 98, 101, 114, 63, 368031, 0, 799345, 9, 110, 101, 103, 97, 116, 105, 118, 101, 63, 368031, 0, 799345, 9, 112, 111, 115, 105, 116, 105, 118, 101, 63, 368031, 0, 799345, 12, 108, 105, 115, 116, 45, 62, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 6, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 15, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 16, 98, 105, 110, 97, 114, 121, 45, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 61, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 60, 63, 368031, 0, 799345, 8, 115, 116, 114, 105, 110, 103, 62, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 60, 61, 63, 368031, 0, 799345, 9, 115, 116, 114, 105, 110, 103, 62, 61, 63, 368031, 0, 799345, 12, 118, 101, 99, 116, 111, 114, 45, 62, 108, 105, 115, 116, 368031, 0, 799345, 9, 98, 111, 111, 108, 101, 97, 110, 61, 63, 368031, 0, 799345, 6, 101, 113, 117, 97, 108, 63, 368031, 0, 799345, 10, 94, 97, 115, 115, 111, 99, 105, 97, 116, 101, 368031, 0, 799345, 5, 97, 115, 115, 111, 99, 368031, 0, 799345, 4, 97, 115, 115, 113, 368031, 0, 799345, 4, 118, 111, 105, 100, 368031, 0, 799345, 5, 118, 111, 105, 100, 63, 368031, 0, 799345, 6, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 97, 112, 112, 101, 110, 100, 368031, 0, 799345, 7, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 114, 101, 118, 101, 114, 115, 101, 368031, 0, 799345, 9, 108, 105, 115, 116, 45, 115, 101, 116, 33, 368031, 0, 799345, 3, 109, 97, 120, 368031, 0, 799345, 3, 109, 105, 110, 368031, 0, 799345, 5, 101, 114, 114, 111, 114, 368031, 0, 799345, 14, 110, 117, 109, 98, 101, 114, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 1, 61, 368031, 0, 799345, 6, 102, 111, 114, 109, 97, 116, 368031, 0, 799345, 13, 115, 101, 120, 112, 114, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 21, 115, 101, 120, 112, 114, 45, 62, 100, 105, 115, 112, 108, 97, 121, 45, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 4, 98, 105, 110, 43, 368031, 0, 945311, 1, 799345, 4, 98, 105, 110, 45, 368031, 0, 799345, 5, 98, 105, 110, 60, 63, 368031, 0, 945311, 0, 799345, 5, 110, 117, 108, 108, 63, 368031, 0, 799345, 3, 99, 100, 114, 368031, 0, 799345, 5, 112, 97, 105, 114, 63, 368031, 0, 799345, 9, 114, 101, 109, 97, 105, 110, 100, 101, 114, 368031, 0, 945311, 2, 799345, 3, 99, 97, 114, 368031, 0, 799345, 4, 99, 111, 110, 115, 368031, 0, 799345, 5, 97, 112, 112, 108, 121, 368031, 0, 799345, 4, 98, 105, 110, 42, 368031, 0, 799345, 4, 98, 105, 110, 47, 368031, 0, 799345, 13, 99, 104, 97, 114, 45, 62, 105, 110, 116, 101, 103, 101, 114, 368031, 0, 799345, 5, 98, 105, 110, 61, 63, 368031, 0, 181048, 65, 181048, 90, 181048, 97, 181048, 122, 799345, 13, 105, 110, 116, 101, 103, 101, 114, 45, 62, 99, 104, 97, 114, 368031, 0, 799345, 13, 115, 116, 114, 105, 110, 103, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 945311, -1, 799345, 10, 115, 116, 114, 105, 110, 103, 45, 114, 101, 102, 368031, 0, 799345, 11, 115, 116, 114, 105, 110, 103, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 3, 101, 113, 63, 368031, 0, 799345, 11, 118, 101, 99, 116, 111, 114, 45, 115, 101, 116, 33, 368031, 0, 799345, 11, 109, 97, 107, 101, 45, 118, 101, 99, 116, 111, 114, 368031, 0, 799345, 13, 118, 101, 99, 116, 111, 114, 45, 108, 101, 110, 103, 116, 104, 368031, 0, 799345, 10, 118, 101, 99, 116, 111, 114, 45, 114, 101, 102, 368031, 0, 799345, 8, 98, 111, 111, 108, 101, 97, 110, 63, 368031, 0, 799345, 5, 99, 104, 97, 114, 63, 368031, 0, 799345, 7, 110, 117, 109, 98, 101, 114, 63, 368031, 0, 799345, 7, 115, 116, 114, 105, 110, 103, 63, 368031, 0, 799345, 7, 115, 121, 109, 98, 111, 108, 63, 368031, 0, 799345, 7, 118, 101, 99, 116, 111, 114, 63, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 97, 114, 33, 368031, 0, 945311, 48, 945311, 123, 945311, 10, 799345, 18, 84, 111, 111, 32, 109, 97, 110, 121, 32, 97, 114, 103, 117, 109, 101, 110, 116, 115, 181048, 126, 181048, 115, 181048, 37, 181048, 10, 799345, 27, 85, 110, 114, 101, 99, 111, 103, 110, 105, 122, 101, 100, 32, 109, 101, 116, 97, 45, 99, 104, 97, 114, 97, 99, 116, 101, 114, 799345, 0, 799345, 2, 40, 41, 799345, 2, 35, 116, 799345, 2, 35, 102, 799345, 14, 115, 121, 109, 98, 111, 108, 45, 62, 115, 116, 114, 105, 110, 103, 368031, 0, 799345, 1, 40, 799345, 1, 41, 799345, 7, 35, 60, 118, 111, 105, 100, 62, 799345, 10, 112, 114, 111, 99, 101, 100, 117, 114, 101, 63, 368031, 0, 799345, 12, 35, 60, 112, 114, 111, 99, 101, 100, 117, 114, 101, 62, 799345, 11, 87, 104, 97, 116, 39, 115, 32, 116, 104, 105, 115, 799345, 1, 35, 181048, 34, 885397, 1851, 11, 181048, 92, 181048, 110, 885397, 1858, 11, 885397, 1856, 1860, 181048, 13, 181048, 114, 885397, 1868, 11, 885397, 1856, 1870, 885397, 1856, 11, 885397, 1856, 1876, 885397, 1856, 1853, 181048, 32, 799345, 11, 111, 99, 116, 97, 108, 45, 99, 104, 97, 114, 115, 368031, 0, 799345, 17, 78, 111, 116, 32, 97, 110, 32, 111, 99, 116, 97, 108, 32, 99, 104, 97, 114, 799345, 8, 113, 117, 111, 116, 105, 101, 110, 116, 368031, 0, 945311, 8, 181048, 48, 799345, 1, 32, 799345, 3, 32, 46, 32, 799345, 9, 35, 92, 110, 101, 119, 108, 105, 110, 101, 799345, 7, 35, 92, 115, 112, 97, 99, 101, 181048, 9, 799345, 5, 35, 92, 116, 97, 98, 799345, 7, 35, 13, 101, 116, 117, 114, 110, 181048, 35, 885397, 1983, 1876, 945311, 4, 799345, 4, 98, 105, 110, 60, 368031, 0, 799345, 4, 98, 105, 110, 61, 368031, 0, 799345, 4, 98, 105, 110, 62, 368031, 0, 799345, 14, 115, 116, 114, 105, 110, 103, 45, 62, 115, 121, 109, 98, 111, 108, 368031, 0, 799345, 8, 115, 101, 116, 45, 99, 100, 114, 33, 368031, 0, 799345, 8, 105, 110, 116, 101, 103, 101, 114, 63, 368031, 0};
void *a = &arr;
memcpy(&((*machine).mem[10]), a, 2046*4);
MOV(ADDR(0), IMM(2056));

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
//creating bucket for format
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1277)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1286), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for sexpr->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1287)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1303), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for sexpr->display-string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1304)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1328), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin+
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1329)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1336), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin-
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1339)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1346), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1347)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1355), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for null?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1358)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1366), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cdr
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1367)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1373), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for pair?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1374)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1382), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for remainder
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1383)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1395), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for car
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1398)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1404), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for cons
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1405)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1412), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for apply
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1413)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1421), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin*
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1422)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1429), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin/
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1430)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1437), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char->integer
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1438)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1454), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1455)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1463), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for integer->char
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1472)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1488), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1489)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1505), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1508)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1521), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1522)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1536), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1537)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1551), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for eq?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1552)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1558), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-set!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1559)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1573), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for make-vector
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1574)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1588), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-length
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1589)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1605), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector-ref
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1606)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1619), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for boolean?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1620)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1631), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for char?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1632)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1640), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for number?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1641)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1651), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1652)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1662), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1663)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1673), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for vector?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1674)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1684), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-car!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1685)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1696), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for symbol->string
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1774)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1791), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for procedure?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1807)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1820), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for octal-chars
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1887)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1901), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for quotient
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1921)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1932), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin<
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1990)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(1997), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin=
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(1998)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(2005), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for bin>
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(2006)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(2013), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for string->symbol
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(2014)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(2031), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for set-cdr!
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(2032)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(2043), R0); // save the bucket address in the symbol object 
MOV(ADDR(R1), R0); // make previous bucket point to this one 
ADD(R0,IMM(2)); // store bucket address in R1 for future linkage 
MOV(R1,R0);
//creating bucket for integer?
PUSH(IMM(3)); // allocate space for new bucket 
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R0, IMM(0)), IMM(2044)); // add pointer to the string 
MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value 
MOV(ADDR(2055), R0); // save the bucket address in the symbol object 
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
MOV(R1, INDD(IMM(1372),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_PLUS
PUSH(LABEL(BIN_PLUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1335),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for APPLY
PUSH(LABEL(APPLY));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1420),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MINUS
PUSH(LABEL(BIN_MINUS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1345),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_MUL
PUSH(LABEL(BIN_MUL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1428),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for BIN_DIV
PUSH(LABEL(BIN_DIV));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1436),1));
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
MOV(R1, INDD(IMM(2012),1));
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
MOV(R1, INDD(IMM(1462),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1354),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQUAL_TO
PUSH(LABEL(EQUAL_TO));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(2004),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for LOWER_THAN
PUSH(LABEL(LOWER_THAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1996),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CAR
PUSH(LABEL(CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1403),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CDR
PUSH(LABEL(CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1372),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_TO_SYMBOL
PUSH(LABEL(STRING_TO_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(2030),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for REMAINDER
PUSH(LABEL(REMAINDER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1394),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PROCEDURE
PUSH(LABEL(IS_PROCEDURE));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1819),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_VECTOR
PUSH(LABEL(IS_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1683),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_SYMBOL
PUSH(LABEL(IS_SYMBOL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1672),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_STRING
PUSH(LABEL(IS_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1661),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_CHAR
PUSH(LABEL(IS_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1639),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NUMBER
PUSH(LABEL(IS_NUMBER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1650),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NUMBER
PUSH(LABEL(IS_NUMBER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(2054),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_BOOLEAN
PUSH(LABEL(IS_BOOLEAN));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1630),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_PAIR
PUSH(LABEL(IS_PAIR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1381),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for IS_NULL
PUSH(LABEL(IS_NULL));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1365),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for INTEGER_TO_CHAR
PUSH(LABEL(INTEGER_TO_CHAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1487),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CHAR_TO_INTEGER
PUSH(LABEL(CHAR_TO_INTEGER));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1453),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1504),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_LENGTH
PUSH(LABEL(STRING_LENGTH));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1604),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_STRING
PUSH(LABEL(MAKE_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1550),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for MAKE_VECTOR
PUSH(LABEL(MAKE_VECTOR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1587),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SYMBOL_TO_STRING
PUSH(LABEL(SYMBOL_TO_STRING));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1790),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CAR
PUSH(LABEL(SET_CAR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1695),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for SET_CDR
PUSH(LABEL(SET_CDR));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(2042),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for CONS
PUSH(LABEL(CONS));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1411),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_SET
PUSH(LABEL(STRING_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1535),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for VECTOR_SET
PUSH(LABEL(VECTOR_SET));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1572),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for STRING_REF
PUSH(LABEL(STRING_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1520),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for VECTOR_REF
PUSH(LABEL(VECTOR_REF));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1618),1));
MOV(INDD(R1, 1), R0);

//This is the bindings for EQ2
PUSH(LABEL(EQ2));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(1557),1));
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

 // DEFINE  not

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2635));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2633:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2632);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2633);

L_FOR1_END_2632:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2631:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2630);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2631);

L_FOR2_END_2630:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2634);
L_CLOS_CODE_2635: 
PUSH(FP);
MOV(FP, SP);
//IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_2638); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2637);
		    L_THEN_2638:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_2636);
		    L_ELSE_2637:
		    MOV(R0,IMM(14));

		    L_IF_EXIT_2636:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2634:
MOV(R1, INDD(IMM(21), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2639);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2639:


 // DEFINE  list

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2627));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2625:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2624);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2625);

L_FOR1_END_2624:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2623:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2622);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2623);

L_FOR2_END_2622:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2626);
L_CLOS_CODE_2627: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
//body of the lambda goes here
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_2626:
MOV(R1, INDD(IMM(29), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2628);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2628:


 // DEFINE  add1

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2619));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2617:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2616);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2617);

L_FOR1_END_2616:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2615:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2614);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2615);

L_FOR2_END_2614:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2618);
L_CLOS_CODE_2619: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(1337));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1335));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2618:
MOV(R1, INDD(IMM(37), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2621);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2621:


 // DEFINE  sub1

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2610));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2608:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2607);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2608);

L_FOR1_END_2607:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2606:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2605);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2606);

L_FOR2_END_2605:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2609);
L_CLOS_CODE_2610: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(1337));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1345));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2609:
MOV(R1, INDD(IMM(45), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2612);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2612:


 // DEFINE  bin>?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2601));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2599:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2598);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2599);

L_FOR1_END_2598:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2597:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2596);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2597);

L_FOR2_END_2596:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2600);
L_CLOS_CODE_2601: 
PUSH(FP);
MOV(FP, SP);

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
MOV(R0, IMM(1354));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2600:
MOV(R1, INDD(IMM(54), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2603);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2603:


 // DEFINE  bin<=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2591));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2589:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2588);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2589);

L_FOR1_END_2588:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2587:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2586);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2587);

L_FOR2_END_2586:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2590);
L_CLOS_CODE_2591: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2590:
MOV(R1, INDD(IMM(64), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2594);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2594:


 // DEFINE  bin>=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2581));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2579:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2578);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2579);

L_FOR1_END_2578:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2577:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2576);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2577);

L_FOR2_END_2576:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2580);
L_CLOS_CODE_2581: 
PUSH(FP);
MOV(FP, SP);

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
MOV(R0, IMM(1354));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2580:
MOV(R1, INDD(IMM(74), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2584);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2584:


 // DEFINE  zero?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2572));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2570:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2569);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2570);

L_FOR1_END_2569:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2568:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2567);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2568);

L_FOR2_END_2567:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2571);
L_CLOS_CODE_2572: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(1356));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2571:
MOV(R1, INDD(IMM(83), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2574);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2574:


 // DEFINE  length

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2557));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2555:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2554);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2555);

L_FOR1_END_2554:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2553:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2552);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2553);

L_FOR2_END_2552:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2556);
L_CLOS_CODE_2557: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2560); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2559);
		    L_THEN_2560:MOV(R0,IMM(1356));

		    JUMP(L_IF_EXIT_2558);
		    L_ELSE_2559:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    L_IF_EXIT_2558:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2556:
MOV(R1, INDD(IMM(93), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2565);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2565:


 // DEFINE  list?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2541));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2539:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2538);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2539);

L_FOR1_END_2538:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2537:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2536);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2537);

L_FOR2_END_2536:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2540);
L_CLOS_CODE_2541: 
PUSH(FP);
MOV(FP, SP);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
JUMP_NE(L_OR_END_2542);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2542);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2545); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2544);
		    L_THEN_2545:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_2543);
		    L_ELSE_2544:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_2543:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_2542);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2542);
L_OR_END_2542:

POP(FP);
RETURN;
L_CLOS_EXIT_2540:
MOV(R1, INDD(IMM(102), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2550);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2550:


 // DEFINE  even?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2531));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2529:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2528);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2529);

L_FOR1_END_2528:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2527:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2526);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2527);

L_FOR2_END_2526:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2530);
L_CLOS_CODE_2531: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1396));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1394));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2530:
MOV(R1, INDD(IMM(111), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2534);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2534:


 // DEFINE  odd?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2520));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2518:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2517);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2518);

L_FOR1_END_2517:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2516:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2515);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2516);

L_FOR2_END_2515:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2519);
L_CLOS_CODE_2520: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1396));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1394));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2519:
MOV(R1, INDD(IMM(119), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2524);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2524:


 // DEFINE  map

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2479));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2477:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2476);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2477);

L_FOR1_END_2476:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2475:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2474);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2475);

L_FOR2_END_2474:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2478);
L_CLOS_CODE_2479: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2503));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2501:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2500);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2501);

L_FOR1_END_2500:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2499:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2498);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2499);

L_FOR2_END_2498:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2502);
L_CLOS_CODE_2503: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2510));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2508:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2507);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2508);

L_FOR1_END_2507:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2506:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2505);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2506);

L_FOR2_END_2505:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2509);
L_CLOS_CODE_2510: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
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
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2509:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2502:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2487));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2485:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2484);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2485);

L_FOR1_END_2484:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2483:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2482);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2483);

L_FOR2_END_2482:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2486);
L_CLOS_CODE_2487: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2494));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2492:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2491);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2492);

L_FOR1_END_2491:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2490:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2489);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2490);

L_FOR2_END_2489:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2493);
L_CLOS_CODE_2494: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
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
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2493:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2486:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2478:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2400));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2398:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2397);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2398);

L_FOR1_END_2397:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2396:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2395);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2396);

L_FOR2_END_2395:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2399);
L_CLOS_CODE_2400: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2456));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2454:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2453);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2454);

L_FOR1_END_2453:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2452:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2451);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2452);

L_FOR2_END_2451:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2455);
L_CLOS_CODE_2456: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2463));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2461:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2460);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2461);

L_FOR1_END_2460:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2459:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2458);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2459);

L_FOR2_END_2458:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2462);
L_CLOS_CODE_2463: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2466); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2465);
		    L_THEN_2466:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_2464);
		    L_ELSE_2465:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_2464:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2462:

POP(FP);
RETURN;
L_CLOS_EXIT_2455:
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2408));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2406:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2405);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2406);

L_FOR1_END_2405:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2404:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2403);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2404);

L_FOR2_END_2403:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2407);
L_CLOS_CODE_2408: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2431));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2429:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2428);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2429);

L_FOR1_END_2428:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2427:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2426);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2427);

L_FOR2_END_2426:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2430);
L_CLOS_CODE_2431: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2438));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_2436:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2435);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2436);

L_FOR1_END_2435:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2434:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2433);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2434);

L_FOR2_END_2433:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2437);
L_CLOS_CODE_2438: 
PUSH(FP);
MOV(FP, SP);
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2441); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2440);
		    L_THEN_2441:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_2439);
		    L_ELSE_2440:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
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
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_2439:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2437:

POP(FP);
RETURN;
L_CLOS_EXIT_2430:
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2416));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2414:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2413);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2414);

L_FOR1_END_2413:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2412:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2411);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2412);

L_FOR2_END_2411:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2415);
L_CLOS_CODE_2416: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2418));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_2420:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2421);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2420);

L_FOR1_END_2421:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2422:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2423);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2422);

L_FOR2_END_2423:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2419);
L_CLOS_CODE_2418: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2419:

POP(FP);
RETURN;
L_CLOS_EXIT_2415:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2407:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2399:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2513);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2513:


 // DEFINE  yn

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2350));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2348:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2347);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2348);

L_FOR1_END_2347:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2346:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2345);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2346);

L_FOR2_END_2345:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2349);
L_CLOS_CODE_2350: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2368));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2366:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2365);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2366);

L_FOR1_END_2365:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2364:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2363);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2364);

L_FOR2_END_2363:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2367);
L_CLOS_CODE_2368: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2374));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2372:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2371);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2372);

L_FOR1_END_2371:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2370:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2369);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2370);

L_FOR2_END_2369:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2373);
L_CLOS_CODE_2374: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2383));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_2381:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2380);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2381);

L_FOR1_END_2380:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2379:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2378);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2379);

L_FOR2_END_2378:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2382);
L_CLOS_CODE_2383: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2389));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_2387:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2386);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2387);

L_FOR1_END_2386:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2385:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2384);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2385);

L_FOR2_END_2384:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2388);
L_CLOS_CODE_2389: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2388:

POP(FP);
RETURN;
L_CLOS_EXIT_2382:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2373:

POP(FP);
RETURN;
L_CLOS_EXIT_2367:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2358));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2356:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2355);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2356);

L_FOR1_END_2355:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2354:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2353);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2354);

L_FOR2_END_2353:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2357);
L_CLOS_CODE_2358: 
PUSH(FP);
MOV(FP, SP);

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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2357:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2349:
MOV(R1, INDD(IMM(132), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2392);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2392:


 // DEFINE  +

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2328));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2326:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2325);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2326);

L_FOR1_END_2325:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2324:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2323);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2324);

L_FOR2_END_2323:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2327);
L_CLOS_CODE_2328: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2335));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2333:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2332);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2333);

L_FOR1_END_2332:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2331:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2330);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2331);

L_FOR2_END_2330:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2334);
L_CLOS_CODE_2335: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2338); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2337);
		    L_THEN_2338:MOV(R0,IMM(1356));

		    JUMP(L_IF_EXIT_2336);
		    L_ELSE_2337:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1335));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_2336:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2334:

POP(FP);
RETURN;
L_CLOS_EXIT_2327:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2314));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2312:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2311);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2312);

L_FOR1_END_2311:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2310:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2309);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2310);

L_FOR2_END_2309:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2313);
L_CLOS_CODE_2314: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2320));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2318:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2317);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2318);

L_FOR1_END_2317:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2316:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2315);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2316);

L_FOR2_END_2315:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2319);
L_CLOS_CODE_2320: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2319:

POP(FP);
RETURN;
L_CLOS_EXIT_2313:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2344);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2344:


 // DEFINE  *

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2290));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2288:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2287);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2288);

L_FOR1_END_2287:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2286:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2285);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2286);

L_FOR2_END_2285:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2289);
L_CLOS_CODE_2290: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2297));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2295:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2294);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2295);

L_FOR1_END_2294:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2293:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2292);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2293);

L_FOR2_END_2292:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2296);
L_CLOS_CODE_2297: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2300); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2299);
		    L_THEN_2300:MOV(R0,IMM(1337));

		    JUMP(L_IF_EXIT_2298);
		    L_ELSE_2299:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1428));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_2298:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2296:

POP(FP);
RETURN;
L_CLOS_EXIT_2289:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2276));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2274:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2273);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2274);

L_FOR1_END_2273:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2272:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2271);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2272);

L_FOR2_END_2271:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2275);
L_CLOS_CODE_2276: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2282));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2280:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2279);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2280);

L_FOR1_END_2279:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2278:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2277);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2278);

L_FOR2_END_2277:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2281);
L_CLOS_CODE_2282: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2281:

POP(FP);
RETURN;
L_CLOS_EXIT_2275:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2306);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2306:


 // DEFINE  -

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2262));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2264:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2265);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2264);

L_FOR1_END_2265:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2266:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2267);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2266);

L_FOR2_END_2267:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2263);
L_CLOS_CODE_2262: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2257); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2256);
		    L_THEN_2257:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1356));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1345));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_2255);
		    L_ELSE_2256:
		    
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1345));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_2255:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2263:
MOV(R1, INDD(IMM(147), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2268);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2268:


 // DEFINE  /

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2248));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2250:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2251);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2250);

L_FOR1_END_2251:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2252:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2253);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2252);

L_FOR2_END_2253:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2249);
L_CLOS_CODE_2248: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2243); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2242);
		    L_THEN_2243:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1337));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1436));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_2241);
		    L_ELSE_2242:
		    
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1436));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_2241:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2249:
MOV(R1, INDD(IMM(152), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2254);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2254:


 // DEFINE  order

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2200));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2198:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2197);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2198);

L_FOR1_END_2197:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2196:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2195);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2196);

L_FOR2_END_2195:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2199);
L_CLOS_CODE_2200: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2222));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2220:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2219);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2220);

L_FOR1_END_2219:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2218:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2217);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2218);

L_FOR2_END_2217:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2221);
L_CLOS_CODE_2222: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2229));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2227:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2226);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2227);

L_FOR1_END_2226:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2225:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2224);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2225);

L_FOR2_END_2224:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2228);
L_CLOS_CODE_2229: 
PUSH(FP);
MOV(FP, SP);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
JUMP_NE(L_OR_END_2230);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2230);
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2233); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2232);
		    L_THEN_2233:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_2231);
		    L_ELSE_2232:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_2231:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_2230);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2230);
L_OR_END_2230:

POP(FP);
RETURN;
L_CLOS_EXIT_2228:

POP(FP);
RETURN;
L_CLOS_EXIT_2221:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2208));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2206:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2205);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2206);

L_FOR1_END_2205:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2204:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2203);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2204);

L_FOR2_END_2203:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2207);
L_CLOS_CODE_2208: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2210));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2212:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2213);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2212);

L_FOR1_END_2213:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2214:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2215);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2214);

L_FOR2_END_2215:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2211);
L_CLOS_CODE_2210: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2211:

POP(FP);
RETURN;
L_CLOS_EXIT_2207:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2199:
MOV(R1, INDD(IMM(161), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2240);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2240:


 // DEFINE  andmap

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2187));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2189:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2190);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2189);

L_FOR1_END_2190:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2191:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2192);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2191);

L_FOR2_END_2192:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2188);
L_CLOS_CODE_2187: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2169));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2167:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2166);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2167);

L_FOR1_END_2166:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2165:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2164);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2165);

L_FOR2_END_2164:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2168);
L_CLOS_CODE_2169: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2176));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2174:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2173);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2174);

L_FOR1_END_2173:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2172:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2171);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2172);

L_FOR2_END_2171:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2175);
L_CLOS_CODE_2176: 
PUSH(FP);
MOV(FP, SP);

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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
JUMP_NE(L_OR_END_2177);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2177);
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2180); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2179);
		    L_THEN_2180:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_2178);
		    L_ELSE_2179:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_2178:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_2177);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2177);
L_OR_END_2177:

POP(FP);
RETURN;
L_CLOS_EXIT_2175:

POP(FP);
RETURN;
L_CLOS_EXIT_2168:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2161));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2159:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2158);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2159);

L_FOR1_END_2158:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2157:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2156);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2157);

L_FOR2_END_2156:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2160);
L_CLOS_CODE_2161: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2160:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2188:
MOV(R1, INDD(IMM(171), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2193);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2193:


 // DEFINE  <>

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2127));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2125:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2124);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2125);

L_FOR1_END_2124:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2123:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2122);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2123);

L_FOR2_END_2122:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2126);
L_CLOS_CODE_2127: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2134));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2132:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2131);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2132);

L_FOR1_END_2131:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2130:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2129);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2130);

L_FOR2_END_2129:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2133);
L_CLOS_CODE_2134: 
PUSH(FP);
MOV(FP, SP);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
JUMP_NE(L_OR_END_2135);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2135);
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2149));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2147:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2146);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2147);

L_FOR1_END_2146:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2145:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2144);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2145);

L_FOR2_END_2144:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2148);
L_CLOS_CODE_2149: 
PUSH(FP);
MOV(FP, SP);

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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1275));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2148:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(171));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2138); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2137);
		    L_THEN_2138:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_2136);
		    L_ELSE_2137:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_2136:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_2135);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_2135);
L_OR_END_2135:

POP(FP);
RETURN;
L_CLOS_EXIT_2133:

POP(FP);
RETURN;
L_CLOS_EXIT_2126:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2111));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2109:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2108);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2109);

L_FOR1_END_2108:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2107:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2106);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2107);

L_FOR2_END_2106:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2110);
L_CLOS_CODE_2111: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2117));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2115:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2114);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2115);

L_FOR1_END_2114:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2113:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2112);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2113);

L_FOR2_END_2112:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2116);
L_CLOS_CODE_2117: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2116:

POP(FP);
RETURN;
L_CLOS_EXIT_2110:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2153);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2153:


 // DEFINE  foldr

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2071));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2069:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2068);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2069);

L_FOR1_END_2068:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2067:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2066);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2067);

L_FOR2_END_2066:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2070);
L_CLOS_CODE_2071: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2087));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2085:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2084);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2085);

L_FOR1_END_2084:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2083:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2082);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2083);

L_FOR2_END_2082:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2086);
L_CLOS_CODE_2087: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2094));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2092:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2091);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2092);

L_FOR1_END_2091:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2090:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2089);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2090);

L_FOR2_END_2089:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2093);
L_CLOS_CODE_2094: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_2097); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_2096);
		    L_THEN_2097:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));

		    JUMP(L_IF_EXIT_2095);
		    L_ELSE_2096:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_2095:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_2093:

POP(FP);
RETURN;
L_CLOS_EXIT_2086:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2079));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2077:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2076);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2077);

L_FOR1_END_2076:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2075:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2074);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2075);

L_FOR2_END_2074:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2078);
L_CLOS_CODE_2079: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 2));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2078:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_2070:
MOV(R1, INDD(IMM(186), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2103);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2103:


 // DEFINE  compose

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2054));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2052:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2051);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2052);

L_FOR1_END_2051:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2050:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2049);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2050);

L_FOR2_END_2049:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2053);
L_CLOS_CODE_2054: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2061));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2059:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2058);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2059);

L_FOR1_END_2058:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2057:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2056);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2057);

L_FOR2_END_2056:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2060);
L_CLOS_CODE_2061: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_2060:

POP(FP);
RETURN;
L_CLOS_EXIT_2053:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2033));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_2031:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2030);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2031);

L_FOR1_END_2030:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2029:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2028);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2029);

L_FOR2_END_2028:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2032);
L_CLOS_CODE_2033: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2039));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_2037:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2036);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2037);

L_FOR1_END_2036:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2035:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2034);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_2035);

L_FOR2_END_2034:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2038);
L_CLOS_CODE_2039: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_2047));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_2045:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_2044);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_2045);

L_FOR1_END_2044:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_2043:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_2042);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_2043);

L_FOR2_END_2042:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_2046);
L_CLOS_CODE_2047: 
PUSH(FP);
MOV(FP, SP);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_2046:
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(186));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_2038:

POP(FP);
RETURN;
L_CLOS_EXIT_2032:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2064);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2064:


 // DEFINE  caar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2025);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2025:


 // DEFINE  cadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2023);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2023:


 // DEFINE  cdar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2021);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2021:


 // DEFINE  cddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2019);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2019:


 // DEFINE  caaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(205));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2017);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2017:


 // DEFINE  caadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(213));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2015);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2015:


 // DEFINE  cadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(221));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2013);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2013:


 // DEFINE  caddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(229));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2011);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2011:


 // DEFINE  cdaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(205));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2009);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2009:


 // DEFINE  cdadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(213));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2007);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2007:


 // DEFINE  cddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(221));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2005);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2005:


 // DEFINE  cdddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(229));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2003);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2003:


 // DEFINE  caaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(238));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_2001);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_2001:


 // DEFINE  caaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(247));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1999);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1999:


 // DEFINE  caadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(256));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1997);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1997:


 // DEFINE  caaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(265));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1995);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1995:


 // DEFINE  cadaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(274));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1993);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1993:


 // DEFINE  cadadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(283));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1991);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1991:


 // DEFINE  caddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(292));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1989);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1989:


 // DEFINE  cadddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(301));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1987);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1987:


 // DEFINE  cdaaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(238));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1985);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1985:


 // DEFINE  cdaadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(247));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1983);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1983:


 // DEFINE  cdadar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(256));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1981);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1981:


 // DEFINE  cdaddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(265));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1979);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1979:


 // DEFINE  cddaar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(274));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1977);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1977:


 // DEFINE  cddadr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(283));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1975);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1975:


 // DEFINE  cdddar

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(292));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1973);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1973:


 // DEFINE  cddddr

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(301));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1971);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1971:


 // DEFINE  ^variadic-right-from-binary

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1931));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1929:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1928);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1929);

L_FOR1_END_1928:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1927:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1926);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1927);

L_FOR2_END_1926:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1930);
L_CLOS_CODE_1931: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1953));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1951:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1950);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1951);

L_FOR1_END_1950:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1949:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1948);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1949);

L_FOR2_END_1948:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1952);
L_CLOS_CODE_1953: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1960));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1958:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1957);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1958);

L_FOR1_END_1957:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1956:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1955);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1956);

L_FOR2_END_1955:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1959);
L_CLOS_CODE_1960: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1963); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1962);
		    L_THEN_1963:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));

		    JUMP(L_IF_EXIT_1961);
		    L_ELSE_1962:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_1961:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1959:

POP(FP);
RETURN;
L_CLOS_EXIT_1952:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1939));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1937:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1936);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1937);

L_FOR1_END_1936:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1935:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1934);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1935);

L_FOR2_END_1934:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1938);
L_CLOS_CODE_1939: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1945));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1943:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1942);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1943);

L_FOR1_END_1942:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1941:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1940);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1941);

L_FOR2_END_1940:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1944);
L_CLOS_CODE_1945: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1944:

POP(FP);
RETURN;
L_CLOS_EXIT_1938:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1930:
MOV(R1, INDD(IMM(492), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1969);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1969:


 // DEFINE  ^variadic-left-from-binary

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1880));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1878:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1877);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1878);

L_FOR1_END_1877:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1876:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1875);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1876);

L_FOR2_END_1875:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1879);
L_CLOS_CODE_1880: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1908));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1906:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1905);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1906);

L_FOR1_END_1905:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1904:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1903);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1904);

L_FOR2_END_1903:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1907);
L_CLOS_CODE_1908: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1915));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1913:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1912);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1913);

L_FOR1_END_1912:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1911:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1910);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1911);

L_FOR2_END_1910:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1914);
L_CLOS_CODE_1915: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1918); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1917);
		    L_THEN_1918:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_1916);
		    L_ELSE_1917:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_1916:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1914:

POP(FP);
RETURN;
L_CLOS_EXIT_1907:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1888));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1886:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1885);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1886);

L_FOR1_END_1885:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1884:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1883);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1884);

L_FOR2_END_1883:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1887);
L_CLOS_CODE_1888: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1894));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1892:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1891);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1892);

L_FOR1_END_1891:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1890:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1889);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1890);

L_FOR2_END_1889:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1893);
L_CLOS_CODE_1894: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1897); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1896);
		    L_THEN_1897:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));

		    JUMP(L_IF_EXIT_1895);
		    L_ELSE_1896:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_1895:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1893:

POP(FP);
RETURN;
L_CLOS_EXIT_1887:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1879:
MOV(R1, INDD(IMM(522), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1924);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1924:


 // DEFINE  ^char-op

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1862));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1860:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1859);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1860);

L_FOR1_END_1859:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1858:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1857);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1858);

L_FOR2_END_1857:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1861);
L_CLOS_CODE_1862: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1869));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1867:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1866);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1867);

L_FOR1_END_1866:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1865:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1864);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1865);

L_FOR2_END_1864:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1868);
L_CLOS_CODE_1869: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1868:

POP(FP);
RETURN;
L_CLOS_EXIT_1861:
MOV(R1, INDD(IMM(534), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1873);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1873:


 // DEFINE  <

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1354));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1855);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1855:


 // DEFINE  <=

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(64));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1853);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1853:


 // DEFINE  >

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(54));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1851);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1851:


 // DEFINE  >=

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(74));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1849);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1849:


 // DEFINE  char=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1462));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1847);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1847:


 // DEFINE  char<=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(64));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1844);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1844:


 // DEFINE  char<?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1354));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1841);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1841:


 // DEFINE  char>=?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(74));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1838);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1838:


 // DEFINE  char>?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(54));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(534));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1835);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1835:


 // DEFINE  char-uppercase?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1826));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1824:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1823);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1824);

L_FOR1_END_1823:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1822:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1821);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1822);

L_FOR2_END_1821:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1825);
L_CLOS_CODE_1826: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
MOV(R0,IMM(1464));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1829); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1828);
		    L_THEN_1829:
 //TC-APPLIC 

MOV(R0,IMM(1466));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1827);
		    L_ELSE_1828:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1827:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1825:
MOV(R1, INDD(IMM(627), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1832);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1832:


 // DEFINE  char-lowercase?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1813));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1811:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1810);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1811);

L_FOR1_END_1810:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1809:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1808);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1809);

L_FOR2_END_1808:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1812);
L_CLOS_CODE_1813: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
MOV(R0,IMM(1468));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(577));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1816); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1815);
		    L_THEN_1816:
 //TC-APPLIC 

MOV(R0,IMM(1470));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1814);
		    L_ELSE_1815:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1814:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1812:
MOV(R1, INDD(IMM(646), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1819);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1819:


 // DEFINE  char-upcase

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1464));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1468));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1788));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1786:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1785);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1786);

L_FOR1_END_1785:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1784:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1783);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1784);

L_FOR2_END_1783:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1787);
L_CLOS_CODE_1788: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1795));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1794);
L_CLOS_CODE_1795: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_1798); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1797);
		    L_THEN_1798:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1487));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_1796);
		    L_ELSE_1797:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_1796:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1794:

POP(FP);
RETURN;
L_CLOS_EXIT_1787:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1806);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1806:


 // DEFINE  char-downcase

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1464));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1468));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1762));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1760:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1759);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1760);

L_FOR1_END_1759:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1758:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1757);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1758);

L_FOR2_END_1757:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1761);
L_CLOS_CODE_1762: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1769));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1768);
L_CLOS_CODE_1769: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_1772); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1771);
		    L_THEN_1772:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1487));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_1770);
		    L_ELSE_1771:
		    // PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_1770:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1768:

POP(FP);
RETURN;
L_CLOS_EXIT_1761:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1780);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1780:


 // DEFINE  char-ci<=?

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1749);
L_CLOS_CODE_1750: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1754);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1754:


 // DEFINE  char-ci<?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1738));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1736:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1735);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1736);

L_FOR1_END_1735:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1734:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1733);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1734);

L_FOR2_END_1733:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1737);
L_CLOS_CODE_1738: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1737:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1742);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1742:


 // DEFINE  char-ci=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1726));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1724:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1723);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1724);

L_FOR1_END_1723:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1722:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1721);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1722);

L_FOR2_END_1721:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1725);
L_CLOS_CODE_1726: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1725:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1730);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1730:


 // DEFINE  char-ci>?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1714));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1712:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1711);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1712);

L_FOR1_END_1711:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1710:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1709);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1710);

L_FOR2_END_1709:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1713);
L_CLOS_CODE_1714: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1713:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1718);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1718:


 // DEFINE  char-ci>=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1702));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1700:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1699);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1700);

L_FOR1_END_1699:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1698:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1697);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1698);

L_FOR2_END_1697:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1701);
L_CLOS_CODE_1702: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1701:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1706);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1706:


 // DEFINE  string->list

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1678));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1676:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1675);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1676);

L_FOR1_END_1675:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1674:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1673);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1674);

L_FOR2_END_1673:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1677);
L_CLOS_CODE_1678: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1685));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1683:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1682);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1683);

L_FOR1_END_1682:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1681:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1680);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1681);

L_FOR2_END_1680:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1684);
L_CLOS_CODE_1685: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1506));
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
		    JUMP_NE(L_THEN_1688); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1687);
		    L_THEN_1688:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_1686);
		    L_ELSE_1687:
		    
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
MOV(R0, IMM(1520));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1337));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_1686:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1684:

POP(FP);
RETURN;
L_CLOS_EXIT_1677:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1661));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1659:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1658);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1659);

L_FOR1_END_1658:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1657:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1656);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1657);

L_FOR2_END_1656:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1660);
L_CLOS_CODE_1661: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1668));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1666:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1665);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1666);

L_FOR1_END_1665:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1664:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1663);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1664);

L_FOR2_END_1663:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1667);
L_CLOS_CODE_1668: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1337));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1504));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_1667:

POP(FP);
RETURN;
L_CLOS_EXIT_1660:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1694);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1694:


 // DEFINE  list->string

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1601);
L_CLOS_CODE_1602: 
PUSH(FP);
MOV(FP, SP);

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1609);
L_CLOS_CODE_1610: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1550));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1617);
L_CLOS_CODE_1618: 
PUSH(FP);
MOV(FP, SP);

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1633);
L_CLOS_CODE_1634: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1641));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_1639:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1638);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1639);

L_FOR1_END_1638:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1637:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1636);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1637);

L_FOR2_END_1636:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1640);
L_CLOS_CODE_1641: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
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
		    JUMP_NE(L_THEN_1644); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1643);
		    L_THEN_1644:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));

		    JUMP(L_IF_EXIT_1642);
		    L_ELSE_1643:
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1535));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1337));
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_1642:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1640:

POP(FP);
RETURN;
L_CLOS_EXIT_1633:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1626));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1624:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1623);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1624);

L_FOR1_END_1623:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1622:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1621);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1622);

L_FOR2_END_1621:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1625);
L_CLOS_CODE_1626: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(1356));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1625:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1617:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
MOV(R1, INDD(IMM(777), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1653);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1653:


 // DEFINE  string-upcase

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1591));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1589:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1588);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1589);

L_FOR1_END_1588:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1587:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1586);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1587);

L_FOR2_END_1586:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1590);
L_CLOS_CODE_1591: 
PUSH(FP);
MOV(FP, SP);

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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1590:
MOV(R1, INDD(IMM(794), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1595);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1595:


 // DEFINE  string-downcase

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1580));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1579);
L_CLOS_CODE_1580: 
PUSH(FP);
MOV(FP, SP);

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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1579:
MOV(R1, INDD(IMM(813), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1584);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1584:


 // DEFINE  list-ref

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1564));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1562:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1561);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1562);

L_FOR1_END_1561:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1560:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1559);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1560);

L_FOR2_END_1559:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1563);
L_CLOS_CODE_1564: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_1567); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1566);
		    L_THEN_1567:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_1565);
		    L_ELSE_1566:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1337));
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_1565:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1563:
MOV(R1, INDD(IMM(825), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1573);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1573:


 // DEFINE  list?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1548));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1546:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1545);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1546);

L_FOR1_END_1545:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1544:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1543);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1544);

L_FOR2_END_1543:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1547);
L_CLOS_CODE_1548: 
PUSH(FP);
MOV(FP, SP);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
JUMP_NE(L_OR_END_1549);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1549);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1552); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1551);
		    L_THEN_1552:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_1550);
		    L_ELSE_1551:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1550:
		    // OR
CMP(ADDR(R0), IMM(T_BOOL));
JUMP_NE(L_OR_END_1549);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1549);
L_OR_END_1549:

POP(FP);
RETURN;
L_CLOS_EXIT_1547:
MOV(R1, INDD(IMM(102), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1557);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1557:


 // DEFINE  ormap

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1535));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1537:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1538);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1537);

L_FOR1_END_1538:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1539:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1540);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1539);

L_FOR2_END_1540:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1536);
L_CLOS_CODE_1535: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1517));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1515:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1514);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1515);

L_FOR1_END_1514:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1513:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1512);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1513);

L_FOR2_END_1512:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1516);
L_CLOS_CODE_1517: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1524));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
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
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1520);

L_FOR2_END_1519:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1523);
L_CLOS_CODE_1524: 
PUSH(FP);
MOV(FP, SP);
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1527); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1526);
		    L_THEN_1527:
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
JUMP_NE(L_OR_END_1528);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1528);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
JUMP_NE(L_OR_END_1528);
CMP(INDD(R0,1), IMM(0));
JUMP_NE(L_OR_END_1528);
L_OR_END_1528:

		    JUMP(L_IF_EXIT_1525);
		    L_ELSE_1526:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1525:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1523:

POP(FP);
RETURN;
L_CLOS_EXIT_1516:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1509));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1507:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1506);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1507);

L_FOR1_END_1506:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1505:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1504);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1505);

L_FOR2_END_1504:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1508);
L_CLOS_CODE_1509: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1508:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1536:
MOV(R1, INDD(IMM(834), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1541);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1541:


 // DEFINE  member?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1491));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1489:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1488);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1489);

L_FOR1_END_1488:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1487:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1486);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1487);

L_FOR2_END_1486:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1490);
L_CLOS_CODE_1491: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1499));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1497:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1496);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1497);

L_FOR1_END_1496:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1495:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1494);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1495);

L_FOR2_END_1494:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1498);
L_CLOS_CODE_1499: 
PUSH(FP);
MOV(FP, SP);

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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1498:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(834));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1490:
MOV(R1, INDD(IMM(845), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1501);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1501:


 // DEFINE  negative?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1482));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1480:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1479);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1480);

L_FOR1_END_1479:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1478:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1477);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1478);

L_FOR2_END_1477:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1481);
L_CLOS_CODE_1482: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(1356));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1481:
MOV(R1, INDD(IMM(858), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1484);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1484:


 // DEFINE  positive?

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1472);
L_CLOS_CODE_1473: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(1356));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1472:
MOV(R1, INDD(IMM(871), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1475);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1475:


 // DEFINE  list->vector

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1415));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1413:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1412);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1413);

L_FOR1_END_1412:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1411:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1410);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1411);

L_FOR2_END_1410:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1414);
L_CLOS_CODE_1415: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1423));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1421:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1420);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1421);

L_FOR1_END_1420:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1419:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1418);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1419);

L_FOR2_END_1418:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1422);
L_CLOS_CODE_1423: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1430);
L_CLOS_CODE_1431: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1447));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1445:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1444);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1445);

L_FOR1_END_1444:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1443:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1442);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1443);

L_FOR2_END_1442:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1446);
L_CLOS_CODE_1447: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1454));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_1452:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1451);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1452);

L_FOR1_END_1451:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1450:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1449);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1450);

L_FOR2_END_1449:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1453);
L_CLOS_CODE_1454: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
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
		    JUMP_NE(L_THEN_1457); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1456);
		    L_THEN_1457:// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));

		    JUMP(L_IF_EXIT_1455);
		    L_ELSE_1456:
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1572));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1337));
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1453:

POP(FP);
RETURN;
L_CLOS_EXIT_1446:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1439));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_1437:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1436);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1437);

L_FOR1_END_1436:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1435:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1434);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1435);

L_FOR2_END_1434:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1438);
L_CLOS_CODE_1439: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(1356));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1438:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1430:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1422:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1414:
MOV(R1, INDD(IMM(887), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1466);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1466:


 // DEFINE  vector

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1406));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1404:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1403);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1404);

L_FOR1_END_1403:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1402:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1401);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_1402);

L_FOR2_END_1401:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1405);
L_CLOS_CODE_1406: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1405:
MOV(R1, INDD(IMM(897), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1408);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1408:


 // DEFINE  binary-string=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1374));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1372:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1371);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1372);

L_FOR1_END_1371:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1370:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1369);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1370);

L_FOR2_END_1369:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1373);
L_CLOS_CODE_1374: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1504));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1504));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1382));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1380:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1379);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1380);

L_FOR1_END_1379:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1378:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1377);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1378);

L_FOR2_END_1377:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1381);
L_CLOS_CODE_1382: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_1385); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1384);
		    L_THEN_1385:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1393));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1391:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1390);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1391);

L_FOR1_END_1390:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1389:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1388);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1389);

L_FOR2_END_1388:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1392);
L_CLOS_CODE_1393: 
PUSH(FP);
MOV(FP, SP);

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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(171));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_1392:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1383);
		    L_ELSE_1384:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1383:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1381:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1373:
MOV(R1, INDD(IMM(916), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1400);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1400:


 // DEFINE  binary-string<?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1320));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1318:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1317);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1318);

L_FOR1_END_1317:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1316:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1315);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1316);

L_FOR2_END_1315:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1319);
L_CLOS_CODE_1320: 
PUSH(FP);
MOV(FP, SP);

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1337);
L_CLOS_CODE_1338: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1345));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1343:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1342);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1343);

L_FOR1_END_1342:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1341:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1340);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1341);

L_FOR2_END_1340:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1344);
L_CLOS_CODE_1345: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1348); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1347);
		    L_THEN_1348:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_1346);
		    L_ELSE_1347:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1351); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1350);
		    L_THEN_1351:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_1349);
		    L_ELSE_1350:
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1354); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1353);
		    L_THEN_1354:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1352);
		    L_ELSE_1353:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_1352:
		    
		    L_IF_EXIT_1349:
		    
		    L_IF_EXIT_1346:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1344:

POP(FP);
RETURN;
L_CLOS_EXIT_1337:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1328));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1326:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1325);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1326);

L_FOR1_END_1325:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1324:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1323);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1324);

L_FOR2_END_1323:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1327);
L_CLOS_CODE_1328: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1327:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1319:
MOV(R1, INDD(IMM(935), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1367);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1367:


 // DEFINE  binary-string>?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1311));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1309:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1308);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1309);

L_FOR1_END_1308:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1307:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1306);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1307);

L_FOR2_END_1306:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1310);
L_CLOS_CODE_1311: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1310:
MOV(R1, INDD(IMM(954), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1313);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1313:


 // DEFINE  binary-string<=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1301));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1299:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1298);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1299);

L_FOR1_END_1298:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1297:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1296);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1297);

L_FOR2_END_1296:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1300);
L_CLOS_CODE_1301: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1300:
MOV(R1, INDD(IMM(974), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1304);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1304:


 // DEFINE  binary-string>=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1291));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1289:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1288);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1289);

L_FOR1_END_1288:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1287:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1286);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1287);

L_FOR2_END_1286:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1290);
L_CLOS_CODE_1291: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_1290:
MOV(R1, INDD(IMM(994), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1294);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1294:


 // DEFINE  string=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(916));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1284);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1284:


 // DEFINE  string<?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(935));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1282);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1282:


 // DEFINE  string>?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(954));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1280);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1280:


 // DEFINE  string<=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(974));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1278);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1278:


 // DEFINE  string>=?

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(994));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(161));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1276);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1276:


 // DEFINE  vector->list

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1258));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1256:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1255);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1256);

L_FOR1_END_1255:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1254:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1253);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1254);

L_FOR2_END_1253:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1257);
L_CLOS_CODE_1258: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1265));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1263:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1262);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1263);

L_FOR1_END_1262:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1261:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1260);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1261);

L_FOR2_END_1260:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1264);
L_CLOS_CODE_1265: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1506));
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
		    JUMP_NE(L_THEN_1268); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1267);
		    L_THEN_1268:// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_1266);
		    L_ELSE_1267:
		    
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
MOV(R0, IMM(1618));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1337));
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_1266:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1264:

POP(FP);
RETURN;
L_CLOS_EXIT_1257:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1241));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1239:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1238);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1239);

L_FOR1_END_1238:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1237:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1236);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1237);

L_FOR2_END_1236:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1240);
L_CLOS_CODE_1241: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1248));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1246:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1245);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1246);

L_FOR1_END_1245:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1244:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1243);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1244);

L_FOR2_END_1243:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1247);
L_CLOS_CODE_1248: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

MOV(R0,IMM(11));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1337));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1604));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_1247:

POP(FP);
RETURN;
L_CLOS_EXIT_1240:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1274);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1274:


 // DEFINE  boolean=?

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1220));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1218:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1217);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1218);

L_FOR1_END_1217:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1216:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1215);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1216);

L_FOR2_END_1215:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1219);
L_CLOS_CODE_1220: 
PUSH(FP);
MOV(FP, SP);
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
MOV(R0, IMM(1630));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1230); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1229);
		    L_THEN_1230:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1630));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1228);
		    L_ELSE_1229:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1228:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1223); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1222);
		    L_THEN_1223://IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1226); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1225);
		    L_THEN_1226:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_1224);
		    L_ELSE_1225:
		    
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    L_IF_EXIT_1224:
		    
		    JUMP(L_IF_EXIT_1221);
		    L_ELSE_1222:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1221:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1219:
MOV(R1, INDD(IMM(1085), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1233);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1233:


 // DEFINE  equal?

 //APPLIC 

PUSH(IMM(0));
//IF 
MOV(R0,IMM(12));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1212); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1211);
		    L_THEN_1212:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_1210);
		    L_ELSE_1211:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_1210:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1098));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1096:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1095);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1096);

L_FOR1_END_1095:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1094:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1093);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1094);

L_FOR2_END_1093:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1097);
L_CLOS_CODE_1098: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1113));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1111:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1110);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1111);

L_FOR1_END_1110:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1109:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1108);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1109);

L_FOR2_END_1108:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1112);
L_CLOS_CODE_1113: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1120));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_1118:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1117);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1118);

L_FOR1_END_1117:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1116:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1115);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1116);

L_FOR2_END_1115:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1119);
L_CLOS_CODE_1120: 
PUSH(FP);
MOV(FP, SP);
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
MOV(R0, IMM(1630));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1207); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1206);
		    L_THEN_1207:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1630));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1205);
		    L_ELSE_1206:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1205:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1123); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1122);
		    L_THEN_1123:
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1121);
		    L_ELSE_1122:
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
MOV(R0, IMM(1639));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1201); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1200);
		    L_THEN_1201:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1639));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1199);
		    L_ELSE_1200:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1199:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1126); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1125);
		    L_THEN_1126:
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1124);
		    L_ELSE_1125:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1129); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1128);
		    L_THEN_1129:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_1127);
		    L_ELSE_1128:
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
MOV(R0, IMM(1650));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1193); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1192);
		    L_THEN_1193:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1650));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1191);
		    L_ELSE_1192:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1191:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1132); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1131);
		    L_THEN_1132:
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1130);
		    L_ELSE_1131:
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
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1187); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1186);
		    L_THEN_1187:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1185);
		    L_ELSE_1186:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1185:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1135); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1134);
		    L_THEN_1135://IF 

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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1178); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1177);
		    L_THEN_1178:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1176);
		    L_ELSE_1177:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1176:
		    
		    JUMP(L_IF_EXIT_1133);
		    L_ELSE_1134:
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
MOV(R0, IMM(1661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1173); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1172);
		    L_THEN_1173:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1171);
		    L_ELSE_1172:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1171:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1138); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1137);
		    L_THEN_1138:
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1136);
		    L_ELSE_1137:
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
MOV(R0, IMM(1672));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1167); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1166);
		    L_THEN_1167:
 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1672));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1165);
		    L_ELSE_1166:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1165:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1141); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1140);
		    L_THEN_1141:
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
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1139);
		    L_ELSE_1140:
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
MOV(R0, IMM(1683));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1155); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1154);
		    L_THEN_1155://IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1683));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1158); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1157);
		    L_THEN_1158:
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
MOV(R0, IMM(1604));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1604));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 

		    JUMP(L_IF_EXIT_1156);
		    L_ELSE_1157:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1156:
		    
		    JUMP(L_IF_EXIT_1153);
		    L_ELSE_1154:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1153:
		    
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1144); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1143);
		    L_THEN_1144:
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1142);
		    L_ELSE_1143:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1147); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1146);
		    L_THEN_1147:
 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_1145);
		    L_ELSE_1146:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_1145:
		    
		    L_IF_EXIT_1142:
		    
		    L_IF_EXIT_1139:
		    
		    L_IF_EXIT_1136:
		    
		    L_IF_EXIT_1133:
		    
		    L_IF_EXIT_1130:
		    
		    L_IF_EXIT_1127:
		    
		    L_IF_EXIT_1124:
		    
		    L_IF_EXIT_1121:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1119:

POP(FP);
RETURN;
L_CLOS_EXIT_1112:
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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1105);
L_CLOS_CODE_1106: 
PUSH(FP);
MOV(FP, SP);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_1105:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1097:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1213);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1213:


 // DEFINE  ^associate

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1055));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1053:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1052);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1053);

L_FOR1_END_1052:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1051:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1050);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1051);

L_FOR2_END_1050:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1054);
L_CLOS_CODE_1055: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1070));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1068:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1067);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1068);

L_FOR1_END_1067:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1066:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1065);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1066);

L_FOR2_END_1065:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1069);
L_CLOS_CODE_1070: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1077));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1076);
L_CLOS_CODE_1077: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1080); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1079);
		    L_THEN_1080:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_1078);
		    L_ELSE_1079:
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
		    JUMP_NE(L_THEN_1083); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1082);
		    L_THEN_1083:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_1081);
		    L_ELSE_1082:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_1081:
		    
		    L_IF_EXIT_1078:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_1076:

POP(FP);
RETURN;
L_CLOS_EXIT_1069:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1063));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1061:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1060);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1061);

L_FOR1_END_1060:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1059:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1058);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1059);

L_FOR2_END_1058:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1062);
L_CLOS_CODE_1063: 
PUSH(FP);
MOV(FP, SP);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

POP(FP);
RETURN;
L_CLOS_EXIT_1062:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1054:
MOV(R1, INDD(IMM(1109), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1090);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1090:


 // DEFINE  assoc

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1095));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1109));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1048);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1048:


 // DEFINE  assq

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1109));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1046);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1046:


 // DEFINE  void

 //APPLIC 

PUSH(IMM(0));
//IF 
MOV(R0,IMM(12));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_1043); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1042);
		    L_THEN_1043:MOV(R0,IMM(12));

		    JUMP(L_IF_EXIT_1041);
		    L_ELSE_1042:
		    MOV(R0,IMM(10));

		    L_IF_EXIT_1041:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1033));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1031:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1030);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1031);

L_FOR1_END_1030:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1029:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1028);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1029);

L_FOR2_END_1028:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1032);
L_CLOS_CODE_1033: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1040));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_1038:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1037);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1038);

L_FOR1_END_1037:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1036:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1035);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1036);

L_FOR2_END_1035:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1039);
L_CLOS_CODE_1040: 
PUSH(FP);
MOV(FP, SP);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));

POP(FP);
RETURN;
L_CLOS_EXIT_1039:

POP(FP);
RETURN;
L_CLOS_EXIT_1032:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1044);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1044:


 // DEFINE  void?

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(1134));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1015));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_1013:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_1012);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_1013);

L_FOR1_END_1012:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_1011:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_1010);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_1011);

L_FOR2_END_1010:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1014);
L_CLOS_CODE_1015: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_1022));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_1021);
L_CLOS_CODE_1022: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1557));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_1021:

POP(FP);
RETURN;
L_CLOS_EXIT_1014:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1025);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1025:


 // DEFINE  append

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_991));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_989:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_988);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_989);

L_FOR1_END_988:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_987:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_986);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_987);

L_FOR2_END_986:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_990);
L_CLOS_CODE_991: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_998));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_997);
L_CLOS_CODE_998: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_1001); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_1000);
		    L_THEN_1001:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_999);
		    L_ELSE_1000:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_999:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_997:

POP(FP);
RETURN;
L_CLOS_EXIT_990:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_969));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_967:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_966);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_967);

L_FOR1_END_966:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_965:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_964);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_965);

L_FOR2_END_964:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_968);
L_CLOS_CODE_969: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_976));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_974:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_973);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_974);

L_FOR1_END_973:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_972:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_971);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_972);

L_FOR2_END_971:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_975);
L_CLOS_CODE_976: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_979); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_978);
		    L_THEN_979:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_977);
		    L_ELSE_978:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_977:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_975:

POP(FP);
RETURN;
L_CLOS_EXIT_968:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_949));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_947:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_946);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_947);

L_FOR1_END_946:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_945:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_944);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_945);

L_FOR2_END_944:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_948);
L_CLOS_CODE_949: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_955));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_953:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_952);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_953);

L_FOR1_END_952:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_951:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_950);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_951);

L_FOR2_END_950:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_954);
L_CLOS_CODE_955: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_958); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_957);
		    L_THEN_958:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_956);
		    L_ELSE_957:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_956:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_954:

POP(FP);
RETURN;
L_CLOS_EXIT_948:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_1007);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_1007:


 // DEFINE  string-append

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_937));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_935:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_934);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_935);

L_FOR1_END_934:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_933:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_932);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_933);

L_FOR2_END_932:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_936);
L_CLOS_CODE_937: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_936:
MOV(R1, INDD(IMM(1170), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_941);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_941:


 // DEFINE  vector-append

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_927));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_925:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_924);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_925);

L_FOR1_END_924:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);

//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_923:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_922);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_923);

L_FOR2_END_922:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_926);
L_CLOS_CODE_927: 

PUSH(FP);
MOV(FP, SP);
MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1420));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_926:
MOV(R1, INDD(IMM(1187), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_931);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_931:


 // DEFINE  reverse

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_905));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_903:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_902);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_903);

L_FOR1_END_902:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_901:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_900);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_901);

L_FOR2_END_900:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_904);
L_CLOS_CODE_905: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_912));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_910:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_909);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_910);

L_FOR1_END_909:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_908:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_907);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_908);

L_FOR2_END_907:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_911);
L_CLOS_CODE_912: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_915); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_914);
		    L_THEN_915:// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_913);
		    L_ELSE_914:
		    
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_913:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_911:

POP(FP);
RETURN;
L_CLOS_EXIT_904:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_890));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_888:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_887);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_888);

L_FOR1_END_887:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_886:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_885);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_886);

L_FOR2_END_885:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_889);
L_CLOS_CODE_890: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_897));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_896);
L_CLOS_CODE_897: 
PUSH(FP);
MOV(FP, SP);

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_896:

POP(FP);
RETURN;
L_CLOS_EXIT_889:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_921);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_921:


 // DEFINE  string-reverse

 //APPLIC 

PUSH(IMM(0));

 //FVAR 
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1198));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(777));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
PUSH(R0);

 //FVAR 
MOV(R0, IMM(197));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_882);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_882:


 // DEFINE  list-ref

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_871));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_869:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_868);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_869);

L_FOR1_END_868:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_867:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_866);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_867);

L_FOR2_END_866:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_870);
L_CLOS_CODE_871: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_874); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_873);
		    L_THEN_874:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_872);
		    L_ELSE_873:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1337));
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_872:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_870:
MOV(R1, INDD(IMM(825), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_880);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_880:


 // DEFINE  list-set!

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_854);
L_CLOS_CODE_855: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_858); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_857);
		    L_THEN_858:
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
MOV(R0, IMM(1695));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_856);
		    L_ELSE_857:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(2));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1337));
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_856:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_854:
MOV(R1, INDD(IMM(1229), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_864);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_864:


 // DEFINE  max

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_843));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_841:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_840);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_841);

L_FOR1_END_840:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_839:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_838);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_839);

L_FOR2_END_838:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_842);
L_CLOS_CODE_843: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_846); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_845);
		    L_THEN_846:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_844);
		    L_ELSE_845:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_844:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_842:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_829));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_827:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_826);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_827);

L_FOR1_END_826:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_825:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_824);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_825);

L_FOR2_END_824:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_828);
L_CLOS_CODE_829: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_831));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_833:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_834);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_833);

L_FOR1_END_834:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_835:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_836);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_835);

L_FOR2_END_836:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_832);
L_CLOS_CODE_831: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(186));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_832:

POP(FP);
RETURN;
L_CLOS_EXIT_828:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_848);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_848:


 // DEFINE  min

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_816));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_814:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_813);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_814);

L_FOR1_END_813:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_812:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_811);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_812);

L_FOR2_END_811:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_815);
L_CLOS_CODE_816: 
PUSH(FP);
MOV(FP, SP);
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
		    JUMP_NE(L_THEN_819); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_818);
		    L_THEN_819:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_817);
		    L_ELSE_818:
		    // PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    L_IF_EXIT_817:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_815:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_802));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_800:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_799);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_800);

L_FOR1_END_799:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_798:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_797);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_798);

L_FOR2_END_797:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_801);
L_CLOS_CODE_802: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_804));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_806:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_807);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_806);

L_FOR1_END_807:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_808:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_809);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_808);

L_FOR2_END_809:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_805);
L_CLOS_CODE_804: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(186));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_805:

POP(FP);
RETURN;
L_CLOS_EXIT_801:
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_821);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_821:


 // DEFINE  error
MOV(R0,IMM(1337));
MOV(R1, INDD(IMM(1252), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_794);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_794:


 // DEFINE  number->string

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_773));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_771:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_770);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_771);

L_FOR1_END_770:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_769:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_768);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_769);

L_FOR2_END_768:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_772);
L_CLOS_CODE_773: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_780));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_778:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_777);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_778);

L_FOR1_END_777:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_776:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_775);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_776);

L_FOR2_END_775:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_779);
L_CLOS_CODE_780: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1701));
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
		    JUMP_NE(L_THEN_783); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_782);
		    L_THEN_783:
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_781);
		    L_ELSE_782:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1701));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1394));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1701));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1701));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1394));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_781:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_779:

POP(FP);
RETURN;
L_CLOS_EXIT_772:
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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_753);
L_CLOS_CODE_754: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1699));
PUSH(R0);
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_763));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_761:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_760);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_761);

L_FOR1_END_760:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_759:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_758);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_759);

L_FOR2_END_758:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_762);
L_CLOS_CODE_763: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1697));
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
MOV(R0, IMM(1487));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_762:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(126));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_753:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_793);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_793:


 // DEFINE  =

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_726));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_725);
L_CLOS_CODE_726: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_733));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_731:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_730);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_731);

L_FOR1_END_730:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_729:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_728);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_729);

L_FOR2_END_728:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_732);
L_CLOS_CODE_733: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_736); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_735);
		    L_THEN_736:MOV(R0,IMM(14));

		    JUMP(L_IF_EXIT_734);
		    L_ELSE_735:
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1462));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_739); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_738);
		    L_THEN_739:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_737);
		    L_ELSE_738:
		    MOV(R0,IMM(12));

		    L_IF_EXIT_737:
		    
		    L_IF_EXIT_734:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_732:

POP(FP);
RETURN;
L_CLOS_EXIT_725:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_712));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_710:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_709);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_710);

L_FOR1_END_709:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_708:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_707);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_708);

L_FOR2_END_707:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_711);
L_CLOS_CODE_712: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_714));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_716:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_717);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_716);

L_FOR1_END_717:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_718:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_719);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_718);

L_FOR2_END_719:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_715);
L_CLOS_CODE_714: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_715:

POP(FP);
RETURN;
L_CLOS_EXIT_711:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_746);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_746:


 // DEFINE  format

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_649));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_647:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_646);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_647);

L_FOR1_END_646:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_645:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_644);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_645);

L_FOR2_END_644:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_648);
L_CLOS_CODE_649: 
PUSH(FP);
MOV(FP, SP);

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_655);
L_CLOS_CODE_656: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_659); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_658);
		    L_THEN_659:
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1468));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_662); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_661);
		    L_THEN_662:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1327));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_660);
		    L_ELSE_661:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1725));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_665); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_664);
		    L_THEN_665:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1302));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_663);
		    L_ELSE_664:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1727));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_668); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_667);
		    L_THEN_668:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1729));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_666);
		    L_ELSE_667:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1723));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_671); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_670);
		    L_THEN_671:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1723));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_669);
		    L_ELSE_670:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1731));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1285));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1252));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_669:
		    
		    L_IF_EXIT_666:
		    
		    L_IF_EXIT_663:
		    
		    L_IF_EXIT_660:
		    
		    L_IF_EXIT_657:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_655:

POP(FP);
RETURN;
L_CLOS_EXIT_648:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_615));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_613:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_612);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_613);

L_FOR1_END_612:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_611:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_610);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_611);

L_FOR2_END_610:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_614);
L_CLOS_CODE_615: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_622));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_621);
L_CLOS_CODE_622: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_625); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_624);
		    L_THEN_625://IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_639); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_638);
		    L_THEN_639:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_637);
		    L_ELSE_638:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1703));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1285));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1252));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_637:
		    
		    JUMP(L_IF_EXIT_623);
		    L_ELSE_624:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1723));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_628); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_627);
		    L_THEN_628:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(1));
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_626);
		    L_ELSE_627:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_626:
		    
		    L_IF_EXIT_623:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_621:

POP(FP);
RETURN;
L_CLOS_EXIT_614:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_599));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_597:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_596);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_597);

L_FOR1_END_596:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_595:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_594);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_595);

L_FOR2_END_594:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_598);
L_CLOS_CODE_599: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_603));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_605:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_606);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_605);

L_FOR1_END_606:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(2));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_607:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_608);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(L_FOR2_START_607);

L_FOR2_END_608:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_604);
L_CLOS_CODE_603: 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(1));
CALL(MAKE_LIST);
MOV(FPARG(IMM(4)),R0);
MOV(FPARG(IMM(2)), IMM(2));

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
MOV(R0, IMM(761));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_604:

POP(FP);
RETURN;
L_CLOS_EXIT_598:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1285), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_704);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_704:


 // DEFINE  sexpr->string

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_557));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_555:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_554);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_555);

L_FOR1_END_554:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_553:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_552);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_553);

L_FOR2_END_552:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_556);
L_CLOS_CODE_557: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_564));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_562:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_561);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_562);

L_FOR1_END_561:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_560:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_559);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_560);

L_FOR2_END_559:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_563);
L_CLOS_CODE_564: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1729));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_567); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_566);
		    L_THEN_567:MOV(R0,IMM(1945));

		    JUMP(L_IF_EXIT_565);
		    L_ELSE_566:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1885));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_570); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_569);
		    L_THEN_570:MOV(R0,IMM(1956));

		    JUMP(L_IF_EXIT_568);
		    L_ELSE_569:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1965));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_573); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_572);
		    L_THEN_573:MOV(R0,IMM(1967));

		    JUMP(L_IF_EXIT_571);
		    L_ELSE_572:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1866));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(566));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_576); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_575);
		    L_THEN_576:MOV(R0,IMM(1974));

		    JUMP(L_IF_EXIT_574);
		    L_ELSE_575:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1885));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(587));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_579); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_578);
		    L_THEN_579:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

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
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 4));
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
MOV(R0,IMM(1985));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_577);
		    L_ELSE_578:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
MOV(R0,IMM(1856));
PUSH(R0);
MOV(R0,IMM(1983));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(29));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    L_IF_EXIT_577:
		    
		    L_IF_EXIT_574:
		    
		    L_IF_EXIT_571:
		    
		    L_IF_EXIT_568:
		    
		    L_IF_EXIT_565:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_563:

POP(FP);
RETURN;
L_CLOS_EXIT_556:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_504));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_503);
L_CLOS_CODE_504: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_511));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_509:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_508);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_509);

L_FOR1_END_508:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_507:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_506);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_507);

L_FOR2_END_506:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_510);
L_CLOS_CODE_511: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_519));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_518);
L_CLOS_CODE_519: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_522); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_521);
		    L_THEN_522:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_520);
		    L_ELSE_521:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_525); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_524);
		    L_THEN_525:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 6));
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
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_543));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_542);
L_CLOS_CODE_543: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1937));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_542:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_523);
		    L_ELSE_524:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
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
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_533));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_532);
L_CLOS_CODE_533: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1940));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_532:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    L_IF_EXIT_523:
		    
		    L_IF_EXIT_520:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_518:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_510:

POP(FP);
RETURN;
L_CLOS_EXIT_503:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_479));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_477:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_476);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_477);

L_FOR1_END_476:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_475:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_474);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_475);

L_FOR2_END_474:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_478);
L_CLOS_CODE_479: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1935));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_486);
L_CLOS_CODE_487: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_494));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_492:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_491);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_492);

L_FOR1_END_491:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_490:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_489);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_490);

L_FOR2_END_489:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_493);
L_CLOS_CODE_494: 
PUSH(FP);
MOV(FP, SP);

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
PUSH(R0);

 //FVAR 
MOV(R0, IMM(137));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1487));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_493:

POP(FP);
RETURN;
L_CLOS_EXIT_486:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_478:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_402));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_400:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_399);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_400);

L_FOR1_END_399:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_398:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_397);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_398);

L_FOR2_END_397:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_401);
L_CLOS_CODE_402: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_409));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_407:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_406);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_407);

L_FOR1_END_406:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_405:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_404);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_405);

L_FOR2_END_404:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_408);
L_CLOS_CODE_409: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1933));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1394));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 5));
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_417));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_415:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_414);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_415);

L_FOR1_END_414:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_413:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_412);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_413);

L_FOR2_END_412:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_416);
L_CLOS_CODE_417: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1933));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1931));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_425));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_423:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_422);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_423);

L_FOR1_END_422:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_421:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_420);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_421);

L_FOR2_END_420:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_424);
L_CLOS_CODE_425: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1933));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1394));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 5));
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
PUSH(IMM(6));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_433));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(5));

L_FOR1_START_431:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_430);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_431);

L_FOR1_END_430:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_429:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_428);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_429);

L_FOR2_END_428:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_432);
L_CLOS_CODE_433: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1933));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1931));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(IMM(7));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_441));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(6));

L_FOR1_START_439:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_438);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_439);

L_FOR1_END_438:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_437:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_436);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_437);

L_FOR2_END_436:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_440);
L_CLOS_CODE_441: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1933));
PUSH(R0);
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1394));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 4));
MOV(R0, INDD(R0, 5));
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
PUSH(IMM(8));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_449));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(7));

L_FOR1_START_447:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_446);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_447);

L_FOR1_END_446:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_445:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_444);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_445);

L_FOR2_END_444:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_448);
L_CLOS_CODE_449: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1933));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1931));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
PUSH(IMM(9));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_457));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(8));

L_FOR1_START_455:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_454);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_455);

L_FOR1_END_454:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_453:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_452);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_453);

L_FOR2_END_452:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_456);
L_CLOS_CODE_457: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(83));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_460); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_459);
		    L_THEN_460:
 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 4));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 2));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(29));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    JUMP(L_IF_EXIT_458);
		    L_ELSE_459:
		    
 //TC-APPLIC 

// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 5));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1902));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1900));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1252));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_458:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_456:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_448:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_440:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_432:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_424:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_416:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_408:

POP(FP);
RETURN;
L_CLOS_EXIT_401:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_311));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_309:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_308);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_309);

L_FOR1_END_308:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_307:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_306);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_307);

L_FOR2_END_306:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_310);
L_CLOS_CODE_311: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_337));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_335:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_334);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_335);

L_FOR1_END_334:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_333:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_332);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_333);

L_FOR2_END_332:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_336);
L_CLOS_CODE_337: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_344));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_343);
L_CLOS_CODE_344: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_347); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_346);
		    L_THEN_347:MOV(R0,IMM(11));

		    JUMP(L_IF_EXIT_345);
		    L_ELSE_346:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1729));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_350); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_349);
		    L_THEN_350:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1863));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_348);
		    L_ELSE_349:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1866));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_353); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_352);
		    L_THEN_353:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1873));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_351);
		    L_ELSE_352:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1856));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_356); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_355);
		    L_THEN_356:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1879));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_354);
		    L_ELSE_355:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1851));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_359); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_358);
		    L_THEN_359:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0,IMM(1882));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    JUMP(L_IF_EXIT_357);
		    L_ELSE_358:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1885));
PUSH(R0);

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(587));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_362); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_361);
		    L_THEN_362:
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1453));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 4));
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
MOV(R0,IMM(1876));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    JUMP(L_IF_EXIT_360);
		    L_ELSE_361:
		    
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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1411));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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

		    L_IF_EXIT_360:
		    
		    L_IF_EXIT_357:
		    
		    L_IF_EXIT_354:
		    
		    L_IF_EXIT_351:
		    
		    L_IF_EXIT_348:
		    
		    L_IF_EXIT_345:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_343:

POP(FP);
RETURN;
L_CLOS_EXIT_336:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_319));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_318);
L_CLOS_CODE_319: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_326));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_324:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_323);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_324);

L_FOR1_END_323:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_322:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_321);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_322);

L_FOR2_END_321:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_325);
L_CLOS_CODE_326: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1853));
PUSH(R0);

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
MOV(R0,IMM(1853));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1153));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(3)); // pushing number of arguments 
PUSH(IMM(3)); // pushing again for reference for frame removal 
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_325:

POP(FP);
RETURN;
L_CLOS_EXIT_318:
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_310:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_284));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_283);
L_CLOS_CODE_284: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_291));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_289:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_288);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_289);

L_FOR1_END_288:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_287:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_286);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_287);

L_FOR2_END_286:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_290);
L_CLOS_CODE_291: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


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
MOV(R0, IMM(1604));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_299));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_297:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_296);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_297);

L_FOR1_END_296:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_295:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_294);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_295);

L_FOR2_END_294:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_298);
L_CLOS_CODE_299: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
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
MOV(R0,IMM(1848));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_298:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_290:

POP(FP);
RETURN;
L_CLOS_EXIT_283:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_217));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_215:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_214);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_215);

L_FOR1_END_214:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_213:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_212);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_213);

L_FOR2_END_212:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_216);
L_CLOS_CODE_217: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_224));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

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
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
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
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_220);

L_FOR2_END_219:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_223);
L_CLOS_CODE_224: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_227); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_226);
		    L_THEN_227:MOV(R0,IMM(1762));

		    JUMP(L_IF_EXIT_225);
		    L_ELSE_226:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1630));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_230); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_229);
		    L_THEN_230://IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_275); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_274);
		    L_THEN_275:MOV(R0,IMM(1766));

		    JUMP(L_IF_EXIT_273);
		    L_ELSE_274:
		    MOV(R0,IMM(1770));

		    L_IF_EXIT_273:
		    
		    JUMP(L_IF_EXIT_228);
		    L_ELSE_229:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1639));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_233); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_232);
		    L_THEN_233:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 7));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_231);
		    L_ELSE_232:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1650));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_236); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_235);
		    L_THEN_236:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1270));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_234);
		    L_ELSE_235:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1672));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_239); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_238);
		    L_THEN_239:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1790));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_237);
		    L_ELSE_238:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_242); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_241);
		    L_THEN_242:
 //TC-APPLIC 

MOV(R0,IMM(1795));
PUSH(R0); // finished evaluating arg 

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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 6));
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
MOV(R0,IMM(1792));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    JUMP(L_IF_EXIT_240);
		    L_ELSE_241:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_245); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_244);
		    L_THEN_245:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 3));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_243);
		    L_ELSE_244:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1683));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    L_THEN_248:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 2));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_246);
		    L_ELSE_247:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1143));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_251); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_250);
		    L_THEN_251:MOV(R0,IMM(1798));

		    JUMP(L_IF_EXIT_249);
		    L_ELSE_250:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1819));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_254); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_253);
		    L_THEN_254:MOV(R0,IMM(1821));

		    JUMP(L_IF_EXIT_252);
		    L_ELSE_253:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1835));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1302));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1252));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_252:
		    
		    L_IF_EXIT_249:
		    
		    L_IF_EXIT_246:
		    
		    L_IF_EXIT_243:
		    
		    L_IF_EXIT_240:
		    
		    L_IF_EXIT_237:
		    
		    L_IF_EXIT_234:
		    
		    L_IF_EXIT_231:
		    
		    L_IF_EXIT_228:
		    
		    L_IF_EXIT_225:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_223:

POP(FP);
RETURN;
L_CLOS_EXIT_216:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_198));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_196:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_195);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_196);

L_FOR1_END_195:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_194:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_193);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_194);

L_FOR2_END_193:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_197);
L_CLOS_CODE_198: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_205));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_203:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_202);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_203);

L_FOR1_END_202:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_201:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_200);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_201);

L_FOR2_END_200:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_204);
L_CLOS_CODE_205: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1143));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_208); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_207);
		    L_THEN_208:MOV(R0,IMM(1760));

		    JUMP(L_IF_EXIT_206);
		    L_ELSE_207:
		    
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    L_IF_EXIT_206:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_204:

POP(FP);
RETURN;
L_CLOS_EXIT_197:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(8)); // pushing number of arguments 
PUSH(IMM(8)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1302), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_591);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_591:


 // DEFINE  sexpr->display-string

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_180));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_178:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_177);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_178);

L_FOR1_END_177:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_176:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_175);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_176);

L_FOR2_END_175:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_179);
L_CLOS_CODE_180: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_187));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_185:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_184);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_185);

L_FOR1_END_184:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_183:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_182);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_183);

L_FOR2_END_182:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_186);
L_CLOS_CODE_187: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(29));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(777));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_186:

POP(FP);
RETURN;
L_CLOS_EXIT_179:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_127));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_125:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_124);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_125);

L_FOR1_END_124:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_123:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_122);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_123);

L_FOR2_END_122:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_126);
L_CLOS_CODE_127: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_134));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_132:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_131);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_132);

L_FOR1_END_131:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_130:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_129);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_130);

L_FOR2_END_129:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_133);
L_CLOS_CODE_134: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_142));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_141);
L_CLOS_CODE_142: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_145); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_144);
		    L_THEN_145:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_143);
		    L_ELSE_144:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_148); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_147);
		    L_THEN_148:
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 3));
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
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_166));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_164:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_163);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_164);

L_FOR1_END_163:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_162:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_161);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_162);

L_FOR2_END_161:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_165);
L_CLOS_CODE_166: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1937));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_165:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_146);
		    L_ELSE_147:
		    
 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
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
PUSH(IMM(5));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_156));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(4));

L_FOR1_START_154:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_153);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_154);

L_FOR1_END_153:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_152:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_151);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_152);

L_FOR2_END_151:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_155);
L_CLOS_CODE_156: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1940));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 0));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_155:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    L_IF_EXIT_146:
		    
		    L_IF_EXIT_143:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_141:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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
L_CLOS_EXIT_133:

POP(FP);
RETURN;
L_CLOS_EXIT_126:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_100));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_98:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_97);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_98);

L_FOR1_END_97:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_96:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_95);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_96);

L_FOR2_END_95:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_99);
L_CLOS_CODE_100: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_107));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_105:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_104);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_105);

L_FOR1_END_104:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_103:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_102);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_103);

L_FOR2_END_102:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_106);
L_CLOS_CODE_107: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


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
MOV(R0, IMM(1604));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_115));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(3));

L_FOR1_START_113:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_112);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_113);

L_FOR1_END_112:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_111:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_110);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_111);

L_FOR2_END_110:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_114);
L_CLOS_CODE_115: 
PUSH(FP);
MOV(FP, SP);

 //TC-APPLIC 


 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(1));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
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
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 1));
MOV(R0, INDD(R0, 1));
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
MOV(R0,IMM(1848));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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
L_CLOS_EXIT_114:
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
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
L_CLOS_EXIT_106:

POP(FP);
RETURN;
L_CLOS_EXIT_99:
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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_34));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(1));

L_FOR1_START_32:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_31);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_32);

L_FOR1_END_31:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_30:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_29);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_30);

L_FOR2_END_29:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_33);
L_CLOS_CODE_34: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_41));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_39:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_38);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_39);

L_FOR1_END_38:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_37:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_36);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_37);

L_FOR2_END_36:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_40);
L_CLOS_CODE_41: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1365));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_44); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_43);
		    L_THEN_44:MOV(R0,IMM(1762));

		    JUMP(L_IF_EXIT_42);
		    L_ELSE_43:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1630));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_47); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_46);
		    L_THEN_47://IF 
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(L_THEN_91); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_90);
		    L_THEN_91:MOV(R0,IMM(1766));

		    JUMP(L_IF_EXIT_89);
		    L_ELSE_90:
		    MOV(R0,IMM(1770));

		    L_IF_EXIT_89:
		    
		    JUMP(L_IF_EXIT_45);
		    L_ELSE_46:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1639));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_50); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_49);
		    L_THEN_50:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 4));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_48);
		    L_ELSE_49:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1650));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_53); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_52);
		    L_THEN_53:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1270));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_51);
		    L_ELSE_52:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1672));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_56); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_55);
		    L_THEN_56:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1790));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_54);
		    L_ELSE_55:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1381));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_59); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_58);
		    L_THEN_59:
 //TC-APPLIC 

MOV(R0,IMM(1795));
PUSH(R0); // finished evaluating arg 

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
MOV(R0, IMM(1372));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1403));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, INDD(R0, 3));
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
MOV(R0,IMM(1792));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1170));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    JUMP(L_IF_EXIT_57);
		    L_ELSE_58:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1661));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_62); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_61);
		    L_THEN_62:// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));

		    JUMP(L_IF_EXIT_60);
		    L_ELSE_61:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1683));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_65); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_64);
		    L_THEN_65:
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
// BVAR 
MOV(R0, FPARG(0));
MOV(R0, INDD(R0, 0));
MOV(R0, INDD(R0, 2));
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    JUMP(L_IF_EXIT_63);
		    L_ELSE_64:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1143));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_68); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_67);
		    L_THEN_68:MOV(R0,IMM(1798));

		    JUMP(L_IF_EXIT_66);
		    L_ELSE_67:
		    //IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1819));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_71); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_70);
		    L_THEN_71:MOV(R0,IMM(1821));

		    JUMP(L_IF_EXIT_69);
		    L_ELSE_70:
		    
 //TC-APPLIC 

// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1835));
PUSH(R0); // finished evaluating arg 
MOV(R0,IMM(1302));
PUSH(R0); // finished evaluating arg 

 //FVAR 
MOV(R0, IMM(1252));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
ADD(R1, IMM(2));
MOV(FPARG(R1), LOCAL(IMM(0)));
MOV(R1,R4); // start copying arg1
ADD(R1, IMM(1));
MOV(FPARG(R1), LOCAL(IMM(1)));
MOV(R1,R4); // start copying arg2
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

		    L_IF_EXIT_69:
		    
		    L_IF_EXIT_66:
		    
		    L_IF_EXIT_63:
		    
		    L_IF_EXIT_60:
		    
		    L_IF_EXIT_57:
		    
		    L_IF_EXIT_54:
		    
		    L_IF_EXIT_51:
		    
		    L_IF_EXIT_48:
		    
		    L_IF_EXIT_45:
		    
		    L_IF_EXIT_42:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_40:

POP(FP);
RETURN;
L_CLOS_EXIT_33:
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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_14);
L_CLOS_CODE_15: 
PUSH(FP);
MOV(FP, SP);

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
MOV(INDD(R1, 2), LABEL(L_CLOS_CODE_22));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(2));

L_FOR1_START_20:
CMP(R5,R6);
JUMP_EQ(L_FOR1_END_19);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(L_FOR1_START_20);

L_FOR1_END_19:
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

L_FOR2_START_18:
CMP(R5,R6);
JUMP_EQ(L_FOR2_END_17);
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(L_FOR2_START_18);

L_FOR2_END_17:
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(L_CLOS_EXIT_21);
L_CLOS_CODE_22: 
PUSH(FP);
MOV(FP, SP);
//IF 

 //APPLIC 

PUSH(IMM(0));
// PVAR 
MOV(R1, IMM(0));
ADD(R1, IMM(3));
MOV(R0, FPARG(R1));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1143));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
		    JUMP_NE(L_THEN_25); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(L_ELSE_24);
		    L_THEN_25:MOV(R0,IMM(1760));

		    JUMP(L_IF_EXIT_23);
		    L_ELSE_24:
		    
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
//TC-APPLIC CONTINUE 
MOV(R2,FPARG(-2)); // save old FP 
MOV(R3,FPARG(-1)); // save old RET 
MOV(R4,FPARG(1)); // save actual number of current args 
MOV(R1,R4); // start copying arg0
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

		    L_IF_EXIT_23:
		    
POP(FP);
RETURN;
L_CLOS_EXIT_21:

POP(FP);
RETURN;
L_CLOS_EXIT_14:
PUSH(R0);

 //FVAR 
MOV(R0, IMM(132));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(5)); // pushing number of arguments 
PUSH(IMM(5)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
MOV(R1, INDD(IMM(1327), IMM(1)));
MOV(INDD(R1,IMM(1)), R0);
MOV(R0, IMM(IMM(10)));
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_190);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_190:


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1988));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1550));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_7);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_7:


 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1468));
PUSH(R0);
MOV(R0,IMM(1988));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1550));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(2)); // pushing number of arguments 
PUSH(IMM(2)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_5);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_5:


 //APPLIC 

PUSH(IMM(0));

 //APPLIC 

PUSH(IMM(0));
MOV(R0,IMM(1988));
PUSH(R0);

 //FVAR 
MOV(R0, IMM(1550));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
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
MOV(R0, IMM(1504));
MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address 
MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address 
//APPLIC CONTINUE
PUSH(IMM(1)); // pushing number of arguments 
PUSH(IMM(1)); // pushing again for reference for frame removal 
PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure 
CALLA(INDD(R0,IMM(2))); // calling the procedure body 
DROP(IMM(1)); // dropping the environment 
POP(R1); // get number of actual args 
DROP(R1); // drop args 
DROP(IMM(2)); // drop (possibly false) args count and magic number 
	CMP(IND(R0),T_VOID);
	JUMP_EQ(L_JUMP_VOID_3);
	PUSH(R0);
        CALL(WRITE_SOB);
        DROP(IMM(1));
        OUT(IMM(2), IMM('\n'));
        L_JUMP_VOID_3:

L_END_OF_PROGRAM:
STOP_MACHINE;

return 0;
}
