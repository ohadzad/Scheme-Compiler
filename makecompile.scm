(load "comp.scm")
(display "Compiling main.scm\n")
(compile-scheme-file "main.scm" "main.c")
(exit)