;;; singular.el --- Emacs support for Computer Algebra System Singular

;; $Id: singular.el,v 1.4 1998-07-23 08:38:08 schmidt Exp $

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
;;
;; - use `singular' as prefix for all global symbols
;; - use `singular-debug' as prefix for all global symbols concerning
;;   debugging.
;;
;; - mark dependencies on Emacs flavor/version with a comment of the form
;;   `;; Emacs[ version]' resp.  `;; XEmacs[ version]'
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

  ;; is there any better way to replace in strings??
  (while (string-match "\n" string)
    (setq string (concat (substring string 0 (match-beginning 0))
			 "^J"
			 (substring string (match-end 0)))))

  (if (> (length string) 16)
      (concat "<" (substring string 0 7) ">...<" (substring string -8) ">")
    (concat "<" string ">")))

(defmacro singular-debug (mode form)
  "Major debugging hook for singular.el.
Evaluates FORM if and only if `singular-debug' equals `all' or if MODE
is an element of `singular-debug'."
  `(if (or (eq singular-debug 'all)
	   (memq ,mode singular-debug))
       ,form))
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
more recent version of Emaxs or XEmacs.  singular.el is guaranteed to run
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
;;}}}

;;{{{ Singular interactive mode
;;{{{ Key map
(defvar singular-interactive-mode-map ()
  "Key map to use in Singular interactive mode.")

(if (not singular-interactive-mode-map)
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
		       'singular-interactive-mode-map))))
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

;;{{{ Input and output filters
(defconst singular-bogus-output-filter-calls
  (cond
   ;; XEmacs
   ((eq singular-emacs-flavor 'xemacs) 2)
   ;; Emacs
   (t 1))
  "Number of bogus runs of hooks on `comint-output-filter-functions'.")

;; debugging filters
(defvar singular-debug-bogus-output-filter-cnt 0
  "Number of bogus runs of hooks on `comint-output-filter-functions' yet to do.
This variable is set to `singular-bogus-output-filter-calls' in
`singular-debug-input-filter' and decremented for each bogus run of
`singular-debug-output-filter' until it becomes zero.")

(defun singular-debug-input-filter (string)
  "Echo STRING and reset `singular-debug-bogus-output-filter-cnt'."
  (message "Input filter: %s" (singular-debug-format string))
  (setq singular-debug-bogus-output-filter-cnt
	singular-bogus-output-filter-calls))

(defun singular-debug-output-filter (string)
  "Echo STRING and `singular-debug-bogus-output-filter-cnt'.
Decrement "
  (if (zerop singular-debug-bogus-output-filter-cnt)
      (message "Output filter (real): %s"
	       (singular-debug-format string))
    (message "Output filter (bogus %d): %s"
	     singular-debug-bogus-output-filter-cnt
	     (singular-debug-format string))
    (setq singular-debug-bogus-output-filter-cnt
	  (1- singular-debug-bogus-output-filter-cnt))))
;;}}}

;;{{{ Singular interactive mode

;; Note:
;;
;; In contrast to shell.el, `singular' does not run
;; `singular-interactive-mode' every time a new Singular process is
;; started, but only when a new buffer is created.  This behaviour seems
;; more intuitive w.r.t. local variables and hooks.  So far so good.
;;
;; But there is a slight problem: `make-comint' runs `comint-mode'
;; every time it creates a new process and overwrites a few but
;; important major-mode related variables.  Another consequence of
;; `comint-mode' being run more than once is that buffer-local
;; variables that are not declared to be permanent local are killed by
;; `comint-mode'.  Last not least, there is allmost no difference
;; between the `comint-mode-hook' and the `comint-exec-hook' since
;; both are run every time Singular starts up.
;;
;; The best solution seemed to define an advice to `comint-mode' which
;; inhibits its execution if `singular-interactive-mode' is already
;; up.

(defadvice comint-mode (around singular-interactive-mode activate)
  "Do not run `comint-mode' if `singular-interactive-mode' is already up."
  (if (not (eq major-mode 'singular-interactive-mode))
      ad-do-it))
  
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

  ;; run comint mode and do basic mode setup.  The `let' around
  ;; `comint-mode' ensures that `comint-mode' really will be run.  See
  ;; the above advice.
  (let (major-mode) (comint-mode))
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
  (add-hook 'comint-output-filter-functions
	    'comint-truncate-buffer nil t)

  ;; get name of history file (if any)
  (setq comint-input-ring-file-name (getenv "SINGULARHIST"))
  (if (or (not comint-input-ring-file-name)
	  (equal comint-input-ring-file-name "")
	  (equal (file-truename comint-input-ring-file-name) "/dev/null"))
      (setq comint-input-ring-file-name nil))

  ;; marking of input and output
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
		    (message "Sentinel message: %s" (substring message 0 -1)))
    (if (string-match "finished\\|exited" message)
	(let ((process-buffer (process-buffer process)))
	  (if (and process-buffer
		   (buffer-name process-buffer)
		   (set-buffer process-buffer))
	      (progn
		(singular-debug 'interactive (message "Writing input ring back"))
		(comint-write-input-ring)))))))

(defun singular (&optional singular-executable singular-name singular-switches)
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
	 (singular-executable (or singular-executable
				  singular-default-executable))
	 (singular-name (or singular-name
			    singular-default-name))
	 (singular-switches (or singular-switches
				singular-default-switches))

	 ;; buffer associated with Singular, nil if there is none
	 (buffer-name (singular-process-name-to-buffer-name singular-name))
	 (buffer (get-buffer buffer-name)))

    ;; create new buffer if there has been none and call
    ;; `singular-interactive-mode'
    (if (not buffer)
	(progn
	  (singular-debug 'interactive (message "Creating new buffer"))
	  (setq buffer (get-buffer-create buffer-name))
	  (set-buffer buffer)
	  (singular-debug 'interactive (message "Calling `singular-interactive-mode'"))
	  (singular-interactive-mode)))

    ;; create new process if there has been none
    (if (not (comint-check-proc buffer))
	(progn
	  (singular-debug 'interactive (message "Starting new Singular"))
	  (setq buffer (apply 'make-comint singular-name singular-executable
			      (if (file-exists-p singular-start-file) singular-start-file)
			      singular-switches))
	  (set-process-sentinel (get-buffer-process buffer) 'singular-exit-sentinel)
	  (set-buffer buffer)
	  (singular-debug 'interactive (message "Reading input ring"))
	  (comint-read-input-ring t)
	  (run-hooks 'singular-exec-hook)))

    ;; pop to buffer
    (singular-debug 'interactive (message "Calling `pop-to-buffer'"))
    (pop-to-buffer buffer)))

;; for convenience only
(defalias 'Singular 'singular)
;;}}}
;;}}}

(provide 'singular)

;;; singular.el ends here.
