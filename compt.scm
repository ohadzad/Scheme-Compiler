(load "compiler.scm")

(define sa string-append)

(define void-loc "IMM(10)")

(define nil-loc "IMM(11)")

(define true-loc "IMM(14)")

(define false-loc "IMM(12)")

(define address-of-symbol_table-pointer "IMM(1)")

(define *initial-constant-table*
`((10 ,(if #f #t) (937610))
  (11 () (722689))
  (12 ,#f (741553 0))
  (14 ,#t (741553 1)))  )


(define constants_table *initial-constant-table*)

(define builtin_Scheme_procedures '(apply <  =  >  +  / *  - boolean? car cdr char->integer char? cons eq? integer? integer->char make-string 
make-vector null? number? pair? procedure? remainder set-car! set-cdr! string-length string-ref string-set! 
string->symbol string? symbol? symbol->string vector-length vector-ref vector-set! vector? zero?))


(define compile-scheme-file
        (lambda (input-name output-name)
                (let*
                        ((parsed-exp-list (map annotate-tc (map pe->lex-pe (map parse (tokens->sexprs (file->tokens input-name))))))
                        (code (generate parsed-exp-list))
                        (port (open-output-file output-name)))
                (display "\n-----constants_table-----\n")
                (display constants_table)
                (display "\narray:\n")
                (display (list->c-array constants_table))
                (display "\n-------\n")
                (display code port)
                (close-output-port port))))

(define make-list
        (sa "//makes a list of all current arguments starting from argument specified by R1
//example: if you want a list of all the arguments, mov 0 to R1 and call MAKE_LIST
//example: if you want a list of all the arguments starting from the second, mov 1 to R1 and call MAKE_LIST
MAKE_LIST: 
CMP(FPARG(2), R1);
JUMP_NE(MAKE_LIST_CONT);
MOV(R0," nil-loc ");
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
RETURN;"
))      

(define print-stack
        "void print_stack(char* comment){
        int i;
        printf(\"printing stack, FP: %d SP: %d %s\\n\", (int)(FP), (int)(SP), comment);
        for(i=SP+5; i>=0; --i){
        if(SP == i){
                printf(\"SP \");
        }
        if(FP == i){
                printf(\"FP\");
        }
        printf(\"\telement %d: \", i);
        SHOW(\" \", STACK(i));
        }
}")

(define define-support-methods
        (sa make-list print-stack))

(define generate
        (lambda (lst) 
            (begin
              (set! constants_table (add-const (append (find-define lst) (find-consts lst)) constants_table))
                (sa
                        "\n // starting code \n"
                        start-of-file 
                        "\n // definition of constants \n"
                        (generate_constants_table constants_table)
                        "\n //generating symbol table\n"
                        (generate_symbol_table)
                        "JUMP(L_END_DEFS);\n"
                        "\n // methods for error handling \n"
                        define-errors 
                        "\n // support methods \n"
                        define-support-methods 
                        "L_END_DEFS:\n"
                        "\n // fixing the stack before executing the actual code\n"
                        fix-stack
                        "\n // actual code\n"
                        (apply 
				sa 
				(map 
					(lambda (x) 
						(if	(pe_define? x)
							(code-gen x '() '(()) )
							(sa (code-gen x '() '(()) ) print-sob-code))) 
					lst))
                        end-of-file
))))

(define fix-stack
        (sa
                "PUSH(IMM(0)); //magic number \n"
                "PUSH(IMM(0)); //0 arguments \n"
                "PUSH(IMM(0)); //0 arguments indicator \n"
                "PUSH(IMM(1)); //empty environment \n"
                "CALL(MALLOC);\n"
                "DROP(IMM(1));\n"
                "PUSH(R0);\n"
                "PUSH(IMM(9999)); // dummy return address \n"
                "PUSH(IMM(9999)); //just like the beginning of a function body \n"
                "MOV(FP,SP);\n"
                ;"print_stack(\"THIS IS THE STACK AT THE BEGINNING\");\n"
                ))


(define define-errors
        (sa
                "ERROR_NOT_A_CLOSURE:\n"
                "printf(\"Not a closure.\\n\");\n"
                "JUMP(L_END_OF_PROGRAM);\n"
                "ERROR_WRONG_ARGUMENT_NUMBER:\n"
                "printf(\"Wrong number of parameters.\");\n"
                "JUMP(L_END_OF_PROGRAM);\n"
                "ERROR_UNDEFINED:\n"
                "printf(\"Variable undefined.\");\n"
                "JUMP(L_END_OF_PROGRAM);\n"
                ))





(define code-gen 
        (lambda (pe params env )
                (display "\n")(display pe)(display "\n")
                (cond
                        ((pe-const? pe) (code-gen-const pe))
                        ((pe-seq? pe) (code-gen-seq pe params env ))
                        ((pe-if3? pe)   (code-gen-if3 pe params env))
                        ((pe-or? pe) (code-gen-or pe params env))
                        ((pe-applic? pe) (code-gen-applic pe params env))
                        ((pe-pvar? pe) (code-gen-pvar pe))
                        ((pe-bvar? pe) (code-gen-bvar pe))
                        ((pe-lambda-simple? pe) (code-gen-lambda-simple pe params env))
                        ((pe-lambda-optional? pe) (code-gen-lambda-optional pe params env))
                        ((pe-lambda-variadic? pe) (code-gen-lambda-variadic pe params env))
                        ((pe-tc-applic? pe) (code-gen-tc-applic pe params env))
                        ((pe-fvar? pe) (code-gen-fvar pe))
		;	((pe-fvar? pe) "")
			((pe_define? pe)(code_gen_define pe params env))
                       ; ((pe_define? pe) "")
                        (else (display "Error!!!!!!!!!! expression")(display pe)(display "wasn't caught in the big cond."))
)))

(define pe_define? 
	(lambda (pe) (tagged? pe 'define)))
	
(define code_gen_define
	(lambda(pe params env)
		(let ((sym (cadadr pe))(val (caddr pe)))
			(sa
				(code-gen val params env)
				"MOV(R1, INDD(IMM(" (number->string (get_const_address sym))"), IMM(1)));\n"
				"MOV(INDD(R1,IMM(1)), R0);\n"
				"MOV(R0, IMM(" void-loc "));\n"
			)
		)
	))


(define pe-fvar?
	(lambda (pe) (tagged? pe 'fvar)))

(define code-gen-fvar
	(lambda (pe)
		(let ((sym (cadr pe)))
			(sa	
				"\n //FVAR \n"
				"MOV(R0, IMM(" (number->string (get_const_address sym))"));\n"
				"MOV(R0, INDD(R0,IMM(1))); // R0 holds the bucket address \n"
				"MOV(R0, INDD(R0,IMM(1))); // R0 holds the value address \n"
				"CMP(R0, IMM(0));\n"
				"JUMP_EQ(ERROR_UNDEFINED);\n"
			))))
	
(define pe-tc-applic?
        (lambda (pe) (tagged? pe 'tc-applic)))
        
(define code-gen-tc-applic
        (lambda (pe params env)
                (letrec (
                        (jump-label (label-maker "L_JUMP"))
                        (param-num-num (length (caddr pe)))
                        (param-num (number->string (length (caddr pe))))
                        (copyargs 
                                (lambda (n i)
                                        (if (= i n)
                                                "\n //done copying args \n"
                                                (let 
                                                ((si (number->string i)))
                                                (sa
                                                        "MOV(R1,FPARG(1)); // start copying arg" si "\n"
                                                        "ADD(R1, IMM(" (number->string (- 2 i)) "));\n"
                                                        "MOV(FPARG(R1), LOCAL(IMM(" si ")));\n"
                                                        (copyargs n (+ i 1)))
                                                        ))))
                        )
                (display "\n")(display "\n")(display jump-label)(display "\n")(display pe)(display "\n")
                (sa
                        "\n //TC-APPLIC \n\n"
                        (apply sa (map (lambda (e) (sa (code-gen e params env) "PUSH(R0); // finished evaluating arg \n")) (reverse (caddr pe))))               ;pushing the arguments in reverse
                        ;"print_stack(\"after pushing args\");\n"
                        (code-gen (cadr pe) params env) ;evaluating  the procedure
                        "CMP(ADDR(R0), IMM(T_CLOSURE));\n"      ;checking for errors
                        "JUMP_EQ(" jump-label ");\n"
                        "printf(\"At " jump-label "\\n\");\n"
                        "fflush(stdout);\n"
                        ;"PUSH(R0);\n"
                        ;"CALL(WRITE_SOB);\n"
                        ;"POP(R0);\n"
                        jump-label ":\n"
                        "MOV(R2,FPARG(-2)); // save old FP \n"
                        "MOV(R3,FPARG(-1)); // save old RET \n"
                        "MOV(R4,FPARG(1)); // save actual number of current args \n"
                        (copyargs param-num-num 0)
                        "MOV(R5,FP);\n"
                        "SUB(R5,R4); //R5 should point to the lowest arg \n"
                        "SUB(R5, IMM(" (number->string (- 5 param-num-num)) ")); // R5 should hold the correct place for SP \n"
                        "MOV(SP,R5);\n"
                        ;"print_stack(\"sp should be just above the copied args\");\n"
                        "PUSH(IMM(" param-num ")); //push number of params \n"
                        "PUSH(IMM(" param-num ")); //push indicator \n"
                        "PUSH(INDD(R0,IMM(1))); //push the ENV \n"
                        "PUSH(R3); //push the old RET \n"
                        "MOV(FP,R2); //update FP to the old FP \n"
                        ;"print_stack(\"STACK AFTER TC-APPLIC REPAIR\");\n"
                        "JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place\n"
))))



(define pe-lambda-simple?
        (lambda (pe)
                (tagged? pe 'lambda-simple)))

                
(define pe-lambda-variadic?
        (lambda (pe)
                (tagged? pe 'lambda-variadic)))
;(trace code-gen)
(define code-gen-lambda-variadic
        (lambda (pe params env)
          (let  (
                (body   (code-gen (caddr pe) (cadr pe) (cons params env))) ; fix body
                ;(par   (code-gen (cadr pe) params env))
                (ENV_SIZE (length env))
                (L_CLOS_CODE (label-maker "L_CLOS_CODE"))
                (L_CLOS_EXIT (label-maker "L_CLOS_EXIT"))
                (L_FOR1_START (label-maker "L_FOR1_START"))
                (L_FOR1_END (label-maker "L_FOR1_END"))
                (L_FOR2_START (label-maker "L_FOR2_START"))
                (L_FOR2_END (label-maker "L_FOR2_END"))
                )
                (sa 
"
//creating closure
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(" (number->string (+ 1 ENV_SIZE)) "));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(" L_CLOS_CODE "));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(" (number->string ENV_SIZE) "));

" L_FOR1_START ":
CMP(R5,R6);
JUMP_EQ(" L_FOR1_END ");
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(" L_FOR1_START ");

" L_FOR1_END ":
//for (i = 0; i < " (number->string ENV_SIZE) "; i++){          //here we copy the old environments step by step
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

" L_FOR2_START ":
CMP(R5,R6);
JUMP_EQ(" L_FOR2_END ");
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(" L_FOR2_START ");

" L_FOR2_END ":
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(" L_CLOS_EXIT ");
" L_CLOS_CODE ": 

PUSH(FP);
MOV(FP, SP);

MOV(R1, IMM(0));
CALL(MAKE_LIST);
MOV(FPARG(IMM(3)), R0);
MOV(FPARG(2), IMM(1));

//body of the lambda goes here
" body "
POP(FP);
RETURN;
" L_CLOS_EXIT ":\n"

                ))))            
                
(define pe-lambda-optional?
        (lambda (pe) (tagged? pe 'lambda-opt)))

        
        
        
(define code-gen-lambda-optional
        (lambda (pe params env)
          (let*
                ((newparams `(,@(cadr pe) ,(caddr pe)))
                (par-len (length newparams))
                (body   (code-gen (cadddr pe) newparams (cons params env)))
                ;       (par    (code-gen (cadr pe) params env))
                (ENV_SIZE (length env))
                (L_CLOS_CODE (label-maker "L_CLOS_CODE"))
                (L_CLOS_EXIT (label-maker "L_CLOS_EXIT"))
                (L_FOR1_START (label-maker "L_FOR1_START"))
                (L_FOR1_END (label-maker "L_FOR1_END"))
                (L_FOR2_START (label-maker "L_FOR2_START"))
                (L_FOR2_END (label-maker "L_FOR2_END"))
                )
                (sa 
"
PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(" (number->string (+ 1 ENV_SIZE)) "));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(" L_CLOS_CODE "));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(" (number->string ENV_SIZE) "));

" L_FOR1_START ":
CMP(R5,R6);
JUMP_EQ(" L_FOR1_END ");
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(" L_FOR1_START ");

" L_FOR1_END ":
//for (i = 0; i < " (number->string ENV_SIZE) "; i++){          //here we copy the old environments step by step
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

" L_FOR2_START ":
CMP(R5,R6);
JUMP_EQ(" L_FOR2_END ");
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+2)
  INCR(R5);
 JUMP(" L_FOR2_START ");

" L_FOR2_END ":
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(" L_CLOS_EXIT ");
" L_CLOS_CODE ": 
PUSH(FP);
MOV(FP, SP);
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(" (number->string (- par-len 1)) "));
JUMP_LT(ERROR_WRONG_ARGUMENT_NUMBER);
MOV(R1,IMM(" (number->string (- par-len 1)) "));
CALL(MAKE_LIST);
MOV(FPARG(IMM("(number->string (+ par-len 2)) ")),R0);
MOV(FPARG(IMM(2)), IMM(" (number->string par-len) "));
" body "
POP(FP);
RETURN;
" L_CLOS_EXIT ":\n"
))))

        
        
        
;(trace code-gen)
(define code-gen-lambda-simple
        (lambda (pe params env)
          (let  (
                (par-len (number->string (length (cadr pe)) ))
                (body   (code-gen (caddr pe) (cadr pe) (cons params env)))
                ;       (par    (code-gen (cadr pe) params env))
                (ENV_SIZE (length env))
                (L_CLOS_CODE (label-maker "L_CLOS_CODE"))
                (L_CLOS_EXIT (label-maker "L_CLOS_EXIT"))
                (L_FOR1_START (label-maker "L_FOR1_START"))
                (L_FOR1_END (label-maker "L_FOR1_END"))
                (L_FOR2_START (label-maker "L_FOR2_START"))
                (L_FOR2_END (label-maker "L_FOR2_END"))
                )
                (sa 
"
//LAMBDA-SIMPLE

PUSH(IMM(3));
CALL(MALLOC);
DROP(IMM(1));
MOV(ADDR(R0), IMM(T_CLOSURE));
MOV(R1, R0);  //R1 has the Closure return address
PUSH(IMM(" (number->string (+ 1 ENV_SIZE)) "));
CALL(MALLOC);
DROP(IMM(1));
MOV(INDD(R1, 1), R0);
MOV(INDD(R1, 2), LABEL(" L_CLOS_CODE "));       //put the closure_code_start at cell number 3 (2) of the new SOB
//begining of the for loop
MOV(R5, IMM(0));
MOV(R6, IMM(" (number->string ENV_SIZE) "));

" L_FOR1_START ":
CMP(R5,R6);
JUMP_EQ(" L_FOR1_END ");
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(1));
  MOV(INDD(R0, R7),INDD(FPARG(0), R5));
  INCR(R5);
 JUMP(" L_FOR1_START ");

" L_FOR1_END ":
//for (i = 0; i < " (number->string ENV_SIZE) "; i++){          //here we copy the old environments step by step
//    MOV(INDD(R0, IMM(i+1)),INDD(FPARG(0), IMM(i)));
//}
//end of for loop
MOV(R2, R0)     //R2 has the location of the new Env
PUSH(FPARG(IMM(2)));
CALL(MALLOC);         //create new environment using malloc
DROP(IMM(1));
MOV(ADDR(R2), R0);
//begining of the 2nd for loop
MOV(R5, IMM(0));
MOV(R6, FPARG(IMM(2)));

" L_FOR2_START ":
CMP(R5,R6);
JUMP_EQ(" L_FOR2_END ");
//for statement body
  MOV(R7, R5);
  ADD(R7, IMM(3));
   MOV(INDD(R0, R5), FPARG(R7));                //R0[i] <- FPARG(i+3)
  INCR(R5);
 JUMP(" L_FOR2_START ");

" L_FOR2_END ":
//for (i = 0 ; i < FPARG(IMM(1)); i++){         //now we should copy the new params from the stack
//    MOV(INDD(R0, IMM(i)), FPARG(IMM(i+2)));           //R0[i] <- FPARG(i+2)
//}
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(" L_CLOS_EXIT ");
" L_CLOS_CODE ": 
PUSH(FP);
MOV(FP, SP);
//print_stack(\"THIS IS THE STACK AFTER GOING INTO FUNCTION BODY AND PUSHING FP\");
//checking number of arguments match the length of the lambda
CMP(IMM(FPARG(1)), IMM(" par-len "));
JUMP_NE(ERROR_WRONG_ARGUMENT_NUMBER);
" body "
POP(FP);
RETURN;
" L_CLOS_EXIT ":\n"

                ))))
                
(define pe-pvar?
        (lambda (e) (tagged? e 'pvar)))
        
(define code-gen-pvar
        (lambda (pe)
                (let ((location (number->string (caddr pe))))
                        (sa
                                "MOV(R1, IMM(" location "));\n"
                                "ADD(R1, IMM(3));\n" 
                                "MOV(R0, FPARG(R1));\n"))))
                
                        
(define pe-bvar?
        (lambda (e) (tagged? e 'bvar)))
        
(define code-gen-bvar
        (lambda (pe)
                (let ((env-pos (number->string (caddr pe)))(param-pos (number->string (cadddr pe))))
                        (sa
                                "MOV(R0, INDD(INDD(FPARG(IMM(0)),IMM(" env-pos ")),IMM(" param-pos ")));\n" ))))
(define pe-applic?
        (lambda (pe)
                (tagged? pe 'applic)))
                
                
(define code-gen-applic
        (lambda (pe params env)
                (let ((jump-label (label-maker "L_JUMP"))( param-num (number->string (length (caddr pe)))))
                (display "\n")(display "\n")(display jump-label)(display "\n")(display pe)(display "\n")
                (sa
                        "\n //APPLIC \n\n"
                        "PUSH(IMM(0));\n"       ;pushing the magic number
                        (apply sa (map (lambda (e) (sa (code-gen e params env) "PUSH(R0);\n")) (reverse (caddr pe))))           ;pushing the arguments in reverse
                        (code-gen (cadr pe) params env) ;evaluating  the procedure
                        "CMP(ADDR(R0), IMM(T_CLOSURE));\n"      ;checking for errors
                        "JUMP_EQ(" jump-label ");\n"
                        "printf(\"At " jump-label "\\n\");\n"
                        "fflush(stdout);\n"
                        ;"PUSH(R0);\n"
                        ;"CALL(WRITE_SOB);\n"
                        ;"POP(R0);\n"
                        jump-label ":\n"
                        "PUSH(IMM(" param-num ")); // pushing number of arguments \n"   ;
                        "PUSH(IMM(" param-num ")); // pushing again for reference for frame removal \n"
                        "PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure \n" ;
                        ;"PUSH(R0);\n"
                        ;"CALL(WRITE_SOB);\n"
                        ;"OUT(IMM(2), IMM('\\n'));\n"
                        ;"POP(R0);\n"
                        ;"print_stack(\"STACK IN APPLIC JUST BEFORE FUNCTION CALL\");\n"
                        "CALLA(INDD(R0,IMM(2))); // calling the procedure body \n"              ;
                        "DROP(IMM(1)); // dropping the environment \n"  ;
                        "POP(R1); // get number of actual args \n"              
                        "DROP(R1); // drop args \n"
                        "DROP(IMM(2)) // drop (possibly false) args count and magic number \n"
))))

(define pe-or?
        (lambda (x)
                (tagged? x 'or)))
                
(define code-gen-or
        (lambda (pe params env)
                (let* ((end-label (label-maker "L_OR_END"))
                        (code-to-append (sa 
                                "CMP(ADDR(R0), IMM(T_BOOL));\n"
                                "JUMP_NE(" end-label ");\n"
                                "CMP(INDD(R0,1), IMM(0));\n"
                                "JUMP_NE(" end-label ");\n")))
                        (sa 
                                (apply sa (map
                                        (lambda (pe)
                                                (sa (code-gen pe params env) code-to-append))
                                        (cadr pe)))
                                end-label ":\n"))))
                        
                        
(define pe-seq?
        (lambda (pe)
                (tagged? pe 'seq)))

(define code-gen-seq
        (lambda (pe params env)
                (apply sa (map (lambda (x) (code-gen x params env)) (cadr pe)))))
                        
(define code-gen-if3
      (lambda (pe params env)
          (let ((test   (code-gen (cadr pe) params env))
                (dit    (code-gen (caddr pe) params env))
                (dif    (code-gen (cadddr pe) params env))
                (L_THEN (label-maker "L_THEN"))
                (L_ELSE (label-maker "L_ELSE"))
                (L_IF_EXIT (label-maker "L_IF_EXIT"))
                )
                
                (sa test "CMP(ADDR(R0), T_BOOL);\n JUMP_NE(" L_THEN "); \n CMP(INDD(R0, 1), IMM(0));\n JUMP_EQ(" L_ELSE ");\n"
                    L_THEN ":\n" dit L_ELSE ":\n" dif L_IF_EXIT ":\n"))))
(define pe-if3?
        (lambda (pe)
                (tagged? pe 'if-3)))
                
(define pe-const?
        (lambda (pe)
                (tagged? pe 'const)))
                
(define mr0 
        (lambda (val)
                (sa "MOV(R0," val ");\n")))
                


(define start-of-file
        "#include <stdio.h>
#include <stdlib.h>

#include \"cisc.h\"

/* change to 0 for no debug info to be printed: */
#define DO_SHOW 1

/* for debugging only, use SHOW(\"<some message>, <arg> */
#if DO_SHOW
#define SHOW(msg, x) { printf(\"%s %s = %ld\\n\", (msg), (#x), (x)); }
#else
#define SHOW(msg, x) {}
#endif

int main()
{
  START_MACHINE;

  JUMP(CONTINUE);

#include \"char.lib\"
#include \"io.lib\"
#include \"math.lib\"
#include \"string.lib\"
#include \"system.lib\"
#include \"scheme.lib\"

CONTINUE:
 ")

(define end-of-file
        "L_END_OF_PROGRAM:
STOP_MACHINE;

return 0;
}
")

(define print-sob-code
        "PUSH(R0);
        CALL(WRITE_SOB);    
        DROP(IMM(1));
        OUT(IMM(2), IMM('\\n'));\n")

(define testparse
        (lambda (x)
                (parse (car (tokens->sexprs (string->tokens x))))))



(define counter
        (lambda ()
                (let ((x 0))
                        (lambda () 
                                (set! x (+ x 1))
                                x))))
                                
(define label-generator
        (lambda ()
                (let ((c (counter)))
                        (lambda (label)
                                (sa label "_" (number->string (c)))))))

(define label-maker (label-generator))

;***************************************************************************
;*********************      Const handling *********************************
;***************************************************************************


(define starting-point 10)

(define code-gen-const
        (lambda (pe)
                (let* ((val (cadr pe))
                        (mem_add (sa "IMM(" (number->string (get_const_address_from_dict val constants_table)) ")" ))
                        )
                (mr0 mem_add))))
                        
  
(define list->c-array 
      (lambda (consts) 
              (string-append "{" (apply string-append 
                                            (cdr (apply append (map (lambda (v) 
                                                                            (list ", " (number->string v))) 
                                                                      (apply append (map caddr consts)))))) 
                              "}")))
(define topo-sort
  (lambda (e)
          (cond ((pair? e)
                  `(,@(topo-sort (car e))
                    ,@(topo-sort (cdr e))
                    ,e))
                ((vector? e)
                 `(,@(apply append (map topo-sort (vector->list e)))
                   ,e))
                (else `(,e)))))

(define find-consts 
    (lambda (lst) 
      (if (pair? lst)
	  (if (pe-const? lst)
	      `(,(cadr lst))
	      (append (find-consts (car lst)) (find-consts (cdr lst))))
	  '())))
	  
(define find-define 
    (lambda (lst) 
      (if (pair? lst)
	  (if (pe_define? lst)
	      `(,(cadadr lst))
	      (append (find-define (car lst)) (find-define (cdr lst))))
	  '())))	  


(define pe-list->flat-const-list (lambda (pe-list)
                                          (apply append (map topo-sort (find-consts pe-list)))))
                                          
(define get_size_of_smbl
  (lambda (smbl)
           (cond
                        ((boolean? smbl) 2)
                        ((number? smbl) 2)
                        ((pair? smbl) 3)
                        ((symbol? smbl) 2)
                        ((string? smbl) (+ 2 (string-length smbl)))
                        (else 3))))
(define get_modified_space
            (lambda (lst)
                    (let ((free_mem (car lst))
                          (smbl (cadr lst))
                          )
                      (+ (get_size_of_smbl smbl) free_mem))))
(define get_next_free_space 
    (lambda (lst)
         (if  (pair? lst)
              (if (= (length (cdr lst)) 0)
                  (get_modified_space (car lst))
                  (get_next_free_space (cdr lst)))
                starting-point)))
                
                
(define get_const_address_from_dict
  (lambda (smbl lst)
    (let  ((val (if (pair? lst)
                    (if (pair? (car lst))
                        (if (equal? (cadar lst) smbl)
                            (caar lst)
                            #f)
                         #f)
                    #f)))
                  
          (if (= 0 (length (cdr lst)))
              val
              (or val (get_const_address_from_dict smbl (cdr lst)))))))
              
              
(define get_type_id
  (lambda (smbl)
  (cond 
          ((null? smbl) 722689)
          ((boolean? smbl) 741553)
          ((char? smbl) 181048)
          ((number? smbl) 945311)
          ((string? smbl) 799345)
          ((symbol? smbl) 368031)
          ((pair? smbl) 885397)
          ((vector? smbl) 335728)
          ((procedure? smbl) 276405)
          (else 937610)
          )))
          
(define get_string_val
  (lambda (smbl) 
    (let* ( (split_list  (string->list smbl))
    )
    (map char->integer split_list ))))
  
  
(define add-weight 
  (lambda (topo_lst free_mem new_list)
  (let* ( (smbl (car topo_lst))
          (typee (get_type_id smbl))
          (next_free (+ (get_size_of_smbl smbl) free_mem))
          (var_exists (get_const_address_from_dict smbl new_list))
          (endings (if var_exists `()   
                        (cond ((pair? smbl) `(,typee ,(get_const_address_from_dict (car smbl) new_list) ,(get_const_address_from_dict (cdr smbl) new_list)))
                              ((string? smbl)  `(,typee ,(string-length smbl) ,@(get_string_val smbl)))
                              ((number? smbl)  `(,typee ,smbl))
                              ((symbol? smbl)  `(,typee 0))
                              (else `(,typee)))))
          (new_var (if var_exists `()   `((,free_mem ,smbl ,endings))))
                        )
          (if (= 0 (length (cdr topo_lst)))
              (append new_list new_var )
              (add-weight (cdr topo_lst) (if var_exists free_mem next_free) (append new_list new_var))))))
          
(define add_string_per_symbol
  (lambda (lst)
    (let ((cur_var_lst (if (symbol? (car lst))
                          (list (symbol->string (car lst)) (car lst))
                          (list (car lst)))))
          (if (= 0 (length (cdr lst)))
              cur_var_lst
              (append cur_var_lst (add_string_per_symbol (cdr lst)))
              )
              )))

(define add-const 
  (lambda (cst lst)
    (let* ((topo_srtd (add_string_per_symbol (topo-sort cst))))
          (add-weight topo_srtd (get_next_free_space lst) lst))))

(define generate_constants_table
  (lambda(sym_tbl)
  (let ((arr_size (- (get_next_free_space sym_tbl) 10))
        (arr (list->c-array sym_tbl))
  
  )
  (sa "
PUSH(R0);
POP(R0);
int arr[" (number->string arr_size ) "] = " arr ";
void *a = &arr;
memcpy(&((*machine).mem[10]), a, " (number->string arr_size ) "*4);
MOV(ADDR(0), IMM(" (number->string (get_next_free_space constants_table)) "));
"))))

;******************************************************************************
;******************* End of Const Handeling ***********************************
;******************************************************************************

(define get-symbols
	(lambda () 
		(get_symbols_from_dict constants_table)
		
		))


(define get_symbols_from_dict 
	(lambda (dict)
		(cond
			((null? dict) dict)
			((symbol? (cadar dict)) `(,(cadar dict) ,@(get_symbols_from_dict (cdr dict))
			))
			(else (get_symbols_from_dict (cdr dict)))
		)
))

(define symbol_table
        (lambda (symbols)
		(display symbols)
                (if     (null? symbols)
                        "MOV(ADDR(R1), IMM(0));\n"
                        (sa
                                "//creating bucket for " (symbol->string (car symbols)) "\n"
                                "PUSH(IMM(3)); // allocate space for new bucket \n"
                                "CALL(MALLOC);\n"
                                "DROP(IMM(1));\n"
                                "MOV(INDD(R0, IMM(0)), IMM(" (number->string (get_const_address (symbol->string (car symbols)))) ")); // add pointer to the string \n"
                                "MOV(INDD(R0, IMM(1)), IMM(0)); // this symbol holds no value \n"
                                "MOV(ADDR(" (number->string (+ 1 (get_const_address (car symbols)))) "), R0); // save the bucket address in the symbol object \n"
                                "MOV(ADDR(R1), R0); // make previous bucket point to this one \n"
                                "ADD(R0,IMM(2)); // store bucket address in R1 for future linkage \n"
                                "MOV(R1,R0);\n"
                                (symbol_table (cdr symbols))
                                )
)))

(define get_const_address
	(lambda (x) (get_const_address_from_dict x constants_table)))

(define generate_symbol_table 
        (lambda () 
		(sa 
			"MOV(R1, " address-of-symbol_table-pointer ");\n"
			(symbol_table (get-symbols)))
			))

(define compare-strings
"COMPARE_STRINGS:
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
")

(define get-last-bucket
(sa
"GET_LAST_BUCKET:
PUSH(FP);
MOV(FP,SP);
MOV(R0, ADDR(" address-of-symbol_table-pointer "));

GET_LAST_BUCKET_LOOP:
CMP(INDD(R0, IMM(2)), IMM(0));
JUMP_EQ(GET_LAST_BUCKET_END);
MOV(R0,INDD(R0,IMM(2)));
JUMP(GET_LAST_BUCKET_LOOP);

GET_LAST_BUCKET_END:
POP(FP);
RETURN;"))

(define test
        (lambda (s)
                (annotate-tc (pe->lex-pe (testparse s)))))