(define pending-move #f)
(define current-player "Don")
(define player-list '("Don"))
(define (add-player! n) (set! current-player n) #t)
