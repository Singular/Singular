;;; singular.el --- Emacs support for Computer Algebra System Singular

;; $Id: singular.el,v 1.17 1998-08-05 07:07:05 wichmann Exp $

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
;; - documentation on the customization of the modes is in the
;;   doc-strings to `singular-mode-configuration' and
;;   `singular-interactive-mode-configuration', resp.
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
;; - we assume that the buffer is *not* read-only

;;}}}

(require 'comint)

;;{{{ Code common to both modes
;;{{{ Debugging stuff
(defvar singular-debug nil
  "*List of modes to debug or t to debug all modes.
Currently, there are the modes `interactive', `interactive-filter',
`interactive-simple-secs', and `interactive-sections'.")

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
Evaluates FORM if `singular-debug' equals t or if MODE is an element
of `singular-debug', othwerwise ELSE-FORM."
  `(if (or (eq singular-debug t)
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

;; Note:
;;
;; These fonts look quite good:
;; "-adobe-courier-bold-r-*-*-18-*-*-*-*-*-*-*" for input
;; "-adobe-courier-bold-o-*-*-18-*-*-*-*-*-*-*" for output
;;
;; For my (Jens) Emacs a quite good variant is:
;; "-misc-fixed-bold-*-*-*-15-*-*-*-*-*-*-*" for input
;; "-misc-fixed-medium-*-*-*-15-*-*-*-*-*-*-*" for output

(make-face 'singular-input-face)
(set-face-background 'singular-input-face "Orange")
(defvar singular-input-face 'singular-input-face
  "Face for user input.
This face should have set background only.")

(make-face 'singular-output-face)
;(set-face-font 'singular-output-face "-adobe-courier-bold-o-*-*-18-*-*-*-*-*-*-*")
(set-face-background 'singular-output-face "Wheat")
(defvar singular-output-face 'singular-output-face
  "Face for Singular output.
This face should have set background only.")

(defun singular-lookup-face (face-type)
  "Return face belonging to FACE-TYPE.
NOT READY [should be rewritten completely.  Interface should stay the same.]!"
  (cond ((eq face-type 'input) singular-input-face)
	((eq face-type 'output) singular-output-face)))
;;}}}

;;{{{ Font-locking
(defvar singular-font-lock-keywords-1
  '(
    ("\\<\\(poly\\|ideal\\)\\>" 1 font-lock-keyword-face)
    ("\\<\\(ring\\)\\>)" 1 font-lock-variable-face)
;;    ("\\<^   \\? no\\>" font-lock-warn-face)
    )
  "Subdued level for highlighting in singular-(interactive)-mode")

(defvar singular-font-lock-keywords-2 
  (append
   singular-font-lock-keywords-1
   '(
     ("skipping" font-lock-warn-face)
;;     ()))
     ))
  "Gaudy level for highlihgting in singular-(interactive)-mode") 

(defvar singular-font-lock-keywords singular-font-lock-keywords-1
  "Default highlighting for singular-(interactive)-mode")

(defvar singular-emacs-font-lock-defaults 
  '((singular-font-lock-keywords
     singular-font-lock-keywords-1
     singular-font-lock-keywords-2)
    ;; KEYWORDS-ONLY (don't fontify comments/strings when non-nil) 
    nil
    ;; CASE-FOLD (ignore case when non-nil)
    nil
    ;; SYNTAX-ALIST
    ((?_ . "w"))
;;; was soll ich da nehmen NOT READY	(( . )) 
    ;; SYNTAX_BEGIN
    (beginning-of-defun)
;;    beginning-of-line 
    (font-lock-comment-start-regexp . "//")) ;; gibt es nich im XEmacs
  "Emacs-default for font-lock-mode in singular-(interactive)-mode")

(defvar singular-xemacs-font-lock-defaults 
  '((singular-font-lock-keywords
     singular-font-lock-keywords-1
     singular-font-lock-keywords-2)

    ;; KEYWORDS-ONLY (don't fontify comments/strings when non-nil) 
    nil

    ;; CASE-FOLD (ignore case when non-nil)
    nil

    ;; SYNTAX-ALIST
    nil;;; was soll ich da nehmen NOT READY	(( . )) 

    ;; SYNTAX_BEGIN
    nil) ;;    beginning-of-line 
  "XEmacs-default for font-lock-mode in singular-(interactive)-mode")

(cond 
 ;; XEmacs
 ((eq singular-emacs-flavor 'xemacs)
  (singular-debug 'interactive (message "setting up font-lock for XEmacs"))
  (put 'singular-interactive-mode 'font-lock-defaults
       singular-xemacs-font-lock-defaults)))


;;}}}
;;}}}

;;{{{ Singular interactive mode
;;{{{ Key map and menus
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
  (define-key singular-interactive-mode-map "\C-m" 'singular-send-or-copy-input))

;; NOT READY
;; This is just a temporary hack for XEmacs demo.
(defvar singular-interactive-mode-menu nil)
(defvar singular-interactive-mode-menu-2
  (purecopy '("Singular"
	      ["start default" singular-other t]
	      ["start..." singular t]
	      ["exit" singular t])))

(defvar singular-interactive-mode-menu-1
  '("Commands"
    ["load file..." singular-load-file t]
    ("load library"
     ["all.lib" (singular-load-library "all.lib" t) t]
     ["general.lib" (singular-load-library "general.lib" t) t]
     ["matrix.lib" (singular-load-library "matrix.lib" t) t]
     ["sing.lib" (singular-load-library "sing.lib" t) t]
     ["elim.lib" (singular-load-library "elim.lib" t) t]
     ["inout.lib" (singular-load-library "inout.lib" t) t]
     ["random.lib" (singular-load-library "random.lib" t) t]
     ["deform.lib" (singular-load-library "deform.lib" t) t]
     ["homolg.lib" (singular-load-library "homolog.lib" t) t]
     ["poly.lib" (singular-load-library "poly.lib" t) t]
     ["factor.lib" (singular-load-library "factor.lib" t) t]
     ["ring.lib" (singular-load-library "ring.lib" t) t]
     ["primdec.lib" (singular-load-library "primdex.lib" t) t]
     "---"
     ["other..." singular-load-library t])
    "---"
    ["load demo" singular-demo-load (not singular-demo-mode)]
    ["exit demo" singular-demo-exit singular-demo-mode]
    "---"
    ["truncate lines" (setq truncate-lines (not truncate-lines))
       :style toggle :selected truncate-lines]
    ["fold last section" singular t]
    ["fold current section" singular t]
    ["fold all sections" singular t]
    ))

;;(add-menu-button nil ["Singular" (singular) t] "Start Singular")
;;}}}

;;{{{ Syntax table
(defvar singular-interactive-mode-syntax-table nil
  "Syntax table for singular-interactive-mode")
(if singular-interactive-mode-syntax-table
    ()
  (setq singular-interactive-mode-syntax-table (make-syntax-table))
  (modify-syntax-entry ?/ ". 1456" singular-interactive-mode-syntax-table))
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

(defun singular-run-hook-with-arg-and-value (hook value)
  "Call functions on HOOK.
Provides argument VALUE.  If a function returns a non-nil value it
replaces VALUE as new argument to the functions.  Returns final
VALUE."
  (let (result)
    (while hook
      (setq result (funcall (car hook) value))
      (and result (setq value result))
      (setq hook (cdr hook)))
    value))

(defmacro singular-process ()
  "Return process of current buffer."
  (get-buffer-process (current-buffer)))

(defmacro singular-process-mark ()
  "Return process mark of current buffer."
  (process-mark (get-buffer-process (current-buffer))))

(defun singular-load-file (file &optional noexpand)
  "docu NOT READY"
  (interactive "fLoad file: ")
  (let ((filename (if noexpand
		      file
		    (expand-file-name file))))
    (singular-send-string (get-buffer-process (current-buffer))
			  (concat "< \"" filename "\";"))))

(defun singular-load-library (file &optional noexpand)
  "docu NOT READY"
  (interactive "fLoad Library: ")
  (let ((filename (if noexpand
		      file
		    (expand-file-name file))))
    (singular-send-string (get-buffer-process (current-buffer))
			  (concat "LIB \"" filename "\";"))))
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

;;{{{ Skipping and stripping prompts and newlines and other things

;; Note:
;;
;; All of these functions modify the match data!

(defun singular-strip-white-space (string &optional trailing leading)
  "Strip off trailing or leading white-space from STRING.
Strips off trailing white-space if optional argument TRAILING is
non-nil.
Strips off leading white-space if optional argument LEADING is
non-nil."
  (let ((beg 0)
	(end (length string)))
    (and leading
	 (string-match "\\`\\s-*" string)
	 (setq beg (match-end 0)))
    (and trailing
	 (string-match "\\s-*\\'" string beg)
	 (setq end (match-beginning 0)))
    (substring string beg end)))

(defconst singular-extended-prompt-regexp "\\([?>.] \\)"
  "Matches one Singular prompt.
Should not be anchored neither to start nor to end!")

(defconst singular-strip-leading-prompt-regexp
  (concat "\\`" singular-extended-prompt-regexp "+")
  "Matches Singular prompt anchored to string start.")

(defun singular-strip-leading-prompt (string)
  "Strip leading prompts from STRING.
May or may not return STRING or a modified copy of it."
  (if (string-match singular-strip-leading-prompt-regexp string)
      (substring string (match-end 0))
    string))

(defconst singular-remove-prompt-regexp
  (concat "^" singular-extended-prompt-regexp
	  "*" singular-extended-prompt-regexp)
  "Matches a non-empty sequence of prompts at start of a line.")

(defun singular-remove-prompt (beg end)
  "Remove all superfluous prompts from region between BEG and END.
Removes all but the last prompt of a sequence if that sequence ends at
END.
The region between BEG and END should be accessible.
Leaves point after the last prompt found."
  (let ((end (copy-marker end))
	prompt-end)
    (goto-char beg)
    (while (and (setq prompt-end
		      (re-search-forward singular-remove-prompt-regexp end t))
		(not (= end prompt-end)))
      (delete-region (match-beginning 0) prompt-end))

    ;; check for trailing prompt
    (if prompt-end
	(delete-region (match-beginning 0)  (match-beginning 2)))
    (set-marker end nil)))

(defconst singular-skip-prompt-forward-regexp
  (concat singular-extended-prompt-regexp "*")
  "Matches an arbitary sequence of Singular prompts.")

(defun singular-skip-prompt-forward ()
  "Skip forward over prompts."
  (looking-at singular-skip-prompt-forward-regexp)
  (goto-char (match-end 0)))

(defun singular-skip-prompt-backward ()
  "Skip backward over prompts."
  (while (re-search-backward singular-extended-prompt-regexp (- (point) 2) t)))
;;}}}

;;{{{ Simple section stuff for both Emacs and XEmacs

;; Note:
;;
;; Sections and simple sections are used to mark Singular's input and
;; output for further access.  Here are some general notes on simple
;; sections.  Sections are explained in the respective folding.
;;
;; In general, simple sections are more or less Emacs' overlays or XEmacs
;; extents, resp.  But they are more than simply an interface to overlays
;; or sections.
;;
;; - Simple sections are non-empty portions of text.  They are interpreted
;;   as left-closed, right-opened intervals, i.e., the start point of a
;;   simple sections belongs to it whereas the end point does not.
;; - Simple sections start and end at line borders only.
;; - Simple sections do not overlap.  Thus, any point in the buffer may be
;;   covered by at most one simple section.
;; - Besides from their start and their end, simple sections have some type
;;   associated. 
;; - Simple sections are realized using overlays (extents for XEmacs)
;;   which define the start and, end, and type (via properties) of the
;;   simple section.  Actually, as a lisp object a simple section is
;;   nothing else but the underlying overlay.
;; - There may be so-called clear simple sections.  Clear simple sections
;;   have not an underlying overlay.  Instead, they start at the end of the
;;   preceding non-clear simple section, end at the beginning of the next
;;   non-clear simple section, and have the type defined by
;;   `singular-simple-sec-clear-type'.  Clear simple sections are
;;   represented by nil.
;; - Buffer narrowing does not restrict the extent of completely or
;;   partially inaccessible simple sections.  But one should note that
;;   some of the functions assume that there is no narrowing in
;;   effect.
;; - After creation, simple sections are not modified any further.
;;
;; - In `singular-interactive-mode', the whole buffer is covered with
;;   simple sections from the very beginning of the file up to the
;;   beginning of the line containing the last input or output.  The
;;   remaining text up to `(point-max)' may be interpreted as covered by
;;   one clear simple section.  Thus, it is most reasonable to define
;;   `input' to be the type of clear simple sections.

(defvar singular-simple-sec-clear-type 'input
  "Type of clear simple sections.
If nil no clear simple sections are used.")

(defvar singular-simple-sec-last-end nil
  "Marker at the end of the last simple section.
Should be initialized by `singular-simple-sec-init' before any calls to
`singular-simple-sec-create' are done.

This variable is buffer-local.")

(defun singular-simple-sec-init (pos)
  "Initialize global variables belonging to simple section management.
Creates the buffer-local marker `singular-simple-sec-last-end' and
initializes it to POS."
  (make-local-variable 'singular-simple-sec-last-end)
  (if (not (markerp singular-simple-sec-last-end))
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
END should be larger than `singular-simple-sec-last-end'.
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
  "Return start of clear section at position POS.
Assumes that no narrowing is in effect."
  (let ((previous-overlay-change (1+ (point))))
    ;; this `while' loop at last will run into the end of the next
    ;; non-clear overlay or stop at bob.  Since POS may be right at the end
    ;; of a previous non-clear location, we have to search at least one
    ;; time from POS+1 backwards.
    (while (not (or (singular-emacs-simple-sec-before previous-overlay-change)
		    (eq previous-overlay-change (point-min))))
      (setq previous-overlay-change
	    (previous-overlay-change previous-overlay-change)))
    previous-overlay-change))

(defun singular-emacs-simple-sec-end-at (pos)
  "Return end of clear section at position POS.
Assumes that no narrowing is in effect."
  (let ((next-overlay-change (next-overlay-change (point))))
    ;; this `while' loop at last will run into the beginning of the next
    ;; non-clear overlay or stop at eob.  Since POS may not be at the
    ;; beginning of a non-clear simple section we may start searching
    ;; immediately.
    (while (not (or (singular-emacs-simple-sec-at next-overlay-change)
		    (eq next-overlay-change (point-max))))
      (setq next-overlay-change
	    (next-overlay-change next-overlay-change)))
    next-overlay-change))

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
This is the same as `singular-simple-section-at' except if POS falls
on a section border.  In this case `singular-simple-section-before'
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

;;{{{ Simple section stuff for XEmacs
(defun singular-xemacs-simple-sec-create (type end)
  "Create a new simple section of type TYPE.
Creates the section from end of previous simple section up to END.
END should be larger than `singular-simple-sec-last-end'.
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
      (setq simple-sec (singular-xemacs-simple-sec-before last-end))
      (if (eq type (singular-xemacs-simple-sec-type simple-sec))
	  ;; move existing extent
	  (setq simple-sec (set-extent-endpoints simple-sec 
						 (extent-start-position simple-sec) end))
	;; create new extent
	(setq simple-sec (make-extent last-end end))
	;; set type property
	(set-extent-property simple-sec 'singular-type type)
	;; set face
	(set-extent-property simple-sec 'face (singular-lookup-face type)))))
	    
    ;; update end of last simple section
    (set-marker singular-simple-sec-last-end end)
    simple-sec))

(defun singular-xemacs-simple-sec-reset-last (pos)
  "Reset end of last simple section to POS after accidental extension.
Updates `singular-simple-sec-last-end', too."
  (let ((simple-sec (singular-xemacs-simple-sec-at pos)))
    (if simple-sec 
	(set-extent-endpoints simple-sec (extent-start-position simple-sec) pos))
    (set-marker singular-simple-sec-last-end pos)))

(defun singular-xemacs-simple-sec-start (simple-sec)
  "Return start of non-clear simple section SIMPLE-SEC."
  (extent-start-position simple-sec))

(defun singular-xemacs-simple-sec-end (simple-sec)
  "Return end of non-clear simple section SIMPLE-SEC."
  (extent-end-position simple-sec))

(defun singular-xemacs-simple-sec-start-at (pos)
  "Return start of clear section at position POS.
Assumes that no narrowing is in effect."
  (let ((previous-extent-change (1+ (point))))
    ;; this `while' loop at last will run into the end of the next
    ;; non-clear extent or stop at bob.  Since POS may be right at the end
    ;; of a previous non-clear location, we have to search at least one
    ;; time from POS+1 backwards.
    (while (not (or (singular-xemacs-simple-sec-before previous-extent-change)
		    (eq previous-extent-change (point-min))))
      (setq previous-extent-change
	    (previous-extent-change previous-extent-change)))
    previous-extent-change))

(defun singular-xemacs-simple-sec-end-at (pos)
  "Return end of clear section at position POS.
Assumes that no narrowing is in effect."
  (let ((next-extent-change (next-extent-change (point))))
    ;; this `while' loop at last will run into the beginning of the next
    ;; non-clear extent or stop at eob.  Since POS may not be at the
    ;; beginning of a non-clear simple section we may start searching
    ;; immediately.
    (while (not (or (singular-xemacs-simple-sec-at next-extent-change)
		    (eq next-extent-change (point-max))))
      (setq next-extent-change
	    (next-extent-change next-extent-change)))
    next-extent-change))

(defun singular-xemacs-simple-sec-type (simple-sec)
  "Return type of SIMPLE-SEC."
  (if simple-sec
      (extent-property simple-sec 'singular-type)
    singular-simple-sec-clear-type))

(defun singular-xemacs-simple-sec-at (pos)
  "Return simple section at position POS."
  (map-extents (function (lambda (ext args) ext))
	       ;; is this pos-pos-region OK? I think so.
	       (current-buffer) pos pos nil nil 'singular-type))

(defun singular-xemacs-simple-sec-before (pos)
  "Return simple section before position POS.
This is the same as `singular-simple-section-at' except if POS falls
on a section border.  In this case `singular-simple-section-before'
returns the previous simple section instead of the current one."
  (singular-xemacs-simple-sec-at (max 1 (1- pos))))

(defun singular-xemacs-simple-sec-in (beg end)
  "Return a list of all simple sections intersecting with the region from BEG to END.
A simple section intersects the region if the section and the region
have at least one character in common.
The result contains both clear and non-clear simple sections in the
order they appear in the region."
  ;; NOT READY
  nil)
;;}}}

;;{{{ Section stuff

;; Note:
;;
;; Sections are built on simple sections.  Their purpose is to cover the
;; difference between clear and non-clear simple sections.
;;
;; - Sections consist of a simple section, its type, and its start and end
;;   points.  This is redundant information only in the case of non-clear
;;   simple section.
;; - Sections are read-only objects, neither are they modified nor are they
;;   created.
;; - Buffer narrowing does not restrict the extent of completely or
;;   partially inaccessible sections.  In contrast to simple sections
;;   the functions concerning sections do not assume that there is no
;;   narrowing in effect.
;; - Sections are independent from implementation dependencies.  There are
;;   no different versions of the functions for Emacs and XEmacs.

(defun singular-section-at (pos &optional restricted)
  "Return section at position POS.
Returns section intersected with current restriction if RESTRICTED is
non-nil."
  (let* ((simple-sec (singular-simple-sec-at pos))
	 (type (singular-simple-sec-type simple-sec))
	 start end)
    (if simple-sec
	(setq start (singular-simple-sec-start simple-sec)
	      end  (singular-simple-sec-end simple-sec))
      (save-restriction
	(widen)
	(setq start (singular-simple-sec-start-at pos)
	      end (singular-simple-sec-end-at pos))))
    (if restricted
	(vector simple-sec type
		(max start (point-min)) (min end (point-max)))
      (vector simple-sec type start end))))

(defun singular-section-before (pos &optional restricted)
  "Return section before position POS.
This is the same as `singular-section-at' except if POS falls on a
section border.  In this case `singular-section-before' returns the
previous section instead of the current one.
Returns section intersected with current restriction if RESTRICTED is
non-nil."
  (singular-section-at (max 1 (1- pos)) restricted))

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

;;{{{ Getting section contents
(defun singular-input-section-to-string (section &optional end raw)
  "Get content of SECTION as string.
Returns text between start of SECTION and END if optional argument END
is non-nil.  END should be a position inside SECTION.
Strips leading prompts and trailing white space unless optional argument
RAW is non-nil."
  (save-restriction
    (widen)
    (let ((string (if end
		      (buffer-substring (singular-section-start section) end)
		    (buffer-substring (singular-section-start section)
				      (singular-section-end section)))))
      (if raw string
	(singular-strip-leading-prompt
	 (singular-strip-white-space string t))))))
;;}}}

;;{{{ Last input and output section
(defun singular-last-input-section (&optional no-error)
  "Return last input section.
Returns nil if optional argument NO-ERROR is non-nil and there is no
last input section defined, throws an error otherwise."
  (let ((last-input-start (marker-position singular-last-input-section-start))
	(last-input-end (marker-position singular-current-output-section-start)))
    (cond ((and last-input-start last-input-end)
	   (vector (singular-simple-sec-at last-input-start) 'input
		   last-input-start last-input-end))
	  (no-error nil)
	  (t (error "No last input section defined")))))

(defun singular-current-output-section (&optional no-error)
  "Return current output section.
Returns nil if optional argument NO-ERROR is non-nil and there is no
current output section defined, throws an error otherwise."
  (let ((current-output-start (marker-position singular-current-output-section-start))
	(current-output-end (save-excursion
			      (goto-char (singular-process-mark))
			      (singular-skip-prompt-backward)
			      (and (bolp) (point)))))
    (cond ((and current-output-start current-output-end)
	   (vector (singular-simple-sec-at current-output-start) 'output
		   current-output-start current-output-end))
	  (no-error nil)
	  (t (error "No current output section defined")))))

(defun singular-last-output-section (&optional no-error)
  "Return last output section.
Returns nil if optional argument NO-ERROR is non-nil and there is no
last output section defined, throws an error otherwise."
  (let ((last-output-start (marker-position singular-last-output-section-start))
	(last-output-end (marker-position singular-last-input-section-start)))
    (cond ((and last-output-start last-output-end)
	   (vector (singular-simple-sec-at last-output-start) 'output
		   last-output-start last-output-end))
	  (no-error nil)
	  (t (error "No last output section defined")))))

(defun singular-latest-output-section (&optional no-error)
  "Return latest output section.
This is the current output section if it is defined, otherwise the
last output section.
Returns nil if optional argument NO-ERROR is non-nil and there is no
latest output section defined, throws an error otherwise."
  (or (singular-current-output-section t)
      (singular-last-output-section t)
      (if no-error
	  nil
	(error "No latest output section defined"))))
;;}}}

;;{{{ Folding sections
(defvar singular-folding-ellipsis "Singular I/O ..."
  "Ellipsis to show for folded input or output.")

(defun singular-fold-internal (start end fold)
  "(Un)fold region from START to END.
Folds if FOLD is non-nil, otherwise unfolds.
Folding affects undo information and buffer modified flag.
Assumes that there is no narrowing in effect."
  (save-excursion
    (if fold
	(progn
	  (goto-char start) (insert ?\r)
	  (subst-char-in-region start end ?\n ?\r t))
      (subst-char-in-region start end ?\r ?\n t)
      (goto-char start) (delete-char 1))))

(defun singular-section-foldedp (section)
  "Return t iff SECTION is folded.
Assumes that there is no narrowing in effect."
  (eq (char-after (singular-section-start section)) ?\r))

(defun singular-fold-section (section)
  "\(Un)fold SECTION.
\(Un)folds section at point and goes to beginning of section if called
interactively.
Unfolds folded sections and folds unfolded sections."
  (interactive (list (singular-section-at (point))))
  (let ((start (singular-section-start section))
	;; we have to save restrictions this way since we change text
	;; outside the restriction.  Note that we do not use a marker for
	;; `old-point-min'.  This way, even partial narrowed sections are
	;; folded properly if they have been narrowed at bol.  Nice but
	;; dirty trick: The insertion of a `?\r' at beginning of section
	;; advances the beginning of the restriction such that it displays
	;; the `?\r' immediately before bol.  Seems worth it.
	(old-point-min (point-min))
	(old-point-max (point-max-marker)))
    (unwind-protect
	(progn
	  (widen)
	  (singular-fold-internal start (singular-section-end section)
				  (not (singular-section-foldedp section))))
      ;; this is unwide-protected
      (narrow-to-region old-point-min old-point-max)
      (set-marker old-point-max nil))
    (if (interactive-p) (goto-char (max start (point-min))))))
;;}}}

;;{{{ Input and output filters

;; debugging filters
(defun singular-debug-pre-input-filter (string)
  "Display STRING and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Pre-input filter: %s"
			   (singular-debug-format string)))
  (singular-debug 'interactive-filter
		  (message "Pre-input filter: (li %S ci %S lo %S co %S)"
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start)))
  nil)

(defun singular-debug-post-input-filter (beg end)
  "Display BEG, END, and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Post-input filter: (beg %S end %S)" beg end))
  (singular-debug 'interactive-filter
		  (message "Post-input filter: (li %S ci %S lo %S co %S)"
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start))))

(defun singular-debug-pre-output-filter (string)
  "Display STRING and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Pre-output filter: %s"
			   (singular-debug-format string)))
  (singular-debug 'interactive-filter
		  (message "Pre-output filter: (li %S ci %S lo %S co %S)"
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start)))
  nil)

(defun singular-debug-post-output-filter (beg end simple-sec-start)
  "Display BEG, END, SIMPLE-SEC-START, and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Post-output filter: (beg %S end %S sss %S)"
			   beg end simple-sec-start))
  (singular-debug 'interactive-filter
		  (message "Post-output filter: (li %S ci %S lo %S co %S)"
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start))))

;; stripping prompts
(defun singular-remove-prompt-filter (beg end simple-sec-start)
  "Strip prompts from last simple section."
  (if simple-sec-start (singular-remove-prompt simple-sec-start end)))
;;}}}

;;{{{ Demo mode
(defvar singular-demo-chunk-regexp "\\(\n\n\\)"
  "Regular expressions to recognize chunks of a demo file.
If there is a subexpression specified its content is removed when the
chunk is displayed.")

(defvar singular-demo-mode nil
  "Non-nil if Singular demo mode is on.

This variable is buffer-local.")

(defvar singular-demo-old-mode-name nil
  "Used to store previous `mode-name' before switching to demo mode.

This variable is buffer-local.")

(defvar singular-demo-end nil
  "Marker pointing to end of demo file.

This variable is buffer-local.")

(defvar singular-demo-command-on-enter nil
  "Singular command to send when entering demo mode or nil if no string to send.")

(defvar singular-demo-command-on-leave nil
  "Singular command to send when leaving demo mode or nil if no string to send.")
  
(defun singular-demo-mode (mode)
  "Switch between demo mode states.
MODE may be either:
- `init' to initialize global variables;
- `exit' to clean up demo and leave Singular demo mode;
- `enter' to enter Singular demo mode;
- `leave' to leave Singular demo mode.

Modifies the global variables `singular-demo-mode',
`singular-demo-end', and `singular-demo-old-mode-name' to reflect the
new state of Singular demo mode."
  (cond
   ;; initialization.  Should be called only once.
   ((eq mode 'init)
    (make-local-variable 'singular-demo-mode)
    (make-local-variable 'singular-demo-mode-old-name)
    (make-local-variable 'singular-demo-mode-end)
    (if (not (and (boundp 'singular-demo-end)
		  singular-demo-end))
	(setq singular-demo-end (make-marker))))

   ;; final exit.  Clean up demo.
   ((and (eq mode 'exit)
	 singular-demo-mode)
    (setq mode-name singular-demo-old-mode-name
	  singular-demo-mode nil)
    ;; clean up hidden rest of demo file if existent
    (let ((old-point-min (point-min))
	  (old-point-max (point-max)))
      (unwind-protect
	  (progn
	    (widen)
	    (delete-region old-point-max singular-demo-end))
	;; this is unwide-protected
	(narrow-to-region old-point-min old-point-max)))
    (if (and singular-demo-command-on-leave
	     (singular-process))
	(send-string (singular-process) singular-demo-command-on-leave))
    (force-mode-line-update))

   ;; enter demo mode
   ((and (eq mode 'enter)
	 (not singular-demo-mode))
    (setq singular-demo-old-mode-name mode-name
	  mode-name "Singular Demo"
	  singular-demo-mode t)
    (if singular-demo-command-on-enter
	(send-string (singular-process) singular-demo-command-on-enter))
    (force-mode-line-update))

   ;; leave demo mode
   ((and (eq mode 'leave)
	 singular-demo-mode)
    (setq mode-name singular-demo-old-mode-name
	  singular-demo-mode nil)
    (if singular-demo-command-on-leave
	(send-string (singular-process) singular-demo-command-on-leave))
    (force-mode-line-update))))

(defun singular-demo-exit ()
  "Prematurely exit singular demo mode."
  (interactive)
  (singular-demo-mode 'exit))

(defun singular-demo-show-next-chunk ()
  "Show next chunk of demo file at input prompt.
Moves point to end of buffer and widenes the buffer such that the next
chunk of the demo file becomes visible.
Finds and removes chunk separators as specified by
`singular-demo-chunk-regexp'.
Removing chunk separators affects undo information and buffer-modified
flag.
Leaves demo mode after showing last chunk."
  (let ((old-point-min (point-min)))
    (unwind-protect
	(progn
	  (goto-char (point-max))
	  (widen)
	  (if (re-search-forward singular-demo-chunk-regexp singular-demo-end 'limit)
	      (and (match-beginning 1)
		   (delete-region (match-beginning 1) (match-end 1)))
	    ;; remove trailing white-space
	    (skip-syntax-backward "-")
	    (delete-region (point) singular-demo-end)
	    (singular-demo-mode 'leave)))

      ;; this is unwind-protected
      (narrow-to-region old-point-min (point)))))

(defun singular-demo-load (demo-file)
  "Load demo file DEMO-FILE and enter Singular demo mode.
For a description of the Singular demo mode one should refer to the
doc-string of `singular-interactive-mode'.
Moves point to end of buffer and inserts contents of DEMO-FILE there."
  (interactive "fLoad demo file: ")

  ;; check for running demo
  (and singular-demo-mode
       (error "Another demo is already running"))

  (let ((old-point-min (point-min)))
    (unwind-protect
	(progn
	  (goto-char (point-max))
	  (widen)
	  ;; load file and remember its end
	  (set-marker singular-demo-end
		      (+ (point) (nth 1 (insert-file-contents demo-file)))))

      ;; completely hide demo file.
      ;; This is unwide protected.
      (narrow-to-region old-point-min (point)))

    ;; switch demo mode on
    (singular-demo-mode 'enter)))
;;}}}
      
;;{{{ Some lengthy notes on input and output

;; NOT READY[so sorry]!

;;}}}

;;{{{ Sending input
(defvar singular-pre-input-filter-functions nil
  "Functions to call before input is sent to process.
These functions get one argument, a string containing the text which
is to be sent to process.  The functions should return either nil
or a string.  In the latter case the returned string replaces the
string to be sent to process.

This is a buffer-local variable, not a buffer-local hook!

`singular-run-hook-with-arg-and-value' is used to run the functions in
the list.")

(defvar singular-post-input-filter-functions nil
  "Functions to call after input is sent to process.
These functions get two arguments BEG and END.
If `singular-input-filter' has been called with a string as argument
BEG and END gives the position of this string after insertion into the
buffer.
If `singular-input-filter' has been called with a position as argument
BEG and END equal process mark and that position, resp.
The functions may assume that no narrowing is in effect and may change
point at will.

This hook is buffer-local.")

(defvar singular-current-input-section-start nil
  "Marker to the start of the current input section.
This marker points nowhere on startup or if there is no current input
section.

This variable is buffer-local.")

(defvar singular-last-input-section-start nil
  "Marker to the start of the last input section.
This marker points nowhere on startup.

This variable is buffer-local.")

(defun singular-input-filter-init (pos)
  "Initialize all variables concerning input.
POS is the position of the process mark."
  ;; localize variables not yet localized in `singular-interactive-mode'
  (make-local-variable 'singular-current-input-section-start)
  (make-local-variable 'singular-last-input-section-start)

  ;; initialize markers
  (if (not (markerp singular-current-input-section-start))
      (setq singular-current-input-section-start (make-marker)))
  (if (not (markerp singular-last-input-section-start))
      (setq singular-last-input-section-start (make-marker))))

(defun singular-send-string (process string)
  "Send newline terminated STRING to to process PROCESS.
Runs the hooks on `singular-pre-input-filter-functions' in the buffer
associated to PROCESS.  The functions get the non-terminated string."
  (let ((process-buffer (process-buffer process)))

    ;; check whether buffer is still alive
    (if (and process-buffer (buffer-name process-buffer))
	(save-excursion
	  (set-buffer process-buffer)
	  (send-string
	   process
	   (concat (singular-run-hook-with-arg-and-value
		    singular-pre-input-filter-functions string)
		   "\n"))))))

(defun singular-input-filter (process string-or-pos)
  "Insert/update input from user in buffer associated to PROCESS.
Inserts STRING-OR-POS followed by a newline at process mark if it is a
string.
Assumes that the input is already inserted and that it is placed
between process mark and STRING-OR-POS if the latter is a position.
Inserts a newline after STRING-OR-POS.

Takes care off:
- current buffer as well as point and restriction in buffer associated
  with process, even against non-local exits.
Updates:
- process mark;
- current and last sections;
- simple sections;
- mode line.

Runs the hooks on `singular-pre-input-filter-functions' and
`singular-post-input-filter-functions'.

For a more detailed descriptions of the input filter, the markers it
sets, and input filter functions refer to the section \"Some lengthy
notes on input and output\" in singular.el."
  (let ((process-buffer (process-buffer process)))

    ;; check whether buffer is still alive
    (if (and process-buffer (buffer-name process-buffer))
	(let ((old-buffer (current-buffer))
	      (old-pmark (marker-position (process-mark process)))
	      old-point old-point-min old-point-max)
	  (unwind-protect
	      (let (simple-sec-start)
		(set-buffer process-buffer)
		;; the following lines are not protected since the
		;; unwind-forms refer the variables being set here
		(setq old-point (point-marker)
		      old-point-min (point-min-marker)
		      old-point-max (point-max-marker)

		;; get end of last simple section (equals start of
		;; current)
		      simple-sec-start (marker-position singular-simple-sec-last-end))

		;; prepare for insertion
		(widen)
		(set-marker-insertion-type old-point t)
		(set-marker-insertion-type old-point-max t)

		;; insert string at process mark and advance process
		;; mark after insertion.  If it not a string simply
		;; jump to desired position and insrt a newline.
		(if (stringp string-or-pos)
		    (progn
		      (goto-char old-pmark)
		      (insert string-or-pos))
		  (goto-char string-or-pos))
		(insert ?\n)
		(set-marker (process-mark process) (point))

		;; create new simple section and update section markers
		(cond
		 ((eq (singular-simple-sec-create 'input (point)) 'empty)
		  nil)
		 ;; a new simple section has been created ...
		 ((null (marker-position singular-current-input-section-start))
		  ;; ... and even a new input section has been created!
		  (set-marker singular-current-input-section-start
			      simple-sec-start)
		  (set-marker singular-last-output-section-start
			      singular-current-output-section-start)
		  (set-marker singular-current-output-section-start nil)))

		;; run post-output hooks and force mode-line update
		(run-hook-with-args 'singular-post-input-filter-functions
				    old-pmark (point)))

	    ;; restore buffer, restrictions and point
	    (narrow-to-region old-point-min old-point-max)
	    (set-marker old-point-min nil)
	    (set-marker old-point-max nil)
	    (goto-char old-point)
	    (set-marker old-point nil)
	    (set-buffer old-buffer))))))
	    
(defun singular-get-old-input (get-section)
  "Retrieve old input.
Retrivies from beginning of current section to point if GET-SECTION is
non-nil, otherwise on a per-line base."
  (if get-section
      ;; get input from input section
      (let ((section (singular-section-at (point))))
	(if (eq (singular-section-type section) 'input)
	    (setq old-input (singular-input-section-to-string section (point)))
	  (error "Not on an input section")))
    ;; get input from line
    (save-excursion
      (beginning-of-line)
      (singular-skip-prompt-forward)
      (let ((old-point (point)))
	(end-of-line)
	(buffer-substring old-point (point))))))

(defun singular-send-or-copy-input (send-full-section)
  "Send input from current buffer to associated process.
NOT READY[old input copying, demo mode,
	  eol-on-send, history, SEND-FULL-SECTION]!"
  (interactive "P")

  (let ((process (get-buffer-process (current-buffer)))
	pmark)
    ;; some checks and initializations
    (or process (error "Current buffer has no process"))
    (setq pmark (marker-position (process-mark process)))

    (cond
     (;; check for demo mode and show next chunk if necessary
      (and singular-demo-mode
	  (eq (point) pmark)
	  (eq pmark (point-max)))
      (singular-demo-show-next-chunk))

     (;; get old input
      (< (point) pmark)
      (let ((old-input (singular-get-old-input send-full-section)))
	(goto-char pmark)
	(insert old-input)))

     (;; send input from pmark to point after doing history expansion
      t
      ;; go to desired position
      (if comint-eol-on-send (end-of-line))
      (if send-full-section (goto-char (point-max)))

      ;; do history expansion
      (if (eq comint-input-autoexpand 'input)
	  (comint-replace-by-expanded-history t))
      (let* ((input (buffer-substring pmark (point))))

	;; insert input into history
	(if (and (funcall comint-input-filter input)
		 (or (null comint-input-ignoredups)
		     (not (ring-p comint-input-ring))
		     (ring-empty-p comint-input-ring)
		     (not (string-equal (ring-ref comint-input-ring 0) input))))
	    (ring-insert comint-input-ring input))
	(setq comint-input-ring-index nil)

	;; send string to process ...
	(singular-send-string process input)
	;; ... and insert it into buffer ...
	(singular-input-filter process (point)))))))
;;}}}

;;{{{ Receiving output
(defvar singular-pre-output-filter-functions nil
  "Functions to call before output is inserted into the buffer.
These functions get one argument, a string containing the text sent
from process.  The functions should return either nil or a string.
In the latter case the returned string replaces the string sent from
process.

This is a buffer-local variable, not a buffer-local hook!

`singular-run-hook-with-arg-and-value' is used to run the functions in
this list.")

(defvar singular-post-output-filter-functions nil
  "Functions to call after output is inserted into the buffer.
These functions get three arguments BEG, END, and SIMPLE-SEC-START.
The region between BEG and END is what has been inserted into the
buffer.
SIMPLE-SEC-START is the start of the simple section which has been
created on insertion or nil if no simple section has been created.
The functions may assume that no narrowing is in effect and may change
point at will.

This hook is buffer-local.")

(defvar singular-current-output-section-start nil
  "Marker to the start of the current output section.
This marker points nowhere on startup or if there is no current output
section.

This variable is buffer-local.")

(defvar singular-last-output-section-start nil
  "Marker to the start of the last output section.
This marker points nowhere on startup.

This variable is buffer-local.")

(defun singular-output-filter-init (pos)
  "Initialize all variables concerning output including process mark.
Set process mark to POS."

  ;; localize variables not yet localized in `singular-interactive-mode'
  (make-local-variable 'singular-current-output-section-start)
  (make-local-variable 'singular-last-output-section-start)

  ;; initialize markers
  (if (not (markerp singular-current-output-section-start))
      (setq singular-current-output-section-start (make-marker)))
  (if (not (markerp singular-last-output-section-start))
      (setq singular-last-output-section-start (make-marker)))
  (set-marker (singular-process-mark) pos))

(defun singular-output-filter (process string)
  "Insert STRING containing output from PROCESS into its associated buffer.
Takes care off:
- current buffer as well as point and restriction in buffer associated
  with process, even against non-local exits.
Updates:
- process mark;
- current and last sections;
- simple sections;
- mode line.
Runs the hooks on `singular-pre-output-filter-functions' and
`singular-post-output-filter-functions'.

For a more detailed descriptions of the output filter, the markers it
sets, and output filter functions refer to the section \"Some lengthy
notes on input and output\" in singular.el."
  (let ((process-buffer (process-buffer process)))

    ;; check whether buffer is still alive
    (if (and process-buffer (buffer-name process-buffer))
	(let ((old-buffer (current-buffer))
	      (old-pmark (marker-position (process-mark process)))
	      old-point old-point-min old-point-max)
	  (unwind-protect
	      (let (simple-sec-start)
		(set-buffer process-buffer)
		;; the following lines are not protected since the
		;; unwind-forms refer the variables being set here
		(setq old-point (point-marker)
		      old-point-min (point-min-marker)
		      old-point-max (point-max-marker)

		;; get end of last simple section (equals start of
		;; current)
		      simple-sec-start (marker-position singular-simple-sec-last-end)

		;; get string to insert
		      string (singular-run-hook-with-arg-and-value
			      singular-pre-output-filter-functions
			      string))

		;; prepare for insertion
		(widen)
		(set-marker-insertion-type old-point t)
		(set-marker-insertion-type old-point-max t)

		;; insert string at process mark and advance process
		;; mark after insertion
		(goto-char old-pmark)
		(insert string)
		(set-marker (process-mark process) (point))

		;; create new simple section and update section markers
		(cond
		 ((eq (singular-simple-sec-create 'output (point)) 'empty)
		  (setq simple-sec-start nil))
		 ;; a new simple section has been created ...
		 ((null (marker-position singular-current-output-section-start))
		  ;; ... and even a new output section has been created!
		  (set-marker singular-current-output-section-start
			      simple-sec-start)
		  (set-marker singular-last-input-section-start
			      singular-current-input-section-start)
		  (set-marker singular-current-input-section-start nil)))

		;; run post-output hooks and force mode-line update
		(run-hook-with-args 'singular-post-output-filter-functions
				    old-pmark (point) simple-sec-start)
		(force-mode-line-update))

	    ;; restore buffer, restrictions and point
	    (narrow-to-region old-point-min old-point-max)
	    (set-marker old-point-min nil)
	    (set-marker old-point-max nil)
	    (goto-char old-point)
	    (set-marker old-point nil)
	    (set-buffer old-buffer))))))
;;}}}

;;{{{ Singular interactive mode
(defun singular-interactive-mode ()
  "Major mode for interacting with Singular.

NOT READY [how to send input]!

NOT READY [multiple Singulars]!

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
  (set-syntax-table singular-interactive-mode-syntax-table)
  (if singular-interactive-mode-menu
      ()
    (easy-menu-define singular-interactive-mode-menu 
		      singular-interactive-mode-map ""
		      singular-interactive-mode-menu-1))
  (easy-menu-add singular-interactive-mode-menu)
  (setq comment-start "// ")
  (setq comment-start-skip "// *")
  (setq comment-end "")

  ;; customize comint for Singular
  (setq comint-prompt-regexp singular-prompt-regexp)
  (setq comint-delimiter-argument-list singular-delimiter-argument-list)
  (setq comint-input-ignoredups singular-input-ignoredups)
  (make-local-variable 'comint-buffer-maximum-size)
  (setq comint-buffer-maximum-size singular-buffer-maximum-size)
  (setq comint-input-ring-size singular-input-ring-size)
  (setq comint-input-filter singular-history-filter)

  ;; get name of history file (if any)
  (setq comint-input-ring-file-name (getenv "SINGULARHIST"))
  (if (or (not comint-input-ring-file-name)
	  (equal comint-input-ring-file-name "")
	  (equal (file-truename comint-input-ring-file-name) "/dev/null"))
      (setq comint-input-ring-file-name nil))

  ;; initialize singular demo mode, input and output filters
  (singular-demo-mode 'init)
  (make-local-variable 'singular-pre-input-filter-functions)
  (make-local-hook 'singular-post-input-filter-functions)
  (make-local-variable 'singular-pre-output-filter-functions)
  (make-local-hook 'singular-post-output-filter-functions)

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

  ;; debugging filters
  (singular-debug 'interactive-filter
		  (add-hook 'singular-pre-input-filter-functions
			    'singular-debug-pre-input-filter nil t))
  (singular-debug 'interactive-filter
		  (add-hook 'singular-post-input-filter-functions
			    'singular-debug-post-input-filter nil t))
  (singular-debug 'interactive-filter
		  (add-hook 'singular-pre-output-filter-functions
			    'singular-debug-pre-output-filter nil t))
  (singular-debug 'interactive-filter
		  (add-hook 'singular-post-output-filter-functions
			    'singular-debug-post-output-filter nil t))

  ;; other input or output filters
  (add-hook 'singular-post-output-filter-functions
	    'singular-remove-prompt-filter nil t)

  ;; font-locking
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (make-local-variable 'font-lock-defaults)
    (singular-debug 'interactive (message "Setting up font-lock for emacs"))
    (setq font-lock-defaults singular-font-lock-defaults)))

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
    ;; exit demo mode if necessary
    (singular-demo-mode 'exit)
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
  (let ((old-buffer (current-buffer)))
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

	    ;; go to the end of the buffer, initialize I/O and simple
	    ;; sections
	    (goto-char (point-max))
	    (singular-input-filter-init (point))
	    (singular-output-filter-init (point))
	    (singular-simple-sec-init (point))

	    ;; feed process with start file and read input ring.  Take
	    ;; care about the undo information.
	    (if start-file
		(let ((buffer-undo-list t) start-string)
		  (singular-debug 'interactive (message "Feeding start file"))
		  (sleep-for 1)			; try to avoid timing errors
		  (insert-file-contents start-file)
		  (setq start-string (buffer-substring (point) (point-max)))
		  (delete-region (point) (point-max))
		  (send-string process start-string)))
	    (singular-debug 'interactive (message "Reading input ring"))
	    (comint-read-input-ring t)

	    ;; execute hooks
	    (run-hooks 'comint-exec-hook)
	    (run-hooks 'singular-exec-hook))
	  
	  buffer)
      ;; this code is unwide-protected
      (set-buffer old-buffer))))

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
