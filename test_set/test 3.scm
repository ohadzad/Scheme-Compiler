
(define (atom? x)
  (not (pair? x)))


;;Functions to manipulate a binary tree
(define (leaf? node) (atom? node))
(define (left node) (cadr node))
(define (right node) (caddr node))
(define (label node) (if (leaf? node) node (car node)))

;; Preorder Traversal

(define (pre-order node)
  (cons
   (label node)
   (if (leaf? node)
       '()
       (append
	(pre-order (left node))
	(pre-order (right node))))))

;;Post-order traversal
(define (post-order node)
  (list
   (if (leaf? node) '()
       (append
	(post-order (left node))
	(post-order (right node))))
   (label node)))

;;in-order traversal
(define (in-order node)
  (if (leaf? node) (label node)
      (list
       (in-order (left node))
       (cons
	(label node)
	(in-order (right node))))))

(define tree '(A (B C D)(E (F G H) I)))
(equal? (pre-order tree) '(A B C D E F G H I))
(equal? (post-order tree) '(((() C () D) B ((() G () H) F () I) E) A))
(equal? (in-order tree) '((C (B . D)) (A (G (F . H)) (E . I))))
