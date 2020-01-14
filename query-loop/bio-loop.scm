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

(load "biogrid.scm")
(format #t "Loaded raw data in ~6f seconds\n" (elapsed-secs))
(cog-report-counts)

; ------------------------------------------------------------------
; Define the main benchmarking routine


; Run the benchmark
(exit)
; ------------------------------------------------------------------
