(define (power-set set)
(if (null? set) '(())
(let ((power-set-of-rest (power-set (cdr set))))
(append power-set-of-rest
(map (lambda (subset) (cons (car set) subset))
power-set-of-rest)))))

(define (remove ls elem)
(cond ((null? ls) '())
((equal? (car ls) elem) (remove (cdr ls) elem))
(else (cons (car ls) (remove (cdr ls) elem)))))

(define (all-permutations items)
(if (null? items) '(())
(apply append
(map (construct-permutations-generator items) items))))

(define (all-permutations items)
(if (null? items) '(())
(apply append
(map (construct-permutations-generator items) items))))

(define (construct-permutation-prepender element)
(lambda (permutation) (cons element permutation)))

(and (equal? ((construct-permutation-prepender "where") '("did" "she" "go")) '("where" "did" "she" "go"))

    (equal? (map (construct-permutation-prepender "vanilla") '(("ice" "cream") ("extract") ("latte" "with" "soy" "milk"))) '(("vanilla" "ice" "cream")
  ("vanilla" "extract")
  ("vanilla" "latte" "with" "soy" "milk")))
    
    (equal? (map (construct-permutation-prepender 1) '((2 3 4) (2 4 3) (3 2 4) (3 4 2) (4 2 3) (4 3 2))) '((1 2 3 4) (1 2 4 3) (1 3 2 4) (1 3 4 2) (1 4 2 3)
  (1 4 3 2))
)
    )