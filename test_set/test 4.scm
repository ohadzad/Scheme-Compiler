;insertion sort: 

(define (part x y)
  (if (equal? (car x) y)
      (cdr x)
      (cons (car x) (part (cdr x) y))
  )
)
 
(define (sort2 x)
  (if (null? x)
      x
      (cons (apply min x) (sort2 (part x (apply min x))))
  )
)
 
(equal? (sort2 '(4 5 -4 5 1 0 3 2)) '(-4 0 1 2 3 4 5 5))

;merge sort: 

(define (part x a b f)
  (if (null? x)
      (list a b)
      (if f
          (part (cdr x) (cons (car x)  a) b (not f))
          (part (cdr x) a (cons (car x) b)   (not f))
      )
  )
)
 
(define (merge a b x)
  (cond
      ((null? a) (append (reverse b) x))
      ((null? b) (append (reverse a) x))
      (else (if (< (car a) (car b))
                (merge (cdr a) b (cons (car a) x))
                (merge a (cdr b) (cons (car b) x))
            )
      )
  )
)
 
(define (sortm x)
  (if (null? (cdr x))
      x
      (let* (
            	(par (part x '() '() #t))
            	(l (sortm (car par)))
       	  	(r (sortm (cadr par)))
            )
            (reverse (merge l r '()))
      )
  )
)
 
(equal? (sortm '(4 2342 56421 1 213 52 1 5451 -5 2 -4 2 -1 -111)) '(-111 -5 -4 -1 1 1 2 2 4 52 213 2342 5451 56421))


;Quicksort 

(define (part x a b f)
  (if (null? x)
      (list a b)
      (if (< (car x) f)
          (part (cdr x) (cons (car x)  a) b f)
          (part (cdr x) a (cons (car x) b)  f)
      )
  )
)
 
(define (qsort x)
  (if (null? x)
       x
       (if (null? (cdr x))
           x
           (let* (
                  (par (part (cdr x) '() '() (car x)))
                  (l (qsort (car par)))
                  (r (qsort (cadr par)))
                  )
             (append l (list (car x)) r)
           )
       )
   )
)
 
(equal? (qsort '(54 4562 243 26345 123 53 -234 -2 -4 -6 1 4 5 -5 5 -5 9 0)) '(-234 -6 -5 -5 -4 -2 0 1 4 5 5 9 53 54 123 243 4562 26345))

(define
  (depth x)
      (begin
        (if (or (null? x) (not (pair? x) ) )
            0
            (max
                 ( + 1 (depth (car x)) )
                 (depth (cdr x))
            )
        )
      )
)
 
(= (depth '( ( ( 1 2 ) ( 3 4 ) ( ( 4 5 ) ( 6 ( 7 ) ) ) ) ) ) 5)
(= (depth '( 1 ) ) 1)
(= (depth '( 1 ( 2 ( 3 ( 4 ) ) ) ) ) 4)