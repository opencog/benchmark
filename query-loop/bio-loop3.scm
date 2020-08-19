;
; bio-loop3.scm
;
; Similar to bio-loop.scm and bio-loop2.scm, but using QueryLink.
; Unlike those, this runs multiple threads so that parallelism
; performance can be understood. To make the benchmarks comparable,
; this still performs counting, so as to count the number of times
; a gene appears in a triangle.
;
; The code below is partly a cut-n-paste of bio-loop2.scm, with
; various changes to make it multi-threaded.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

(include "load-data.scm")

; ------------------------------------------------------------------
; Define the main benchmarking routine

;; This defines a triangle-shaped search; one endpoint is fixed,
;; and we are looking for two other genes that interact with the
;; endpoint and form a triangle.
;;
;; Note this uses QueryLink; the other benchmarks use MeetLink!
(define (find-output-interactors gene)
	; (define gn (cog-name gene))
	; (define va (Variable (string-append "$a-" gn)))
	; (define vb (Variable (string-append "$b-" gn)))
	(define va (Variable "$a"))
	(define vb (Variable "$b"))
	(Query
		(VariableList
			(TypedVariable va (Type 'GeneNode))
			(TypedVariable vb (Type 'GeneNode))
		)
		(And
			(Evaluation (Predicate "interacts_with")
				(List gene va))
			(Evaluation (Predicate "interacts_with")
				(List va vb))
			(Evaluation (Predicate "interacts_with")
				(List gene vb)))
		(List gene (Set va vb))
	))

;; -----------
;; Run the triangle-benchmark
(define (run-threaded-triangle-benchmark)

	; Create a set of search patterns, one for each gene in the gene list.
	(define exec-set
		(Set (map (lambda (gene-name)
					(find-output-interactors (Gene gene-name)))
				gene-list)))

	; Create threded executer.
	(define (make-exec-thr nthreads)
		(ExecuteThreadedLink (Number nthreads) exec-set))

	(define (interaction-counts nthreads)
		(define junk (format #t "Start benchmark for ~A threads\n" nthreads))

		; Perform the search
		(define bench-secs (make-timer))
		(define results (cog-execute! (make-exec-thr nthreads)))
		(define result-list (cog-value->list results))
		(define rlen (length result-list))

		;; Assorted statistics-gathering bogosity to make this comparable
		;; to the other benchmarks.
		(define counts
			(map
				(lambda (rslt)
					; Collect up some stats
					(define tri-list (cog-value->list rslt))
					(define gene-name
						(if (= 0 (length tri-list)) ""
							(cog-name (gar (car tri-list)))))
					(for-each
						(lambda (tri)
							(define gtop (gar tri))
							(define gene-pair (gdr tri))
							(define gene-a (cog-outgoing-atom gene-pair 0))
							(define gene-b (cog-outgoing-atom gene-pair 1))
							(cog-inc-count! gtop 1)
							(cog-inc-count! gene-a 1)
							(cog-inc-count! gene-b 1))
						tri-list)
					(cons gene-name rlen))
				result-list))

		(define run-time (bench-secs))
		(format #t "Triangle relations for ~A threads in ~6f seconds\n"
				nthreads run-time)
		(format #t "Rate ~6f genes/sec Rate per thread: ~6f genes/sec\n"
				(/ rlen run-time) (/ rlen (* run-time nthreads)))
		(sleep 1)
	)

	(interaction-counts 1)
	(interaction-counts 2)
	(interaction-counts 3)
	(interaction-counts 4)
	(interaction-counts 6)
	(interaction-counts 8)
	(interaction-counts 12)
	(interaction-counts 16)
	*unspecified*
)

;; -----------
; Run the benchmark three times
(display "Will run the threaded triangle benchmark three times ...\n")
(run-threaded-triangle-benchmark)
(sleep 1)
(run-threaded-triangle-benchmark)
(sleep 1)
(run-threaded-triangle-benchmark)
(sleep 1)

(exit)

; =================================================================
