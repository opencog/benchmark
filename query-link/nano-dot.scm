;
; nano-dot.scm
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
; Takes dot products in varrious ways.
(define (measure-dot-products WORD-LST)

	(define (qdot-simple WRD-A WRD-B)
		(Query
			; The search variable.
			(TypedVariable (Variable "$conseq") (Type 'ConnectorSeq))

			; What to look for.
			(Present
				(Section WRD-A (Variable "$conseq"))
				(Section WRD-B (Variable "$conseq")))

			; Multiply the counts on the search results.
			(Times
				(CountOf (Section WRD-A (Variable "$conseq")))
				(CountOf (Section WRD-B (Variable "$conseq"))))))

	; Avoid atomspace polution.
	(define (wrap func)
		(cog-push-atomspace)
		(let ((rc (func)))
			(cog-pop-atomspace)
			rc))

	; Return a dot-product of a word with "the".
	(define (simple-dot LEFT-WRD)
		(define (func)
			(cog-execute! (Accumulate (qdot-simple LEFT-WRD (Word "the")))))
		(wrap func))

	; Performance stats
	(define start-time (current-time))
	(define (elapsed-secs)
		(define now (current-time))
		(define diff (- now start-time))
		(set! start-time now)
		diff)

	(define (simple-dot-x LEFT-WRD)
		(display ".")
(display LEFT-WRD) (newline)
		(cog-value-ref (simple-dot LEFT-WRD) 0))

	; Loop over all words.
	(define tot-cnt
		(fold
			(lambda (WRD CNT) (+ CNT (simple-dot-x WRD)))
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
(measure-dot-products (psa 'left-basis))
(exit)
; ------------------------------------------------------------------
