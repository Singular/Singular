;;; singular.el --- Emacs support for Computer Algebra System Singular

;; $Id: singular.el,v 1.8 1998-07-28 14:44:57 schmidt Exp $

;;; Commentary:


;;; Code:

;;{{{ Style and coding conventions

;; Style and coding conventions:
;;
;; - "Singular" is written with an upper-case `S' in comments, doc
;;   strings, and messages.  As part of symbols, it is written with
;;   a lower-case `s'.
;; - use a `fill-column' of 70 for doc strings and comments
;; - use foldings to structure the source code but try not to exceed a
;;   maximal depth of two folding (one folding in another folding which is
;;   on top-level)
;; - use lowercase folding titles except for first word
;; - folding-marks are `;;{{{' and `;;}}}' resp., for sake of standard
;;   conformity
;; - mark incomplete doc strings or code with `NOT READY' (optionally
;;   followed by an explanation what exactly is missing)
;;
;; - use `singular' as prefix for all global symbols
;; - use `singular-debug' as prefix for all global symbols concerning
;;   debugging.
;;
;; - mark dependencies on Emacs flavor/version with a comment of the form
;;   `;; Emacs[ <version>]'     resp.
;;   `;; XEmacs[ <version>][ <nasty comment>]' (in that order, if
;;   possible)
;; - use a `cond' statement to execute Emacs flavor/version-dependent code,
;;   not `if'.  This is to make such checks more extensible.
;; - try to define different functions for different flavors/version
;;   and use `singular-fset' at library-loading time to set the function
;;   you really need.  If the function is named `singular-<basename>', the
;;   flavor/version-dependent functions should be named
;;   `singular-<flavor>[-<version>]-<basename>'.

;; - use `singular-debug' for debugging output/actions
;; - to switch between buffer and process names, use the functions
;;   `singular-process-name-to-buffer-name' and
;;   `singular-buffer-name-to-process-name'

;;}}}

(require 'comint)

;;{{{ Code common to both modes
;;{{{ Debugging stuff
(defvar singular-debug nil
  "*List of modes to debug or `all' to debug all modes.
Currently, only the mode `interactive' is supported.")

(defun singular-debug-format (string)
  "Return STRING in a nicer format."
  (save-match-data
    (while (string-match "\n" string)
      (setq string (replace-match "^J" nil nil string)))

    (if (> (length string) 16)
	(concat "<" (substring string 0 7) ">...<" (substring string -8) ">")
      (concat "<" string ">"))))

(defmacro singular-debug (mode form &optional else-form)
  "Major debugging hook for singular.el.
Evaluates FORM if `singular-debug' equals `all' or if MODE is an
element of `singular-debug', othwerwise ELSE-FORM"
  `(if (or (eq singular-debug 'all)
	   (memq ,mode singular-debug))
       ,form
     ,else-form))
;;}}}

;;{{{ Determining version
(defvar singular-emacs-flavor nil
  "A symbol describing the current Emacs.
Currently, only Emacs \(`emacs') and XEmacs are supported \(`xemacs').")

(defvar singular-emacs-major-version nil
  "An integer describing the major version of the current emacs.")

(defvar singular-emacs-minor-version nil
  "An integer describing the major version of the current emacs.")

(defun singular-fset (real-function emacs-function xemacs-function
				    &optional emacs-19-function)
  "Set REAL-FUNCTION to one of the functions, in dependency on Emacs flavor and version.
Sets REAL-FUNCTION to XEMACS-FUNCTION if `singular-emacs-flavor' is
`xemacs'.  Sets REAL-FUNCTION to EMACS-FUNCTION if `singular-emacs-flavor'
is `emacs' and `singular-emacs-major-version' is 20.  Otherwise, sets
REAL-FUNCTION to EMACS-19-FUNCTION which defaults to EMACS-FUNCTION.

This is not as common as would be desirable.  But it is sufficient so far."
  (cond
   ;; XEmacs
   ((eq singular-emacs-flavor 'xemacs)
    (fset real-function xemacs-function))
   ;; Emacs 20
   ((eq singular-emacs-major-version 20)
    (fset real-function emacs-function))
   ;; Emacs 19
   (t
    (fset real-function (or emacs-19-function emacs-function)))))

(defun singular-set-version ()
  "Determine flavor, major version, and minor version of current emacs.
singular.el is guaranteed to run on Emacs 19.34, Emacs 20.2, and XEmacs
20.2.  It should run on newer version and on slightly older ones, too."

  ;; get major and minor versions first
  (if (and (boundp 'emacs-major-version)
	   (boundp 'emacs-minor-version))
      (setq singular-emacs-major-version emacs-major-version
	    singular-emacs-minor-version emacs-minor-version)
    (with-output-to-temp-buffer "*singular warnings*"
      (princ
"You seem to have quite an old Emacs or XEmacs version.  Some of the
features from singular.el will not work properly.  Consider upgrading to a
more recent version of Emacs or XEmacs.  singular.el is guaranteed to run
on Emacs 19.34, Emacs 20.2, and XEmacs 20.2."))
    ;; assume the oldest version we support
    (setq singular-emacs-major-version 19
	  singular-emacs-minor-version 34))

  ;; get flavor
  (if (string-match "XEmacs\\|Lucid" emacs-version)
      (setq singular-emacs-flavor 'xemacs)
    (setq singular-emacs-flavor 'emacs)))

(singular-set-version)
;;}}}

;;{{{ Faces
(make-face 'singular-input-face)
(set-face-background 'singular-input-face "peach puff")
(defvar singular-input-face 'singular-input-face
  "Face for user input.
This face should have set background only.")

(make-face 'singular-output-face)
(set-face-background 'singular-output-face "blanched almond")
(defvar singular-output-face 'singular-output-face
  "Face for Singular output.
This face should have set background only.")

(defun singular-lookup-face (face-type)
  "Return face belonging to FACE-TYPE.
NOT READY [should be rewritten completely.  Interface should stay the same.]!"
  (cond ((eq face-type 'input) singular-input-face)
	((eq face-type 'output) singular-output-face)))
;;}}}
;;}}}

;;{{{ Singular interactive mode
;;{{{ Key map
(defvar singular-interactive-mode-map ()
  "Key map to use in Singular interactive mode.")

(if singular-interactive-mode-map
    ()
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (setq singular-interactive-mode-map
	  (nconc (make-sparse-keymap) comint-mode-map)))
   ;; XEmacs
   (t
    (setq singular-interactive-mode-map (make-keymap))
    (set-keymap-parents singular-interactive-mode-map (list comint-mode-map))
    (set-keymap-name singular-interactive-mode-map
		     'singular-interactive-mode-map)))
  (define-key singular-interactive-mode-map "\C-m" 'singular-send-input))
;;}}}

;;{{{ Miscellaneous

;; Note:
;;
;; We assume a one-to-one correspondance between Singular buffers
;; and Singular processes.  We always have
;; (equal buffer-name (concat "*" process-name "*")).

(defun singular-buffer-name-to-process-name (buffer-name)
  "Create the process name for BUFFER-NAME.
The process name is the buffer name with surrounding `*' stripped
off."
  (substring buffer-name 1 -1))

(defun singular-process-name-to-buffer-name (process-name)
  "Create the buffer name for PROCESS-NAME.
The buffer name is the process name with surrounding `*'."
  (concat "*" process-name "*"))

(defmacro singular-process-mark ()
  "Return process mark of current buffer."
  (process-mark (get-buffer-process (current-buffer))))
;;}}}

;;{{{ Customizing variables of comint

;; Note:
;;
;; In contrast to the variables from comint.el, all the variables
;; below are global variables.  It would not make any sense to make
;; them buffer-local since
;; o they are read only when Singular interactive mode comes up;
;; o since they are Singular-dependent and not user-dependent, i.e.,
;;   the user would not mind to change them.
;;
;; For the same reasons these variables are not marked as
;; "customizable" by a leading `*'.

(defvar singular-prompt-regexp "^> "
  "Regexp to match prompt patterns in Singular.
Should not match the continuation prompt \(`.'), only the regular
prompt \(`>').

This variable is used to initialize `comint-prompt-regexp' when
Singular interactive mode starts up.")

(defvar singular-delimiter-argument-list '(?= ?\( ?\) ?, ?;)
  "List of characters to recognize as separate arguments.

This variable is used to initialize `comint-delimiter-argument-list'
when Singular interactive mode starts up.")

(defvar singular-input-ignoredups t
  "If non-nil, don't add input matching the last on the input ring.

This variable is used to initialize `comint-input-ignoredups' when
Singular interactive mode starts up.")

(defvar singular-buffer-maximum-size 2048
  "The maximum size in lines for Singular buffers.

This variable is used to initialize `comint-buffer-maximum-size' when
Singular interactive mode starts up.")

(defvar singular-input-ring-size 64
  "Size of input history ring.

This variable is used to initialize `comint-input-ring-size' when
Singular interactive mode starts up.")

(defvar singular-history-filter-regexp "\\`\\(..?\\|\\s *\\)\\'"
  "Regular expression to filter strings *not* to insert in the history.
By default, input consisting of less than three characters and input
consisting of white-space only is not added to the history.")

(defvar singular-history-filter
  (function (lambda (string)
	      (not (string-match singular-history-filter-regexp string))))
  "Predicate for filtering additions to input history.

This variable is used to initialize `comint-input-filter' when
Singular interactive mode starts up.")
;;}}}

;;{{{ Simple section stuff for both Emacs and XEmacs

;; Note:
;;
;; NOT READY[was sind und wollen simple sections]!

(defvar singular-simple-sec-clear-type 'input
  "Type of clear simple sections.
If nil no clear simple sections are used.")

(defvar singular-simple-sec-last-end nil
  "Marker at the end of the last simple section.")

(defun singular-simple-sec-init (pos)
  "Initialize global variables belonging to simple section management.
Creates the buffer-local marker `singular-simple-sec-last-end' and
initializes it to POS."
  (make-local-variable 'singular-simple-sec-last-end)
  (if (not (and (boundp 'singular-simple-sec-last-end)
		singular-simple-sec-last-end))
      (setq singular-simple-sec-last-end (make-marker)))
  (set-marker singular-simple-sec-last-end pos))

;; Note:
;;
;; The rest of the folding is either marked as
;; Emacs
;; or
;; XEmacs

(singular-fset 'singular-simple-sec-create
	       'singular-emacs-simple-sec-create
	       'singular-xemacs-simple-sec-create)

(singular-fset 'singular-simple-sec-reset-last
	       'singular-emacs-simple-sec-reset-last
	       'singular-xemacs-simple-sec-reset-last)

(singular-fset 'singular-simple-sec-start
	       'singular-emacs-simple-sec-start
	       'singular-xemacs-simple-sec-start)

(singular-fset 'singular-simple-sec-end
	       'singular-emacs-simple-sec-end
	       'singular-xemacs-simple-sec-end)

(singular-fset 'singular-simple-sec-start-at
	       'singular-emacs-simple-sec-start-at
	       'singular-xemacs-simple-sec-start-at)

(singular-fset 'singular-simple-sec-end-at
	       'singular-emacs-simple-sec-end-at
	       'singular-xemacs-simple-sec-end-at)

(singular-fset 'singular-simple-sec-type
	       'singular-emacs-simple-sec-type
	       'singular-xemacs-simple-sec-type)

(singular-fset 'singular-simple-sec-at
	       'singular-emacs-simple-sec-at
	       'singular-xemacs-simple-sec-at)

(singular-fset 'singular-simple-sec-before
	       'singular-emacs-simple-sec-before
	       'singular-xemacs-simple-sec-before)

(singular-fset 'singular-simple-sec-in
	       'singular-emacs-simple-sec-in
	       'singular-xemacs-simple-sec-in)
;;}}}

;;{{{ Simple section stuff for Emacs
(defun singular-emacs-simple-sec-create (type end)
  "Create a new simple section of type TYPE.
Creates the section from end of previous simple section up to END.
Returns the new simple section or `empty' if no simple section has
been created.
Assumes that no narrowing is in effect.
Updates `singular-simple-sec-last-end'."
  (let ((last-end (marker-position singular-simple-sec-last-end))
	;; `simple-sec' is the new simple section or `empty'
	simple-sec)

    ;; get beginning of line before END.  At this point we need that there
    ;; are no restrictions.
    (setq end (let ((old-point (point)))
		(goto-char end) (beginning-of-line)
		(prog1 (point) (goto-char old-point))))

    (cond
     ;; do not create empty sections
     ((eq end last-end) (setq simple-sec 'empty))
     ;; create only non-clear simple sections
     ((not (eq type singular-simple-sec-clear-type))
      ;; if type has not changed we only have to extend the previous
      ;; simple section
      (setq simple-sec (singular-emacs-simple-sec-before last-end))
      (if (eq type (singular-emacs-simple-sec-type simple-sec))
	  ;; move existing overlay
	  (setq simple-sec (move-overlay simple-sec (overlay-start simple-sec) end))
	;; create new overlay
	(setq simple-sec (make-overlay last-end end))
	;; set type property
	(overlay-put simple-sec 'singular-type type)
	;; set face
	(overlay-put simple-sec 'face (singular-lookup-face type))
	;; evaporate empty sections
	(overlay-put simple-sec 'evaporate t))))
	    
    ;; update end of last simple section
    (set-marker singular-simple-sec-last-end end)
    simple-sec))

(defun singular-emacs-simple-sec-reset-last (pos)
  "Reset end of last simple section to POS after accidental extension.
Updates `singular-simple-sec-last-end', too."
  (let ((simple-sec (singular-emacs-simple-sec-at pos)))
    (if simple-sec (move-overlay simple-sec (overlay-start simple-sec) pos))
    (set-marker singular-simple-sec-last-end pos)))

(defun singular-emacs-simple-sec-start (simple-sec)
  "Return start of non-clear simple section SIMPLE-SEC."
  (overlay-start simple-sec))

(defun singular-emacs-simple-sec-end (simple-sec)
  "Return end of non-clear simple section SIMPLE-SEC."
  (overlay-end simple-sec))

(defun singular-emacs-simple-sec-start-at (pos)
  "Return start of clear section at position POS."
  (save-restriction
    (widen)
    (let ((previous-overlay-change (1+ (point))))
      ;; this `while' loop at last will run into the end of the next
      ;; non-clear overlay or stop at bob.  Since POS may be right at the end
      ;; of a previous non-clear location, we have to search at least one
      ;; time from POS+1 backwards.
      (while (not
	      (or (singular-emacs-simple-sec-before previous-overlay-change)
		  (eq previous-overlay-change (point-min))))
	(setq previous-overlay-change
	      (previous-overlay-change previous-overlay-change)))
      previous-overlay-change)))

(defun singular-emacs-simple-sec-end-at (pos)
  "Return end of clear section at position POS."
  (save-restriction
    (widen)
    (let ((next-overlay-change (next-overlay-change (point))))
      ;; this `while' loop at last will run into the beginning of the next
      ;; non-clear overlay or stop at eob.  Since POS may not be at the
      ;; beginning of a non-clear simple section we may start searching
      ;; immediately.
      (while (not
	      (or (singular-emacs-simple-sec-at next-overlay-change)
		  (eq next-overlay-change (point-max))))
	(setq next-overlay-change
	      (next-overlay-change next-overlay-change)))
      next-overlay-change)))

(defun singular-emacs-simple-sec-type (simple-sec)
  "Return type of SIMPLE-SEC."
  (if simple-sec
      (overlay-get simple-sec 'singular-type)
    singular-simple-sec-clear-type))

(defun singular-emacs-simple-sec-at (pos)
  "Return simple section at position POS."
  (let ((overlays (overlays-at pos)) simple-sec)
    ;; be careful, there may be other overlays!
    (while (and overlays (not simple-sec))
      (if (singular-emacs-simple-sec-type (car overlays))
	  (setq simple-sec (car overlays)))
      (setq overlays (cdr overlays)))
    simple-sec))

(defun singular-emacs-simple-sec-before (pos)
  "Return simple section before position POS.
This is the same as `singular-emacs-section-at' except if POS falls on
a section border.  In this case `singular-emacs-section-before'
returns the previous simple section instead of the current one."
  (singular-emacs-simple-sec-at (max 1 (1- pos))))

(defun singular-emacs-simple-sec-in (beg end)
  "Return a list of all simple sections intersecting with the region from BEG to END.
A simple section intersects the region if the section and the region
have at least one character in common.
The result contains both clear and non-clear simple sections in the
order in that the appear in the region."
  ;; NOT READY
  nil)
;;}}}

;;{{{ Section stuff

;; Note:
;;
;; NOT READY[was sind und wollen sections im Gegensatz zu simple
;; sections?]!

(defun singular-section-at (pos &optional restricted)
  "Return section at position POS.
Returns section intersected with current restriction if RESTRICTED is
non-nil."
  (let* ((simple-sec (singular-simple-sec-at pos))
	 (type (singular-simple-sec-type simple-sec))
	 (start (if simple-sec
		    (singular-simple-sec-start simple-sec)
		  (singular-simple-sec-start-at pos)))
	 (end (if simple-sec
		  (singular-simple-sec-end simple-sec)
		(singular-simple-sec-end-at pos))))
    (if restricted
	(vector simple-sec type
		(max start (point-min)) (min end (point-max)))
      (vector simple-sec type start end))))

(defmacro singular-section-simple-sec (section)
  "Return underlying simple section of SECTION."
  `(aref ,section 0))

(defmacro singular-section-type (section)
  "Return type of SECTION."
  `(aref ,section 1))

(defmacro singular-section-start (section)
  "Return start of SECTION."
  `(aref ,section 2))

(defmacro singular-section-end (section)
  "Return end of SECTION."
  `(aref ,section 3))
;;}}}

;;{{{ Folding sections
(defvar singular-folding-ellipsis "Singular I/O ..."
  "Ellipsis to show for folded input or output.")

(defun singular-fold-internal (list fold)
  "(Un)fold regions in LIST.
LIST should have the format (START1 END1 START2 END2 ...).
Folds if FOLD is non-nil, otherwise unfolds.
Folds without affecting undo information, buffer-modified flag, and
even for read-only files.
Assumes that there is no narrowing in effect."
  (let ((inhibit-read-only t) (buffer-undo-list t)
	(modified (buffer-modified-p))
	(old-point (point)))
    (unwind-protect
	;; do it !!
	(if fold
	    (while list
	      (goto-char (car list)) (insert ?\r)
	      (subst-char-in-region (car list) (nth 1 list) ?\n ?\r t)
	      (setq list (cdr (cdr list))))
	  (while list
	    (subst-char-in-region (car list) (nth 1 list) ?\r ?\n t)
	    (goto-char (car list)) (delete-char 1)
	    (setq list (cdr (cdr list)))))

      ;; we have to restore the point and the modified flag.  The read-only
      ;; state and undo information are restored by the outer `let'.
      ;; This code is unwide-protected.
      (goto-char old-point)
      (or modified
	  (set-buffer-modified-p nil)))))

(defun singular-section-foldedp (section)
  "Return t iff SECTION is folded.
Assumes that there is no narrowing in effect."
  (eq (char-after (singular-section-start section)) ?\r))

(defun singular-fold-section (section)
  "Fold SECTION.
Folds section at current cursor position and goes to beginning of
section if called interactively."
  (interactive (list (singular-section-at (point))))
  (let ((start (singular-section-start section))
	;; we have to save restrictions this way since we change text
	;; outside the restriction.  Note that we do not use a marker for
	;; `old-point-min'.  This way, even partial narrowed sections are
	;; folded properly if they have been narrowed at bol.  Nice but
	;; dirty trick.
	(old-point-min (point-min))
	(old-point-max (point-max-marker)))
    (unwind-protect
	(progn
	  (widen)
	  (singular-fold-internal (list start
					(singular-section-end section))
				  (not (singular-section-foldedp section))))
      (narrow-to-region old-point-min old-point-max)
      (set-marker old-point-max nil))
    (if (interactive-p) (goto-char (max start (point-min))))))
;;}}}

;;{{{ Debugging input and output filters
(defun singular-debug-input-filter (string)
  "Echo STRING in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Input filter: %s"
			   (singular-debug-format string))))

(defun singular-debug-output-filter (string)
  "Echo STRING in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Output filter: %s"
			   (singular-debug-format string))))
;;}}}

;;{{{ Sending input and receiving output

;;{{{ Some lengthy notes on filters

;; Note:
;;
;; The filters and other functions have access to four important markers,
;; `comint-last-input-start', `comint-last-input-end',
;; `comint-last-output-start', and the buffers process mark.  They are
;; initialized to nil (except process mark, which is initialized to
;; `(point-max)') when Singular is called in `singular'.  These markers are
;; modified by `comint-send-input' and `comint-output-filter' but not in a
;; quite reliable way.  Here are some valid invariants and pre-/post-
;; conditions.
;;
;; Output filters:
;; ---------------
;; The output filters may be sure that they are run in the process buffer
;; and that the process buffer is still alive.  `comint-output-filter'
;; ensures this.  But `comint-output-filter' does neither catch changes in
;; match data done by the filters nor does it protect against non-local
;; exits of itself or of one of the filters.  As a result, the current
;; buffer may be changed in `comint-output-filter'!
;;
;; `comint-output-filter' is called also from `comint-send-input' (dunno
;; why).  The following holds only for executions of `comint-output-filter'
;; as a result of Singular output being processed.
;;
;; We have the following preconditions for any output filters (up to
;; changes through other filter functions):
;; - The argument STRING is what has been inserted in the buffer.  Not
;;   really reliable.
;; - `comint-last-input-end' <= `comint-last-output-start' <= process mark
;;   if all of them are defined
;; - The text between `comint-last-output-start' and process mark is the
;;   one which has been inserted immediately before.
;; - The text between `comint-last-input-end' (if it is defined) and
;;   process mark is the one which has been inserted into buffer since last
;;   user input.
;; - It seems to be a reasonable assumption that the text between process
;;   mark and `(point-max)' is user input.
;;
;; The standard filters which come with comint.el do not change the markers
;; in the preconditions described above.  But they may change the text
;; (e.g., `comint-strip-ctrl-m').
;;
;; Post-conditions for `comint-output-filter':
;; - `comint-last-output-start' <= process mark.  The region between them
;;   is the text which has been inserted immediately before.
;; - `comint-last-input-start' and `comint-last-input-end' are unchanged.
;;
;; Input filters:
;; --------------
;; `comint-send-input' ensures that the process is still alive.  Further
;; preconditions for any input filter (up to changes through filter
;; functions):
;; - The (CR-terminated) argument STRING is what will be sent to the
;;   process (up to slight differences between XEmacs and Emacs).  Not
;;   really reliable.
;; - process mark <= `(point)'
;; - The (CR-terminated) text between process mark and `(point)' is what
;;   has been inserted by the user.
;;
;; Post-conditions for `comint-send-input':
;; - `comint-last-input-start' <= `comint-last-input-end'
;;                              = `comint-last-output-start' (!)
;;                              = process mark = `(point)'.
;;   The region between the first of them is what has been inserted by the
;;   user.
;;
;; Invariants which always hold outside `comint-send-input' and
;; `comint-output-filter':
;; ------------------------------------------------------------
;; - `comint-last-input-start' <= `comint-last-input-end' <= process mark
;;   if all of them are defined.  The region between the first of them is
;;   the last input entered by the user, the region between the latter of
;;   them is the text from Singular printed since the last input.
;; - `comint-last-output-start' <= process mark if both are defined.
;; - It is a reasonable assumption that the text from process mark up to
;;   `(point-max)' is user input.

;;}}}

(defun singular-send-input (string)
  "NOT READY!!"
  ;; STRING is always nil when called interactively
  (interactive (list nil))

  (let ((process (get-buffer-process (current-buffer)))
	pmark)
    ;; some checks and initializations
    (or process (error "Current buffer has no process"))
    (setq pmark (marker-position (process-mark process)))

    ;; NOT READY[history expansion, handling of STRING]!

    ;; note that the input string does not include its terminal newline
    (let* ((raw-input (buffer-substring pmark (point)))
	   (input raw-input)
	   (history raw-input))

      ;; insert newline into buffer
      (insert-before-markers ?\n)

      ;; insert input into history
      (if (and (funcall comint-input-filter history)
	       (or (null comint-input-ignoredups)
		   (not (ring-p comint-input-ring))
		   (ring-empty-p comint-input-ring)
		   (not (string-equal (ring-ref comint-input-ring 0) history))))
	  (ring-insert comint-input-ring history))

      ;; run hooks and reset index into history
      (run-hook-with-args 'comint-input-filter-functions (concat input "\n"))
      (setq comint-input-ring-index nil)

      ;; update markers and create a new simple section
      (set-marker comint-last-input-start pmark)
      (set-marker comint-last-input-end (point))
      (set-marker (process-mark process) (point))
      (singular-debug 'interactive-simple-secs
		      (message "Simple input section: %S"
			       (singular-simple-sec-create 'input (point)))
		      (singular-simple-sec-create 'input (point)))

      ;; do it !!
      (send-string process input)
      (send-string process "\n"))))

(defun singular-output-filter (process string)
  "Insert STRING containing output from PROCESS into its associated buffer.

Takes care off:
- current buffer, even in case of non-local exits;
- point and restriction in buffer associated with process;
- markers which should not be advanced when inserting output.
Updates:
- process mark;
- `comint-last-output-start';
- simple sections;
- mode line.
Runs the hooks on `comint-output-filter-functions'.

For a more detailed descriptions of the output filter, the markers it
sets, and output filter functions refer to the section \"Some lengthy
notes on filters\" in singular.el."
  (let ((process-buffer (process-buffer process))
	(save-buffer (current-buffer)))

    ;; check whether buffer is still alive
    (if (and process-buffer (buffer-name process-buffer))
	(unwind-protect
	    (progn
	      (set-buffer process-buffer)
	      (let ((save-point (point))
		    (save-point-min (point-min))
		    (save-point-max (point-max))
		    (save-pmark (marker-position (process-mark process)))
		    (inhibit-read-only t)
		    (n (length string)))
		(widen)
		(goto-char save-pmark)

		;; adjust point and narrowed region borders
		(if (<= (point) save-point) (setq save-point (+ save-point n)))
		(if (< (point) save-point-min) (setq save-point-min (+ save-point-min n)))
		(if (<= (point) save-point-max) (setq save-point-max (+ save-point-max n)))

		;; do it !!
		(insert-before-markers string)

		;; reset markers and simple sections which may have
		;; been advanced by above insertion.  We rely on the
		;; fact that `set-marker' always returns some non-nil
		;; value.  Looks nicer this way.
		(and (= comint-last-input-end (point))
		     (set-marker comint-last-input-end save-pmark)
		     ;; this may happen only on startup and only if
		     ;; `comint-last-input-end' has been modified,
		     ;; too.  Hence, we check for it after the first
		     ;; test.
		     (= comint-last-input-start (point))
		     (set-marker comint-last-input-start save-pmark))
		(and (= singular-simple-sec-last-end (point))
		     (singular-simple-sec-reset-last save-pmark))

		;; set new markers and create/extend new simple section
		(set-marker comint-last-output-start save-pmark)
		(singular-debug 'interactive-simple-secs
				(message "Simple output section: %S"
					 (singular-simple-sec-create 'output (point)))
				(singular-simple-sec-create 'output (point)))

		;; restore old values, run hooks, and force mode line update
		(narrow-to-region save-point-min save-point-max)
		(goto-char save-point)
		(run-hook-with-args 'comint-output-filter-functions string)
		(force-mode-line-update)))

	  ;; this is unwind-protected
	  (set-buffer save-buffer)))))
;;}}}

;;{{{ Singular interactive mode
(defun singular-interactive-mode ()
  "Major mode for interacting with Singular.

NOT READY [how to send input]!

NOT READY [multiple Singulars]!

Singular buffers are automatically limited in length \(by default, to
2048 lines).  This limit may be adjusted by setting
`singular-buffer-maximum-size' before Singular interactive mode starts
up or by setting `comint-buffer-maximum-size' while Singular
interactive mode is running.

\\{singular-interactive-mode-map}
Customization: Entry to this mode runs the hooks on `comint-mode-hook'
and `singular-interactive-mode-hook' \(in that order).  Before each
input, the hooks on `comint-input-filter-functions' are run.  After
each Singular output, the hooks on `comint-output-filter-functions'
are run.

NOT READY [much more to come.  See shell.el.]!"
  (interactive)

  ;; run comint mode and do basic mode setup
  (comint-mode)
  (setq major-mode 'singular-interactive-mode)
  (setq mode-name "Singular Interaction")
  (use-local-map singular-interactive-mode-map)

  ;; customize comint for Singular
  (setq comint-prompt-regexp singular-prompt-regexp)
  (setq comint-delimiter-argument-list singular-delimiter-argument-list)
  (setq comint-input-ignoredups singular-input-ignoredups)
  (make-local-variable 'comint-buffer-maximum-size)
  (setq comint-buffer-maximum-size singular-buffer-maximum-size)
  (setq comint-input-ring-size singular-input-ring-size)
  (setq comint-input-filter singular-history-filter)
  ;; do not add `comint-truncate-buffer' if it already has been added
  ;; globally.  This is sort of a bug in `add-hook'.
  (and (default-boundp 'comint-output-filter-functions)
       (not (memq 'comint-truncate-buffer
		  (default-value 'comint-output-filter-functions)))
       (add-hook 'comint-output-filter-functions
		 'comint-truncate-buffer nil t))

  ;; get name of history file (if any)
  (setq comint-input-ring-file-name (getenv "SINGULARHIST"))
  (if (or (not comint-input-ring-file-name)
	  (equal comint-input-ring-file-name "")
	  (equal (file-truename comint-input-ring-file-name) "/dev/null"))
      (setq comint-input-ring-file-name nil))

  ;; selective display
  (setq selective-display t)
  (setq selective-display-ellipses t)
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (setq buffer-display-table (or (copy-sequence standard-display-table)
				   (make-display-table)))
    (set-display-table-slot buffer-display-table
     'selective-display (vconcat singular-folding-ellipsis)))
    ;; XEmacs
   (t
    (set-glyph-image invisible-text-glyph singular-folding-ellipsis (current-buffer))))

  ;; input and output filters
  (singular-debug 'interactive-filter
		  (add-hook 'comint-input-filter-functions
			    'singular-debug-input-filter nil t))
  (singular-debug 'interactive-filter
		  (add-hook 'comint-output-filter-functions
			    'singular-debug-output-filter nil t))

  (run-hooks 'singular-interactive-mode-hook))
;;}}}

;;{{{ Starting singular
(defvar singular-start-file "~/.emacs_singularrc"
  "Name of start-up file to pass to Singular.
If the file named by this variable exists it is given as initial input
to any Singular process being started.  Note that this may lose due to
a timing error if Singular discards input when it starts up.")

(defvar singular-default-executable "Singular"
  "Default name of Singular executable.
Used by `singular' when new Singular processes are started.")

(defvar singular-default-name "singular"
  "Default process name for Singular process.
Used by `singular' when new Singular processes are started.")

(defvar singular-default-switches '("-t")
  "Default switches for Singular processes.
Used by `singular' when new Singular processes are started.")

(defun singular-exit-sentinel (process message)
 "Clean up after termination of Singular.
Writes back input ring after regular termination of Singular if
process buffer is still alive."
  (save-excursion
    (singular-debug 'interactive
		    (message "Sentinel: %s" (substring message 0 -1)))
    (if (string-match "finished\\|exited" message)
	(let ((process-buffer (process-buffer process)))
	  (if (and process-buffer
		   (buffer-name process-buffer)
		   (set-buffer process-buffer))
	      (progn
		(singular-debug 'interactive (message "Writing input ring back"))
		(comint-write-input-ring)))))))

(defun singular-exec (buffer name executable start-file switches)
  "Start a new Singular process NAME in BUFFER, running EXECUTABLE.
EXECUTABLE should be a string denoting an executable program.
SWITCHES should be a list of strings that are passed as command line
switches.  START-FILE should be the name of a file which contents is
sent to the process.

Deletes any old processes running in that buffer.
Moves point to the end of BUFFER.
Initializes all important markers and the simple sections.
Runs `comint-exec-hook' and `singular-exec-hook' (in that order).
Returns BUFFER."
  (let ((save-buffer (current-buffer)))
    (unwind-protect
	(progn
	  (set-buffer buffer)

	  ;; delete any old processes
	  (let ((process (get-buffer-process buffer)))
	    (if process (delete-process process)))

	  ;; create new process
	  (singular-debug 'interactive (message "Starting new Singular"))
	  (let ((process (comint-exec-1 name buffer executable switches)))

	    ;; set process filter and sentinel
	    (set-process-filter process 'singular-output-filter)
	    (set-process-sentinel process 'singular-exit-sentinel)
	    (make-local-variable 'comint-ptyp)
	    (setq comint-ptyp process-connection-type) ; T if pty, NIL if pipe.

	    ;; go to the end of the buffer, set up markers, and
	    ;; initialize simple sections
	    (goto-char (point-max))
	    (set-marker comint-last-input-start (point))
	    (set-marker comint-last-input-end (point))
	    (set-marker comint-last-output-start (point))
	    (set-marker (process-mark process) (point))
	    (singular-simple-sec-init (point))

	    ;; feed process with start file and read input ring
	    (if start-file
		(progn
		  (singular-debug 'interactive (message "Feeding start file"))
		  (sleep-for 1)			; try to avoid timing errors
		  (insert-file-contents start-file)
		  (setq start-file (buffer-substring (point) (point-max)))
		  (delete-region (point) (point-max))
		  (comint-send-string process start-file)))
	    (singular-debug 'interactive (message "Reading input ring"))
	    (comint-read-input-ring t)

	    ;; execute hooks
	    (run-hooks 'comint-exec-hook)
	    (run-hooks 'singular-exec-hook))
	  
	  buffer)
      ;; this code is unwide-protected
      (set-buffer save-buffer))))

;; Note:
;;
;; In contrast to shell.el, `singular' does not run
;; `singular-interactive-mode' every time a new Singular process is
;; started, but only when a new buffer is created.  This behaviour seems
;; more intuitive w.r.t. local variables and hooks.

(defun singular (&optional executable name switches)
  "Run an inferior Singular process, with I/O through an Emacs buffer.

NOT READY [arguments, default values, and interactive use]!

If buffer exists but Singular is not running, starts new Singular.
If buffer exists and Singular is running, just switches to buffer.
If a file `~/.emacs_singularrc' exists, it is given as initial input.
Note that this may lose due to a timing error if Singular discards
input when it starts up.

If a new buffer is created it is put in Singular interactive mode,
giving commands for sending input and handling ouput of Singular.  See
`singular-interactive-mode'.

Every time `singular' starts a new Singular process it runs the hooks
on `comint-exec-hook' and `singular-exec-hook' \(in that order).

Type \\[describe-mode] in the Singular buffer for a list of commands."
  ;; handle interactive calls
  (interactive (list singular-default-executable
		     singular-default-name
		     singular-default-switches))

  (let* (;; get default values for optional arguments
	 (executable (or executable singular-default-executable))
	 (name (or name singular-default-name))
	 (switches (or switches singular-default-switches))

	 (buffer-name (singular-process-name-to-buffer-name name))
	 ;; buffer associated with Singular, nil if there is none
	 (buffer (get-buffer buffer-name)))

    (if (not buffer)
	(progn
	  ;; create new buffer and call `singular-interactive-mode'
	  (singular-debug 'interactive (message "Creating new buffer"))
	  (setq buffer (get-buffer-create buffer-name))
	  (set-buffer buffer)
	  (singular-debug 'interactive (message "Calling `singular-interactive-mode'"))
	  (singular-interactive-mode)))

    (if (not (comint-check-proc buffer))
	;; create new process if there is none
	(singular-exec buffer name executable
		       (if (file-exists-p singular-start-file)
			   singular-start-file)
		       switches))

    ;; pop to buffer
    (singular-debug 'interactive (message "Calling `pop-to-buffer'"))
    (pop-to-buffer buffer)))

;; for convenience only
(defalias 'Singular 'singular)
;;}}}
;;}}}

(provide 'singular)

;;; singular.el ends here.
