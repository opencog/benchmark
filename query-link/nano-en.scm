;
; cut-dj.scm
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog logger))
(use-modules (opencog persist) (opencog persist-sql))
(use-modules (opencog nlp) (opencog nlp learn))
(use-modules (opencog matrix) (opencog exec))
(use-modules (opencog sheaf))
(use-modules (opencog cogserver))

; Start the cogserver on port 19405
; (start-cogserver "micro-en.conf")

; Open the database.
; Edit the below, setting the database name, user and password.
(sql-open "postgres:///en_nano")

(define pca (make-pseudo-cset-api))
(define psa (add-pair-stars pca))
