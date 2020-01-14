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

(load "gene-list.scm")

(format #t "Start loading raw data ...\n")
(elapsed-secs)
(primitive-load "biogrid.scm")
(format #t "Loaded raw data in ~6f seconds\n" (elapsed-secs))
(format #t "AtomSpace contents: ~A\n" (cog-report-counts))

; ------------------------------------------------------------------
; Define the main benchmarking routine

;; This defines a triangle-shaped search; one endpoint is fixed,
;; and we are looking for two other genes that interact with the
;; endpoint and form a triangle.
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

; Run the benchmark
(for-each
	(lambda (gene-name)
		(define gene (Gene gene-name))
		(define query (find-output-interactors gene))
		(elapsed-secs)
		(define result (cog-execute! query))
		(format #t "Ran query ~A in ~6f seconds; got ~A results\n"
			gene-name (elapsed-secs) (cog-arity result))
		(cog-delete result)
	)
	gene-list)

(exit)
; ------------------------------------------------------------------
