;;; compiler.scm
;;;
;;; Programmers: Ohad Zadok, Tsah Weiss, Hadar Oz

;;; general support routines

(define with (lambda (s f) (apply f s)))

(define member?
  (lambda (a s)
    (ormap
     (lambda (b) (eq? a b))
     s)))

(define file->list
  (lambda (filename)
    (let ((port (open-input-file filename)))
      (letrec ((loop
		(lambda ()
		  (let ((ch (read-char port)))
		    (if (eof-object? ch) '()
			(cons ch (loop)))))))
	(let ((s (loop)))
	  (close-input-port port)
	  s)))))

(define make-char-between?
  (lambda (char<=?)
    (lambda (char-from char-to)
      (lambda (char)
	(and (char<=? char-from char)
	     (char<=? char char-to))))))

;;; The scanner recognizes parenthesis and single quote.
;;; It knows to ignore comments up to the end of the current input line,
;;; as well as whitespaces.

(define list->tokens
  (letrec ((st-init
	    (lambda (s)
	      (cond
	       ((null? s) '())
	       ((char=? (car s) #\;) (st-comment s))
	       ((char=? (car s) #\.) `((dot) ,@(st-init (cdr s))))
	       ((char=? (car s) #\') `((single-quote) ,@(st-init (cdr s))))
	       ((char=? (car s) #\`) `((quasiquote) ,@(st-init (cdr s))))
	       ((char=? (car s) #\,) (st-unquote (cdr s)))
	       ((char=? (car s) #\() `((lparen) ,@(st-init (cdr s))))
	       ((char=? (car s) #\)) `((rparen) ,@(st-init (cdr s))))
	       ((char=? (car s) #\#) (st-hash (cdr s)))
	       ((char=? (car s) #\") (st-string (cdr s) '()))
	       ((char-whitespace? (car s)) (st-init (cdr s)))
	       ((char-symbol? (car s))
		(st-symbol/number (cdr s) (list (car s))))
	       (else (scanner-error "What's this" s)))))
	   (st-unquote
	    (lambda (s)
	      (cond ((null? s) `((,'unquote) ,@(st-init '())))
		    ((char=? (car s) #\@)
		     `((,'unquote-splicing) ,@(st-init (cdr s))))
		    (else `((,'unquote) ,@(st-init s))))))
	   (st-symbol/number
	    (lambda (s chars)
	      (cond ((null? s)
		     `(,(make-symbol/number-token chars) ,@(st-init '())))
		    ((char-symbol? (car s))
		     (st-symbol/number (cdr s) (cons (car s) chars)))
		    ((char-delimiter? (car s))
		     `(,(make-symbol/number-token chars) ,@(st-init s)))
		    (else (scanner-error "At the end of a symbol: " s)))))
	   (st-string
	    (lambda (s chars)
	      (cond ((null? s)
		     (scanner-error "Expecting a \" char to close the string"))
		    ((char=? (car s) #\")
		     `((string ,(list->string (reverse chars)))
		       ,@(st-init (cdr s))))
		    ((char=? (car s) #\\) (st-meta-char (cdr s) chars))
		    (else (st-string (cdr s) (cons (car s) chars))))))
	   (st-meta-char
	    (lambda (s chars)
	      (cond ((null? s)
		     (scanner-error
		      "Expecting a string meta-char; reached EOF"))
		    ((char=? (car s) #\\) (st-string (cdr s) (cons #\\ chars)))
		    ((char=? (car s) #\") (st-string (cdr s) (cons #\" chars)))
		    ((char-ci=? (car s) #\n)
		     (st-string (cdr s) (cons #\newline chars)))
		    ((char-ci=? (car s) #\r)
		     (st-string (cdr s) (cons #\return chars)))
		    ((char-ci=? (car s) #\t)
		     (st-string (cdr s) (cons #\tab chars)))
		    ((char-ci=? (car s) #\f)
		     (st-string (cdr s) (cons #\page chars)))
		    (else (scanner-error "What kind of a meta-char is " s)))))
	   (st-hash
	    (lambda (s)
	      (cond ((null? s)
		     (scanner-error
		      "Expecting something after #, but reached end"))
		    ((char=? (car s) #\() `((vector) ,@(st-init (cdr s))))
		    ((char=? (car s) #\\) (st-char-1 (cdr s)))
		    ((char-ci=? (car s) #\f)
		     `((boolean #f) ,@(st-init (cdr s))))
		    ((char-ci=? (car s) #\t)
		     `((boolean #t) ,@(st-init (cdr s))))
		    ((char=? (car s) #\;) `((comment) ,@(st-init (cdr s))))
		    (else (scanner-error
			   "Expecting t, f, \\, ( after #, but found" s)))))
	   (st-char-1
	    (lambda (s)
	      (cond ((null? s) (error 'scanner "Must be one char after #\\"))
		    (else (st-char (cdr s) (list (car s)))))))
	   (st-char
	    (lambda (s chars)
	      (cond ((null? s) `((char ,(make-char chars)) ,@(st-init '())))
		    ((char-delimiter? (car s))
		     `((char ,(make-char chars)) ,@(st-init s)))
		    (else (st-char (cdr s) (cons (car s) chars))))))
	   (st-comment
	    (lambda (s)
	      (cond ((null? s) (st-init '()))
		    ((char=? (car s) #\newline) (st-init (cdr s)))
		    (else (st-comment (cdr s)))))))
    (lambda (s)
      (st-init s))))

(define make-symbol/number-token
  (lambda (chars)
    (let* ((string (list->string (reverse chars)))
	   (maybe-number (string->number string)))
      (if (number? maybe-number)
	  `(number ,maybe-number)
	  `(symbol ,(string->symbol (string-downcase string)))))))

(define make-char
  (lambda (chars)
    (cond ((null? chars) (scanner-error "Found #\\ without any char"))
	  ((null? (cdr chars)) (car chars))
	  (else (let* ((string (list->string (reverse chars)))
		       (maybe-number (string->number string 8)))
		  (if (number? maybe-number)
		      (integer->char maybe-number)
		      (cond ((string-ci=? string "return") #\return)
			    ((string-ci=? string "newline") #\newline)
			    ((string-ci=? string "space") #\space)
			    ((string-ci=? string "tab") #\tab)
			    ((string-ci=? string "page") #\page)
			    (else (scanner-error
				   "Can't recognize the following character: "
				   (format "#\\~s" string))))))))))

(define char-alphabetic? ((make-char-between? char-ci<=?) #\a #\z))
(define char-decimal? ((make-char-between? char<=?) #\0 #\9))

(define char-symbol?
  (let ((punc-chars (string->list "!@$%^*-_=+<>./?:")))
    (lambda (char)
      (or (char-alphabetic? char)
	  (char-decimal? char)
	  (ormap 
	   (lambda (punc-char) (char=? punc-char char))
	   punc-chars)))))

(define char-whitespace?
  (lambda (char)
    (char<=? char #\space)))

(define char-delimiter?
  (lambda (char)
    (or (char-whitespace? char)
	(not (char-symbol? char)))))

(define scanner-error
  (lambda (message s)
    (if (null? s)
	(error 'list-tokens message)
	(error 'list-tokens
	       (format "~a: [~s]~a"
		       message
		       (car s)
		       (list->string (cdr s)))))))

(define file->tokens
  (lambda (filename)
    (list->tokens
     (file->list filename))))

(define string->tokens
  (lambda (string)
    (list->tokens
     (string->list string))))
     
;gets as many sexpressions as possible from a token list
(define get-sexprs
	(lambda (toks ret-exps)
        (get-sexpr toks
			(lambda (sexpr toks)
                (get-sexprs 	
					toks
					(lambda (sexprs toks) (ret-exps (cons sexpr sexprs) toks))))
			(lambda () (ret-exps '() toks)))
))
				  
;gets exactly one sexpression from a token list
(define get-sexpr
    (lambda (token ret-sexpr+toks ret-fail)
        (cond 
			((null? token) (ret-fail))
            ((or 	
					(eq? (caar token) 'boolean)
					(eq? (caar token) 'string)
					(eq? (caar token) 'char)
					(eq? (caar token) 'number)
					(eq? (caar token) 'symbol)
				)
				(ret-sexpr+toks (cadar token) (cdr token)))
            ((eq? (caar token) 'vector)
				(get-sexprs 
					(cdr token)
                    (lambda (sexprs toks)
                            (cond
								((null? toks) (ret-fail))
								((eq? (caar toks) 'rparen) 
									(ret-sexpr+toks
									(list->vector 
										sexprs)
										(cdr toks)))
								(else (ret-fail))))
							))
			((eq? (caar token) 'lparen)
				(get-sexprs 
					(cdr token)
					(lambda (sexprs toks)
						(cond
							((null? toks) (ret-fail))
							((eq? (caar toks) 'rparen)
								(ret-sexpr+toks sexprs (cdr toks)))
							((eq? (caar toks) 'dot )
								(get-sexpr 
									(cdr toks)
									(lambda (sexpr toks)
										(cond
											((null? toks) (ret-fail))
											((eq? (caar toks) 'rparen ) (ret-sexpr+toks `(,@sexprs .,sexpr) (cdr toks)))
											(else (ret-fail))))
									ret-fail))
							(else (ret-fail))))))
			((eq? (caar token) 'single-quote)
				(get-sexpr (cdr token) (lambda (exp tokens1) (ret-sexpr+toks (list 'quote exp) tokens1)) ret-fail))
			((eq? (caar token) 'quasiquote)
				(get-sexpr (cdr token) (lambda (exp tokens1) (ret-sexpr+toks (list 'quasiquote exp) tokens1)) ret-fail))
			((eq? (caar token) 'unquote)
				(get-sexpr (cdr token) (lambda (exp tokens1) (ret-sexpr+toks (list 'unquote exp) tokens1)) ret-fail))
			((eq? (caar token) 'unquote-splicing)
				(get-sexpr (cdr token) (lambda (exp tokens1) (ret-sexpr+toks (list 'unquote-splicing exp) tokens1)) ret-fail))
							
			(else (ret-fail))
			)))
			
;converts a list of tokens to a list of sexpressions
(define tokens->sexprs (lambda (tokens)
								(get-sexprs tokens (lambda (e s) e))))
								
								

;parses and tags the given sexpression.								
(define parse
	(lambda (sexp)
		(cond
			((constant? sexp) (parse-constant sexp))
			((variable? sexp) (parse-variable sexp))
			((qq? sexp)(parse (expand-qq (cadr sexp))))
			((if? sexp) (parse-if sexp))
			((lambda? sexp)(parse-lambda sexp))
			((define? sexp)(parse-define sexp))
			((sequence? sexp)(parse-seq sexp))
			((or? sexp)(parse-or sexp))
			((let? sexp)(parse (expand-let sexp)))
			((let*? sexp)(parse (expand-let* sexp)))
			((letrec? sexp) (parse (expand-letrec sexp)))
			((and? sexp)(parse (expand-and sexp)))
			((cond? sexp)(parse (expand-cond sexp)))
			((application? sexp) (parse-application sexp))
			(else 'error)
		)))

;reserved words in scheme
(define *reserved-words*
  '(and begin cond define do else if lambda
	let let* letrec or quasiquote
	quote set! unquote unquote-splicing))
		
;the method will search the *reserved-words* list for the e expression and return true if it exists false otherwise.
(define reserved-symbol?
	(lambda (e) (letrec ((exist? (lambda(lst obj)(if (pair? lst) (if (eq? obj (car lst)) #t (exist? (cdr lst) obj)) #f))))
						(exist? *reserved-words* e))))
;predicates and handlers for different expression types:

(define constant?
	(lambda (e)
		(or 
			(boolean? e)
			(char? e)
			(string? e)
			(vector? e)
			(number? e)
			(and (pair? e) (eq? (car e) 'quote)))))

(define parse-constant
	(lambda (e)
		(if (pair? e)
			`(const ,(cadr e))
			`(const ,e))))
			
(define variable?
	(lambda (e)
		(and (symbol? e) (not (reserved-symbol? e)))))
		
(define parse-variable
	(lambda (e)
		`(var ,e)))
		

(define qq?
	(lambda (e)
		(and (pair? e) (eq? (car e) 'quasiquote))))

(define expand-qq
  (lambda (e)
    (cond ((unquote? e) (cadr e))
	  ((unquote-splicing? e) (error 'expand-qq "unquote-splicing here makes no sense!"))
	  ((pair? e)
	   (let ((a (car e))
		 (b (cdr e)))
	     (cond ((unquote-splicing? a) `(append ,(cadr a) ,(expand-qq b)))
		   ((unquote-splicing? b) `(cons ,(expand-qq a) ,(cadr b)))
		   (else `(cons ,(expand-qq a) ,(expand-qq b))))))
	  ((vector? e) `(list->vector ,(expand-qq (vector->list e))))
	  ((or (null? e) (symbol? e)) `',e)
	  (else e))))

(define ^quote?
  (lambda (tag)
    (lambda (e)
      (and (pair? e)
	   (eq? (car e) tag)
	   (pair? (cdr e))
	   (null? (cddr e))))))

(define unquote? (^quote? 'unquote))
(define unquote-splicing? (^quote? 'unquote-splicing))

(define if?
	(lambda (e)
		(and (pair? e) (eq? (car e) 'if))))

;an object with a #<void> value		
(define void-object (if #f #f))

(define parse-if
	(lambda (e)
		(if (null? (cdddr e))
			`(if-3 ,(parse (cadr e)) ,(parse (caddr e)) (const ,void-object))
			`(if-3 ,(parse (cadr e)) ,(parse (caddr e)) ,(parse (cadddr e))))))

(define define? 
	(lambda (e) (if (pair? e)
					(eq? (car e) 'define)
					#f)))

(define parse-define 
	(lambda (e)
		(if (variable? (cadr e))
			`(define ,(parse (cadr e)) ,(parse (caddr e)))
			(if (pair? (cadr e))
				(if (variable? (caadr e))
					(parse-define `(define ,(caadr e) (lambda ,(cdadr e) ,(caddr e))))
					(error 'parse-define "no such define method"))))))

(define lambda? 
	(lambda (e) (if (pair? e)
					(eq? (car e) 'lambda)
					#f)))
					
(define parse-lambda 
	(lambda (e)
	(let (	(argl (cadr e))
		(body (parse (if (< 1 (length (cddr e))) (beginify (cddr e)) (caddr e)))))
		(lambda-tagger 	argl
						(lambda () `(lambda-simple ,argl ,body))
						(lambda (s a) `(lambda-opt ,s ,a ,body))
						(lambda () `(lambda-variadic ,argl, body))))))

(define lambda-tagger
	(lambda (e ret-pro ret-imp ret-sym)
			(cond 	(	(pair? e)
						(lambda-tagger 	(cdr e)
										ret-pro
										(lambda (s a)
												(ret-imp (cons (car e) s) a))
										(lambda ()
												(ret-imp (list (car e)) (cdr e)))))
					((null? e) (ret-pro))
					(else (ret-sym)))))
					
(define sequence? 
	(lambda (e)
		(and (pair? e) (eq? (car e) 'begin))))

;tags a list with the 'begin tag
(define beginify
	(lambda (es)
			(cond 	((null? es) void-object)
					((null? (cdr  es)) (car es))
					(else `(begin ,@es) ))))
(define parse-seq
	(lambda (e)
		(cond 	
			((null? (cdr e)) `(const ,void-object))
			((null? (cddr e)) (parse (cadr e)))
			(else `(seq ,(map parse (cdr e)))))))
		
(define or?
	(lambda (e)
		(and (pair? e) (eq? (car e) 'or))))

(define parse-or
	(lambda (e)
		(let ((lst (cdr e)) (len (length (cdr e))))
			(cond 
				((= len 0) '(const #f))
				((= len 1) (parse (car lst)))
				(else `(or ,(map parse lst)))))))
(define let?
	(lambda (e)
		(and 
			(list? e)
			(eq? (car e) 'let)
			(let-structure? e 'let)
			)))

(define let-structure?
	(lambda (e s)
		(if 
			(and 
				(list? (cadr e))
				(>= (length (cadr e)) 0)
				(andmap 
					(lambda (x) (and (list? x)(eq? (length x) 2)))
					(cadr e))
				(> (length (cddr e)) 0))
			#t
			(error s "Unsupported let structure"))))
			
(define expand-let
	(lambda (e)
		(let* (
				(varsdef (cadr e)) 
				(varlist (map  car varsdef))
				(valueslist (map cadr varsdef))
				(bodies (cddr e)))
			`((lambda ,varlist ,@bodies) ,@valueslist))))

(define and?
	(lambda (e)
		(and (pair? e) (eq? (car e) 'and))))

(define expand-and
	(lambda (e)
		(cond
			((= 1 (length e)) `#t)
			((= 2 (length e)) `,(cadr e))
			((= 3 (length e)) `(if ,(cadr e) ,(caddr e) #f))
			((< 3 (length e)) (let ((rest (expand-and (cons 'and (cddr e)))))
									`(if ,(cadr e) ,rest  #f)))
			(else 'error)
			)))
			
(define cond?
	(lambda (e)
		(if (and (pair? e) (eq? (car e) 'cond) (< 1 (length e)))
			(pair? (cadr e)) 
			#f
		)))
		
(define expand-cond
	(lambda (e)
		(let (	(do_if (cons 'begin (cdadr e)))
				(cnd_if (caadr e)))
		(if (eq? (caadr e) `else)
			do_if
			(cond (	(< 1 (length (cdr e)))
					(list 'if cnd_if do_if (expand-cond (cons 'cond (cddr e)))))
				(	(= 1 (length (cdr e)))
					(list 'if cnd_if do_if ))
				(else 'error))
				
				))))
			
(define let*?
	(lambda (e)
		(and 
			(list? e)
			(eq? (car e) 'let*)
			(let-structure? e 'let*)
			)))
			
(define expand-let*
	(lambda (e)
		(let ((vardef (cadr e)) (bodies (cddr e)))
			(expand-let*-rec vardef bodies))))
			
(define expand-let*-rec
	(lambda (vardef bodies)
		(if (null? (cdr vardef))
			`(let (,(car vardef)) ,@bodies)
			`(let (,(car vardef)) ,(expand-let*-rec (cdr vardef) bodies)))))
			
(define letrec?
	(lambda (e)
		(and 
			(list? e)
			(eq? (car e) 'letrec)
			(let-structure? e 'letrec)
			)))
			
(define with (lambda (s f) (apply f s)))

(define yn
  (lambda fs
    (let ((ms (map
		  (lambda (fi)
		    (lambda ms
		      (apply fi
			     (map (lambda (mi)
				    (lambda args
				      (apply (apply mi ms) args)))
			       ms))))
		fs)))
      (apply (car ms) ms))))

(define expand-letrec
  (lambda (e)
    (with e
      (lambda (_letrec ribs . exprs)
	(let* ((names `(,(gensym) ,@(map car ribs)))
	       (fs `((lambda ,names ,@exprs)
		     ,@(map (lambda (rib) `(lambda ,names ,(cadr rib)))
			 ribs))))
	  `(yn ,@fs))))))
	  
(define application? 
	(lambda (e)
		(pair? e)))
		
(define parse-application
	(lambda (e)
		(if (pair? (cdr e)) 
			`(applic ,(parse (car e)) ,(map parse (cdr e)))
			`(applic ,(parse (car e)) ()))))
			

;===========ASSIGNMENT 3 STARTS HERE ==========================

;tells if expression e is tagged with the tag tag
(define tagged?
  (lambda (e tag)
    (and (pair? e) (equal? tag (car e)))))

;annotates tail calls in expression e acorrding to tp? which specifies whether we are in tail position
(define run
  (lambda (e tp?)
    (cond 
      ((tagged? e 'const) e)
      ((tagged? e 'var) e)
	  	((tagged? e 'pvar) e)
	  	((tagged? e 'bvar) e)
	  	((tagged? e 'fvar) e)
      ((tagged? e 'if-3) (if-tp e tp?))
      ((tagged? e 'seq) `(seq ,(seq-tp (cadr e) tp?)))
      ((tagged? e 'or) `(or ,(or-tp (cadr e) tp?)))
      ((tagged? e 'define) `(define ,(cadr e) ,(run (caddr e) #f)))
      ((tagged? e 'applic) (applic-tp e tp?))
      ((tagged? e 'lambda-simple)   `(,(car e) ,(cadr e) ,(run (caddr e) #t)))
	  	((tagged? e 'lambda-variadic) `(,(car e) ,(caadr e) ,(run (caddr e) #t)))
      ((tagged? e 'lambda-opt) `(lambda-opt ,(cadr e) ,(caddr e) ,(run (cadddr e) #t)))
      )))
      
      
;handlers for different cases in run:
      
(define if-tp
  (lambda (e tp?)
    (let ((test (cadr e))(dit (caddr e))(dif (cadddr e)))
      `(if-3 ,(run test #f) ,(run dit tp?) ,(run dif tp?)))))
      
      
(define seq-tp
  (lambda (e tp?)
    (if
      (null? (cdr e))
      `(,(run (car e) tp?))
      `(,(run (car e) #f) ,@(seq-tp (cdr e) tp?)))))
      
(define or-tp
  (lambda (e tp?)
    (if
      (null? (cdr e))
      `(,(run (car e) tp?))
      `(,(run (car e) #f) ,@(or-tp (cdr e) tp?)))))
  
(define applic-tp
  (lambda (e tp?)
    `(
	,(if tp? 'tc-applic 'applic) 
	,(run (cadr e) #f)
	,(map
	    (lambda (x)
	      (run x #f))
	    (caddr e)))))


;annotates tail calls in expresion e
(define annotate-tc
  (lambda (e)
    (run e #f)))
	
	
;searches for a var in a list of ribs CPS style
(define search-in-ribs
	(lambda (a env ret-maj+min ret-nf)
		(if (null? env)
			(ret-nf)
			(search-in-rib a (car env)
				(lambda (min) (ret-maj+min 0 min))
				(lambda ()
						(search-in-ribs a (cdr env)
							(lambda (maj min)(ret-maj+min (+ 1 maj) min))
							ret-nf))))))
	
;searches for a var in a rib CPS style
(define search-in-rib
	(lambda (a s ret-min ret-nf)
		(cond ((null? s) (ret-nf))
			((eq? (car s) a) (ret-min 0))
			(else (search-in-rib a (cdr s)
					(lambda (min)(ret-min (+ 1 min)))
					ret-nf)))))
					
;handlers for different cases in runl
					
(define var->lex-pe (lambda (pe params env)
	(with pe
		(lambda (_ v)
			(search-in-rib v params
				(lambda (min) `(pvar ,v ,min))
				(lambda ()
						(search-in-ribs v env
							(lambda (maj min ) `(bvar ,v ,maj ,min))
							(lambda () `(fvar ,v)))))))))
							
(define lambda-simple->lex-pe (lambda (pe params env)
	(with pe
		(lambda (_ argl body)
			`(lambda-simple ,argl ,(runl body argl (cons params env)) 
										)))))

(define lambda-opt->lex-pe (lambda (pe params env)
	(with pe
		(lambda (_ argl opt body)
			`(lambda-opt ,argl ,opt ,(runl body `(,@argl ,opt) (cons params env)) 
										)))))
			
(define lambda-variadic->lex-pe (lambda (pe params env)
	(with pe
		(lambda (_ argl body)(let ((argl `(,argl)))
			`(lambda-variadic ,argl ,(runl body argl (cons params env)) 
										))))))
(define if->lex-pe (lambda (pe params env)
	(with pe
		(lambda (_ con if-t if-f )
			`(if-3 ,(runl con params env) ,(runl if-t params env) ,(runl if-f params env) 
										)))))
									
(define sym->lex-pe (lambda (sym pe params env)
	(with pe
		(lambda (_ seq-list)(let (	(runl-wp (lambda (expr) (runl expr params env)))
										)
			`(,sym ,(map runl-wp seq-list) 
										))))))
										
(define define->lex-pe (lambda (pe params env)
	(with pe
		(lambda (_ . seq-list)(let (	(runl-wp (lambda (expr) (runl expr params env)))
										)
			`(define ,@(map runl-wp seq-list) 
										))))))
(define applic->lex-pe (lambda (pe params env)
	(with pe
		(lambda (_ func-name seq-list)(let (	(runl-wp (lambda (expr) (runl expr params env)))
										)
			`(applic ,(runl func-name params env) ,(map runl-wp seq-list) 
										))))))
										
;converts a parsed expression into a lexical scoped expression
(define pe->lex-pe	
	(lambda (pe)
		(runl pe '() '())))

;actually does the converting
(define runl	
	(lambda (pe params env)
		(if (pair? pe)
			(cond 
				((tagged? pe 'var) (var->lex-pe pe params env))
				((tagged? pe 'lambda-simple) (lambda-simple->lex-pe pe params env))
				((tagged? pe 'lambda-opt) (lambda-opt->lex-pe pe params env))
				((tagged? pe 'lambda-variadic) (lambda-variadic->lex-pe pe params env))
				((tagged? pe 'const) pe)
				((tagged? pe 'if-3) (if->lex-pe pe params env))
				((tagged? pe 'seq) (sym->lex-pe 'seq pe params env))
				((tagged? pe 'or) (sym->lex-pe 'or pe params env))
				((tagged? pe 'define) (define->lex-pe pe params env))
				((tagged? pe 'applic) (applic->lex-pe pe params env))
				(else pe))
			pe)))
			
			
			
			

;******************************************************************************
;******************************************************************************
;******************************************************************************

;******************************************************************************
;**************************** FINAL PROJECT ***********************************
;******************************************************************************

;******************************************************************************
;******************************************************************************
;******************************************************************************





(define sa string-append)

;******************************************************************************
;**************************** Symbol Handeling ********************************
;******************************************************************************


(define address_of_symbol_table_pointer "IMM(1)")

;Finds all the symbols in the program.
(define get-symbols
	(lambda () 
		(get_symbols_from_dict constants_table)
		
		))

;Finda all the symbols in a constants dictionary
(define get_symbols_from_dict 
	(lambda (dict)
		(cond
			((null? dict) dict)
			((symbol? (cadar dict)) `(,(cadar dict) ,@(get_symbols_from_dict (cdr dict))
			))
			(else (get_symbols_from_dict (cdr dict)))
		)
))

;Takes a list of symbols and generates CISC code for creating the initial symbol table.
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

;Generates code for creating the initial symbol table.
(define generate_symbol_table 
        (lambda () 
		(sa 
			"MOV(R1, " address_of_symbol_table_pointer ");\n"
			(symbol_table (get-symbols)))
			))


;****************************************************************************
;******************* End of Symbol Handeling ********************************
;****************************************************************************

;***************************************************************************
;************************** Const handling *********************************
;***************************************************************************

(define *initial-constant-table*
`((10 ,(if #f #t) (937610))
  (11 () (722689))
  (12 ,#f (741553 0))
  (14 ,#t (741553 1)))  )

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
	  (if (or (pe-const? lst) (pe-fvar? lst))
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
                        ((char?  smbl)  2)
                        ((number? smbl) 2)
                        ((pair? smbl) 3)
                        ((symbol? smbl) 2)
                        ((string? smbl) (+ 2 (string-length smbl)))
                        ((vector? smbl) (+ 2 (vector-length smbl)))
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
      (let*  ((return_f (lambda () #f ))
	      (val (if (pair? lst)
                    (if (pair? (car lst))
                        (if (equal? (cadar lst) smbl)
                            (caar lst)
                            (return_f))
                         (return_f))
                    (return_f))))
                  
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
    
(define get_vector_val
  (lambda (smbl lst) 
    (let ( (split_list  (vector->list smbl))
	   (gcafd (lambda(x)(get_const_address_from_dict x lst)))
    )
    (map gcafd split_list ))))  

    
  
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
                              ((char? smbl)  `(,typee ,(char->integer smbl)))
                              ((symbol? smbl)  `(,typee 0))
                              ((vector? smbl)  `(,typee ,(vector-length smbl) ,@(get_vector_val smbl new_list)))
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

(define get_const_address
	(lambda (x) (get_const_address_from_dict x constants_table)))

;Location of the void object.
(define void-loc "IMM(10)")

;Location of the nil object.
(define nil-loc "IMM(11)")

;Location of the true boolean.
(define true-loc "IMM(14)")

;Location of the false boolean.
(define false-loc "IMM(12)")

;******************************************************************************
;******************* End of Const Handeling ***********************************
;******************************************************************************

;******************************************************************************
;******************* Start Support Methods ************************************
;******************************************************************************

;CISC function for copying a string. Returns the new string in R0.
(define copy_string
"
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
")


;CISC function for comparing 2 string objects. Returns 1 or 0 in R0.
(define compare_strings
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


;CISC function for getting the last bucket in the symbol table
(define get_last_bucket
(sa
"GET_LAST_BUCKET:
PUSH(FP);
MOV(FP,SP);
MOV(R0, ADDR(" address_of_symbol_table_pointer "));

GET_LAST_BUCKET_LOOP:
CMP(INDD(R0, IMM(2)), IMM(0));
JUMP_EQ(GET_LAST_BUCKET_END);
MOV(R0,INDD(R0,IMM(2)));
JUMP(GET_LAST_BUCKET_LOOP);

GET_LAST_BUCKET_END:
POP(FP);
RETURN;"))

;CISC function:
;makes a list of all current arguments starting from argument specified by R1
;example: if you want a list of all the arguments, mov 0 to R1 and call MAKE_LIST
;example: if you want a list of all the arguments starting from the second, mov 1 to R1 and call MAKE_LIST
(define make-list
        (sa "
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
RETURN;
"
))

;Groups all support methods.
(define define_support_methods
        (sa make-list get_last_bucket compare_strings copy_string))


;******************************************************************************
;********************* End Support Methods ************************************
;******************************************************************************

;******************************************************************************
;******************************** Main Code ***********************************
;******************************************************************************

(define constants_table *initial-constant-table*)

;Compiles the scheme file input-name to the c file output-name.
(define compile-scheme-file
        (lambda (input-name output-name)
                (let*
                        (
                        (parsed-exp-list 
				(map annotate-tc 
					(map pe->lex-pe 
						(map parse 
							(tokens->sexprs 
								(append 
									(string->tokens support_code)
									(file->tokens input-name))))))
									)
                        (code (generate parsed-exp-list))
                        (port (open-output-file output-name)))
                (display "\n-----constants_table-----\n")
                (display constants_table)
                (display "\narray:\n")
                (display (list->c-array constants_table))
                (display "\n-------\n")
                (display code port)
                (close-output-port port))))

                
                
                
(define add-consts
	(lambda (lst dict)
		(if
			(null? lst)
			dict
			(add-consts (cdr lst) (add-const (car lst) dict)))
))
                
                
;Generates the entire code for a list of s-expressions.
(define generate
        (lambda (lst) 
            (begin
              (set! constants_table (begin (display (append (find-define lst) (find-consts lst) builtin_Scheme_procedures))(add-consts (append (find-define lst) (find-consts lst) builtin_Scheme_procedures) constants_table)))
                (sa
                        "\n // starting code \n"
                        start-of-file 
                        "JUMP(L_END_DEFS);\n"
                        "\n // support methods \n"
                        define_support_methods 
                       "\n //Primiteves Procedures"
			 primitive_procedures
                        "L_END_DEFS:\n"
                        "\n // definition of constants \n"
                        (generate_constants_table constants_table)
                        "\n //generating symbol table\n"
                        (generate_symbol_table)
                        "\n // bindings primitive procedures to their symbols\n"
                        (bindings)
                        "\n // fixing the stack before executing the actual code\n"
                        fix-stack
                        "\n // actual code\n"
                        (apply 
				sa 
				(map 
					(lambda (x) 
						(sa
						(if	(pe_define? x)
							(code-gen x '() '(()) )
							(sa (code-gen x '() '(()) ) print-sob-code)))) 
					lst))
                        end-of-file
))))

;CISC code for fixing the stack at the beginning of the program.
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
                ))


;CISC code for the beginning of the file.
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

;CISC code for the end of the program.
(define end-of-file
        "L_END_OF_PROGRAM:
STOP_MACHINE;

return 0;
}
")

;CISC code for printing a scheme object.
(define print-sob-code
        "PUSH(R0);
        CALL(WRITE_SOB);    
        DROP(IMM(1));
        OUT(IMM(2), IMM('\\n'));\n")

;Creates a 1 based counter.
(define counter
        (lambda ()
                (let ((x 0))
                        (lambda () 
                                (set! x (+ x 1))
                                x))))

;Returns a generator for unique labels from strings.
(define label-generator
        (lambda ()
                (let ((c (counter)))
                        (lambda (label)
                                (sa label "_" (number->string (c)))))))

;The global label generator.
(define label-maker (label-generator))


(define testparse
        (lambda (x)
                (parse (car (tokens->sexprs (string->tokens x))))))

(define test
        (lambda (s)
                (annotate-tc (pe->lex-pe (testparse s)))))

;******************************************************************************
;************************  End of Main Code ***********************************
;******************************************************************************

;******************************************************************************
;*************************** Code Generation **********************************
;******************************************************************************

;Main function for generating CISC code for a given s-expression.
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
			((pe_define? pe)(code_gen_define pe params env))
                        (else (display "Error!!!!!!!!!! expression")(display pe)(display "wasn't caught in the big cond."))
)))

;SPECIFIC TYPES OF S-EXPRESSIONS:

;DEFINE

(define pe_define? 
	(lambda (pe) (tagged? pe 'define)))
	
(define code_gen_define
	(lambda(pe params env)
		(let ((sym (cadadr pe))(val (caddr pe)))
			(sa	"\n // DEFINE  " (symbol->string sym) "\n"
				(code-gen val params env)
				"MOV(R1, INDD(IMM(" (number->string (get_const_address sym))"), IMM(1)));\n"
				"MOV(INDD(R1,IMM(1)), R0);\n"
				"MOV(R0, IMM(" void-loc "));\n"
			)
		)
	))


;FVAR
	
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
			))))

;TAIL-CALL APPLICATION
			
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
                                                        "MOV(R1,R4); // start copying arg" si "\n"
                                                        "ADD(R1, IMM(" (number->string (- 2 i)) "));\n"
                                                        "MOV(FPARG(R1), LOCAL(IMM(" si ")));\n"
                                                        (copyargs n (+ i 1)))
                                                        ))))
                        )
                (display "\n")(display "\n")(display jump-label)(display "\n")(display pe)(display "\n")
                (sa
                        "\n //TC-APPLIC \n\n"
                        (apply sa (map (lambda (e) (sa (code-gen e params env) "PUSH(R0); // finished evaluating arg \n")) (reverse (caddr pe))))               ;pushing the arguments in reverse
                        (code-gen (cadr pe) params env) ;evaluating  the procedure
                        "//TC-APPLIC CONTINUE \n"
                        "MOV(R2,FPARG(-2)); // save old FP \n"
                        "MOV(R3,FPARG(-1)); // save old RET \n"
                        "MOV(R4,FPARG(1)); // save actual number of current args \n"
                        (copyargs param-num-num 0)
                        "MOV(R5,FP);\n"
                        "SUB(R5,R4); //R5 should point to the lowest arg \n"
                        "SUB(R5, IMM(" (number->string (- 5 param-num-num)) ")); // R5 should hold the correct place for SP \n"
                        "MOV(SP,R5);\n"
                        "PUSH(IMM(" param-num ")); //push number of params \n"
                        "PUSH(IMM(" param-num ")); //push indicator \n"
                        "PUSH(INDD(R0,IMM(1))); //push the ENV \n"
                        "PUSH(R3); //push the old RET \n"
                        "MOV(FP,R2); //update FP to the old FP \n"
                        "JUMPA(INDD(R0,IMM(2))); //we jump instead of call because we already have the return address in place\n"
))))

;LAMBDA - SIMPLE

(define pe-lambda-simple?
        (lambda (pe)
                (tagged? pe 'lambda-simple)))
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
                (jump_label (label-maker "JUMP_SIMPLE"))
                )
                (display "\n\n")(display jump_label)(display "\n\n")(display pe)(display "\n\n")
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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(" L_CLOS_EXIT ");
" L_CLOS_CODE ": 
PUSH(FP);
MOV(FP, SP);
" body "
POP(FP);
RETURN;
" L_CLOS_EXIT ":\n"

                ))))

;LAMBDA VARIADIC


(define pe-lambda-variadic?
        (lambda (pe)
                (tagged? pe 'lambda-variadic)))

(define code-gen-lambda-variadic
        (lambda (pe params env)
          (let  (
                (body   (code-gen (caddr pe) (cadr pe) (cons params env))) ; fix body
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
//LAMBDA VARIADIC
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
                

;LAMBDA OPTIONAL

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
//LAMBDA OPTIONAL
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
//ending of the 2nd for loop
MOV(R0, R1);
JUMP(" L_CLOS_EXIT ");
" L_CLOS_CODE ": 
PUSH(FP);
MOV(FP, SP);
MOV(R1,IMM(" (number->string (- par-len 1)) "));
CALL(MAKE_LIST);
MOV(FPARG(IMM("(number->string (+ par-len 2)) ")),R0);
MOV(FPARG(IMM(2)), IMM(" (number->string par-len) "));
" body "
POP(FP);
RETURN;
" L_CLOS_EXIT ":\n"
))))

        
;PARAMETER VARIABLE
                
(define pe-pvar?
        (lambda (e) (tagged? e 'pvar)))
        
(define code-gen-pvar
        (lambda (pe)
                (let ((location (number->string (caddr pe))))
                        (sa
				"// PVAR \n"
                                "MOV(R1, IMM(" location "));\n"
                                "ADD(R1, IMM(3));\n" 
                                "MOV(R0, FPARG(R1));\n"))))
;BOUND VARIABLE
                        
(define pe-bvar?
        (lambda (e) (tagged? e 'bvar)))
        
(define code-gen-bvar
        (lambda (pe)
                (let ((env-pos (number->string (caddr pe)))(param-pos (number->string (cadddr pe))))
                        (sa
                                "// BVAR \n"
                                "MOV(R0, FPARG(0));\n"
                                "MOV(R0, INDD(R0, "env-pos"));\n"
                                "MOV(R0, INDD(R0, "param-pos"));\n"
                                ))))

;APPLICATION
                                
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
                        "//APPLIC CONTINUE\n"
                        "PUSH(IMM(" param-num ")); // pushing number of arguments \n"   ;
                        "PUSH(IMM(" param-num ")); // pushing again for reference for frame removal \n"
                        "PUSH(INDD(R0,IMM(1))); // pushing the environment from the closure \n" ;
                        "CALLA(INDD(R0,IMM(2))); // calling the procedure body \n"              ;
                        "DROP(IMM(1)); // dropping the environment \n"  ;
                        "POP(R1); // get number of actual args \n"              
                        "DROP(R1); // drop args \n"
                        "DROP(IMM(2)); // drop (possibly false) args count and magic number \n"
))))

;OR

(define pe-or?
        (lambda (x)
                (tagged? x 'or)))
                
(define code-gen-or
        (lambda (pe params env)
                (let* ((end-label (label-maker "L_OR_END"))
                        (code-to-append (sa
				"// OR\n"
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
                        

;SEQUENCE
                        
(define pe-seq?
        (lambda (pe)
                (tagged? pe 'seq)))

(define code-gen-seq
        (lambda (pe params env)
                (apply sa (cons "// SEQ \n" (map (lambda (x) (code-gen x params env)) (cadr pe))))))
                        
;IF
                        
(define code-gen-if3
      (lambda (pe params env)
          (let ((test   (code-gen (cadr pe) params env))
                (dit    (code-gen (caddr pe) params env))
                (dif    (code-gen (cadddr pe) params env))
                (L_THEN (label-maker "L_THEN"))
                (L_ELSE (label-maker "L_ELSE"))
                (L_IF_EXIT (label-maker "L_IF_EXIT"))
                )
                
                (sa 
			"//IF \n"
			test "
		    CMP(ADDR(R0), T_BOOL);
		    JUMP_NE(" L_THEN "); 
		    CMP(INDD(R0, 1), 0);
		    JUMP_EQ(" L_ELSE ");
		    "  L_THEN  ":" 
		    dit "
		    JUMP(" L_IF_EXIT ");
		    " L_ELSE ":
		    " dif "
		    " L_IF_EXIT ":
		    "))))
(define pe-if3?
        (lambda (pe)
                (tagged? pe 'if-3)))
                
;CONSTANT

(define pe-const?
        (lambda (pe)
                (tagged? pe 'const)))
                

;CISC code for moving a value to R0.
(define mr0 
        (lambda (val)
                (sa "MOV(R0," val ");\n")))


;******************************************************************************
;********************* End of Code Generation *********************************
;******************************************************************************


;******************************************************************************
;******************************* Support Code *********************************
;******************************************************************************
(define support_code
"
(define not (lambda (x) (if x #f #t)))
(define list (lambda x x))
(define add1 (lambda (n) (bin+ n 1)))
(define sub1 (lambda (n) (bin- n 1)))
(define bin>? (lambda (a b) (bin<? b a)))
(define bin<=? (lambda (a b) (not (bin>? a b))))
(define bin>=? (lambda (a b) (not (bin<? a b))))
(define zero? (lambda (x) (= x 0)))
(define length
  (lambda (s)
    (if (null? s) 0
	(add1 (length (cdr s))))))
(define list?
  (lambda (e)
    (or (null? e)
	(and (pair? e)
	     (list? (cdr e))))))
(define even?
  (lambda (n)
    (zero? (remainder n 2))))
(define odd?
  (lambda (n)
    (not (zero? (remainder n 2)))))
(define map
  ((lambda (y) 
     ((lambda (map1) 
	((lambda (maplist) 
	   (lambda (f . s) 
	     (maplist f s))) 
	 (y (lambda (maplist) 
	      (lambda (f s) 
		(if (null? (car s)) '() 
		    (cons (apply f (map1 car s)) 
			  (maplist f (map1 cdr s))))))))) 
      (y (lambda (map1) 
	   (lambda (f s) 
	     (if (null? s) '() 
		 (cons (f (car s)) 
		       (map1 f (cdr s))))))))) 
   (lambda (f) 
     ((lambda (x) 
	(f (lambda args 
	     (apply (x x) args)))) 
      (lambda (x) 
	(f (lambda args 
	     (apply (x x) args))))))))
(define Yn
  (lambda fs
    (let ((ms (map
		  (lambda (fi)
		    (lambda ms
		      (apply fi
			     (map (lambda (mi)
				    (lambda args
				      (apply (apply mi ms) args)))
			       ms))))
		fs)))
      (apply (car ms) ms))))
(define +
  (letrec ((loop
	    (lambda (s)
	      (if (null? s)
		  0
		  (bin+ (car s)
			(loop (cdr s)))))))
    (lambda s (loop s))))
(define *
  (letrec ((loop
	    (lambda (s)
	      (if (null? s)
		  1
		  (bin* (car s)
			(loop (cdr s)))))))
    (lambda s (loop s))))
(define -
  (lambda (a . s)
    (if (null? s)
	(bin- 0 a)
	(bin- a (apply + s)))))
(define /
  (lambda (a . s)
    (if (null? s)
	(bin/ 1 a)
	(bin/ a (apply * s)))))
(define order
  (lambda (<)
    (letrec ((loop
	      (lambda (a s)
		(or (null? s)
		    (and (< a (car s))
			 (loop (car s) (cdr s)))))))
      (lambda (a . s)
	(loop a s)))))
(define andmap
  (lambda (f . s)
    (letrec ((loop
	      (lambda (s)
		(or (null? (car s))
		    (and (apply f (map car s))
			 (loop (map cdr s)))))))
      (loop s))))
(define <>
  (letrec ((loop
	    (lambda (a s)
	      (or (null? s)
		  (and (andmap (lambda (b) (not (= a b))) s)
		       (loop (car s) (cdr s)))))))
    (lambda s
      (loop (car s) (cdr s)))))
(define foldr
  (lambda (binop final s)
    (letrec ((loop
	      (lambda (s)
		(if (null? s) final
		    (binop (car s) (loop (cdr s)))))))
      (loop s))))
(define compose
  (let ((binary-compose
	 (lambda (f g)
	   (lambda (x)
	     (f (g x))))))
    (lambda s
      (foldr binary-compose (lambda (x) x) s))))
(define caar (compose car car))
(define cadr (compose car cdr))
(define cdar (compose cdr car))
(define cddr (compose cdr cdr))
(define caaar (compose car caar))
(define caadr (compose car cadr))
(define cadar (compose car cdar))
(define caddr (compose car cddr))
(define cdaar (compose cdr caar))
(define cdadr (compose cdr cadr))
(define cddar (compose cdr cdar))
(define cdddr (compose cdr cddr))
(define caaaar (compose car caaar))
(define caaadr (compose car caadr))
(define caadar (compose car cadar))
(define caaddr (compose car caddr))
(define cadaar (compose car cdaar))
(define cadadr (compose car cdadr))
(define caddar (compose car cddar))
(define cadddr (compose car cdddr))
(define cdaaar (compose cdr caaar))
(define cdaadr (compose cdr caadr))
(define cdadar (compose cdr cadar))
(define cdaddr (compose cdr caddr))
(define cddaar (compose cdr cdaar))
(define cddadr (compose cdr cdadr))
(define cdddar (compose cdr cddar))
(define cddddr (compose cdr cdddr))
(define ^variadic-right-from-binary
  (lambda (binary-op base-value)
    (letrec ((op-list
	      (lambda (s)
		(if (null? s) base-value
		    (binary-op (car s) (op-list (cdr s)))))))
      (lambda args
	(op-list args)))))
(define ^variadic-left-from-binary
  (lambda (binary-op base-value)
    (letrec ((op-list
	      (lambda (acc s)
		(if (null? s) acc
		    (op-list (binary-op acc (car s)) (cdr s))))))
      (lambda args
	(if (null? args) base-value
	    (op-list (car args) (cdr args)))))))
(define ^char-op
  (lambda (int-op)
    (lambda (ch1 ch2)
      (int-op (char->integer ch1) (char->integer ch2)))))
(define < (order bin<?))
(define <= (order bin<=?))
(define > (order bin>?))
(define >= (order bin>=?))
(define char=? (order (^char-op bin=?)))
(define char<=? (order (^char-op bin<=?)))
(define char<? (order (^char-op bin<?)))
(define char>=? (order (^char-op bin>=?)))
(define char>? (order (^char-op bin>?)))
(define char-uppercase?
  (lambda (ch)
    (and (char<=? #\\A ch)
	 (char<=? ch #\\Z))))

(define char-lowercase?
  (lambda (ch)
    (and (char<=? #\\a ch)
	 (char<=? ch #\\z))))
(define char-upcase
  (let ((char-aA (- (char->integer #\\a) (char->integer #\\A))))
    (lambda (ch)
      (if (char-lowercase? ch)
	  (integer->char
	   (- (char->integer ch) char-aA))
	  ch))))

(define char-downcase
  (let ((char-aA (- (char->integer #\\a) (char->integer #\\A))))
    (lambda (ch)
      (if (char-uppercase? ch)
	  (integer->char
	   (+ (char->integer ch) char-aA))
	  ch))))
(define char-ci<=?
  (order
   (lambda (ch1 ch2)
     (char<=? (char-upcase ch1) (char-upcase ch2)))))

(define char-ci<?
  (order
   (lambda (ch1 ch2)
     (char<? (char-upcase ch1) (char-upcase ch2)))))

(define char-ci=?
  (order
   (lambda (ch1 ch2)
     (char=? (char-upcase ch1) (char-upcase ch2)))))

(define char-ci>?
  (order
   (lambda (ch1 ch2)
     (char>? (char-upcase ch1) (char-upcase ch2)))))

(define char-ci>=?
  (order
   (lambda (ch1 ch2)
     (char>=? (char-upcase ch1) (char-upcase ch2)))))
(define string->list
  (letrec ((loop
	    (lambda (str n s)
	      (if (= n -1) s
		  (loop str
			(- n 1)
			(cons (string-ref str n) s))))))
    (lambda (str)
      (loop str (- (string-length str) 1) '()))))
(define list->string
  (lambda (s)
    (let* ((n (length s))
	   (str (make-string n)))
      (letrec ((loop
		(lambda (s i)
		  (if (= i n) str
		      (begin
			(string-set! str i (car s))
			(loop (cdr s) (+ i 1)))))))
	(loop s 0)))))
(define string-upcase
  (lambda (string)
    (list->string
     (map char-upcase (string->list string)))))

(define string-downcase
  (lambda (string)
    (list->string
     (map char-downcase (string->list string)))))
(define list-ref
  (lambda (s i)
    (if (zero? i) (car s)
	(list-ref (cdr s) (- i 1)))))
(define list?
  (lambda (e)
    (or (null? e)
	(and (pair? e)
	     (list? (cdr e))))))
(define ormap
  (lambda (f . s)
    (letrec ((loop
	      (lambda (s)
		(and (pair? (car s))
		     (or (apply f (map car s))
			 (loop (map cdr s)))))))
      (loop s))))
(define member?
  (lambda (a s)
    (ormap (lambda (b) (eq? a b)) s)))
(define negative? (lambda (n) (< n 0)))

(define positive? (lambda (n) (> n 0)))
(define list->vector
  (lambda (s)
   (let* ((n (length s))
	   (v (make-vector n)))
     (letrec ((loop
		(lambda (s i)
		  (if (= i n) v
		      (begin
			(vector-set! v i (car s))
			(loop (cdr s) (+ i 1)))))))
	(loop s 0)))))
	
(define vector (lambda args (list->vector args)))

(define binary-string=?
  (lambda (str1 str2)
    (let ((n1 (string-length str1))
	  (n2 (string-length str2)))
      (and (= n1 n2)
	   (let ((s1 (string->list str1))
		 (s2 (string->list str2)))
	     (andmap char=? s1 s2))))))

(define binary-string<?
  (lambda (str1 str2)
    (letrec ((loop
	      (lambda (s1 s2)
		(cond ((null? s1) (pair? s2))
		      ((null? s2) #f)
		      ((char=? (car s1) (car s2))
		       (loop (cdr s1) (cdr s2)))
		      (else (char<? (car s1) (car s2)))))))
      (loop (string->list str1)
	    (string->list str2)))))

(define binary-string>? (lambda (str1 str2) (binary-string<? str2 str1)))

(define binary-string<=?
  (lambda (str1 str2) (not (binary-string>? str1 str2))))

(define binary-string>=?
  (lambda (str1 str2) (not (binary-string<? str1 str2))))

(define string=? (order binary-string=?))
(define string<? (order binary-string<?))
(define string>? (order binary-string>?))
(define string<=? (order binary-string<=?))
(define string>=? (order binary-string>=?))
(define vector->list
  (letrec ((loop
	    (lambda (v n s)
	      (if (= n -1) s
		  (loop v
			(- n 1)
			(cons (vector-ref v n) s))))))
    (lambda (v)
      (loop v (- (vector-length v) 1) '()))))
(define boolean=?
  (lambda (a b)
    (if (and (boolean? a) (boolean? b))
	(if a b (not b))
	#f ; should have generated an error message!
	)))

(define equal?
  (let ((void-object (if #f #f)))
    (letrec ((equal?
	      (lambda (a b)
		(cond
		 ;; bool
		 ((and (boolean? a) (boolean? b))
		  (boolean=? a b))
		 ;; char
		 ((and (char? a) (char? b)) (char=? a b))
		 ;; nil
		 ((null? a) (null? b))
		 ;; number
		 ((and (number? a) (number? b)) (= a b))
		 ;; pair
		 ((and (pair? a) (pair? b))
		  (and (equal? (car a) (car b))
		       (equal? (cdr a) (cdr b))))
		 ;; string
		 ((and (string? a) (string? b)) (string=? a b))
		 ;; symbol
		 ((and (symbol? a) (symbol? b)) (eq? a b))
		 ;; vector
		 ((and (vector? a) (vector? b)
		       (= (vector-length a) (vector-length b)))
		  (equal? (vector->list a) (vector->list b)))
		 ;; void
		 ((eq? a void-object) (eq? b void-object))
		 (else #f)))))
      equal?)))
(define ^associate
  (lambda (equal?)
    (letrec ((run
	      (lambda (a s)
		(cond ((null? s) #f)
		      ((equal? (caar s) a) (car s))
		      (else (run a (cdr s)))))))
      run)))

(define assoc (^associate equal?))

(define assq (^associate eq?))
(define void
  (let ((void-object
	 (if #f #f)))
    (lambda () void-object)))

(define void?
  (let ((void-object (void)))
    (lambda (x) (eq? x void-object))))
(define append
  (letrec ((app2
	    (lambda (s1 s2)
	      (if (null? s1) s2
		  (cons (car s1)
		   (app2 (cdr s1) s2)))))
	   (appl
	    (lambda (s1 s)
	      (if (null? s) s1
		  (app2 s1 (appl (car s) (cdr s)))))))
    (lambda s
      (if (null? s) '()
	  (appl (car s) (cdr s))))))
(define string-append
  (lambda s
    (list->string (apply append (map string->list s)))))

(define vector-append
  (lambda s
    (list->vector (apply append (map vector->list s)))))
(define reverse
  (letrec ((loop
	    (lambda (s r)
	      (if (null? s) r
		  (loop (cdr s) (cons (car s) r))))))
    (lambda (s)
      (loop s '()))))

(define string-reverse
  (compose
   list->string
   reverse
   string->list))

(define list-ref
  (lambda (s i)
    (if (zero? i) (car s)
	(list-ref (cdr s) (- i 1)))))
(define list-set!
  (lambda (s i x)
    (if (zero? i) (set-car! s x)
	(list-set! (cdr s) (- i 1) x))))

(define max
  (let ((binary-max (lambda (a b) (if (> a b) a b))))
    (lambda (a . s)
      (foldr binary-max a s))))

(define min
  (let ((binary-min (lambda (a b) (if (< a b) a b))))
    (lambda (a . s)
      (foldr binary-min a s))))
(define error 1)
(define number->string 
   (letrec ((number->list (lambda (number)
                (if (< number 10) (list number)
                (append (number->list (/ (- number (remainder number 10)) 10)) (list (remainder number 10)))))))
            (list->string (map (lambda (number) (integer->char (+ number 48))) (number->list 123)))))
(define =
	(letrec (
		(loop (lambda (a lst)
			(if
				(null? lst)
				#t
				(and 
					(bin=? a (car lst))
					(loop (car lst) (cdr lst)))
		))))
	(lambda (a . s)
		(loop a s))))
		


(define format
  (letrec ((st0
	    (lambda (fs args)
	      (cond ((null? fs)
		     (if (null? args)
			 '()
			 (error 'format \"Too many arguments\" args)))
		    ((char=? (car fs) #\\~) (st1 (cdr fs) args))
		    (else (cons (car fs) (st0 (cdr fs) args))))))
	   (st1
	    (lambda (fs args)
	      (cond ((null? fs) (st0 fs args))
		    ((char=? (car fs) #\\a)
		     (append (string->list
			      (sexpr->display-string
			       (car args)))
			     (st0 (cdr fs) (cdr args))))
		    ((char=? (car fs) #\\s)
		     (append (string->list (sexpr->string (car args)))
			     (st0 (cdr fs) (cdr args))))
		    ((char=? (car fs) #\\%)
		     (cons #\\newline (st0 (cdr fs) args)))
		    ((char=? (car fs) #\\~)
		     (cons #\\~ (st0 (cdr fs) args)))
		    (else (error 'format
				 \"Unrecognized meta-character\"
				 (car fs)))))))
    (lambda (format-string . args)
      (list->string (st0 (string->list format-string) args)))))

(define sexpr->string
  (letrec ((run
	    (lambda (e)
	      (cond ((null? e) \"()\")
		    ((boolean? e) (if e \"#t\" \"#f\"))
		    ((char? e) (char->string e))
		    ((number? e) (number->string e))
		    ((symbol? e) (symbol->string e))
		    ((pair? e)
		     (string-append \"(\" (pair->string (car e) (cdr e)) \")\"))
		    ((string? e) (string->string e))
		    ((vector? e) (vector->string e))
		    ((void? e) \"#<void>\")
		    ((procedure? e) \"#<procedure>\")
		    (else (error 'sexpr->string \"What's this\" e)))))
	   (vector->string
	    (lambda (v)
	      (let ((n (vector-length v))
		    (s (vector->list v)))
		(string-append
		 \"#\"
		 (run n)
		 (run s)))))
	   (string->string
	    (letrec ((st0
		      (lambda (s)
			(cond ((null? s) '())
			      ((char=? (car s) #\\newline)
			       (append '(#\\\\ #\\n)
				       (st0 (cdr s))))
			      ((char=? (car s) #\\return)
			       (append '(#\\\\ #\\r)
				       (st0 (cdr s))))
			      ((char=? (car s) #\\\\)
			       (append '(#\\\\ #\\\\)
				       (st0 (cdr s))))
			      ((char=? (car s) #\\\")
			       (append '(#\\\\ #\\\")
				       (st0 (cdr s))))
			      ((char<? (car s) #\\space)
			       (append '(#\\\\)
				       (octal-chars (char->integer (car s)))
				       (st0 (cdr s))))
			      (else (cons (car s) (st0 (cdr s))))))))
	      (lambda (string)
		(list->string
		 (append '(#\\\")
			 (st0 (string->list string))
			 '(#\\\"))))))
	   (octal-chars
		      (lambda (m)
			(let* ((o3 (digit (remainder m 8)))
			       (n (quotient m 8))
			       (o2 (digit (remainder n 8)))
			       (n (quotient n 8))
			       (o1 (digit (remainder n 8)))
			       (n (quotient n 8)))
			  (if (zero? n)
			      (list o1 o2 o3)
			      (error 'octal-chars \"Not an octal char\" m)))))
	   (digit
	    (let ((ascii0 (char->integer #\\0)))
	      (lambda (n)
		(integer->char (+ ascii0 n)))))
	   (pair->string
	    (lambda (a d)
	      (let ((string-a (run a)))
		(cond ((null? d) string-a)
		      ((pair? d)
		       (let ((string-d (pair->string (car d) (cdr d))))
			 (string-append string-a \" \" string-d)))
		      (else (let ((string-d (run d)))
			      (string-append string-a \" . \" string-d)))))))
	   (char->string
	    (lambda (ch)
	      (cond ((char=? ch #\\newline) \"#\\\\newline\")
		    ((char=? ch #\\space) \"#\\\\space\")
		    ((char=? ch #\\tab) \"#\\\\tab\")
		    ((char=? ch #\\return) \"#\\return\")
		    ((char<? ch #\\space)
		     (list->string
		      (append '(#\\# #\\\\)
			      (octal-chars (char->integer ch)))))
		    (else (list->string
			   (list #\\# #\\\\ ch)))))))
    (lambda (e)
      (if (void? e)
	  \"\"
	  (run e)))))

(define sexpr->display-string
  (letrec ((run
	    (lambda (e)
	      (cond ((null? e) \"()\")
		    ((boolean? e) (if e \"#t\" \"#f\"))
		    ((char? e) (char->string e))
		    ((number? e) (number->string e))
		    ((symbol? e) (symbol->string e))
		    ((pair? e)
		     (string-append \"(\" (pair->string (car e) (cdr e)) \")\"))
		    ((string? e) e)
		    ((vector? e) (vector->string e))
		    ((void? e) \"#<void>\")
		    ((procedure? e) \"#<procedure>\")
		    (else (error 'sexpr->string \"What's this\" e)))))
	   (vector->string
	    (lambda (v)
	      (let ((n (vector-length v))
		    (s (vector->list v)))
		(string-append
		 \"#\"
		 (run n)
		 (run s)))))
	   (pair->string
	    (lambda (a d)
	      (let ((string-a (run a)))
		(cond ((null? d) string-a)
		      ((pair? d)
		       (let ((string-d (pair->string (car d) (cdr d))))
			 (string-append string-a \" \" string-d)))
		      (else (let ((string-d (run d)))
			      (string-append string-a \" . \" string-d)))))))
	   (char->string
	    (lambda (ch)
	      (list->string
	       (list ch)))))
    (lambda (e)
      (if (void? e)
	  \"\"
	  (run e)))))



")

;******************************************************************************
;**************************  End of Support Code ******************************
;******************************************************************************


;******************************************************************************
;******************* Primitives Procedure   ***********************************
;******************************************************************************
(define EQUAL_CODE "
EQUAL:
  //SHOW(\"Equal?\", R0);
  PUSH(FP);
  MOV(FP, SP);
  MOV(R0, FPARG(3));
  MOV(R1, FPARG(4));
  //SHOW(\"R0\", R0);
  //SHOW(\"R1\", R1);
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
  //SHOW(\"Length str1: \", R5);
  MOV(R6, INDD(R1, 1));
  //SHOW(\"Length str2: \", R6);
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
  //SHOW(\"Length vec1: \", R5);
  MOV(R6, INDD(R1, 1));
  //SHOW(\"Length vec2: \", R6);
  CMP(R5, R6);
  JUMP_NE(EQUAL_NOT_EQUAL); //length do not match
  ADD(R5, IMM(1));
EQUAL_T_VECTOR_LOOP:
  CMP(R5, IMM(1)); //empty vector
  JUMP_EQ(EQUAL_EQUAL);
  //print_memory(0, 51);
  //SHOW(\"R0\", R0);
  //SHOW(\"R1\", R1);
  MOV(R2, R0);
  ADD(R2, R5);
  MOV(R2, ADDR(R2));
  MOV(R3, R1);
  ADD(R3, R5);
  MOV(R3, ADDR(R3));
  
    //call recursively to Equal
  //SHOW(\"GO_REC_NUM:\", R5);
 // SHOW(\"R2:\", R2);
  //SHOW(\"R3:\", R3);
  
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
  //SHOW(\"After call R0:\", R0);
 //print_memory(R0, R0+2);
  MOV(R0, INDD(R0, 1));
  //SHOW(\"R0:\", R0);
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
//SHOW(\"T_CLS R0:\", R0);
//SHOW(\"R1: \", R1);
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

")
(define EQ2_CODE "
EQ2:
  //SHOW(\"Equal?\", R0);
  PUSH(FP);
  MOV(FP, SP);
  MOV(R0, FPARG(3));
  MOV(R1, FPARG(4));
  //SHOW(\"R0\", R0);
  //SHOW(\"R1\", R1);
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

")
;magic_number 0 
;param1
;param2
;2
;2
;0 - env
;call

(define EQ_CODE
"
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
")

(define string->symbol_code
	(sa
"
STRING_TO_SYMBOL:
PUSH(FP);
MOV(FP,SP);
MOV(R1, FPARG(IMM(3)));
MOV(R2,IND(" address_of_symbol_table_pointer "));
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
"
		))


(define cdr_code
	"
CDR:
PUSH(FP);
MOV(FP,SP);
MOV(R0,FPARG(IMM(3)));
MOV(R0,INDD(R0,IMM(2)));
POP(FP);
RETURN;
")

(define car_code
	"
CAR:
PUSH(FP);
MOV(FP,SP);
MOV(R0,FPARG(IMM(3)));
MOV(R0,INDD(R0,IMM(1)));
POP(FP);
RETURN;
")

(define bin+_CODE 
  (sa "
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
"))

(define apply_code
"
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
")


(define bin-_CODE 
  (sa "
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
"))

(define bin*_CODE
  (sa "
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
"))

(define bin/_CODE 
  (sa "
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
"))

(define bin>_CODE 
  (sa "
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
"))

(define bin<_CODE
  (sa "
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
"))

(define bin=_CODE
  (sa "
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
"))

(define remainder_CODE
  (sa "
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
"))


(define IS_PRED_CODE
  (lambda(pred code)
  (sa "
IS_" pred ":
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 0));
CMP(R0, IMM(" code "));
JUMP_EQ(IS_" pred "_EQ);
//not equal
MOV(R0, IMM(0));
JUMP(IS_" pred "_END);
IS_" pred "_EQ:
MOV(R0, IMM(1));
IS_" pred "_END:
PUSH(R0);
CALL(MAKE_SOB_BOOL);
DROP(IMM(1));
POP(FP);
RETURN;
")))

(define TYPE_TO_TYPE
(lambda (type1 type2)
(sa
"
" type1 "_TO_" type2 ":
PUSH(FP);
MOV(FP, SP);
MOV(R0, FPARG(3));
MOV(R0, INDD(R0, 1));
PUSH(R0);
CALL(MAKE_SOB_" type2 ");
DROP(IMM(1));
POP(FP);
RETURN;
")
))


(define STRING_LENGTH_CODE
  (sa "
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
"))

(define MAKE_STRING_CODE
  (sa "
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
"))

(define MAKE_VECTOR_CODE
  (sa "
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
"))

(define STRING_REF_CODE
  (sa "
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
"))

(define VECTOR_REF_CODE
  (sa "
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
"))

(define symbol->string_code
	"
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
")

(define set-car_code
	(sa "
SET_CAR:
PUSH(FP);
MOV(FP,SP);
MOV(R0, FPARG(IMM(3)));
MOV(R1, FPARG(IMM(4)));
MOV(INDD(R0,IMM(1)), R1);
MOV(R0, "void-loc");
POP(FP);
RETURN;
"))

(define set-cdr_code
	(sa "
SET_CDR:
PUSH(FP);
MOV(FP,SP);
MOV(R0, FPARG(IMM(3)));
MOV(R1, FPARG(IMM(4)));
MOV(INDD(R0,IMM(2)), R1);
MOV(R0, "void-loc");
POP(FP);
RETURN;
"))

(define cons_code
	"
CONS:
PUSH(FP);
MOV(FP,SP);
PUSH(FPARG(4));
PUSH(FPARG(3));
CALL(MAKE_SOB_PAIR);
DROP(IMM(2));
POP(FP);
RETURN;
")

(define string_set_code
	(sa "
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
MOV(R0, " void-loc ");
POP(FP);
RETURN;
"))

(define vector_set_code
	(sa "
VECTOR_SET:
PUSH(FP);
MOV(FP,SP);
MOV(R0, FPARG(3));
MOV(R1, FPARG(4));
MOV(R2, FPARG(5));
MOV(R1,(INDD(R1, IMM(1))));
ADD(R1, IMM(2));
MOV(INDD(R0,R1),R2);
MOV(R0, " void-loc ");
POP(FP);
RETURN;
"))

(define primitive_procedures 
	(sa 
		string->symbol_code
		car_code
		cdr_code
		apply_code 
		bin+_CODE 
		bin-_CODE 
		bin*_CODE 
		bin/_CODE 
		bin>_CODE 
		bin<_CODE 
		bin=_CODE
		remainder_CODE 
		(IS_PRED_CODE "PROCEDURE" "276405")
		(IS_PRED_CODE "VECTOR" "335728")
		(IS_PRED_CODE "SYMBOL" "368031")
		(IS_PRED_CODE "STRING" "799345")
		(IS_PRED_CODE "CHAR" "181048")
		(IS_PRED_CODE "NUMBER" "945311")
		(IS_PRED_CODE "BOOLEAN" "741553")
		(IS_PRED_CODE "PAIR" "885397")
		(IS_PRED_CODE "NULL" "722689")
		(TYPE_TO_TYPE "INTEGER" "CHAR")
		(TYPE_TO_TYPE "CHAR" "INTEGER")
		STRING_LENGTH_CODE
		MAKE_STRING_CODE
		MAKE_VECTOR_CODE
		symbol->string_code
		set-car_code
		set-cdr_code
		cons_code
		string_set_code
		STRING_REF_CODE
		VECTOR_REF_CODE
		EQ_CODE
		EQ2_CODE
		EQUAL_CODE
		vector_set_code
		))



(define bind
  (lambda(sym label)
      (let ((sym_address (number->string (get_const_address sym)))
	    )
	  (sa "
//This is the bindings for " label "
PUSH(LABEL(" label "));
PUSH(IMM(7777));
CALL(MAKE_SOB_CLOSURE);
DROP(IMM(2));
MOV(R1, INDD(IMM(" sym_address "),1));
MOV(INDD(R1, 1), R0);
"))
))

(define bindings 
  (lambda ()
      (apply sa 
		(map 
			(lambda (x) (apply bind x))
			'( 
			(cdr "CDR")
			(bin+ "BIN_PLUS")
			(apply "APPLY")
			(bin- "BIN_MINUS")
			(bin* "BIN_MUL")
			(bin/ "BIN_DIV")
			(> "GRATER_THAN")
			(bin> "GRATER_THAN")
			(< "LOWER_THAN")
			(= "EQUAL_TO")
			(bin=? "EQUAL_TO")
			(bin<? "LOWER_THAN")
			(bin= "EQUAL_TO")
			(bin< "LOWER_THAN")
			(car "CAR")
			(cdr "CDR")
			(string->symbol "STRING_TO_SYMBOL")
			(remainder "REMAINDER")
			(procedure? "IS_PROCEDURE")
			(vector?  "IS_VECTOR")
			(symbol?  "IS_SYMBOL")
			(string?  "IS_STRING")
			(char?  "IS_CHAR")
			(number?  "IS_NUMBER")
			(integer? "IS_NUMBER")
			(boolean?  "IS_BOOLEAN")
			(pair?  "IS_PAIR")
			(null? "IS_NULL")
			(integer->char "INTEGER_TO_CHAR")
			(char->integer "CHAR_TO_INTEGER")
			(string-length "STRING_LENGTH")
			(vector-length "STRING_LENGTH")
			(make-string "MAKE_STRING")
			(make-vector "MAKE_VECTOR")
			(symbol->string "SYMBOL_TO_STRING")
			(set-car! "SET_CAR")
			(set-cdr! "SET_CDR")
			(cons "CONS")
			(string-set! "STRING_SET")
			(vector-set! "VECTOR_SET")
			(string-ref "STRING_REF")
			(vector-ref "VECTOR_REF")
			(eq? "EQ2")
			)
			))))

(define builtin_Scheme_procedures 	'(bin+ bin- bin* bin/ > < = bin<? bin=? bin< bin= bin> apply
					remainder procedure? car cdr string->symbol vector? symbol?
					string? char? number? boolean? pair? null? integer->char
					char->integer string-length symbol->string set-car! set-cdr!
					cons make-string vector-length string-set! make-vector string-ref
					vector-ref eq? integer? vector-set!
))
 

;******************************************************************************
;******************* End of Primitives Procedures *****************************
;******************************************************************************	