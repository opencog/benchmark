;
; nano-scm.scm
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog matrix) (opencog exec))
(use-modules (opencog persist) (opencog persist-sql))
(use-modules (opencog nlp))
(use-modules (opencog cogserver))

; Start the cogserver on port 19405
; (start-cogserver "nano-en.conf")

; Open the database.
(sql-open "postgres:///en_nano")

(define pca (make-pseudo-cset-api))
(define psa (add-pair-stars pca))

; Load the atomspace from the database and then close it.
(psa 'fetch-pairs)
(sql-close)

; ------------------------------------------------------------------
; Define the main benchmarking routine
; Count links between sections.
(define (count-full-links WORD-LST)

	; Define variables only once.
	(define blvars
		(VariableList
			(TypedVariable (Variable "r-word") (Type 'WordNode))
			(TypedVariable (Glob "r-pre")
				(TypeSet (Type 'Connector)
					(Interval (Number 0) (Number -1))))
			(TypedVariable (Glob "r-post")
				(TypeSet (Type 'Connector)
					(Interval (Number 0) (Number -1))))
			(TypedVariable (Glob "l-pre")
				(TypeSet (Type 'Connector)
					(Interval (Number 0) (Number -1))))
			(TypedVariable (Glob "l-post")
				(TypeSet (Type 'Connector)
					(Interval (Number 0) (Number -1))))
		))

	; Another invariant part of the pattern
	(define rcon
		(ConnectorSeq
			(Glob "l-pre")
			(Connector (Variable "r-word") (ConnectorDir "+"))
		(Glob "l-post")))

	; Create a BindLink that will obtain the links between sections,
	; and report the results.
	(define (make-blink LEFT-WRD)
		(BindLink blvars
			(And
				; A Setion from the LEFT-WRD connecting to the right.
				(Present (Section LEFT-WRD rcon))
				; A Section from the discovered right-word, pointing left.
				(Present
					(Section
						(Variable "r-word")
						(ConnectorSeq
							(Glob "r-pre")
							(Connector
								LEFT-WRD
								(ConnectorDir "-"))
							(Glob "r-post")))))

			; Temp stand-in for "the real thing"
			(List
				(List (Glob "l-pre") (Any "Left") (Glob "l-post"))
				(List (Glob "r-pre") (Any "Right") (Glob "r-post")))
		))

	; Avoid atomspace polution.
	(define (wrap func)
		(cog-push-atomspace)
		(let ((rc (func)))
			(cog-pop-atomspace)
			rc))

	; Return a count of the links that were found.
	(define (link-count LEFT-WRD)
		(define (func)
			(cog-arity (cog-execute! (make-blink LEFT-WRD))))
		(wrap func))

	; Why is this so slow?
	(define wrd-no 0)
	(define (word-no) (set! wrd-no (+ wrd-no 1)) wrd-no)

	(define start-time (current-time))
	(define since-time (current-time))
	(define (elapsed-secs)
		(define now (current-time))
		(define diff (- now since-time))
		(set! since-time now)
		diff)

	(define tot-cnt 0)
	(define nwrds (length WORD-LST))

	(define (link-count-x LEFT-WRD)
		(define lc (link-count LEFT-WRD))
		(define ti (- (current-time) start-time))
		(if (= 0 ti) (set! ti 1))
		(set! tot-cnt (+ tot-cnt lc))
		(format #t "~A of ~A count=~A in ~A secs wrd ~A"
			(word-no) nwrds lc (elapsed-secs) LEFT-WRD)
		(format #t "--- Elapsed: ~D:~2,'0D:~2,'0D Tot-cnt=~A Avg=~6F secs/word Rate=~6F cnts/sec\n"
			(inexact->exact (floor (/ ti 3600.0)))
			(inexact->exact (floor (/ (remainder ti 3600) 60.)))
			(remainder ti 60)
			tot-cnt
			(/ ti wrd-no)
			(/ tot-cnt ti))
		lc)

	; Count over all words.
	(fold
		(lambda (WRD CNT) (+ CNT (link-count-x WRD)))
		0 WORD-LST)
)
; ------------------------------------------------------------------
