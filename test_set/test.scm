(define fact$ (lambda (n cont)
	      (if (zero? n)
		  (cont 1)
		  (fact$ (- n 1)
			 (lambda (fact-n-1)
			      (cont (* n fact-n-1)))))))
(define identity (lambda (x) x))

(define test (lambda (x . y) (if (null? y) x (bin+ x (apply test y)) ))) 

(define fib$ (lambda (n cont)
		(if (< n 2) 
		    (cont n)
		    (fib$ (- n 1)
			  (lambda (n-1)
			     (fib$ (- n 2)
				   (lambda (n-2)
				      (cont (+ n-1 n-2)))))))))
				      
(and 
(and (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   a
                   )))
             1 2) 3 4 5) 6 7 8 9) 1)
     (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   b
                   )))
             1 2) 3 4 5) 6 7 8 9) 2)
     (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   c
                   )))
             1 2) 3 4 5) 6 7 8 9) '())
     (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   d
                   )))
             1 2) 3 4 5) 6 7 8 9) 3)
     (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   e
                   )))
             1 2) 3 4 5) 6 7 8 9) 4)
     (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   f
                   )))
             1 2) 3 4 5) 6 7 8 9) 5)
      (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   g
                   )))
             1 2) 3 4 5) 6 7 8 9) '())
      (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   h
                   )))
             1 2) 3 4 5) 6 7 8 9) 6)
      (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                  i
                   )))
             1 2) 3 4 5) 6 7 8 9) 7)
       (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                  j
                   )))
             1 2) 3 4 5) 6 7 8 9) 8)
        (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                  k
                   )))
             1 2) 3 4 5) 6 7 8 9) 9)
     (eq? (((
             (lambda (a b . c)
               (lambda (d e f . g)
                 (lambda (h i j k . l)
                   l
                   )))
             1 2) 3 4 5) 6 7 8 9) '())
     
     ) 
     (= (apply test '(1 1 1)) 3)
     (= (fact$ 10 identity) 3628800)
     (= (fib$ 20 identity) 6765)
     )