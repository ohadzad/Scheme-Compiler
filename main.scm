(define s (make-string 2))
(string-set! s 0 (integer->char 97))
(string-set! s 1 (integer->char 98))
(eq? 'ab (string->symbol s))