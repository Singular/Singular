;;; singular.el --- Emacs support for Computer Algebra System Singular

;; $Id: singular.el,v 1.3 1998-07-22 15:09:34 schmidt Exp $

;;; Commentary:


;;; Code:

(require 'comint)

;;{{{ Code Common to Both Modes
;;{{{ Debugging Stuff
(defvar singular-debug nil
  "*List of modes to debug or `all' to debug all modes.
Currently, only the mode `interactive' is supported.")

(defmacro singular-debug (mode form)
  "Major debugging hook for singular.el.
Evaluates FORM if and only if `singular-debug' equals `all' or if MODE
is an element of `singular-debug'."
  `(if (or (eq singular-debug 'all)
	   (memq ,mode singular-debug))
       ,form))
;;}}}
;;}}}

;;{{{ Singular Interactive Mode
;;{{{ Key Map
(defvar singular-interactive-mode-map ()
  "Key map to use in Singular interactive mode.")

(if singular-interactive-mode-map
    ()
  (setq singular-interactive-mode-map
	(nconc (make-sparse-keymap) comint-mode-map)))
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
  
;;{{{ Customization Variables of comint

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

;;{{{ Singular Interactive Mode

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

  (run-hooks 'singular-interactive-mode-hook))
;;}}}

;;{{{ Starting Singular
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
