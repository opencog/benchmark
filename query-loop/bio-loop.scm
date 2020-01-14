;
; bio-loop.scm
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

; Performance stats
(define start-time (current-time))
(define (elapsed-secs)
	(define now (current-time))
	(define diff (- now start-time))
	(set! start-time now)
	diff)

; Hack around custom Atom definitions
(define Gene Concept)

(format #t "Start loading raw data ...\n")
(primitive-load "biogrid.scm")
(format #t "Loaded raw data in ~6f seconds\n" (elapsed-secs))
(format #t "AtomSpace contents: ~A\n" (cog-report-counts))

; ------------------------------------------------------------------
; Define the main benchmarking routine


(define (find-output-interactors gene)
	(Get
		(VariableList
			; (TypedVariable (Variable "$a") (Type 'GeneNode))
			; (TypedVariable (Variable "$b") (Type 'GeneNode))

			(TypedVariable (Variable "$a") (Type 'ConceptNode))
			(TypedVariable (Variable "$b") (Type 'ConceptNode))
		)
		(And
			(Evaluation (Predicate "interacts_with")
				(List gene (Variable "$a")))
			(Evaluation (Predicate "interacts_with")
				(List (Variable "$a") (Variable "$b")))
			(Evaluation (Predicate "interacts_with")
				(List gene (Variable "$b")))
		)))

(define g (find-output-interactors (Gene "MAP2K4")))
(elapsed-secs)
(define result (cog-execute! g))
(format #t "Ran query in ~6f seconds; got ~A results\n"
	(elapsed-secs) (cog-arity result))


; Run the benchmark
(exit)
; ------------------------------------------------------------------
