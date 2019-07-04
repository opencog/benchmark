;
; nano-scm.scm
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog matrix) (opencog exec))
(use-modules (opencog persist) (opencog persist-sql))
(use-modules (opencog nlp) (opencog nlp learn))
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

(cog-report-counts)
(format #t "Loaded ~D words, ~D disjuncts and ~D Sections\n"
	(psa 'left-basis-size) (psa 'right-basis-size)
	(length (psa 'get-all-elts)))

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

	; Performance stats
	(define start-time (current-time))
	(define (elapsed-secs)
		(define now (current-time))
		(define diff (- now start-time))
		(set! start-time now)
		diff)

	(define (link-count-x LEFT-WRD)
		(display ".")
		(link-count LEFT-WRD))

	; Count over all words.
	(define tot-cnt
		(fold
			(lambda (WRD CNT) (+ CNT (link-count-x WRD)))
			0 WORD-LST))

	(define (report)
		(define ti (elapsed-secs))
		(define nwrds (length WORD-LST))
		(newline)
		(newline)
		(if (not (= 152904 tot-cnt))
			(format #t "Measurement failure: incorrect number of links found: ~D\n"
				tot-cnt)
			(format #t "Elapsed: ~D secs Tot-cnt=~A Avg=~6F secs/word Rate=~6F cnts/sec\n"
				ti tot-cnt (/ ti nwrds) (/ tot-cnt ti)))
		(newline)
	)

	(report)
)

; Run the benchmark
(format #t "Will count ~D words " (psa 'left-basis-size))
(count-full-links (psa 'left-basis))
(exit)
; ------------------------------------------------------------------
