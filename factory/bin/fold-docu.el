;; emacs edit mode for this file is -*- Emacs-Lisp -*-
;; $Id: fold-docu.el,v 1.2 1997-07-22 13:41:32 schmidt Exp $

;;{{{ docu
;;
;; fold-docu.el - fold all documentation foldings in a file.
;;
;; To unfold all documentation, call fold-open-buffer.
;;
;;}}}

(require 'folding)

(defun fold-docu ()
  "Fold all documentation foldings in a file."
  (interactive)
  (goto-char (point-min))
  (save-excursion
    (while (re-search-forward "{{{ .*docu$" nil t)
      (fold-hide))))

(provide 'fold-docu)
