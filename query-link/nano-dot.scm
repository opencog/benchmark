;
; nano-dot.scm
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))
(use-modules (opencog persist) (opencog persist-file))
(use-modules (opencog nlp))
; (use-modules (opencog cogserver))

; Start the cogserver on port 19405
; (start-cogserver "nano-en.conf")

; Open the database and load the data.
(define fsn (FileStorageNode "en-nano.atomese"))
(cog-open fsn)
(load-atomspace fsn)
(cog-close fsn)

(cog-report-counts)
(format #t "Loaded ~D words, ~D disjuncts and ~D Sections\n"
	(cog-count-atoms 'Word) (cog-count-atoms 'ConnectorSeq)
	(cog-count-atoms 'Section))

; ------------------------------------------------------------------
; The various kinds of dot products to measure.

; The simplest query, on which all the others are modeled.
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

; ------------
; A more complicated kind of way of doing the above.
(define (qdot-identical WRD-A WRD-B)
	(Query
		; The search variable.
		(VariableList
			(TypedVariable (Variable "$conseq") (Type 'ConnectorSeq))
			(TypedVariable (Variable "$sect-a") (Type 'Section))
			(TypedVariable (Variable "$sect-b") (Type 'Section))
		)

		; What to look for.
		(And
			(Identical (Variable "$sect-a")
				(Section WRD-A (Variable "$conseq")))
			(Identical (Variable "$sect-b")
				(Section WRD-B (Variable "$conseq")))
		)

		; Multiply the counts on the search results.
		(Times
			(CountOf (Variable "$sect-a"))
			(CountOf (Variable "$sect-b")))))

; ------------
; A lambda variant of above.
(define (qdot-lambda WRD-A WRD-B)
	(Query
		; The search variable.
		(VariableList
			(TypedVariable (Variable "$conseq") (Type 'ConnectorSeq))
			(TypedVariable (Variable "$sect-a") (Type 'Section))
			(TypedVariable (Variable "$sect-b") (Type 'Section))
		)

		; What to look for.
		(And
			(Identical (Variable "$sect-a")
				(Section WRD-A (Variable "$conseq")))
			(Identical (Variable "$sect-b")
				(Section WRD-B (Variable "$conseq")))
		)

		; Multiply the counts on the search results.
		(Put
			(Lambda
				(VariableList (Variable "$x") (Variable "$y"))
				(Times
					(CountOf (Variable "$x"))
					(CountOf (Variable "$y"))))
			(List (Variable "$sect-a") (Variable "$sect-b")))))

; ------------
; A version suitable for Shapes.
(define (qdot-choice WRD-A WRD-B)
	(Query
		; The search variable.
		(VariableList
			(TypedVariable (Variable "$conseq")
				(TypeChoice (Type 'ConnectorSeq) (Type 'Shape)))
			(TypedVariable (Variable "$sect-a")
				(TypeChoice (Type 'Section) (Type 'CrossSection)))
			(TypedVariable (Variable "$sect-b")
				(TypeChoice (Type 'Section) (Type 'CrossSection)))
		)

		; What to look for.
		(And
			(Identical (Variable "$sect-a")
				(Choice
					(Section WRD-A (Variable "$conseq"))
					(CrossSection WRD-A (Variable "$conseq"))))
			(Identical (Variable "$sect-b")
				(Choice
					(Section WRD-B (Variable "$conseq"))
					(CrossSection WRD-B (Variable "$conseq"))))
		)

		; Multiply the counts on the search results.
		(Times
			(CountOf (Variable "$sect-a"))
			(CountOf (Variable "$sect-b")))))

; ------------
; A mashup of all the above.
(define (qdot-mashup WRD-A WRD-B)
	(Query
		; The search variable.
		(VariableList
			(TypedVariable (Variable "$conseq")
				(TypeChoice (Type 'ConnectorSeq) (Type 'Shape)))
			(TypedVariable (Variable "$sect-a")
				(TypeChoice (Type 'Section) (Type 'CrossSection)))
			(TypedVariable (Variable "$sect-b")
				(TypeChoice (Type 'Section) (Type 'CrossSection)))
		)

		; What to look for.
		(And
			(Identical (Variable "$sect-a")
				(Choice
					(Section WRD-A (Variable "$conseq"))
					(CrossSection WRD-A (Variable "$conseq"))))
			(Identical (Variable "$sect-b")
				(Choice
					(Section WRD-B (Variable "$conseq"))
					(CrossSection WRD-B (Variable "$conseq"))))
		)

		; Multiply the counts on the search results.
		(Put
			(Lambda
				(VariableList (Variable "$x") (Variable "$y"))
				(Times
					(CountOf (Variable "$x"))
					(CountOf (Variable "$y"))))
			(List (Variable "$sect-a") (Variable "$sect-b")))))

; --------
; Wrapper to return plain numbers.
(define (dot-wrap QRY WRD-A WRD-B)
	(define fv (cog-execute! (Accumulate (QRY WRD-A WRD-B))))
	(define dot (cog-value->list fv))
	(if (< 0 (length dot)) (car dot) 0))

(define (dot-simple WRD-A WRD-B) (dot-wrap qdot-simple WRD-A WRD-B))
(define (dot-identical WRD-A WRD-B) (dot-wrap qdot-identical WRD-A WRD-B))
(define (dot-lambda WRD-A WRD-B) (dot-wrap qdot-lambda WRD-A WRD-B))
(define (dot-choice WRD-A WRD-B) (dot-wrap qdot-choice WRD-A WRD-B))
(define (dot-mashup WRD-A WRD-B) (dot-wrap qdot-mashup WRD-A WRD-B))

; --------
#! =========
; A version that uses MeetLink instead of QueryLink
; Well, GetLink, so that it returns a Set, so that Put is happy.
; But of course, this won't actually work. So this is a dead end
; that we record for posterity.
(define (mdot-meet WRD-A WRD-B)
	(Get
		; The search variable.
		(TypedVariable (Variable "$conseq") (Type 'ConnectorSeq))

		; What to look for.
		(Present
			(Section WRD-A (Variable "$conseq"))
			(Section WRD-B (Variable "$conseq")))))

(define (dot-meet WRD-A WRD-B)
	(define fv (cog-execute! (Accumulate
		(Put
			(Lambda (Variable "$cseq")
			(Times
				(CountOf (Section WRD-A (Variable "$cseq")))
				(CountOf (Section WRD-B (Variable "$cseq")))))
		 (mdot-meet WRD-A WRD-B)))))
	(define dot (cog-value->list fv))
	(if (< 0 (length dot)) (car dot) 0))

======== !#

; --------
; A baseline that just performs the pattern matching, and nothing else.
(define (mdot-fake WRD-A WRD-B)
	(Meet
		; The search variable.
		(TypedVariable (Variable "$conseq") (Type 'ConnectorSeq))

		; What to look for.
		(Present
			(Section WRD-A (Variable "$conseq"))
			(Section WRD-B (Variable "$conseq")))))

(define (dot-fake WRD-A WRD-B)
	(cog-execute! (mdot-fake WRD-A WRD-B)) -42)

; ==================================================================
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
		(define (func) (QRY LEFT-WRD (Word "the")))
		(wrap func))

	; Performance stats
	(define (get-wallclock)
		(/ (get-internal-real-time) internal-time-units-per-second))
	(define start-time (get-wallclock))
	(define (elapsed-secs)
		(define now (get-wallclock))
		(define diff (- now start-time))
		(set! start-time now)
		diff)

	(define (report TOT-CNT)
		(define ti (elapsed-secs))
		(define nwrds (length WORD-LST))
		(if (and (not (= EXPECTED-CNT TOT-CNT)) (not (= (* nwrds -42) TOT-CNT)))
			(format #t "Measurement failure: incorrect number of links found: ~D\n"
				TOT-CNT)
			(format #t "~A: Elapsed: ~4,2F secs Avg=~6F secs/word Rate=~6F words/sec\n"
				(procedure-name QRY) ti (/ ti nwrds) (/ nwrds ti)))
	)

	; Loop over all words.
	(define tot-cnt
		(fold
			(lambda (WRD CNT) (+ CNT (dot-prod WRD)))
			0 WORD-LST))

	; (gc)
	(report tot-cnt)
)

; Run the benchmark
(format #t "Will take dot products of ~D words\n" (cog-count-atoms 'Word))
(define wrds (cog-get-atoms 'Word))
(define expected-count 3884127978)
(measure-dot-products wrds dot-fake expected-count)
(measure-dot-products wrds dot-simple expected-count)
(measure-dot-products wrds dot-identical expected-count)
(measure-dot-products wrds dot-lambda expected-count)
(measure-dot-products wrds dot-choice expected-count)
(measure-dot-products wrds dot-mashup expected-count)

; The olde-school matrix code.
(use-modules (opencog matrix))
(use-modules (opencog nlp learn))
(define pca (make-pseudo-cset-api))
(define psa (add-pair-stars pca))
(define sim (add-similarity-compute psa))
(define (dot-matrix WRD-A WRD-B) (sim 'right-product WRD-A WRD-B))

(measure-dot-products wrds dot-matrix expected-count)

(exit)
; ------------------------------------------------------------------
