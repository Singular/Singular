;;; singular.el --- Emacs support for Computer Algebra System Singular

;;; Commentary:


;;; Code:

(require 'comint)

;;{{{ Code Common to Both Modes
(defvar singular-debug nil
  "*List of states to debug.
Currently, only the state `interactive' is supported.

This variable is buffer local.")

(defmacro singular-debug (state form)
  "Major debugging hook for singular.el.
Evaluates FORM if and only if STATE is an element of
`singular-debug'."
  `(if (memq ,state singular-debug) ,form))
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

;;{{{ Customization Variables of comint
(defvar singular-prompt-regexp "^> "
  "Regexp to match prompt patterns in Singular.
Should not match the continuation prompt \(`.'), only the regular
prompt \(`>').")

(defvar singular-delimiter-argument-list '(?= ?\( ?\) ?, ?;)
  "List of characters to recognize as separate arguments.
This variable is used to initialize `comint-delimiter-argument-list'.")

(defvar singular-input-ignoredups t
  "*If non-nil, don't add input matching the last on the input ring.
This variable is used to initialize `comint-input-ignoredups'.")

(defvar singular-buffer-maximum-size 2048
  "*The maximum size in lines for Singular buffers.
This variable is used to initialize `comint-buffer-maximum-size'.

This variable is buffer-local.")

(defvar singular-input-ring-size 64
  "Size of input history ring.
This variable is used to initialize `comint-input-ring-size'.")
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
  
(defun singular-interactive-mode ()
  "Major mode for interacting with Singular.

\\[comint-send-input] after the end of Singular's output sends the text from the
    end of process to the end of the current line.

\\[comint-send-input] before end of Singular's output copies the current line minus
    the prompt to the end of the buffer and sends it
    \(\\[comint-copy-old-input] just copies the current line).

NOT READY [multiple Singulars]!

Singular buffers are automatically limited in length \(by default,
to 2048 lines).  This limit may be adjusted by setting
`singular-buffer-maximum-size'.

NOT READY [much more to come.  See shell.el.]!"
  (interactive)
  (comint-mode)

  ;; miscellaneous
  (setq major-mode 'singular-interactive-mode)
  (setq mode-name "Singular")
  (use-local-map singular-interactive-mode-map)
  (make-local-variable 'singular-debug)

  ;; customize comint for Singular
  (setq comint-prompt-regexp singular-prompt-regexp)
  (setq comint-delimiter-argument-list singular-delimiter-argument-list)
  (setq comint-input-ignoredups singular-input-ignoredups)
  (make-local-variable 'comint-buffer-maximum-size)
  (make-local-variable 'singular-buffer-maximum-size)
  (setq comint-buffer-maximum-size singular-buffer-maximum-size)
  (setq comint-input-ring-size singular-input-ring-size)

  ;; get name of history file (if any)
  (setq comint-input-ring-file-name (getenv "SINGULARHIST"))
  (if (or (not comint-input-ring-file-name)
	  (equal comint-input-ring-file-name "")
	  (equal (file-truename comint-input-ring-file-name) "/dev/null"))
      (setq comint-input-ring-file-name nil))

  (run-hooks 'singular-interactive-mode-hook)
  (comint-read-input-ring t))
;;}}}

;;{{{ Starting Singular
(defvar singular-start-file "~/.emacs_singularrc"
  "Name of start-up file to pass to Singular.
If the file named by this variable exists it is given as
initial input to any Singular process being started \(Note that
this may lose due to a timing error if Singular discards input
when it starts up.)")

(defvar singular-default-executable "Singular"
  "Default name of Singular executable.")

(defvar singular-default-name "singular"
  "Default process name for Singular process.")

(defvar singular-default-switches '("-t")
  "List of switches to pass to Singular processes on startup.")

(defun singular-exit-sentinel (process message)
 "Clean up after termination of Singular.
Writes back input ring after regular termination of Singular
if process buffer is still alive."
  (save-excursion
    (singular-debug 'interactive
		    (message "Sentinel: %s" (substring message 0 -1)))
    (if (string-match "finished\\|exited" message)
	(let ((process-buffer (process-buffer process)))
	  (and process-buffer
	       (buffer-name process-buffer)
	       (set-buffer process-buffer)
	       (comint-write-input-ring))))))

(defun singular (&optional singular-executable singular-name singular-switches)
  "Run an inferior Singular process, with I/O through an Emacs buffer.

NOT READY [arguments and interactive use]!

If buffer exists but Singular is not running, start new Singular.
If buffer exists and Singular is running, just switch to buffer.
If a file `~/.emacs_singularrc' exists, it is given as initial
input \(Note that this may lose due to a timing error if Singular
discards input when it starts up.)
The buffer is put in Singular interactive mode, giving commands
for sending input and handling ouput of Singular.  See
`singular-interactive-mode'.

\(Type \\[describe-mode] in the Singular buffer for a list of commands.)"
  (interactive (list singular-default-executable
		     singular-default-name
		     singular-default-switches))
  ;; make buffer name canonical
  (let* (;; get default values for optional arguments
	 (singular-executable (or singular-executable
				  singular-default-executable))
	 (singular-name (or singular-name
			    singular-default-name))
	 (singular-switches (or singular-switches
				singular-default-switches))

	 ;; buffer associated with Singular
	 (buffer (singular-process-name-to-buffer-name singular-name)))
    (or (comint-check-proc buffer)
	(save-excursion
	  (singular-debug 'interactive (message "Starting new Singular"))
	  (setq buffer (apply 'make-comint singular-name singular-executable
			      (if (file-exists-p singular-start-file) singular-start-file)
			      singular-switches))
	  (set-process-sentinel (get-buffer-process buffer) 'singular-exit-sentinel)
	  (set-buffer buffer)
	  (singular-debug 'interactive (message "Calling `singular-interactive-mode'"))
	  (singular-interactive-mode)))
    (singular-debug 'interactive (message "Calling `pop-to-buffer'"))
    (pop-to-buffer buffer)))

;; for convenience only
(defalias 'Singular 'singular)
;;}}}

(provide 'singular)

;;; singular.el ends here.
