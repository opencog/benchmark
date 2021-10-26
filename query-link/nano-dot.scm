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
; The various kinds of dot products to measure.

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

; ------------------------------------------------------------------
; Define the main benchmarking routine
; Takes dot products in varrious ways.
(define (measure-dot-products WORD-LST QRY EXPECTED-CNT)

	; Avoid atomspace polution.
	(define (wrap func)
		(cog-push-atomspace)
		(let ((rc (func)))
			(cog-pop-atomspace)
			rc))

	; Return a dot-product of a word with "the".
	(define (dot-prod LEFT-WRD)
		(define (func)
			(cog-execute! (Accumulate (QRY LEFT-WRD (Word "the")))))
		(wrap func))

	; Performance stats
	(define start-time (current-time))
	(define (elapsed-secs)
		(define now (current-time))
		(define diff (- now start-time))
		(set! start-time now)
		diff)

	(define (dot-prod-x LEFT-WRD)
		(define dot (cog-value->list (dot-prod LEFT-WRD)))
		(display ".")
		(if (< 0 (length dot)) (car dot) 0))

	(define (report TOT-CNT)
		(define ti (elapsed-secs))
		(define nwrds (length WORD-LST))
		(newline)
		(newline)
		(if (not (= EXPECTED-CNT TOT-CNT))
			(format #t "Measurement failure: incorrect number of links found: ~D\n"
				TOT-CNT)
			(format #t "~A Elapsed: ~D secs Tot-cnt=~D Avg=~6F secs/word Rate=~6F words/sec\n"
				(procedure-name QRY) ti TOT-CNT (/ ti nwrds) (/ nwrds ti)))
		(newline)
	)

	; Loop over all words.
	(define tot-cnt
		(fold
			(lambda (WRD CNT) (+ CNT (dot-prod-x WRD)))
			0 WORD-LST))

	(report tot-cnt)
)

; Run the benchmark
(format #t "Will count ~D words " (psa 'left-basis-size))
(define wrds (psa 'left-basis))
(measure-dot-products wrds qdot-simple 3884127978)
(exit)
; ------------------------------------------------------------------
