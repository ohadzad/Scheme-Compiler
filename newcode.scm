
(define compile-scheme-file
	(lambda (input-name output-name)
		(let*
			((parsed-exp-list (map parse (tokens->sexprs (file->tokens input-name))))
			(code (code-gen parsed-exp-list))
			(port (open-output-file output-name)))
		(display code port)
		(close-output-port port))))
		
(define generate
	(lambda (lst)
		(string-append
			start-of-file
			define-constants
			(apply string-append (map (lambda (x) (code-gen x '() '())) lst))
			end-of-file
)))



(define void-loc "1")
(define nil-loc "2")
(define true-loc "3")
(define false-loc "5")

(define define-constants
"MOV(ADDR(1), T_VOID)
MOV(ADDR(2), T_NIL)
MOV(ADDR(3), T_BOOL)
MOV(ADDR(4), IMM(1))
MOV(ADDR(5), T_BOOL)
MOV(ADDR(6), IMM(0))
MOV(ADDR(0), IMM(7)
")

(define code-gen 
	(lambda (pe params env)
		(cond
			((pe-const? pe) (code-gen-const pe)))))
			
(define pe-const?
	(lambda (pe)
		(tagged? pe 'const)))
		
(define mr0 
	(lambda (val)
		(sa "MOV(R0," val ")\n")))
		
(define sa string-append)

(define code-gen-const
	(lambda (pe)
		(let ((val (cadr pe)))
		(cond
			((eq? val '()) (mr0 nil-loc))
			((eq? val #f) (mr0 false-loc))
			((eq? val #t) (mr0 true-loc))))))
			
	
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

 CONTINUE:
 ")

(define end-of-file
	"STOP_MACHINE;

  return 0;
  }
")


(compile-scheme-file "main.scm" "main.c")