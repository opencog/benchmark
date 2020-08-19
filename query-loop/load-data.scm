;
; load-data.scm
; Load the genomics dataset data into the AtomSpace.
;
(use-modules (srfi srfi-1))
(use-modules (opencog) (opencog exec))

; Needeed for definition of GeneNode and MoleculeNode
; You can skip this by redefining these two, below.
(use-modules (opencog bioscience))

; Optionally run the cogserver.
; This allows the system to be monitored by saying
; `rlwrap telnet localhost 17001`
; (use-modules (opencog cogserver))
; (start-cogserver)

; Performance stats timer
(define (make-timer)
	(let ((start-time (get-internal-real-time)))
		(lambda ()
			(define now (get-internal-real-time))
			(define diff (/ (- now start-time) internal-time-units-per-second))
			(set! start-time now)
			diff)))

; List of genes on which to perform the query.
(load "gene-list.scm")

; Non-human-readable definitions are used to compress the size
; of the data file to something reasonable-ish.
;
; The `(opencog bioscience)` module defines GeneNode and MoleculeNode
; but if that is not available, just redefine it to be ConceptNode.
; (define Gene Concept)
(define e Evaluation)
(define l List)
(define g Gene)
(define m Molecule)
(define c Concept)
(define s Member)
(define i (Predicate "interacts_with"))
(define x (Predicate "expresses"))
(define b (Predicate "has_biogridID"))
(define d (Predicate "has_pubmed_ID"))
(define z (Predicate "has_entrez_id"))

(format #t "Start loading raw data ...\n")
(define elapsed-secs (make-timer))
; (primitive-load "biogrid.scm")
(primitive-load "biogrid-full.scm")
(format #t "Loaded biogrid data in ~6f seconds\n" (elapsed-secs))
(primitive-load "entrez.scm")
(format #t "Loaded entrez data in ~6f seconds\n" (elapsed-secs))
(primitive-load "uniprot.scm")
(format #t "Loaded uniprot data in ~6f seconds\n" (elapsed-secs))
(primitive-load "reactome-chebi.scm")
(format #t "Loaded chebi reactome in ~6f seconds\n" (elapsed-secs))
(primitive-load "reactome-ncbi.scm")
(format #t "Loaded ncbi reactome in ~6f seconds\n" (elapsed-secs))
(primitive-load "reactome-uniprot.scm")
(format #t "Loaded uniprot reactome in ~6f seconds\n" (elapsed-secs))
(format #t "AtomSpace contents: ~A\n" (cog-report-counts))

; ------------------------------------------------------------------
