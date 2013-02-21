;;; compiler.scm
;;;
;;; Programmer: ???

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
				(> (length (cadr e)) 0)
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
			
								