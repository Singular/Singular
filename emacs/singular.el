;;; singular.el --- Emacs support for Computer Algebra System Singular


;;; Commentary:


;;; Code:

;;{{{ Style and coding conventions

;; Style and coding conventions:
;;
;; - "Singular" is written with an upper-case `S' in comments, doc
;;   strings, and messages.  As part of symbols, it is written with
;;   a lower-case `s'.
;; - When referring to the Singular interactive mode, do it in that
;;   wording.  Use the notation `singular-interactive-mode' only when
;;   really referring to the lisp object.
;; - use a `fill-column' of 75 for doc strings and comments
;; - mark incomplete doc strings or code with `NOT READY' optionally
;;   followed by an explanation what exactly is missing
;;
;; - use foldings to structure the source code but try not to exceed a
;;   maximum depth of two foldings
;; - use lowercase folding titles except for first word
;; - folding-marks are `;;{{{' and `;;}}}' resp., for sake of standard
;;   conformity
;; - use the foldings to modularize code.  That is, each folding should be,
;;   as far as possible, self-content.  Define a function `singular-*-init'
;;   in the folding to do the initialization of the module contained in
;;   that folding.  Call that function from `singular-interactive-mode',
;;   for example, instead of initializing the module directly from
;;   `singular-interactive-mode'.  Look at the code how it is done for the
;;   simple section or for the folding stuff.
;;
;; - use `singular' as prefix for all global symbols
;; - use `singular-debug' as prefix for all global symbols concerning
;;   debugging.
;; - use, whenever possible without names becoming too clumsy, some unique
;;   prefix inside a folding
;;
;; - mark dependencies on Emacs flavor/version with a comment of the form
;;   `;; Emacs[ <version> ]'     resp.
;;   `;; XEmacs[ <version> ][ <nasty comment> ]'
;;   specified in that order, if possible
;; - use a `cond' statement to execute Emacs flavor/version-dependent code,
;;   not `if'.  This is to make such checks more extensible.
;; - try to define different functions for different flavors/version and
;;   use `singular-fset' at library-loading time to set the function you
;;   really need.  If the function is named `singular-<basename>', the
;;   flavor/version-dependent functions should be named
;;   `singular-<flavor>[-<version>]-<basename>'.
;;
;; - use `singular-debug' for debugging output/actions
;; - to switch between buffer and process names, use the functions
;;   `singular-process-name-to-buffer-name' and
;;   `singular-buffer-name-to-process-name'
;; - call the function `singular-keep-region-active' as last statement in
;;   an interactive function that should keep the region active (for
;;   example, in functions that move the point).  This is necessary to keep
;;   XEmacs' zmacs regions active.
;; - to get the process of the current buffer, use `singular-process'.  To
;;   get the current process mark, use `singular-process-mark'.  Both
;;   functions check whether Singular is alive and throw an error if not,
;;   so you do not have to care about that yourself.  If you do not want an
;;   error specify non-nil argument NO-ERROR.  But use them anyway.
;; - we assume that the buffer is *not* read-only
;; - use `=' instead of `eq' when comparing buffer locations.  Even if you
;;   are sure that both operands are integers.

;;}}}

;;{{{ Code common to both modes
;;{{{ Customizing
(defgroup singular nil
  "Emacs interface to Singular.
By now, the Emacs interface to Singular consists of Singular interactive
mode only.  Singular interactive mode provides a convenient front end to
interactive Singular sessions running inside Emacs.
In far future maybe there will be a mode for editing Singular source code
such as libraries or procedures."
  :group 'external)

(defgroup singular-faces nil
  "Faces in Singular mode and Singular interactive mode."
  :group 'faces
  :group 'singular-interactive)
;;}}}

;;{{{ Debugging stuff
(defvar singular-debug nil
  "List of modes to debug or t to debug all modes.
Currently, the following modes are supported:
  `interactive',
  `interactive-filter'.")

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
of `singular-debug', otherwise ELSE-FORM."
  `(if (or (eq singular-debug t)
	   (memq ,mode singular-debug))
       ,form
     ,else-form))
;;}}}

;;{{{ Determining version
(defvar singular-emacs-flavor nil
  "A symbol describing the current Emacs.
Currently, only Emacs \(`emacs') and XEmacs \(`xemacs') are supported.")

(defvar singular-emacs-major-version nil
  "An integer describing the major version of the current emacs.")

(defvar singular-emacs-minor-version nil
  "An integer describing the minor version of the current emacs.")

(defun singular-fset (real-function emacs-function xemacs-function)
  "Set REAL-FUNCTION to one of the functions, in dependency on Emacs flavor and version.
Sets REAL-FUNCTION to XEMACS-FUNCTION if `singular-emacs-flavor' is
`xemacs', otherwise sets REAL-FUNCTION to EMACS-FUNCTION.

This is not as common as it would be desirable.  But it is sufficient so
far."
  (cond
   ;; XEmacs
   ((eq singular-emacs-flavor 'xemacs)
    (fset real-function xemacs-function))
   ;; Emacs
   (t
    (fset real-function emacs-function))))

(defun singular-set-version ()
  "Determine flavor, major version, and minor version of current emacs.
singular.el is guaranteed to run on Emacs 20.3 and XEmacs 20.3.
It should run on newer version and on slightly older ones, too.

This function is called exactly once when singular.el is loaded."
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
on Emacs 20.3 and XEmacs 20.3."))
    ;; assume the oldest version we support
    (setq singular-emacs-major-version 20
	  singular-emacs-minor-version 3))

  ;; get flavor
  (if (string-match "XEmacs\\|Lucid" emacs-version)
      (setq singular-emacs-flavor 'xemacs)
    (setq singular-emacs-flavor 'emacs)))

(singular-set-version)
;;}}}

;;{{{ Syntax table
(defvar singular-mode-syntax-table nil
  "Syntax table for `singular-interactive-mode' resp. `singular-mode'.")

(if singular-mode-syntax-table
    ()
  (setq singular-mode-syntax-table (make-syntax-table))
  ;; stolen from cc-mode.el except for back-tics which are special to Singular
  (modify-syntax-entry ?_  "_"     	singular-mode-syntax-table)
  (modify-syntax-entry ?\\ "\\"    	singular-mode-syntax-table)
  (modify-syntax-entry ?+  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?-  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?=  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?%  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?<  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?>  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?&  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?|  "."     	singular-mode-syntax-table)
  (modify-syntax-entry ?\' "\""    	singular-mode-syntax-table)
  (modify-syntax-entry ?\` "\""    	singular-mode-syntax-table)
  ;; block and line-oriented comments
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (modify-syntax-entry ?/  ". 124b" 	singular-mode-syntax-table)
    (modify-syntax-entry ?*  ". 23"   	singular-mode-syntax-table))
   ;; XEmacs
   (t
    (modify-syntax-entry ?/  ". 1456" 	singular-mode-syntax-table)
    (modify-syntax-entry ?*  ". 23"   	singular-mode-syntax-table)))
  (modify-syntax-entry ?\n "> b"    	singular-mode-syntax-table)
  (modify-syntax-entry ?\^m "> b"    	singular-mode-syntax-table))

(defun singular-mode-syntax-table-init ()
  "Initialize syntax table of current buffer.

This function is called at mode initialization time."
  (set-syntax-table singular-mode-syntax-table))
;;}}}

;;{{{ Miscellaneous
(defsubst singular-keep-region-active ()
  "Do whatever is necessary to keep the region active in XEmacs.
Ignore byte-compiler warnings you might see.  This is not needed for
Emacs."
  ;; XEmacs.  We do not use the standard way here to test for flavor
  ;; because it is presumably faster with that test on `boundp'.
  (and (boundp 'zmacs-region-stays)
       (setq zmacs-region-stays t)))
;;}}}
;;}}}

;;{{{ Singular interactive mode
;;{{{ Customizing

;; Note:
;;
;; Some notes on Customize:
;;
;; - The documentation states that for the `:initialize' option of
;;   `defcustom' the default value is `custom-initialize-set'.  However, in
;;   the source code of Customize `custom-initialize-reset' is used.  So
;;   better always specify the `:initialize' option explicitly.
;; - Customize is bad at setting buffer-local variables or properties.
;;   This is quite natural since Customize itself uses its own buffer.  So
;;   changing buffer-local variables and properties with Customize is
;;   possible only at a "Singular-global" level.  That is, for all buffers
;;   currently having Singular interactive mode as major mode.  The function
;;   `singular-map-buffer' helps to do such customization.
;; - Important note: Customizable variables are not automatically marked as
;;   user options.  This has to be done as usual by marking them with a '*'
;;   as first character of the documentation string.  Without that, the
;;   variables are not accessible to, for example, `set-variable'.
;;
;; Some common customizing patterns:
;;
;; - How to customize buffer-local properties?
;;   First, the `defcustom' itself must not set anything buffer-local since
;;   at time of its definition (most likely) no Singular buffers will be
;;   around.  If there are Singular buffers we do not care about them.  But
;;   anyhow, at definition of the `defcustom' the global default has to be
;;   set.  Hence, the `:initialize' option should be set to
;;   `custom-initialize-default'.
;;   The buffer-local initialization has to be done at mode initialization
;;   time.  The global default value should then be used to set the local
;;   properties.
;;   At last, the function specified with the `:set' option should set the
;;   local properties in all Singular buffers to the new, customized value.
;;   Most likely, the function `singular-map-buffer' may be used for that.
;;   In addition, the function should, of course, set the global value via
;;   `set-default'.
;;   For an example, see `singular-folding-line-move-ignore-folding'.
;;
;; - How to encapsulate other mode's global variables into Singular
;;   interactive mode variables?
;;   Set them always.  That is, set them if the `defcustom' is evaluated
;;   (use `custom-initialize-reset' as `:initial' function) and set them
;;   when the Singular interactive mode variable is customized (by means
;;   of an appropriate `:set' function).
;;   For an example, see `singular-section-face-alist' (which does not
;;   encapsulate another mode's variable, but Singular interactive mode's
;;   own variable `singular-simple-sec-clear-type').

(defgroup singular-interactive nil
  "Running interactive Singular sessions inside Emacs."
  :group 'singular
  :group 'processes)

(defgroup singular-sections-and-foldings nil
  "Sections and foldings in Singular interactive mode."
  :group 'singular-interactive)

(defgroup singular-interactive-miscellaneous nil
  "Miscellaneous settings for Singular interactive mode."
  :group 'singular-interactive)

(defgroup singular-demo-mode nil
  "Settings concerning Singular demo mode."
  :group 'singular-interactive)

(defun singular-map-buffer (func &rest args)
  "Apply FUNC to ARGS in all existing Singular buffers.
That is, in all buffers having Singular interactive major mode.  The
function is executed in the context of the buffer.  This is a must-have for
the customizing stuff to change buffer-local properties."
  (save-excursion
    (mapcar (function
	     (lambda (buffer)
	       (set-buffer buffer)
	       (if (eq major-mode 'singular-interactive-mode)
		   (apply func args))))
	    (buffer-list))))
;;}}}

;;{{{ Comint

;; Note:
;;
;; We require Comint, but we really do not use it too much.  One may argue
;; that this is bad since Comint is a standardized way to communicate with
;; external processes.  One may argue further that many experienced Emacs
;; users are forced now to re-do their Comint customization for Singular
;; interactive mode.  However, we believe that the intersection between
;; experienced Emacs users and users of Singular interactive mode is almost
;; empty.
;;
;; In fact, we used Comint really much in the beginning of this project.
;; Later during development it turned at that using Comint's input and
;; output processing is to inflexible and not appropriate for Singular
;; interactive mode with its input and output sections.  So we begun to
;; rewrite large portions of Comint to adapt it to our needs.  At some
;; point it came clear that it would be best to throw out Comint
;; all together, would not have been there some auxiliary functions which
;; are really useful but annoying to rewrite.  These are, for example, the
;; command line history functions or the completion stuff offered by
;; Comint.
;;
;; Our policy with regard to these remainders of Comint is: Use the
;; functions to bind them to keys, but do not use them internally.
;; Encapsulate Comint customization into Singular interactive mode
;; customization.  In particular, do not take care about Comint settings
;; which already may be present, overwrite them.  Hide Comint from the
;; user.
;;
;; Here is how exactly we use Comint:
;;
;; - All variables necessary to use Comint's input ring are properly
;;   initialized.  One may find this in the `History' folding.
;; - `comint-prompt-regexp' is initialized since it is used in some
;;   of the functions regarding input ring handling.  Furthermore, its
;;   initialization enables us to use functions as `comint-bol', etc.
;;   Initialization is done in the `Skipping and stripping prompts ...'
;;   folding.
;; - We call `comint-mode' as first step in `singular-interactive-mode'.
;;   Most of the work done there is to initialize the local variables as
;;   necessary.  Besides that, the function does nothing that interferes
;;   with Singular interactive mode.  To be consequent we set
;;   `comint-mode-hook' temporarily to nil when calling `comint-mode'.
;; - In `singular-exec', we use `comint-exec-1' to fire up the process.
;;   Furthermore, we set `comint-ptyp' there as it is used in the signal
;;   sending commands of Comint.  All that `comint-exec-1' does is that it
;;   sets up the process environment (it adds or modifies the setting of
;;   the 'TERM' variable), sets the execution directory, and does some
;;   magic with the process coding stuff.
;; - One more time the most important point: we do *not* use Comint's
;;   output and input processing.  In particular, we do not run any of
;;   Comint's hooks on input or output.  Anyway, we do better, don't we?

(require 'comint)

(defun singular-comint-init ()
  "Initialize comint stuff for Singular interactive mode.

This function is called at mode initialization time."
  (setq comint-completion-addsuffix '("/" . "")))
;;}}}

;;{{{ Font-locking
(defvar singular-font-lock-error-face 'singular-font-lock-error-face
  "Face name to use for Singular errors.")

(defvar singular-font-lock-warning-face 'singular-font-lock-warning-face
  "Face name to use for Singular warnings.")

(defvar singular-font-lock-prompt-face 'singular-font-lock-prompt-face
  "Face name to use for Singular prompts.")

(defface singular-font-lock-error-face
  '((((class color)) (:foreground "Red" :bold t))
    (t (:inverse-video t :bold t)))
  "*Font Lock mode face used to highlight Singular errors."
  :group 'singular-faces)

(defface singular-font-lock-warning-face
  '((((class color)) (:foreground "OrangeRed" :bold nil))
    (t (:inverse-video t :bold t)))
  "*Font Lock mode face used to highlight Singular warnings."
  :group 'singular-faces)

(defface singular-font-lock-prompt-face
  '((((class color) (background light)) (:foreground "Blue" :bold t))
    (((class color) (background dark)) (:foreground "LightSkyBlue" :bold t))
    (t (:inverse-video t :bold t)))
  "*Font Lock mode face used to highlight Singular prompts."
  :group 'singular-faces)

(defconst singular-font-lock-singular-types nil
  "List of Singular types.")

(eval-when-compile
  (setq singular-font-lock-singular-types
	'("def" "bigint" "ideal" "int" "intmat" "intvec" "link" "list" "map"
	  "matrix" "module" "number" "poly" "proc" "qring" "resolution" "ring"
	  "string" "vector")))

(defconst singular-interactive-font-lock-keywords-1
  '(
    ("^\\([>.]\\) " 1 singular-font-lock-prompt-face t)
    ("^   [\\?].*" 0 singular-font-lock-error-face t)
    ("^// \\*\\*.*" 0 singular-font-lock-warning-face t)
    )
  "Subdued level highlighting for Singular interactive mode")

(defconst singular-interactive-font-lock-keywords-2
  (append
   singular-interactive-font-lock-keywords-1
   (eval-when-compile
     (list
      (cons
       (concat "\\<" (regexp-opt singular-font-lock-singular-types t) "\\>")
       'font-lock-type-face))))
  "Medium level highlighting for Singular interactive mode")

(defconst singular-interactive-font-lock-keywords-3
  (append
   singular-interactive-font-lock-keywords-2
   '(
     ;; note: we use font-lock-reference-face here even Emacs says that
     ;; this face is obsolete and suggests to use font-lock-constant-face,
     ;; since XEmacs20/21 does not know the constant-face but the
     ;; reference-face.
     ("^   [\\?].*`\\(\\sw\\sw+;?\\)`" 1 font-lock-reference-face t)
     ))
  "Gaudy level highlighting for Singular interactive mode.")

(defconst singular-interactive-font-lock-keywords singular-interactive-font-lock-keywords-1
  "Default highlighting for Singular interactive mode.")

(defconst singular-interactive-font-lock-defaults
  '((singular-interactive-font-lock-keywords
     singular-interactive-font-lock-keywords-1
     singular-interactive-font-lock-keywords-2
     singular-interactive-font-lock-keywords-3)
    ;; KEYWORDS-ONLY (do not fontify strings & comments if non-nil)
    nil
    ;; CASE-FOLD (ignore case if non-nil)
    nil
    ;; SYNTAX-ALIST (add this to Font Lock's syntax table)
    ((?_ . "w"))
    ;; SYNTAX-BEGIN
    singular-section-goto-beginning)
  "Default expressions to highlight in Singular interactive mode.")

(defun singular-interactive-font-lock-init ()
  "Initialize Font Lock mode for Singular interactive mode.

For XEmacs, this function is called exactly once when singular.el is
loaded.
For Emacs, this function is called  at mode initialization time."
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (singular-debug 'interactive (message "Setting up Font Lock mode for Emacs"))
    (set (make-local-variable 'font-lock-defaults)
	 singular-interactive-font-lock-defaults))
   ;; XEmacs
   ((eq singular-emacs-flavor 'xemacs)
    (singular-debug 'interactive (message "Setting up Font Lock mode for XEmacs"))
    (put 'singular-interactive-mode
	 'font-lock-defaults singular-interactive-font-lock-defaults))))

;; XEmacs Font Lock mode initialization
(cond
 ;; XEmacs
 ((eq singular-emacs-flavor 'xemacs)
  (singular-interactive-font-lock-init)))
;;}}}

;;{{{ Key map
(defvar singular-interactive-mode-map nil
  "Key map to use in Singular interactive mode.")

(if singular-interactive-mode-map
    ()
  ;; create empty keymap first
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (setq singular-interactive-mode-map (make-sparse-keymap)))
   ;; XEmacs
   (t
    (setq singular-interactive-mode-map (make-keymap))
    (set-keymap-name singular-interactive-mode-map
		     'singular-interactive-mode-map)))

  ;; global settings
  (define-key help-map [?\C-s]                                'singular-help)

  ;; settings for `singular-interactive-map'
  (substitute-key-definition 'beginning-of-line 'singular-beginning-of-line
			     singular-interactive-mode-map global-map)

  (define-key singular-interactive-mode-map "\t"              'singular-dynamic-complete)
  (define-key singular-interactive-mode-map [?\C-m]	      'singular-send-or-copy-input)
  (define-key singular-interactive-mode-map [?\C-l]           'singular-recenter)

  ;; Comint functions
  (define-key singular-interactive-mode-map [?\M-r]	      'comint-previous-matching-input)
  (define-key singular-interactive-mode-map [?\M-s]	      'comint-next-matching-input)

  ;; C-c prefix
  (define-key singular-interactive-mode-map [?\C-c ?\C-e]     'singular-example)
  (define-key singular-interactive-mode-map [?\C-c ?\C-t]     'singular-toggle-truncate-lines)

  (define-key singular-interactive-mode-map [?\C-c ?\C-f]     'singular-folding-toggle-fold-at-point-or-all)
  (define-key singular-interactive-mode-map [?\C-c ?\C-o]     'singular-folding-toggle-fold-latest-output)
  (define-key singular-interactive-mode-map [?\C-c ?\C-w]     'singular-section-kill)

  (define-key singular-interactive-mode-map [?\C-c ?\C-d]     'singular-demo-load)
  (define-key singular-interactive-mode-map [?\C-c ?\C-l]     'singular-load-library)
  (define-key singular-interactive-mode-map [(control c) (<)] 'singular-load-file)

  (define-key singular-interactive-mode-map [?\C-c ?\C-r]     'singular-restart)
  (define-key singular-interactive-mode-map [?\C-c ?\$] 'singular-exit-singular)
  (define-key singular-interactive-mode-map [?\C-c ?\C-c] 'singular-control-c))

(defun singular-cursor-key-model-set (key-model)
  "Set keys according to KEY-MODEL.
KEY-MODEL should be one of the valid values of `singular-cursor-key-model'."
  ;; convert symbols to list
  (cond ((eq key-model 'emacs)
	 (setq key-model '(cursor cursor history)))
	((eq key-model 'terminal)
	 (setq key-model '(history history cursor))))

  ;; work through list
  (mapcar (function (lambda (spec)
		      (let ((key-description (nth 0 spec))
			    (prev-key (nth 1 spec))
			    (next-key (nth 2 spec)))
			(cond ((eq key-description 'cursor)
			       (define-key singular-interactive-mode-map prev-key 'previous-line)
			       (define-key singular-interactive-mode-map next-key 'next-line))
			      ((eq key-description 'history)
			       (define-key singular-interactive-mode-map prev-key 'comint-previous-input)
			       (define-key singular-interactive-mode-map next-key 'comint-next-input))
			      (t
			       (define-key singular-interactive-mode-map prev-key nil)
			       (define-key singular-interactive-mode-map next-key nil))))))

	  ;; here is where list position are mapped to keys
	  (list (list (nth 0 key-model) [up] [down])
		(list (nth 1 key-model) [?\C-p] [?\C-n])
		(list (nth 2 key-model) [?\M-p] [?\M-n]))))

(defcustom singular-cursor-key-model 'emacs
  "*Keys to use for cursor movement and history access, respectively.
An experienced Emacs user would prefer setting `singular-cursor-key-model'
to `emacs'.  This means that C-p, C-n, and the cursor keys move the cursor,
whereas M-p and M-n scroll through the history of Singular commands.

On the other hand, an user used to running Singular in a, say, xterm, would
prefer setting `singular-cursor-key-model' to `terminal'.  This means that
C-p, C-n, and the cursor keys scroll through the history of Singular
commands, whereas M-p and M-n move the cursor.

For those who do not like neither standard setting, there is the
possibility to set this variable to a list of three elements where
- the first element specifies the key bindings for the cursor keys,
- the second element specifies the key bindings for C-p and C-n, and
- the third element specifies the key bindings for M-p and M-n.
Each list element should be one of
- `cursor', meaning that the corresponding keys are bound to cursor movement,
- `history', meaning that the corresponding keys are bound to history access,
  or
- nil, meaning that the corresponding keys are not bound at all.

Changing this variable has an immediate effect only if one uses
\\[customize] to do so."
  :type '(choice (const :tag "Emacs-like" emacs)
		 (const :tag "Terminal-like" terminal)
		 (list :tag "User-defined"
		      (choice :format "Cursor keys: %[Value Menu%] %v"
			      :value cursor
			      (const :tag "Cursor movement" cursor)
			      (const :tag "History access" history)
			      (const :tag "No binding" nil))
		      (choice :format "C-p, C-n:    %[Value Menu%] %v"
			      :value cursor
			      (const :tag "Cursor movement" cursor)
			      (const :tag "History access" history)
			      (const :tag "No binding" nil))
		      (choice :format "M-p, M-n:    %[Value Menu%] %v"
			      :value history
			      (const :tag "Cursor movement" cursor)
			      (const :tag "History access" history)
			      (const :tag "No binding" nil))))
  :initialize 'custom-initialize-reset
  :set (function
	(lambda (var value)
	  (singular-cursor-key-model-set value)
	  (set-default var value)))
  :group 'singular-interactive-miscellaneous)

(defun singular-interactive-mode-map-init ()
  "Initialize key map for Singular interactive mode.

This function is called  at mode initialization time."
  (use-local-map singular-interactive-mode-map))
;;}}}

;;{{{ Menus and logos
(defvar singular-interactive-mode-menu-1 nil
  "NOT READY [docu]")

(defvar singular-interactive-mode-menu-2 nil
  "NOT READY [docu]")

(defconst singular-menu-initial-library-menu
  '(["other..." (singular-load-library t) t])
  "Menu definition for the initial library sub menu.
This should be a list of vectors.")

(defun singular-menu-build-libraries-menu (definition)
  "Given a description of the libraries and their categories, builds up a
menu definition including submenus which can be given to
`easy-menu-change'.  By side effect sets the variable
`singular-standard-libraries-alist' to the alist of all library names.
This alist can be used for completion."
  (let ((menudef ())
        (libs definition)
        elem)
    (while libs
      (setq elem (car libs))
      (if (> (length elem) 1)
          (setq menudef
                (append
		 (list
		  (append (list (car elem))
			  (singular-menu-build-libraries-menu (cdr elem))))
		 menudef))
        (setq menudef
	      (append (list (vector (car elem)
				    (list 'singular-load-library nil
					  (car elem))
                                    t))
		      menudef))
        (setq singular-standard-libraries-alist
	      (append (list elem) singular-standard-libraries-alist)))
      (setq libs (cdr libs)))
    menudef))

(defun singular-menu-install-libraries ()
  "Update the singular command menu with libraries.
Scans the variable `singular-standard-libraries-with-categories' and builds
up a menu with submenues for each category in the submenu (\"Commands\"
\"Libraries\")."
  (singular-debug 'interactive (message "Installing library menu"))
  ;; To be compatible with older versions of singular.el (resp. of lib-cmpl.el)
  ;; we check whether the variable
  ;; `singular-standard-libraries-with-categories' is set.  If not, we use the
  ;; value of `singular-standard-libraries-alist' instead.
  (if (not singular-standard-libraries-with-categories)
      (setq singular-standard-libraries-with-categories
            singular-standard-libraries-alist))
  (easy-menu-change '("Commands")
		    "Libraries"
		    (append
		     (singular-menu-build-libraries-menu
		      singular-standard-libraries-with-categories)
		     (append '("---") singular-menu-initial-library-menu))))

(defun singular-menu-init ()
  "Initialize menu stuff for Singular interactive mode.

This function is called by `singular-exec'."
  (singular-debug 'interactive (message "Initializing menu stuff"))
  (make-local-variable 'singular-standard-libraries-alist)
  (make-local-variable 'singular-standard-libraries-with-categories))

(defun singular-menu-deinstall-libraries ()
  "Initialize library submenu from singular command menu.
Sets the submenu (\"Commands\" \"Libraries\") to the value of
`singular-menu-initial-library-menu'."
  (singular-debug 'interactive
		  (message "Removing libraries from menu"))
  (easy-menu-change '("Commands") "Libraries" singular-menu-initial-library-menu))

;; For some reasons emacs inserts new menus in the oppsite order.
;; Defining menu-2 prior to menu-1 will result in the following menu:
;;   Singular   Commands
;; That's what we want. So DO NOT exchange both (or ..) statements!
(or singular-interactive-mode-menu-2
    (easy-menu-define
     singular-interactive-mode-menu-2
     singular-interactive-mode-map ""
     (list
      "Commands"
      ["Fold/Unfold Latest Output" singular-folding-toggle-fold-latest-output t]
      ["Fold/Unfold At Point" singular-folding-toggle-fold-at-point-or-all t]
      ["Fold All Output" singular-folding-fold-all-output t]
      ["Unfold All Output" singular-folding-unfold-all-output t]
      "---"
      ["Truncate Lines" singular-toggle-truncate-lines
       :style toggle :selected truncate-lines]
      "--"
      (append
       '("Libraries")
       singular-menu-initial-library-menu)
      ["Load File..." singular-load-file t]
      "---"
      ["Load Demo..." singular-demo-load (or singular-demo-exit-on-load
					     (not singular-demo-mode))]
      ["Exit Demo" singular-demo-exit singular-demo-mode]
      )))

(or singular-interactive-mode-menu-1
    (easy-menu-define singular-interactive-mode-menu-1
		      singular-interactive-mode-map ""
		      '("Singular"
			["Start Default" singular t]
			["Start..." singular-other t]
			["Restart" singular-restart t]
			"---"
			["Interrupt" singular-control-c t]
			["Exit" singular-exit-singular t]
			"---"
			["Preferences" (customize-group 'singular-interactive) t]
			["Singular Example" singular-example t]
			["Singular Help" singular-help t])))

(defun customize-singular-interactive ()
  (interactive)
  (customize-group 'singular-interactive))

(defun singular-interactive-mode-menu-init ()
  "Initialize menus for Singular interactive mode.

This function is called  at mode initialization time."
  ;; Remove any potential menu which comint-mode might has added.
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    ;; Note that easy-menu-remove is a nop in emacs.
    (define-key comint-mode-map [menu-bar signals] nil)
    (define-key comint-mode-map [menu-bar inout] nil)
    (define-key comint-mode-map [menu-bar completion] nil))
   ;;Xemacs
   (t
    (easy-menu-remove '("Singular"))
    (easy-menu-remove '("Comint1"))	; XEmacs 20
    (easy-menu-remove '("Comint2"))	; XEmacs 20
    (easy-menu-remove '("History"))	; XEmacs 20
    (easy-menu-remove '("Complete"))	; XEmacs 21
    (easy-menu-remove '("In/Out"))	; XEmacs 21
    (easy-menu-remove '("Signals"))))	; XEmacs 21

  ;; Note: easy-menu-add is not necessary in emacs, since the menu
  ;; is added automatically with the keymap.
  ;; See help on `easy-menu-add'
  (easy-menu-add singular-interactive-mode-menu-1)
  (easy-menu-add singular-interactive-mode-menu-2))
;;}}}

;;{{{ Skipping and stripping prompts and whitespace and other things

;; Note:
;;
;; Most of these functions handle prompt recognition, prompt skipping,
;; prompt stripping, and so on.  It turned out that it would be very
;; inefficient to use one generic regular expression to do so.  Hence, we
;; decided to hardcode the prompt skipping and stripping in an API.  If one
;; decides to use some other prompt the whole API has to be changed.
;; Hopefully, the Singular prompt does not change in near future ...
;;
;; In addition to the API, the Comint mode variable `comint-mode-regexp' is
;; set on initialization of Singular interactive mode.  Singular
;; interactive mode seems to do quite well without that, but for safety the
;; variable is set nonetheless.

(defsubst singular-prompt-skip-forward ()
  "Skip forward over prompts."
  (if (looking-at "\\([>.] \\)+")
      (goto-char (match-end 0))))

(defsubst singular-prompt-skip-backward ()
  "Skip backward over prompts."
  ;; is that really the simplest and fastest method?  The problem is that
  ;; `re-search-backward' is not greedy so on an regexp as "\\([>.] \\)+"
  ;; it stops right after the first occurrence of the sub-expression.
  ;; Anyway, the `(- (point) 2)' expression is OK, even at bob.
  (while (re-search-backward "[>.] " (- (point) 2) t)))

(defun singular-prompt-remove-string (string)
  "Remove all prompts from STRING."
  (while (string-match "^\\([>.] \\)+" string)
    (setq string (replace-match "" t t string)))
  string)

(defun singular-prompt-remove-region (beg end)
  "Remove all superfluous prompts from region between BEG and END.
Removes only sequences of prompts that start at beginning of line.  Removes
all but the last prompt of a sequence if that sequence ends at END,
otherwise removes all prompts.
The region between BEG and END should be accessible.  BEG should be less
than or equal to END.
Leaves point at the position of the last sequence of prompts which has been
deleted or at BEG if nothing has been deleted."
  ;; we cannot exclude this case, I think
  (if (/= beg end)
      ;; that's a nice trick to keep the last prompt if it ends at END: we
      ;; set `(1- END)' as search limit.  Since BEG /= END there can be no
      ;; problems with the `1-'.
      (let ((end (copy-marker (1- end))))
	(goto-char beg)
	(while (re-search-forward "^\\([>.] \\)+" end t)
	  (delete-region (match-beginning 0) (match-end 0)))
	(set-marker end nil))))

(defun singular-prompt-remove-filter (beg end simple-sec-start)
  "Remove all superfluous prompts from text inserted into buffer."
  (cond (;; if a new simple section has been created remove all
	 ;; prompts from that simple section
	 simple-sec-start
	 (singular-prompt-remove-region simple-sec-start end))
	(;; if no simple section has been created check whether maybe the
	 ;; region between beg and end consists of prompts only.  This in
	 ;; case that the user issued a command that did not output any
	 ;; text.
	 (and (goto-char beg)
	      (re-search-forward "\\([>.] \\)+" end t)
	      (= (match-end 0) end))
	 (singular-prompt-remove-region (progn (beginning-of-line) (point))
					end))))

(defun singular-white-space-strip (string &optional trailing leading)
  "Strip off trailing or leading whitespace from STRING.
Strips off trailing whitespace if optional argument TRAILING is non-nil.
Strips off leading whitespace if optional argument LEADING is non-nil."
  (let (beg end)
    (and leading
	 (string-match "\\`[ \t\n\r\f]+" string)
	 (setq beg (match-end 0)))
    (and trailing
	 (string-match "[ \t\n\r\f]+\\'" string)
	 (setq end (match-beginning 0)))
    (if (or beg end)
	(substring string (or beg 0) (or end (length string)))
      string)))

(defconst singular-comint-prompt-regexp "^\\([>.] \\)+"
  "Regexp to match prompt patterns in Singular.
This variable is used to initialize `comint-prompt-regexp' when Singular
interactive mode starts up.  It is not used in Singular interactive mode
itself!  One should refer to the source code for more information on how to
adapt Singular interactive mode to some other prompt.")

(defun singular-prompt-init ()
  "Initialize prompt skipping and stripping for Singular interactive mode.

This function is called at mode initialization time."
  ;; remove superfluous prompts in singular output
  (add-hook 'singular-post-output-filter-functions 'singular-prompt-remove-filter nil t)

  ;; some relict from Comint mode
  (setq comint-prompt-regexp singular-comint-prompt-regexp))
  ;; required to use prompt-regexp
  (setq comint-use-prompt-regexp t)
;;}}}

;;{{{ Miscellaneous

;; Note:
;;
;; We assume a one-to-one correspondence between Singular buffers and
;; Singular processes.  We always have (equal buffer-name (concat "*"
;; process-name "*")).

(defsubst singular-buffer-name-to-process-name (buffer-name)
  "Create the process name for BUFFER-NAME.
The process name is the buffer name with surrounding `*' stripped off."
  (substring buffer-name 1 -1))

(defsubst singular-process-name-to-buffer-name (process-name)
  "Create the buffer name for PROCESS-NAME.
The buffer name is the process name with surrounding `*'."
  (concat "*" process-name "*"))

(defsubst singular-run-hook-with-arg-and-value (hook value)
  "Call functions on HOOK.
Provides argument VALUE to the functions.  If a function returns a non-nil
value it replaces VALUE as new argument to the remaining functions.
Returns final VALUE."
  (while hook
    (setq value (or (funcall (car hook) value) value)
	  hook (cdr hook)))
  value)

(defsubst singular-process (&optional no-error)
  "Return process of current buffer.
If no process is active this function silently returns nil if optional
argument NO-ERROR is non-nil, otherwise it throws an error."
  (cond ((get-buffer-process (current-buffer)))
	(no-error nil)
	(t (error "No Singular running in this buffer"))))

(defsubst singular-process-mark (&optional no-error)
  "Return process mark of current buffer.
If no process is active this function silently returns nil if optional
argument NO-ERROR is non-nil, otherwise it throws an error."
  (let ((process (singular-process no-error)))
    (and process
	 (process-mark process))))

(defun singular-time-stamp-difference (new-time-stamp old-time-stamp)
  "Return the number of seconds between NEW-TIME-STAMP and OLD-TIME-STAMP.
Both NEW-TIME-STAMP and OLD-TIME-STAMP should be in the format
that is returned, for example, by `current-time'.
Does not return a difference larger than 2^17 seconds."
  (let ((high-difference (min 1 (- (car new-time-stamp) (car old-time-stamp))))
	(low-difference (- (cadr new-time-stamp) (cadr old-time-stamp))))
    (+ (* high-difference 131072) low-difference)))

(defun singular-error (&rest message-args)
  "Apply `message' on MESSAGE-ARGS and do a `ding'.
This function should be used instead of `error' in hooks where calling
`error' is not a good idea."
  (apply 'message message-args)
  (ding))

(defun singular-pop-to-buffer (same-window &rest pop-to-buffer-args)
  "Pop to buffer in same or other window.
Pops to buffer in same window if SAME-WINDOW equals t.  Pops to buffer in
other window if SAME-WINDOW equals nil.  If SAME-WINDOW equals neither t
nor nil the default behaviour of `pop-to-buffer' is used.  The rest of the
arguments is passed unchanged to `pop-to-buffer'."
  (let ((same-window-buffer-names
	 (cond
	  ((null same-window)
	   nil)
	  ((eq same-window t)
	   (let* ((buffer-or-name (car pop-to-buffer-args))
		  (buffer-name (if (bufferp buffer-or-name)
				   (buffer-name buffer-or-name)
				 buffer-or-name)))
	     (list buffer-name)))
	  (t
	   same-window-buffer-names))))
    (apply 'pop-to-buffer pop-to-buffer-args)))
;;}}}

;;{{{ Miscellaneous interactive
(defun singular-recenter (&optional arg)
  "Center point in window and redisplay frame.  With ARG, put point on line ARG.
The desired position of point is always relative to the current window.
Just C-u as prefix means put point in the center of the window.
If ARG is omitted or nil, erases the entire frame and then redraws with
point in the center of the current window.
Scrolls window to the left margin and moves point to beginning of line."
  (interactive "P")
  (singular-reposition-point-and-window)
  (recenter arg))

(defun singular-reposition-point-and-window ()
  "Scroll window to the left margin and move point to beginning of line."
  (interactive)
  (set-window-hscroll (selected-window) 0)
  (move-to-column 0)
  ;; be careful where to place point
  (singular-prompt-skip-forward))

(defun singular-toggle-truncate-lines ()
  "Toggle `truncate-lines'.
A non-nil value of `truncate-lines' means do not display continuation
lines\; give each line of text one screen line.
Repositions window and point after toggling `truncate-lines'."
  (interactive)
  (setq truncate-lines (not truncate-lines))
  ;; reposition so that user does not get confused
  (singular-reposition-point-and-window)
  ;; avoid calling `recenter' since it changes window layout more than
  ;; necessary
  (redraw-frame (selected-frame)))

;; this is not a buffer-local variable even if at first glance it seems
;; that it should be one.  But if one changes buffer the contents of this
;; variable becomes irrelevant since the last command is no longer a
;; horizontal scroll command.  The same is true for the initial value, so
;; we set it to nil.
(defvar singular-scroll-previous-amount nil
  "Amount of previous horizontal scroll command.")

(defun singular-scroll-right (&optional scroll-amount)
  "Scroll selected window SCROLL-AMOUNT columns right.
SCROLL-AMOUNT defaults to amount of previous horizontal scroll command.  If
the command immediately preceding this command has not been a horizontal
scroll command SCROLL-AMOUNT defaults to window width minus 2.
Moves point to leftmost visible column."
  (interactive "P")

  ;; get amount to scroll
  (setq singular-scroll-previous-amount
	(cond (scroll-amount (prefix-numeric-value scroll-amount))
	      ((eq last-command 'singular-scroll-horizontal)
	       singular-scroll-previous-amount)
	      (t (- (frame-width) 2)))
	this-command 'singular-scroll-horizontal)

  ;; scroll
  (scroll-right singular-scroll-previous-amount)
  (move-to-column (window-hscroll))
  ;; be careful where to place point.  But what if `(current-column)'
  ;; equals, say, one?  Well, we simply do not care about that case.
  ;; Should not happen to often.
  (if (eq (current-column) 0)
      (singular-prompt-skip-forward)))

(defun singular-scroll-left (&optional scroll-amount)
  "Scroll selected window SCROLL-AMOUNT columns left.
SCROLL-AMOUNT defaults to amount of previous horizontal scroll command.  If
the command immediately preceding this command has not been a horizontal
scroll command SCROLL-AMOUNT defaults to window width minus 2.
Moves point to leftmost visible column."
  (interactive "P")

  ;; get amount to scroll
  (setq singular-scroll-previous-amount
	(cond (scroll-amount (prefix-numeric-value scroll-amount))
	      ((eq last-command 'singular-scroll-horizontal)
	       singular-scroll-previous-amount)
	      (t (- (frame-width) 2)))
	this-command 'singular-scroll-horizontal)

  ;; scroll
  (scroll-left singular-scroll-previous-amount)
  (move-to-column (window-hscroll))
  ;; be careful where to place point.  But what if `(current-column)'
  ;; equals, say, one?  Well, we simply do not care about that case.
  ;; Should not happen to often.
  (if (eq (current-column) 0)
      (singular-prompt-skip-forward)))

(defun singular-beginning-of-line  (arg)
  "Move point to the beginning of line, then skip past prompt, if any.
If prefix argument is given the prompt is not skipped."
  (interactive "P")
  (beginning-of-line)
  (if (not arg) (singular-prompt-skip-forward)))

(defun singular-load-file (file &optional noexpand)
  "Read a file into Singular (via '< \"FILE\";').
If optional argument NOEXPAND is non-nil, FILE is left as it is entered by
the user, otherwise it is expanded using `expand-file-name'."
  (interactive "fLoad file: ")
  (let* ((filename (if noexpand file (expand-file-name file)))
	 (string (concat "< \"" filename "\";"))
	 (process (singular-process)))
    (singular-input-filter process string)
    (singular-send-string process string)))

(defvar singular-load-library-history nil
  "History list for loading of Singular libraries.
Is used by `singular-load-library'.")

(defun singular-load-library (nonstdlib &optional file)
  "Read a Singular library (via 'LIB \"FILE\";').
If called interactively asks for the name of a standard Singular
library. If called interactively with a prefix argument asks for a file
name of a Singular library."
  (interactive "P")
  (let ((string (or file
		    (if nonstdlib
			(read-file-name "Library file: ")
		      (completing-read "Library: " singular-standard-libraries-alist
				       nil nil nil 'singular-load-library-history))))
	(process (singular-process)))
    (setq string (concat "LIB \"" string "\";"))
    (singular-input-filter process string)
    (singular-send-string process string)))
;;}}}

;;{{{ History
(defcustom singular-history-ignoredups t
  "*If non-nil, do not add input matching the last on the input history."
  :type 'boolean
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

;; this variable is used to set Comint's `comint-input-ring-size'
(defcustom singular-history-size 64
  "*Size of the input history.

Changing this variable has no immediate effect even if one uses
\\[customize] to do so.  The new value will be used only in new Singular
interactive mode buffers."
  :type 'integer
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defcustom singular-history-filter-regexp "\\`\\(..?\\|\\s *\\)\\'"
  "*Regular expression to filter strings *not* to insert in the input history.
By default, input consisting of less than three characters and input
consisting of white-space only is not inserted into the input history."
  :type 'regexp
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defcustom singular-history-explicit-file-name nil
  "*If non-nil, use this as file name to load and save the input history.
If this variable equals nil, the `SINGULARHIST' environment variable is
used to determine the file name.
One should note that the input history is saved to file only on regular
termination of Singular; that is, if one leaves Singular using the commands
`quit\;' or `exit\;'."
  :type '(choice (const nil) file)
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defun singular-history-read ()
  "Read the input history from file.
If `singular-history-explicit-file-name' is non-nil, uses that as file
name, otherwise tries environment variable `SINGULARHIST'.
This function is called from `singular-exec' every time a new Singular
process is started."
  (singular-debug 'interactive (message "Reading input ring"))
  (let ((comint-input-ring-file-name (or singular-history-explicit-file-name
					 (getenv "SINGULARHIST"))))
    ;; `comint-read-input-ring' does nothing if
    ;; `comint-input-ring-file-name' equals nil
    (comint-read-input-ring t)))

(defun singular-history-write ()
  "Write back the input history to file.
If `singular-history-explicit-file-name' is non-nil, uses that as file
name, otherwise tries environment variable `SINGULARHIST'.
This function is called either by `singular-exit-singular' or by
`singular-exit-sentinel' every time a Singular process terminates
regularly."
  (singular-debug 'interactive (message "Writing input ring back"))
  (let ((comint-input-ring-file-name (or singular-history-explicit-file-name
					 (getenv "SINGULARHIST"))))
    ;; `comint-write-input-ring' does nothing if
    ;; `comint-input-ring-file-name' equals nil
    (comint-write-input-ring)))

(defun singular-history-insert (input)
  "Insert string INPUT into the input history if necessary."
  (if (and (not (string-match singular-history-filter-regexp input))
	   (or (not singular-history-ignoredups)
	       (not (ring-p comint-input-ring))
	       (ring-empty-p comint-input-ring)
	       (not (string-equal (ring-ref comint-input-ring 0) input))))
      (ring-insert comint-input-ring input))
  (setq comint-input-ring-index nil))

(defun singular-history-init ()
  "Initialize variables concerning the input history.

This function is called at mode initialization time."
  (setq comint-input-ring-size singular-history-size))
;;}}}

;;{{{ Simple section API for both Emacs and XEmacs

;; Note:
;;
;; Sections and simple sections are used to mark Singular's input and
;; output for further access.  Here are some general notes on simple
;; sections.  Sections are explained in the respective folding.
;;
;; In general, simple sections are more or less Emacs' overlays or XEmacs
;; extents, resp.  But they are more than simply an interface to overlays
;; or extents.
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
;;   do not have an underlying overlay.  Instead, they start at the end of
;;   the preceding non-clear simple section, end at the beginning of the
;;   next non-clear simple section, and have the type defined by
;;   `singular-simple-sec-clear-type'.  Clear simple sections are
;;   represented by nil.
;; - Buffer narrowing does not restrict the extent of completely or
;;   partially inaccessible simple sections.  But one should note that
;;   some of the functions assume that there is no narrowing in
;;   effect.
;; - After creation, simple sections are not modified any further.
;; - There is one nasty little corner case: what if a non-clear simple
;;   section spans up to end of buffer?  By definition, eob is not included
;;   in that section since they are right-opened intervals.  Most of the
;;   functions react as if there is an imaginary empty clear simple section
;;   at eob.
;; - Even though by now there are only two types of different simple
;;   sections there may be an arbitrary number of them.  Furthermore,
;;   simple sections of different types may appear in arbitrary order.
;;
;; - In `singular-interactive-mode', the whole buffer is covered with
;;   simple sections from the very beginning of the file up to the
;;   beginning of the line containing the last input or output.  The
;;   remaining text up to `(point-max)' may be interpreted as covered by
;;   one clear simple section.  Thus, it is most reasonable to define
;;   `input' to be the type of clear simple sections.

(defvar singular-simple-sec-clear-type 'input
  "Type of clear simple sections.
If nil no clear simple sections are used.

One should not set this variable directly.  Rather, one should customize
`singular-section-face-alist'.")

(defvar singular-simple-sec-last-end nil
  "Marker at the end of the last simple section.
Should be initialized by `singular-simple-sec-init' before any calls to
`singular-simple-sec-create' are done.  Instead of accessing this variable
directly one should use the macro `singular-simple-sec-last-end-position'.

This variable is buffer-local.")

(defun singular-simple-sec-init (pos)
  "Initialize variables belonging to simple section management.
Creates the buffer-local marker `singular-simple-sec-last-end' and
initializes it to POS.  POS should be at beginning of a line.

This function is called every time a new Singular session is started."
  (make-local-variable 'singular-simple-sec-last-end)
  (if (not (markerp singular-simple-sec-last-end))
      (setq singular-simple-sec-last-end (make-marker)))
  (set-marker singular-simple-sec-last-end pos))

(defmacro singular-simple-sec-last-end-position ()
  "Return the marker position of `singular-simple-sec-last-end'.
This macro exists more or less for purposes of information hiding only."
  '(marker-position singular-simple-sec-last-end))

(defsubst singular-simple-sec-lookup-face (type)
  "Return the face to use for simple sections of type TYPE.
This accesses the `singular-section-type-alist'.  It does not harm if nil
is associated with TYPE in that alist: In this case, this function will
never be called for that TYPE."
  (cdr (assq type singular-section-face-alist)))

;; Note:
;;
;; The rest of the folding is either marked as
;; Emacs
;; or
;; XEmacs

(singular-fset 'singular-simple-sec-create
	       'singular-emacs-simple-sec-create
	       'singular-xemacs-simple-sec-create)

(singular-fset 'singular-simple-sec-at
	       'singular-emacs-simple-sec-at
	       'singular-xemacs-simple-sec-at)

(singular-fset 'singular-simple-sec-start
	       'singular-emacs-simple-sec-start
	       'singular-xemacs-simple-sec-start)

(singular-fset 'singular-simple-sec-end
	       'singular-emacs-simple-sec-end
	       'singular-xemacs-simple-sec-end)

(singular-fset 'singular-simple-sec-type
	       'singular-emacs-simple-sec-type
	       'singular-xemacs-simple-sec-type)

(singular-fset 'singular-simple-sec-before
	       'singular-emacs-simple-sec-before
	       'singular-xemacs-simple-sec-before)

(singular-fset 'singular-simple-sec-start-at
	       'singular-emacs-simple-sec-start-at
	       'singular-xemacs-simple-sec-start-at)

(singular-fset 'singular-simple-sec-end-at
	       'singular-emacs-simple-sec-end-at
	       'singular-xemacs-simple-sec-end-at)

(singular-fset 'singular-simple-sec-in
	       'singular-emacs-simple-sec-in
	       'singular-xemacs-simple-sec-in)
;;}}}

;;{{{ Simple section API for Emacs
(defsubst singular-emacs-simple-sec-start (simple-sec)
  "Return start of non-clear simple section SIMPLE-SEC.
Narrowing has no effect on this function."
  (overlay-start simple-sec))

(defsubst singular-emacs-simple-sec-end (simple-sec)
  "Return end of non-clear simple section SIMPLE-SEC.
Narrowing has no effect on this function."
  (overlay-end simple-sec))

(defsubst singular-emacs-simple-sec-type (simple-sec)
  "Return type of SIMPLE-SEC.
Returns nil if SIMPLE-SEC happens to be an overlay but not a simple
section.
Narrowing has no effect on this function."
  (if simple-sec
      (overlay-get simple-sec 'singular-type)
    singular-simple-sec-clear-type))

(defsubst singular-emacs-simple-sec-before (pos)
  "Return simple section before buffer position POS.
This is the same as `singular-simple-sec-at' except if POS falls on a
section border.  In this case `singular-simple-section-before' returns the
previous simple section instead of the current one.  If POS falls on
beginning of buffer, the simple section at beginning of buffer is returned.
Narrowing has no effect on this function."
  (singular-emacs-simple-sec-at (max 1 (1- pos))))

(defun singular-emacs-simple-sec-create (type end)
  "Create a new simple section of type TYPE.
Creates the section from end of previous simple section up to the first
beginning of line before END.  That position should be larger than or equal
to `singular-simple-sec-last-end'.  Updates `singular-simple-sec-last-end'.
Returns the new simple section or `empty' if no simple section has been
created.
Assumes that no narrowing is in effect."
  (let ((last-end (singular-simple-sec-last-end-position))
	;; `simple-sec' is the new simple section or `empty'
	simple-sec)

    ;; get beginning of line before END.  At this point we need that there
    ;; are no restrictions.
    (setq end (let ((old-point (point)))
		(goto-char end) (beginning-of-line)
		(prog1 (point) (goto-char old-point))))

    (cond
     ;; do not create empty sections
     ((eq end last-end)
      'empty)
     ;; non-clear simple sections
     ((not (eq type singular-simple-sec-clear-type))
      ;; if type has not changed we only have to extend the previous simple
      ;; section.  If `last-end' happens to be 1 (meaning that we are
      ;; creating the first non-clear simple section in the buffer), then
      ;; `singular-simple-sec-before' returns nil,
      ;; `singular-simple-sec-type' returns the type of clear simple
      ;; sections that definitely does not equal TYPE, and a new simple
      ;; section is created as necessary.
      (setq simple-sec (singular-emacs-simple-sec-before last-end))
      (if (eq type (singular-emacs-simple-sec-type simple-sec))
	  ;; move existing overlay
	  (setq simple-sec (move-overlay simple-sec (overlay-start simple-sec) end))
	;; create new overlay
	(setq simple-sec (make-overlay last-end end))
	;; set type property
	(overlay-put simple-sec 'singular-type type)
	;; set face
	(overlay-put simple-sec 'face (singular-simple-sec-lookup-face type))
	;; evaporate empty sections
	(overlay-put simple-sec 'evaporate t))
      ;; update `singular-simple-sec-last-end' and return new simple
      ;; section
      (set-marker singular-simple-sec-last-end end)
      simple-sec)
     ;; clear simple sections
     (t
      ;; update `singular-simple-sec-last-end' and return nil
      (set-marker singular-simple-sec-last-end end)
      nil))))

(defun singular-emacs-simple-sec-start-at (pos)
  "Return start of clear simple section at position POS.
Assumes the existence of an imaginary empty clear simple section if POS is
at end of buffer and there is non-clear simple section immediately ending
at POS.
Assumes that no narrowing is in effect (since `previous-overlay-change'
implicitly does so)."
  ;; yes, this `(1+ pos)' is OK at eob for
  ;; `singular-emacs-simple-sec-before' as well as
  ;; `previous-overlay-change'
  (let ((previous-overlay-change-pos (1+ pos)))
    ;; this `while' loop at last will run into the end of the next
    ;; non-clear simple section or stop at bob.  Since POS may be right at
    ;; the end of a previous non-clear location, we have to search at least
    ;; one time from POS+1 backwards.
    (while (not (or (singular-emacs-simple-sec-before previous-overlay-change-pos)
		    (eq previous-overlay-change-pos 1)))
      (setq previous-overlay-change-pos
	    (previous-overlay-change previous-overlay-change-pos)))
    previous-overlay-change-pos))

(defun singular-emacs-simple-sec-end-at (pos)
  "Return end of clear simple section at position POS.
Assumes the existence of an imaginary empty clear simple section if POS is
at end of buffer and there is non-clear simple section immediately ending
at POS.
Assumes that no narrowing is in effect (since `next-overlay-change'
implicitly does so)."
  (let ((next-overlay-change-pos (next-overlay-change pos)))
    ;; this `while' loop at last will run into the beginning of the next
    ;; non-clear simple section or stop at eob.  Since POS may not be at
    ;; the beginning of a non-clear simple section we may start searching
    ;; immediately.
    (while (not (or (singular-emacs-simple-sec-at next-overlay-change-pos)
		    (eq next-overlay-change-pos (point-max))))
      (setq next-overlay-change-pos
	    (next-overlay-change next-overlay-change-pos)))
    next-overlay-change-pos))

(defun singular-emacs-simple-sec-at (pos)
  "Return simple section at buffer position POS.
Assumes the existence of an imaginary empty clear simple section if POS is
at end of buffer and there is non-clear simple section immediately ending
at POS.
Narrowing has no effect on this function."
  ;; at eob, `overlays-at' always returns nil so everything is OK for this
  ;; case, too
  (let ((overlays (overlays-at pos)) simple-sec)
    ;; be careful, there may be other overlays!
    (while (and overlays (not simple-sec))
      (if (singular-emacs-simple-sec-type (car overlays))
	  (setq simple-sec (car overlays)))
      (setq overlays (cdr overlays)))
    simple-sec))

(defun singular-emacs-simple-sec-in (beg end)
  "Return a list of all simple sections intersecting with the region from BEG to END.
A simple section intersects the region if the section and the region have
at least one character in common.  The sections are returned with
startpoints in increasing order and clear simple sections (that is, nil's)
inserted as necessary.  BEG is assumed to be less than or equal to END.
The imaginary empty clear simple section at end of buffer is never included
in the result.
Narrowing has no effect on this function."
  (let (overlays overlay-cursor)
    (if (= beg end)
	;; `overlays-in' seems not be correct with respect to this case
	nil
      ;; go to END since chances are good that the overlays come in correct
      ;; order, then
      (setq overlays (let ((old-point (point)))
		       (goto-char end)
		       (prog1 (overlays-in beg end)
			 (goto-char old-point)))

      ;; now, turn overlays that are not simple sections into nils
	    overlays (mapcar (function
			      (lambda (overlay)
				(and (singular-emacs-simple-sec-type overlay)
				     overlay)))
			     overlays)
      ;; then, remove nils from list
	    overlays (delq nil overlays)
      ;; now, we have to sort the list since documentation of `overlays-in'
      ;; does not state anything about the order the overlays are returned in
	    overlays
	    (sort overlays
		  (function
		   (lambda (a b)
		     (< (overlay-start a) (overlay-start b))))))

      ;; at last, we have the list of non-clear simple sections.  Now, go and
      ;; insert clear simple sections as necessary.
      (if (null overlays)
	  ;; if there are no non-clear simple sections at all there can be
	  ;; only one large clear simple section
	  '(nil)
	;; we care about inside clear simple section first
	(setq overlay-cursor overlays)
	(while (cdr overlay-cursor)
	  (if (eq (overlay-end (car overlay-cursor))
		  (overlay-start (cadr overlay-cursor)))
	      (setq overlay-cursor (cdr overlay-cursor))
	    ;; insert nil
	    (setcdr overlay-cursor
		    (cons nil (cdr overlay-cursor)))
	    (setq overlay-cursor (cddr overlay-cursor))))
	;; now, check BEG and END for clear simple sections
	(if (> (overlay-start (car overlays)) beg)
	    (setq overlays (cons nil overlays)))
	;; `overlay-cursor' still points to the end
	(if (< (overlay-end (car overlay-cursor)) end)
	    (setcdr overlay-cursor (cons nil nil)))
	overlays))))
;;}}}

;;{{{ Simple section API for XEmacs
(defsubst singular-xemacs-simple-sec-start (simple-sec)
  "Return start of non-clear simple section SIMPLE-SEC.
Narrowing has no effect on this function."
  (extent-start-position simple-sec))

(defsubst singular-xemacs-simple-sec-end (simple-sec)
  "Return end of non-clear simple section SIMPLE-SEC.
Narrowing has no effect on this function."
  (extent-end-position simple-sec))

(defsubst singular-xemacs-simple-sec-type (simple-sec)
  "Return type of SIMPLE-SEC.
Returns nil if SIMPLE-SEC happens to be an extent but not a simple
section.
Narrowing has no effect on this function."
  (if simple-sec
      (extent-property simple-sec 'singular-type)
    singular-simple-sec-clear-type))

(defsubst singular-xemacs-simple-sec-before (pos)
  "Return simple section before buffer position POS.
This is the same as `singular-simple-sec-at' except if POS falls on a
section border.  In this case `singular-simple-section-before' returns the
previous simple section instead of the current one.  If POS falls on
beginning of buffer, the simple section at beginning of buffer is returned.
Narrowing has no effect on this function."
  (singular-xemacs-simple-sec-at (max 1 (1- pos))))

(defun singular-xemacs-simple-sec-create (type end)
  "Create a new simple section of type TYPE.
Creates the section from end of previous simple section up to the first
beginning of line before END.  That position should be larger than or equal
to `singular-simple-sec-last-end'.  Updates `singular-simple-sec-last-end'.
Returns the new simple section or `empty' if no simple section has been
created.
Assumes that no narrowing is in effect."
  (let ((last-end (singular-simple-sec-last-end-position))
	;; `simple-sec' is the new simple section or `empty'
	simple-sec)

    ;; get beginning of line before END.  At this point we need that there
    ;; are no restrictions.
    (setq end (let ((old-point (point)))
		(goto-char end) (beginning-of-line)
		(prog1 (point) (goto-char old-point))))

    (cond
     ;; do not create empty sections
     ((eq end last-end)
      'empty)
     ;; non-clear simple sections
     ((not (eq type singular-simple-sec-clear-type))
      ;; if type has not changed we only have to extend the previous simple
      ;; section.  If `last-end' happens to be 1 (meaning that we are
      ;; creating the first non-clear simple section in the buffer), then
      ;; `singular-simple-sec-before' returns nil,
      ;; `singular-simple-sec-type' returns the type of clear simple
      ;; sections that definitely does not equal TYPE, and a new simple
      ;; section is created as necessary.
      (setq simple-sec (singular-xemacs-simple-sec-before last-end))
      (if (eq type (singular-xemacs-simple-sec-type simple-sec))
	  ;; move existing extent
	  (setq simple-sec (set-extent-endpoints simple-sec
						 (extent-start-position simple-sec) end))
	;; create new extent
	(setq simple-sec (make-extent last-end end))
	;; set type property
	(set-extent-property simple-sec 'singular-type type)
	;; set face.  In contrast to Emacs, we do not need to set somethin
	;; like `evaporate'.  `detachable' is set by XEmacs by default.
	(set-extent-property simple-sec 'face (singular-simple-sec-lookup-face type)))
      ;; update `singular-simple-sec-last-end' and return new simple
      ;; section
      (set-marker singular-simple-sec-last-end end)
      simple-sec)
     ;; clear simple sections
     (t
      ;; update `singular-simple-sec-last-end' and return nil
      (set-marker singular-simple-sec-last-end end)
      nil))))

(defun singular-xemacs-simple-sec-start-at (pos)
  "Return start of clear simple section at position POS.
Assumes the existence of an imaginary empty clear simple section if POS is
at end of buffer and there is non-clear simple section immediately ending
at POS.
Assumes that no narrowing is in effect (since `previous-extent-change'
implicitly does so)."
  ;; get into some hairy details at end of buffer.  Look if there is a
  ;; non-clear simple section immediately ending at end of buffer and
  ;; return the start of the imaginary empty clear simple section in that
  ;; case.  If buffer is empty this test fails since
  ;; `singular-xemacs-simple-sec-before' (correctly) returns nil.  But in
  ;; that case the following loop returns the correct result.
  (if (and (eq pos (point-max))
	   (singular-xemacs-simple-sec-before pos))
      pos
    (let ((previous-extent-change-pos (min (1+ pos) (point-max))))
      ;; this `while' loop at last will run into the end of the next
      ;; non-clear simple section or stop at bob.  Since POS may be right at
      ;; the end of a previous non-clear location, we have to search at least
      ;; one time from POS+1 backwards.
      (while (not (or (singular-xemacs-simple-sec-before previous-extent-change-pos)
		      (eq previous-extent-change-pos 1)))
	(setq previous-extent-change-pos
	      (previous-extent-change previous-extent-change-pos)))
      previous-extent-change-pos)))

(defun singular-xemacs-simple-sec-end-at (pos)
  "Return end of clear simple section at position POS.
Assumes the existence of an imaginary empty clear simple section if POS is
at end of buffer and there is non-clear simple section immediately ending
at POS.
Assumes that no narrowing is in effect (since `next-extent-change'
implicitly does so)."
  (let ((next-extent-change-pos (next-extent-change pos)))
    ;; this `while' loop at last will run into the beginning of the next
    ;; non-clear simple section or stop at eob.  Since POS may not be at
    ;; the beginning of a non-clear simple section we may start searching
    ;; immediately.
    (while (not (or (singular-xemacs-simple-sec-at next-extent-change-pos)
		    (eq next-extent-change-pos (point-max))))
      (setq next-extent-change-pos
	    (next-extent-change next-extent-change-pos)))
    next-extent-change-pos))

(defun singular-xemacs-simple-sec-at (pos)
  "Return simple section at buffer position POS.
Assumes the existence of an imaginary empty clear simple section if POS is
at end of buffer and there is non-clear simple section immediately ending
at POS.
Narrowing has no effect on this function."
  ;; at eob, `map-extent' always returns nil so everything is OK for this
  ;; case, too.  Do not try to use `extent-at' at this point.  `extent-at'
  ;; does not return extents outside narrowed text.
  (map-extents (function (lambda (ext args) ext))
	       nil pos pos nil nil 'singular-type))

(defun singular-xemacs-simple-sec-in (beg end)
  "Return a list of all simple sections intersecting with the region from BEG to END.
A simple section intersects the region if the section and the region have
at least one character in common.  The sections are returned with
startpoints in increasing order and clear simple sections (that is, nil's)
inserted as necessary.  BEG is assumed to be less than or equal to END.
The imaginary empty clear simple section at end of buffer is never included
in the result.
Narrowing has no effect on this function."
  (let (extents extent-cursor)
    (if (= beg end)
	;; `mapcar-extents' may return some extents in this case, so
	;; exclude it
	nil
      ;; OK, that's a little bit easier than for Emacs ...
      (setq extents (mapcar-extents 'identity nil nil beg end nil 'singular-type))

      ;; now we have the list of non-clear simple sections.  Go and
      ;; insert clear simple sections as necessary.
      (if (null extents)
	  ;; if there are no non-clear simple sections at all there can be
	  ;; only one large clear simple section
	  '(nil)
	;; we care about inside clear simple section first
	(setq extent-cursor extents)
	(while (cdr extent-cursor)
	  (if (eq (extent-end-position (car extent-cursor))
		  (extent-start-position (cadr extent-cursor)))
	      (setq extent-cursor (cdr extent-cursor))
	    ;; insert nil
	    (setcdr extent-cursor
		    (cons nil (cdr extent-cursor)))
	    (setq extent-cursor (cddr extent-cursor))))
	;; now, check BEG and END for clear simple sections
	(if (> (extent-start-position (car extents)) beg)
	    (setq extents (cons nil extents)))
	;; `extent-cursor' still points to the end
	(if (< (extent-end-position (car extent-cursor)) end)
	    (setcdr extent-cursor (cons nil nil)))
	extents))))
;;}}}

;;{{{ Section API

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
;;   partially inaccessible sections.  In contrast to simple sections the
;;   functions concerning sections do not assume that there is no narrowing
;;   in effect.  However, most functions provide an optional argument
;;   RESTRICTED that restricts the start and end point of the returned
;;   sections to the currently active restrictions.  Of course, that does
;;   not affect the range of the underlying simple sections, only the
;;   additional start and end points being returned.  One should note that
;;   by restricting sections one may get empty sections, that is, sections
;;   for which the additional start and end point are equal.
;; - In many cases it is not desirable that the user operates on sections
;;   which are not completely accessible.  To check that a section is
;;   completely accessible the `singular-section-check' function should be
;;   used.
;; - Sections are independent from implementation dependencies.  There are
;;   no different versions of the functions for Emacs and XEmacs.
;; - Whenever possible, one should not access simple section directly.
;;   Instead, one should use the section API.

(defcustom singular-section-face-alist '((input . nil)
					 (output . singular-section-output-face))
  "*Alist that maps section types to faces.
Should be a list consisting of elements (SECTION-TYPE . FACE-OR-NIL), where
SECTION-TYPE is either `input' or `output'.

At any time, the Singular interactive mode buffer is completely covered by
sections of two different types: input sections and output sections.  This
variable determines which faces are used to display the different sections.

If for type SECTION-TYPE the value FACE-OR-NIL is a face it is used to
display the contents of all sections of that particular type.
If instead FACE-OR-NIL equals nil sections of that type become so-called
clear sections.  The content of clear sections is displayed as regular
text, with no faces at all attached to them.

Some notes and restrictions on this variable (believe them or not):
o Changing this variable during a Singular session may cause unexpected
  results (but not too serious ones, though).
o There may be only one clear section type defined at a time.
o Choosing clear input sections is a good idea.
o Choosing clear output sections is a bad idea.
o Consequence: Not to change this variable is a good idea."
  ;; to add new section types, simply extend the `list' widget.
  ;; The rest should work unchanged.  Do not forget to update docu.
  :type '(list (cons :tag "Input sections"
		     (const :format "" input)
		     (choice :format
"Choose either clear or non-clear input sections.  For non-clear sections,
select or modify a face (preferably `singular-section-input-face') used to
display the sections.
%[Choice%]
%v
"
			     (const :tag "Clear sections" nil)
			     (face :tag "Non-clear sections")))
	       (cons :tag "Output sections"
		     (const :format "" output)
		     (choice :format
"Choose either clear or non-clear output sections.  For non-clear sections,
select or modify a face (preferably `singular-section-output-face') used to
display the sections.
%[Choice%]
%v
"
			     (const :tag "Clear sections" nil)
			     (face :tag "Non-clear sections"))))
  :initialize 'custom-initialize-reset
  ;; this function checks for validity (only one clear section
  ;; type) and sets `singular-simple-sec-clear-type' accordingly.
  ;; In case of an error, nothing is set or modified.
  :set (function (lambda (var value)
		   (let* ((cdrs-with-nils (mapcar 'cdr value))
			  (cdrs-without-nils (delq nil (copy-sequence cdrs-with-nils))))
		     (if (> (- (length cdrs-with-nils) (length cdrs-without-nils)) 1)
			 (error "Only one clear section type allowed (see `singular-section-face-alist')")
		       (set-default var value)
		       (setq singular-simple-sec-clear-type (car (rassq nil value)))))))
  :group 'singular-faces
  :group 'singular-sections-and-foldings)

(defface singular-section-input-face '((t nil))
  "*Face to use for input sections.
It may be not sufficient to modify this face to change the appearance of
input sections.  See `singular-section-face-alist' for more information."
  :group 'singular-faces
  :group 'singular-sections-and-foldings)

(defface singular-section-output-face '((t (:bold t)))
  "*Face to use for output sections.
It may be not sufficient to modify this face to change the appearance of
output sections.  See `singular-section-face-alist' for more information."
  :group 'singular-faces
  :group 'singular-sections-and-foldings)

(defsubst singular-section-create (simple-sec type start end)
  "Create and return a new section."
  (vector simple-sec type start end))

(defsubst singular-section-simple-sec (section)
  "Return underlying simple section of SECTION."
  (aref section 0))

(defsubst singular-section-type (section)
  "Return type of SECTION."
  (aref section 1))

(defsubst singular-section-start (section)
  "Return start of SECTION."
  (aref section 2))

(defsubst singular-section-end (section)
  "Return end of SECTION."
  (aref section 3))

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
    (cond
     ;; not restricted first
     ((not restricted)
      (singular-section-create simple-sec type start end))
     ;; restricted and degenerated
     ((and restricted
	   (< end (point-min)))
      (singular-section-create simple-sec type (point-min) (point-min)))
     ;; restricted and degenerated
     ((and restricted
	   (> start (point-max)))
      (singular-section-create simple-sec type (point-max) (point-max)))
     ;; restricted but not degenerated
     (t
      (singular-section-create simple-sec type
			       (max start (point-min))
			       (min end (point-max)))))))

(defun singular-section-before (pos &optional restricted)
  "Return section before position POS.
This is the same as `singular-section-at' except if POS falls on a section
border.  In this case `singular-section-before' returns the previous
section instead of the current one.  If POS falls on beginning of buffer,
the section at beginning of buffer is returned.
Returns section intersected with current restriction if RESTRICTED is
non-nil."
  (singular-section-at (max 1 (1- pos)) restricted))

(defun singular-section-in (beg end &optional restricted)
  "Return a list of all sections intersecting with the region from BEG to END.
A section intersects with the region if the section and the region have at
least one character in common.  The sections are returned in increasing
order.
If optional argument RESTRICTED is non-nil only sections which are
completely in the intersection of the region and the current restriction
are returned."
  ;; exchange BEG and END if necessary as a special service to our users
  (let* ((reg-beg (min beg end))
	 (reg-end (max beg end))
	 ;; we need these since we widen the buffer later on
	 (point-min (point-min))
	 (point-max (point-max))
	 simple-sections)
    (if (and restricted
	     (or (> reg-beg point-max) (< reg-end point-min)))
	;; degenerate restrictions
	nil
      ;; do the intersection if necessary and get simple sections
      (setq reg-beg (if restricted (max reg-beg point-min) reg-beg)
	    reg-end (if restricted (min reg-end point-max) reg-end)
	    simple-sections (singular-simple-sec-in reg-beg reg-end))
      ;; we still have REG-BEG <= REG-END in any case.  SIMPLE-SECTIONS
      ;; contains the list of simple sections intersecting with the region
      ;; from REG-BEG and REG-END.

      (if (null simple-sections)
	  nil
	;; and here we even have REG-BEG < REG-END
	(save-restriction
	  (widen)
	  ;; get sections intersecting with the region from REG-BEG to
	  ;; REG-END
	  (let* ((sections (singular-section-in-internal simple-sections
							 reg-beg reg-end))
		 first-section-start last-section-end)
	    (if (not restricted)
		sections
	      (setq first-section-start (singular-section-start (car sections))
		    last-section-end (singular-section-end (car (last sections))))
	      ;; popping off first element is easy ...
	      (if (< first-section-start point-min)
		  (setq sections (cdr sections)))
	      ;; ... but last element is harder to pop off
	      (cond
	       (;; no elements left
		(null sections)
		nil)
	       (;; one element left
		(null (cdr sections))
		(if (> last-section-end point-max)
		    nil
		  sections))
	       (;; more than one element left
		t
		(if (> last-section-end point-max)
		    (setcdr (last sections 2) nil))
		sections)))))))))

(defun singular-section-in-internal (simple-sections reg-beg reg-end)
  "Create a list of sections from SIMPLE-SECTIONS.
This is the back-end for `singular-section-in'.
First simple section should be such that it contains REG-BEG, last simple
section should be such that it contains or ends at REG-END.  These
arguments are used to find the start resp. end of clear simple sections of
terminal clear simple sections in SIMPLE-SECTIONS.
Assumes that REG-BEG < REG-END.
Assumes that SIMPLE-SECTIONS is not empty.
Assumes that no narrowing is in effect."
  (let* (;; we pop off the extra nil at the end of the loop
	 (sections (cons nil nil))
	 (sections-end sections)
	 (simple-section (car simple-sections))
	 type start end)

    ;; first, get unrestricted start
    (setq start (if simple-section
		    (singular-simple-sec-start simple-section)
		  ;; here we need that no narrowing is in effect
		  (singular-simple-sec-start-at reg-beg)))

    ;; loop through all simple sections but last
    (while (cdr simple-sections)
      (setq simple-section (car simple-sections)
	    type (singular-simple-sec-type simple-section)
	    end (if simple-section
		    (singular-simple-sec-end simple-section)
		  (singular-simple-sec-start (cadr simple-sections)))

	    ;; append the new section to `sections-end'
	    sections-end
	    (setcdr sections-end
		    (cons (singular-section-create simple-section type start end) nil))

	    ;; get next simple section and its start
	    simple-sections (cdr simple-sections)
	    start end))

    ;; care about last simple section
    (setq simple-section (car simple-sections)
	  type (singular-simple-sec-type simple-section)
	  end (if simple-section
		  (singular-simple-sec-end simple-section)
		;; the `1-' is OK since REG-BEG < REG-END.
		;; here we need that no narrowing is in effect
		(singular-simple-sec-end-at (1- reg-end))))
    (setcdr sections-end
	    (cons (singular-section-create simple-section type start end) nil))

    ;; we should not forget to pop off our auxiliary cons-cell
    (cdr sections)))

(defun singular-section-mapsection (func sections &optional type-filter negate-filter)
  "Apply FUNC to each section in SECTIONS, and make a list of the results.
If optional argument TYPE-FILTER is non-nil it should be a list of section
types.  FUNC is then applied only to those sections with type occurring in
TYPE-FILTER.  If in addition optional argument NEGATE-FILTER is non-nil
FUNC is applied only to those sections with type not occurring in
TYPE-FILTER.

In any case the length of the list this function returns equals the
number of sections actually processed."
  (if (not type-filter)
      (mapcar func sections)
    ;; copy the list first
    (let ((sections (copy-sequence sections)))
      ;; filter elements and turn them to t's
      (setq sections
	    (mapcar (function
		     (lambda (section)
		       ;; that strange expression evaluates to t iff the
		       ;; section should be removed.  The `not' is to
		       ;; canonize boolean values to t or nil, resp.
		       (or (eq (not (memq (singular-section-type section) type-filter))
			       (not negate-filter))
			   section)))
		    sections)

      ;; remove t's now
	    sections (delq t sections))

      ;; call function for remaining sections
      (mapcar func sections))))
;;}}}

;;{{{ Section miscellaneous
(defun singular-section-check (section &optional no-error)
  "Check whether SECTION is completely accessible and return t if so.
If otherwise SECTION is restricted either in part or as a whole, this
function fails with an error or returns nil if optional argument NO-ERROR
is non-nil."
  (cond ((and (>= (singular-section-start section) (point-min))
	      (<= (singular-section-end section) (point-max))) t)
	(no-error nil)
	(t (error "section is restricted either in part or as a whole"))))

(defun singular-section-to-string (section &optional raw)
  "Get contents of SECTION as a string.
Returns text between start and end of SECTION.
Removes prompts from section contents unless optional argument RAW is
non-nil.
Narrowing has no effect on this function."
  (save-restriction
    (widen)
    (let ((string (buffer-substring (singular-section-start section)
				    (singular-section-end section))))
      (if raw
	  string
	(singular-prompt-remove-string string)))))
;;}}}

;;{{{ Section miscellaneous interactive
(defun singular-section-goto-beginning ()
  "Move point to beginning of current section."
  (interactive)
  (goto-char (singular-section-start (singular-section-at (point))))
  (singular-keep-region-active))

(defun singular-section-goto-end ()
  "Move point to end of current section."
  (interactive)
  (goto-char (singular-section-end (singular-section-at (point))))
  (singular-keep-region-active))

(defun singular-section-backward (n)
  "Move backward until encountering the beginning of a section.
With argument, do this that many times.  With N less than zero, call
`singular-section-forward' with argument -N."
  (interactive "p")
  (while (> n 0)
    (goto-char (singular-section-start (singular-section-before (point))))
    (setq n (1- n)))
  (if (< n 0)
      (singular-section-forward (- n))
    (singular-keep-region-active)))

(defun singular-section-forward (n)
  "Move forward until encountering the end of a section.
With argument, do this that many times.  With N less than zero, call
`singular-section-backward' with argument -N."
  (interactive "p")
  (while (> n 0)
    (goto-char (singular-section-end (singular-section-at (point))))
    (setq n (1- n)))
  (if (< n 0)
      (singular-section-backward (- n))
    (singular-keep-region-active)))

(defun singular-section-kill (section &optional raw no-error)
  "Kill SECTION.
Puts the contents of SECTION into the kill ring.  Removes prompts from
contents unless optional argument RAW is non-nil.
If called interactively, kills section point currently is in.  Does a raw
section kill if called with a prefix argument, otherwise strips prompts.
Does not kill sections that are restricted either in part or as a whole.
Rather fails with an error in such cases or silently fails if optional
argument NO-ERROR is non-nil."
  (interactive (list (singular-section-at (point))
		     current-prefix-arg nil))
  (when (singular-section-check section no-error)
    (kill-new (singular-section-to-string section raw))
    (delete-region (singular-section-start section)
		   (singular-section-end section))))
;;}}}

;;{{{ Folding sections for both Emacs and XEmacs
(defcustom singular-folding-ellipsis "Singular I/O ..."
  "*Ellipsis to show for folded input or output.
Changing this variable has an immediate effect only if one uses
\\[customize] to do so.
However, even then it may be necessary to refresh display completely (using
\\[recenter], for example) for the new settings to be visible."
  :type 'string
  :initialize 'custom-initialize-default
  :set (function
	(lambda (var value)
	  ;; set in all singular buffers
	  (singular-map-buffer 'singular-folding-set-ellipsis value)
	  (set-default var value)))
  :group 'singular-sections-and-foldings)

(defcustom singular-folding-line-move-ignore-folding t
  "*If non-nil, ignore folded sections when moving point up or down.
This variable is used to initialize `line-move-ignore-invisible'.  However,
documentation states that setting `line-move-ignore-invisible' to a non-nil
value may result in a slow-down when moving the point up or down.  One
should try to set this variable to nil if point motion seems too slow.

Changing this variable has an immediate effect only if one uses
\\[customize] to do so."
  :type 'boolean
  :initialize 'custom-initialize-default
  :set (function
	(lambda (var value)
	  ;; set in all singular buffers
	  (singular-map-buffer 'set 'line-move-ignore-invisible value)
	  (set-default var value)))
  :group 'singular-sections-and-foldings)

(defun singular-folding-set-ellipsis (ellipsis)
  "Set ellipsis to show for folded input or output in current buffer."
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (setq buffer-display-table (or (copy-sequence standard-display-table)
				   (make-display-table)))
    (set-display-table-slot buffer-display-table
			    'selective-display (vconcat ellipsis)))
   ;; XEmacs
   (t
    (set-glyph-image invisible-text-glyph ellipsis (current-buffer)))))

(defun singular-folding-init ()
  "Initializes folding of sections for the current buffer.
That includes setting `buffer-invisibility-spec' and the ellipsis to show
for hidden text.

This function is called at mode initialization time."
  ;; initialize `buffer-invisibility-spec' first
  (let ((singular-invisibility-spec (cons 'singular-interactive-mode t)))
    (if (and (listp buffer-invisibility-spec)
	     (not (member singular-invisibility-spec buffer-invisibility-spec)))
	(setq buffer-invisibility-spec
	      (cons singular-invisibility-spec buffer-invisibility-spec))
      (setq buffer-invisibility-spec (list singular-invisibility-spec))))
  ;; ignore invisible lines on movements
  (set (make-local-variable 'line-move-ignore-invisible)
       singular-folding-line-move-ignore-folding)
  ;; now for the ellipsis
  (singular-folding-set-ellipsis singular-folding-ellipsis))

(defun singular-folding-fold (section &optional no-error)
  "Fold section SECTION if it is not already folded.
Does not fold sections that do not end in a newline or that are restricted
either in part or as a whole.  Rather fails with an error in such cases
or silently fails if optional argument NO-ERROR is non-nil.
This is for safety only: In both cases the result may be confusing to the
user."
  (let* ((start (singular-section-start section))
	 (end (singular-section-end section)))
    (cond ((not (singular-section-check section no-error))
	   nil)
	  ((not (eq (char-before end) ?\n))
	   (unless no-error
	     (error "Section does not end in a newline")))
	  ((not (singular-folding-foldedp section))
	   ;; fold but only if not already folded
	   (singular-folding-fold-internal section)))))

(defun singular-folding-unfold (section &optional no-error invisibility-overlay-or-extent)
  "Unfold section SECTION if it is not already unfolded.
Does not unfold sections that are restricted either in part or as a whole.
Rather fails with an error in such cases or silently fails if optional
argument NO-ERROR is non-nil.
This is for safety only: The result may be confusing to the user.
If optional argument INVISIBILITY-OVERLAY-OR-EXTENT is non-nil it should be
the invisibility overlay or extent, respectively, of the section to
unfold."
  (let* ((start (singular-section-start section))
	 (end (singular-section-end section)))
    (cond ((not (singular-section-check section no-error))
	   nil)
	  ((or invisibility-overlay-or-extent
	       (setq invisibility-overlay-or-extent (singular-folding-foldedp section)))
	   ;; unfold but only if not already unfolded
	   (singular-folding-unfold-internal section invisibility-overlay-or-extent)))))

(defun singular-folding-fold-at-point ()
  "Fold section point currently is in.
Does not fold sections that do not end in a newline or that are restricted
either in part or as a whole.  Rather fails with an error in such cases."
  (interactive)
  (singular-folding-fold (singular-section-at (point))))

(defun singular-folding-unfold-at-point ()
  "Unfold section point currently is in.
Does not unfold sections that are restricted either in part or as a whole.
Rather fails with an error in such cases."
  (interactive)
  (singular-folding-unfold (singular-section-at (point))))

(defun singular-folding-fold-latest-output ()
  "Fold latest output section.
Does not fold sections that do not end in a newline or that are restricted
either in part or as a whole.  Rather fails with an error in such cases."
  (interactive)
  (singular-folding-fold (singular-latest-output-section)))

(defun singular-folding-unfold-latest-output ()
  "Unfolds latest output section.
Does not unfold sections that are restricted either in part or as a whole.
Rather fails with an error in such cases."
  (interactive)
  (singular-folding-unfold (singular-latest-output-section)))

(defun singular-folding-fold-all-output ()
  "Fold all complete, unfolded output sections.
That is, all output sections that are not restricted in part or as a whole
and that end in a newline."
  (interactive)
  (singular-section-mapsection (function (lambda (section) (singular-folding-fold section t)))
			       (singular-section-in (point-min) (point-max) t)
			       '(output)))

(defun singular-folding-unfold-all-output ()
  "Unfold all complete, folded output sections.
That is, all output sections that are not restricted in part or as a whole."
  (interactive)
  (singular-section-mapsection (function (lambda (section) (singular-folding-unfold section t)))
			       (singular-section-in (point-min) (point-max) t)
			       '(output)))

(defun singular-folding-toggle-fold-at-point-or-all (&optional arg)
  "Fold or unfold section point currently is in or all output sections.
Without prefix argument, folds unfolded sections and unfolds folded
sections.  With prefix argument, folds all output sections if argument is
positive, otherwise unfolds all output sections.
Does neither fold nor unfold sections that do not end in a newline or that
are restricted either in part or as a whole.  Rather fails with an error in
such cases."
  (interactive "P")
    (cond ((not arg)
	   ;; fold or unfold section at point
	   (let* ((section (singular-section-at (point)))
		  (invisibility-overlay-or-extent (singular-folding-foldedp section)))
	     (if invisibility-overlay-or-extent
		 (singular-folding-unfold section nil invisibility-overlay-or-extent)
	       (singular-folding-fold section))))
	  ((> (prefix-numeric-value arg) 0)
	   (singular-folding-fold-all-output))
	  (t
	   (singular-folding-unfold-all-output))))

(defun singular-folding-toggle-fold-latest-output (&optional arg)
  "Fold or unfold latest output section.
Folds unfolded sections and unfolds folded sections.
Does neither fold nor unfold sections that do not end in a newline or that
are restricted either in part or as a whole.  Rather fails with an error in
such cases."
  (interactive)
  (let* ((section (singular-latest-output-section))
	 (invisibility-overlay-or-extent (singular-folding-foldedp section)))
    (if invisibility-overlay-or-extent
	(singular-folding-unfold section nil invisibility-overlay-or-extent)
      (singular-folding-fold section))))

;; Note:
;;
;; The rest of the folding is either marked as
;; Emacs
;; or
;; XEmacs

(singular-fset 'singular-folding-fold-internal
	       'singular-emacs-folding-fold-internal
	       'singular-xemacs-folding-fold-internal)

(singular-fset 'singular-folding-unfold-internal
	       'singular-emacs-folding-unfold-internal
	       'singular-xemacs-folding-unfold-internal)

(singular-fset 'singular-folding-foldedp
	       'singular-emacs-folding-foldedp-internal
	       'singular-xemacs-folding-foldedp-internal)
;;}}}

;;{{{ Folding sections for Emacs

;; Note:
;;
;; For Emacs, we use overlays to hide text (so-called "invisibility
;; overlays").  In addition to their `invisible' property, they have the
;; `singular-invisible' property set.  Setting the intangible property does
;; not work very well for Emacs.  We use the variable
;; `line-move-ignore-invisible' which works quite well.

(defun singular-emacs-folding-fold-internal (section)
  "Fold section SECTION.
SECTION should end in a newline.  That terminal newline is not
folded or otherwise ellipsis does not appear.
SECTION should be unfolded."
  (let* ((start (singular-section-start section))
	 ;; do not make trailing newline invisible
	 (end (1- (singular-section-end section)))
	 invisibility-overlay)
    ;; create new overlay and add properties
    (setq invisibility-overlay (make-overlay start end))
    ;; mark them as invisibility overlays
    (overlay-put invisibility-overlay 'singular-invisible t)
    ;; set invisible properties
    (overlay-put invisibility-overlay 'invisible 'singular-interactive-mode)
    ;; evaporate empty invisibility overlays
    (overlay-put invisibility-overlay 'evaporate t)))

(defun singular-emacs-folding-unfold-internal (section &optional invisibility-overlay)
  "Unfold section SECTION.
SECTION should be folded.
If optional argument INVISIBILITY-OVERLAY is non-nil it should be the
invisibility overlay of the section to unfold."
  (let ((invisibility-overlay
	 (or invisibility-overlay
	     (singular-emacs-folding-foldedp-internal section))))
    ;; to keep number of overlays low we delete it
    (delete-overlay invisibility-overlay)))

(defun singular-emacs-folding-foldedp-internal (section)
  "Returns non-nil iff SECTION is folded.
More specifically, returns the invisibility overlay if there is one.
Narrowing has no effect on this function."
  (let* ((start (singular-section-start section))
	 (overlays (overlays-at start))
	 invisibility-overlay)
    ;; check for invisibility overlay
    (while (and overlays (not invisibility-overlay))
      (if (overlay-get (car overlays) 'singular-invisible)
	  (setq invisibility-overlay (car overlays))
	(setq overlays (cdr overlays))))
    invisibility-overlay))
;;}}}

;;{{{ Folding sections for XEmacs

;; Note:
;;
;; For XEmacs, we use extents to hide text (so-called "invisibility
;; extents").  In addition to their `invisible' property, they have the
;; `singular-invisible' property set.  To ignore invisible text we use the
;; variable `line-move-ignore-invisible' which works quite well.

(defun singular-xemacs-folding-fold-internal (section)
  "Fold section SECTION.
SECTION should end in a newline.  That terminal newline is not
folded or otherwise ellipsis does not appear.
SECTION should be unfolded."
  (let* ((start (singular-section-start section))
	 ;; do not make trailing newline invisible
	 (end (1- (singular-section-end section)))
	 invisibility-extent)
    ;; create new extent and add properties
    (setq invisibility-extent (make-extent start end))
    ;; mark them as invisibility extents
    (set-extent-property invisibility-extent 'singular-invisible t)
    ;; set invisible properties
    (set-extent-property invisibility-extent 'invisible 'singular-interactive-mode)))

(defun singular-xemacs-folding-unfold-internal (section &optional invisibility-extent)
  "Unfold section SECTION.
SECTION should be folded.
If optional argument INVISIBILITY-EXTENT is non-nil it should be the
invisibility extent of the section to unfold."
  (let ((invisibility-extent
	 (or invisibility-extent
	     (singular-xemacs-folding-foldedp-internal section))))
    ;; to keep number of extents low we delete it
    (delete-extent invisibility-extent)))

(defun singular-xemacs-folding-foldedp-internal (section)
  "Returns non-nil iff SECTION is folded.
More specifically, returns the invisibility extent if there is one.
Narrowing has no effect on this function."
  ;; do not try to use `extent-at' at this point.  `extent-at' does not
  ;; return extents outside narrowed text.
  (let* ((start (singular-section-start section))
	 (invisibility-extent (map-extents
			    (function (lambda (ext args) ext))
			    nil start start nil nil 'singular-invisible)))
    invisibility-extent))
;;}}}

;;{{{ Online help

;; Note:
;;
;; Catching user's help commands to Singular and translating them to calls
;; to `info' is quite a difficult task due to the asynchronous nature of
;; communication with Singular.  We use an heuristic approach which should
;; work in most cases:
;;
;; - `singular-help-pre-input-filter' scans user's input for help commands.
;;   If user issues a help command the filter sets a time stamp and passes
;;   the input unchanged to Singular.
;; - Singular receives the help command and barfs that it could not process
;;   it.  We call that error message "Singular's response".  That response
;;   in particular contains the help topic the user requested.  If the
;;   response for some reasons is not recognized and filtered in the later
;;   steps the user gets some reasonable response on her command that way.
;; - `singular-help-pre-output-filter' on each output from Singular checks
;;   (using the time stamp set by `singular-help-pre-input-filter') whether
;;   the user issued a help command at most one second ago.  If so,
;;   `singular-help-pre-output-filter' starts checking Singular's output
;;   for the response on the help command.  If it finds one it remembers
;;   the help topic in `singular-help-topic' and removes the response from
;;   Singular's output.
;;   There is some extra magic built into the filter to handle responses
;;   from Singular which are received by emacs not in one string but in
;;   more than one piece (we call that pending output).
;; - As the last step step of this procedure, `singular-post-output-filter'
;;   fires up an Info buffer using `singular-help' if the variable
;;   `singular-help-topic' is non-nil.  This step is separated from the
;;   previous one since joining both leads to some trouble in point
;;   management.  This is mainly due to the fact that `singular-help' opens
;;   a new window.
;;
;; To show some online help, the online help manual has to be available, of
;; course.  There is a number of possibilities for the user to set the file
;; name of the manual explicitly, as described in the documentation string
;; to `singular-help'.  But in general the file name should be recognized
;; automatically by Singular interactive mode.  For that to work, Singular
;; prints the file name when it comes up and option `--emacs' is specified.
;; This is recognized by `singular-scan-header-pre-output-filter' which
;; sets the variable `singular-help-file-name' accordingly.  For more
;; information one should refer to the `Header scanning ...' folding.
;;
;; Another variable which needs to be set for proper operation is
;; `singular-help-topics-alist' for completion of help topics and for
;; recognition of help topics around point.  It is no error for this
;; variable not to be set: simply the features do not work then.

;; this `require' is necessary since we use functions from the Info package
;; which are not declared as `autoload'
(require 'info)

(defcustom singular-help-same-window 'default
  "*Specifies how to open the window for Singular online help.
If this variable equals t, Singular online help comes up in the selected
window.
If this variable equals nil, Singular online help comes up in another
window.
If this variable equals neither t nor nil, the standard Emacs behaviour to
open the Info buffer is adopted (which very much depends on the settings of
`same-window-buffer-names')."
  :initialize 'custom-initialize-default
  :type '(choice (const :tag "This window" t)
		 (const :tag "Other window" nil)
		 (const :tag "Default" default))
  :group 'singular-interactive-miscellaneous)

(defcustom singular-help-explicit-file-name nil
  "*Specifies the file name of the Singular online manual.
If non-nil, used as file name of the Singular online manual.

This variable should be customized only if all other attempts of Singular
interactive mode fail to determine the file name of the Singular online
manual.  For more information one should refer to the `singular-help'
function."
  :initialize 'custom-initialize-default
  :type '(choice (const nil) file)
  :group 'singular-interactive-miscellaneous)

(defvar singular-help-file-name nil
  "File name of the Singular online manual.
This variable should not be modified by the user.

This variable is buffer-local.")

(defconst singular-help-fall-back-file-name "singular.info"
  "Fall-back file name of the Singular online manual.
This variable is used if the file name of the Singular online manual cannot
be determined otherwise.")

(defvar singular-help-time-stamp '(0 0)
  "The time stamp that is set when the user issues a help command.

This variable is buffer-local.")

(defvar singular-help-response-pending nil
  "If non-nil, Singular's response has not been completely received.

This variable is buffer-local.")

(defvar singular-help-topic nil
  "If non-nil, contains help topic to show in post output filter.

This variable is buffer-local.")

(defconst singular-help-command-regexp "^\\s-*\\(help\\|\?\\)"
  "Regular expression to match Singular help commands.")

(defconst singular-help-response-line-1
  "^// \\*\\* Your help command could not be executed\\. Use\n"
  "Regular expression that matches the first line of Singular's response.")

(defconst singular-help-response-line-2
  "^// \\*\\* C-h C-s \\(.*\\)\n"
  "Regular expression that matches the second line of Singular's response.
First subexpression matches help topic.")

(defconst singular-help-response-line-3
  "^// \\*\\* to enter the Singular online help\\. For general\n"
  "Regular expression that matches the third line of Singular's response.")

(defconst singular-help-response-line-4
  "^// \\*\\* information on Singular running under Emacs, type C-h m\\.\n"
  "Regular expression that matches the fourth line of Singular's response.")

(defun singular-help-pre-input-filter (input)
  "Check user's input for help commands.
Sets time stamp if one is found.  Passes user's input on to Singular
unchanged."
  (if (string-match singular-help-command-regexp input)
      (setq singular-help-time-stamp (current-time)))
  ;; return nil so that input passes unchanged
  nil)

(defun singular-help-pre-output-filter (output)
  "Check for Singular's response on a help command.
Removes it and sets `singular-help-topic' accordingly."
  ;; check first
  ;; - whether a help statement has been issued at most one second ago, or
  ;; - whether there is a pending response.
  ;; Only if one of these conditions is met we go on and check text for a
  ;; response on a help command.  Checking uncoditionally every piece of
  ;; output would be far too expensive.
  ;; If check fails nil is returned, what is exactly what we need for the
  ;; filter.
  (if (or (= (cadr (current-time)) (cadr singular-help-time-stamp))
	  singular-help-response-pending)
      ;; if response is pending for more than five seconds, give up
      (if (and singular-help-response-pending
	       (> (singular-time-stamp-difference (current-time) singular-help-time-stamp) 5))
	  ;; this command returns nil, what is exactly what we need for the filter
	  (setq singular-help-response-pending nil)
	;; go through output, removing the response.  If there is a
	;; pending response we nevertheless check for all lines, not only
	;; for the pending one.  At last, pending responses should not
	;; occur to often.
	(when (string-match singular-help-response-line-1 output)
	  (setq output (replace-match "" t t output))
	  (setq singular-help-response-pending t))
	(when (string-match singular-help-response-line-2 output)
	  ;; after all, we found what we are looking for
	  (setq singular-help-topic (substring output (match-beginning 1) (match-end 1)))
	  (setq output (replace-match "" t t output))
	  (setq singular-help-response-pending t))
	(when (string-match singular-help-response-line-3 output)
	  (setq output (replace-match "" t t output))
	  (setq singular-help-response-pending t))
	(when (string-match singular-help-response-line-4 output)
	  (setq output (replace-match "" t t output))
	  ;; we completely removed the help from output!
	  (setq singular-help-response-pending nil))

	;; return modified OUTPUT
	output)))

(defun singular-help-post-output-filter (&rest ignore)
  "Call `singular-help' if `singular-help-topic' is non-nil."
  (when singular-help-topic
    (save-excursion
      (singular-help singular-help-topic))
    (setq singular-help-topic nil)))

(defvar singular-help-topic-history nil
  "History of help topics used as arguments to `singular-help'.")

(defun singular-help (&optional help-topic)
  "Show help on HELP-TOPIC in Singular online manual.

The file name of the Singular online manual is determined in the following
manner:
o if the \(customizable) variable `singular-help-explicit-file-name' is
  non-nil, it is used as file name;
o otherwise, if the variable `singular-help-file-name' is non-nil, is is
  used as file name.  This variable should be set by Singular interactive
  mode itself, but there may be instances where this fails.  Anyway, it
  should be not set by the user.
o otherwise, if the environment variable SINGULAR_INFO_FILE is set, it is
  used as file name;
o otherwise, the constant `singular-help-fall-back-file-name' is used
  as file name."
  (interactive
   (list (completing-read "Help topic: " singular-help-topics-alist
			  nil nil nil 'singular-help-topic-history)))

  ;; get help file and topic
  (let ((help-file-name (or singular-help-explicit-file-name
			    singular-help-file-name
			    (getenv "SINGULAR_INFO_FILE")
			    singular-help-fall-back-file-name))
	(help-topic (cond ((or (null help-topic)
			       (string= help-topic ""))
			   "Top")
			  ;; try to get the real topic from the alist.
			  ;; It's OK if the alist is empty.
			  ((cdr (assoc help-topic
				       singular-help-topics-alist)))
			  (t help-topic)))
	(continue t))

    ;; pop to Info buffer
    (singular-pop-to-buffer singular-help-same-window "*info*")

    ;; test whether we are already in Singular's online manual
    (unless (and (boundp 'Info-current-file)
		 (equal Info-current-file help-file-name))
      ;; jump to Singular's top node
      (condition-case signal
	  (Info-find-node help-file-name "Top")
	;; in case of an error jump to info directory
	(error
	 (Info-directory)
	 ;; if we have been called interactively we pass the error down,
	 ;; otherwise we assumes that we have been called from a hook and
	 ;; call `singular-error'
	 (if (interactive-p)
	     (signal (car signal) (cdr signal))
	   (singular-error "Singular online manual %s not found"
			   help-file-name))
	 ;; do not continue
	 (setq continue nil))))

    (when continue
      ;; jump to desired node
      (condition-case signal
	  (Info-goto-node help-topic)
	;; in case of an error jump to Singular's top node
	(error
	 (Info-goto-node "Top")
	 ;; if we have been called interactively we pass the error down,
	 ;; otherwise we assumes that we have been called from a hook and
	 ;; call `singular-error'
	 (if (interactive-p)
	     (signal (car signal) (cdr signal))
	   (singular-error "Singular help topic %s not found"
			   help-topic)))))))

;; This might not be the best place for singular-example, but this function
;; is some kind of singular help, so the place is not too bad.
;; Note: We use singular-help-topic-history for singular-example, too
(defun singular-example (&optional command)
  "Show Singular example on COMMAND."
  (interactive
   (list (completing-read "Example for: " singular-examples-alist
			  nil nil nil 'singular-help-topic-history)))
  (let ((process (singular-process))
	(string (concat "example " command ";")))
    (singular-input-filter process string)
    (singular-send-string process string)))

(defun singular-help-init ()
  "Initialize online help support for Singular interactive mode.

This function is called at mode initialization time."
  (make-local-variable 'singular-help-file-name)
  (make-local-variable 'singular-help-time-stamp)
  (make-local-variable 'singular-help-response-pending)
  (make-local-variable 'singular-help-topic)
  (add-hook 'singular-pre-input-filter-functions 'singular-help-pre-input-filter)
  (add-hook 'singular-pre-output-filter-functions 'singular-help-pre-output-filter)
  (add-hook 'singular-post-output-filter-functions 'singular-help-post-output-filter))
;;}}}

;;{{{ Singular commands, help topics and standard libraries alists
(defvar singular-commands-alist nil
  "An alist containing all Singular commands to complete.

This variable is buffer-local.")

(defvar singular-help-topics-alist nil
  "An alist containing all Singular help topics to complete.

This variable is buffer-local.")

(defvar singular-standard-libraries-with-categories nil
  "A list containing all Singular standard library names and their category.

This variable is buffer-local.")

(defvar singular-standard-libraries-alist nil
  "An alist containing all Singular standard library names.
This variable is set automatically by `singular-menu-install-libraries'
using the value of `singular-standard-libraries-with-categories'.

This variable is buffer-local.")
;;}}}

;;{{{ Scanning of header and handling of emacs home directory
;;
;; Scanning of header
;;
(defconst singular-scan-header-emacs-home-regexp "^// \\*\\* EmacsDir: \\(.+\\)\n"
  "Regular expression matching the location of emacs home in Singular
header.")

(defconst singular-scan-header-info-file-regexp "^// \\*\\* InfoFile: \\(.+\\)\n"
  "Regular expression matching the location of Singular info file in
Singular header.")

(defconst singular-scan-header-time-stamp 0
  "A time stamp set by singular-scan-header.

This variable is buffer-local.")

(defvar singular-scan-header-scan-for '()
  "List of things to scan for in Singular header.
If `singular-scan-header-pre-output-filter' finds one thing in the current
output, it removes the corresponding value from the list.
If this variable gets nil, `singular-scan-header-pre-output-filter' is
removed from the pre-output-filter.
This variable is initialized in `singular-scan-header-init'. Possible
values of this list are up to now `help-file' and `emacs-home'.

This variable is buffer-local.")

(defun singular-scan-header-got-emacs-home ()
  "Load Singular completion and libraries files.
Assumes that `singular-emacs-home-directory' is set to the appropriate
value and loads the files \"cmd-cmpl.el\", \"hlp-cmpl.el\", \"ex-cmpl.el\",
and \"lib-cmpl.el\".
On success calls `singular-menu-install-libraries'."
  (or (load (singular-expand-emacs-file-name "cmd-cmpl.el" t) t t t)
      (message "Can't find command completion file! Command completion disabled."))
  (or (load (singular-expand-emacs-file-name "hlp-cmpl.el" t) t t t)
      (message "Can't find help topic completion file! Help completion disabled."))
  (or (load (singular-expand-emacs-file-name "ex-cmpl.el" t) t t t)
      (message "Can't find examples completion file! Examples completion disabled."))
  (if (load (singular-expand-emacs-file-name "lib-cmpl.el" t) t t t)
      (singular-menu-install-libraries)
    (message "Can't find library index file!")))


(defun singular-scan-header-pre-output-filter (output)
  "Filter function for hook `singular-pro-output-filter-functions'.
Scans the Singular header for special markers using the regexps
`singular-scan-header-info-file-regexp' and
`singular-scan-header-emacs-home-regexp', removes them, loads the
completion files, the library-list file, calls
`singular-menu-install-libraries' and sets `singular-help-file-name'.
Removes itself from the hook if all special markers were found or if it has
been searching for more than 20 seconds."
  (singular-debug 'interactive (message "scanning header"))
  (let ((changed nil))

    ;; Search for emacs home directory
    (when (string-match singular-scan-header-emacs-home-regexp output)
      (let ((emacs-home (substring output (match-beginning 1) (match-end 1))))
	(singular-debug 'interactive
			(message "scan header: emacs home path found"))
	;; in any case, remove marker from output
	(setq output (replace-match "" t t output))
	(setq changed t)
	;; if not already done, do action an singular-emacs-home
	(when (memq 'emacs-home singular-scan-header-scan-for)
	  (singular-debug 'interactive (message "scan header: initializing emacs-home-directory"))
	  (setq singular-scan-header-scan-for (delq 'emacs-home singular-scan-header-scan-for))
	  (setq singular-emacs-home-directory emacs-home)
	  (singular-scan-header-got-emacs-home))))

    ;; Search for Singular info file
    (when (string-match singular-scan-header-info-file-regexp output)
      (let ((file-name (substring output (match-beginning 1) (match-end 1))))
	(singular-debug 'interactive
			(message "scan header: singular.info path found"))
	;; in any case, remove marker from output
	(setq output (replace-match "" t t output))
	(setq changed t)
	;; if not already done, do action on help-file-name
	(when (memq 'info-file singular-scan-header-scan-for)
	  (singular-debug 'interactive (message "scan header: initializing help-file-name"))
	  (setq singular-scan-header-scan-for (delq 'info-file singular-scan-header-scan-for))
	  (setq singular-help-file-name file-name))))

    ;; Remove from hook if everything is found or if we already waited
    ;; too long.
    (if (or (eq singular-scan-header-scan-for nil)
	    (> (singular-time-stamp-difference (current-time) singular-scan-header-time-stamp) 20))
	(remove-hook 'singular-pre-output-filter-functions 'singular-scan-header-pre-output-filter))

    ;; Return new output string if we changed it, nil otherwise
    (and changed output)))

(defun singular-scan-header-init ()
  "Initialize scanning of header for Singular interactive mode.

This function is called by `singular-exec'."
  (singular-debug 'interactive (message "Initializing scan-header"))
  (set (make-local-variable 'singular-scan-header-time-stamp) (current-time))
  (set (make-local-variable 'singular-scan-header-scan-for) '())

  (make-local-variable 'singular-emacs-home-directory)
  ;; if singular-emacs-home is set try to load the completion files.
  ;; Otherwise set marker that we still have to search for it.
  (if singular-emacs-home-directory
      (singular-scan-header-got-emacs-home)
    (setq singular-scan-header-scan-for (append singular-scan-header-scan-for '(emacs-home))))

  ;; Up to now this seems to be the best place to initialize
  ;; `singular-help-file-name' since singular-help gets initialized
  ;; only on mode start-up, not on Singular start-up
  ;;
  ;; if singular-help-file-name is not set, mark, that we have to scan for it
  (make-local-variable 'singular-help-file-name)
  (or singular-help-file-name
      (setq singular-scan-header-scan-for (append singular-scan-header-scan-for '(info-file))))

  (add-hook 'singular-pre-output-filter-functions 'singular-scan-header-pre-output-filter))

(defun singular-scan-header-exit ()
  "Reinitialize scanning of header for Singular interactive mode.

This function is called by `singular-exit-sentinel'."
  ;; unset variables so that all subsequent calls of Singular will
  ;; scan the header.
  (singular-debug 'interactive (message "Deinitializing scan-header"))
  (setq singular-emacs-home-directory nil)
  (setq singular-help-file-name nil))

;;
;; handling of emacs home directory
;;
;; A note on `singular-emacs-home-directory': If this variable is set
;; before singular.el is evaluated, the header of the first Singular
;; started is NOT searched for the singular-emacs-home-directory.
;; Anyhow, all subsequent calls of Singular will scan the header
;; regardless of the initial state of this variable. (The exit-sentinel
;; will set this variable back to nil.)
;; See also `singular-scan-header-exit'.
(defvar singular-emacs-home-directory nil
  "Path to the emacs sub-directory of Singular as string.
`singular-scan-header-pre-output-filter' searches the Singular header for
the path and sets this variable to the corresponding value.
Its value is redefined on every start of Singular.

This variable is buffer-local.")

(defun singular-expand-emacs-file-name (file &optional noerror)
  "Add absolute path of emacs home directory.
Adds the content of `singular-emacs-home-directory' to the string FILE.
If `singular-emacs-home-directory' is nil, return nil and signal
an error unless optional argument NOERROR is not nil."
  (if singular-emacs-home-directory
      (concat singular-emacs-home-directory
	      (if (memq (aref singular-emacs-home-directory
			      (1- (length singular-emacs-home-directory)))
			'(?/ ?\\))
		  "" "/")
	      file)
    (if noerror
	nil
      (error "Variable singular-emacs-home-directory not set"))))
;;}}}

;;{{{ Filename, Command, and Help Completion
(defun singular-completion-init ()
  "Initialize completion for Singular interactive mode.
Initializes completion of file names, commands, examples, and help topics.

This function is called by `singular-exec'."
  (singular-debug 'interactive (message "Initializing completion"))
  (set (make-local-variable 'singular-commands-alist) nil)
  (set (make-local-variable 'singular-examples-alist) nil)
  (set (make-local-variable 'singular-help-topics-alist) nil))

(defun singular-completion-do (pattern beg end completion-alist)
  "Try completion on string PATTERN using alist COMPLETION-ALIST.
Inserts completed version of PATTERN as new text between BEG and END.
Assumes the COMPLETION-ALIST is not nil."
  (let ((completion (try-completion pattern completion-alist)))
    (cond ((eq completion t)
	   (message "[Sole completion]"))  ;; nothing to complete
	  ((null completion)               ;; no completion found
	   (message "Can't find completion for \"%s\"" pattern)
	   (ding))
	  ((not (string= pattern completion))
	   (delete-region beg end)
	   (insert completion))
	  (t
	   (message "Making completion list...")
	   (let ((list (all-completions pattern
					completion-alist)))
	     (with-output-to-temp-buffer "*Completions*"
	       (display-completion-list list)))
	   (message "Making completion list...%s" "done")))))

(defun singular-dynamic-complete ()
  "Dynamic complete word before point.
Performs file name completion if point is inside a string.
Performs completion of Singular help topics if point is at the end of a
help command (\"help\" or \"?\").
Performs completion of Singular examples if point is at the end of an
example command (\"example\").
Otherwise performs completion of Singular commands."
  (interactive)
  ;; Check if we are inside a string.  The search is done back to the
  ;; process-mark which should be the beginning of the current input.
  ;; No check at this point whether there is a process!
  (if (save-excursion
	(nth 3 (parse-partial-sexp (singular-process-mark) (point))))
      ;; then: inside string, thus expand filename
      (comint-dynamic-complete-as-filename)
    ;; else: expand command or help
    (let ((end (point))
	  (post-prompt (save-excursion
			 (beginning-of-line)
			 (singular-prompt-skip-forward)))
	  beg)
      (cond
       ((save-excursion
	  (goto-char post-prompt)
	  (looking-at "[ \t]*\\([\\?]\\|help \\)[ \t]*\\(.*\\)"))
	;; then: help completion
	(if singular-help-topics-alist
	    (singular-completion-do (match-string 2) (match-beginning 2)
				    end singular-help-topics-alist)
	  (message "Completion of Singular help topics disabled.")
	  (ding)))
       ((save-excursion
	  (goto-char post-prompt)
	  (looking-at "[ \t]*\\(example \\)[ \t]*\\(.*\\)"))
	;; then: example completion
	(if singular-examples-alist
	    (singular-completion-do (match-string 2) (match-beginning 2)
				    end singular-examples-alist)
	  (message "Completion of Singular examples disabled.")
	  (ding)))
       (t
	;; else: command completion
	(save-excursion
	  (skip-chars-backward "a-zA-Z0-9")
	  (setq beg (point)))
	(if singular-commands-alist
	    (singular-completion-do (buffer-substring beg end) beg
				    end singular-commands-alist)
	  (message "Completion of Singular commands disabled.")
	  (ding)))))))
;;}}}

;;{{{ Debugging filters
(defun singular-debug-pre-input-filter (string)
  "Display STRING and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Pre-input filter: %s (li %S ci %S lo %S co %S)"
			   (singular-debug-format string)
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start)))
  nil)

(defun singular-debug-post-input-filter (beg end)
  "Display BEG, END, and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Post-input filter: (beg %S end %S) (li %S ci %S lo %S co %S)"
			   beg end
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start))))

(defun singular-debug-pre-output-filter (string)
  "Display STRING and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Pre-output filter: %s (li %S ci %S lo %S co %S)"
			   (singular-debug-format string)
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start)))
  nil)

(defun singular-debug-post-output-filter (beg end simple-sec-start)
  "Display BEG, END, SIMPLE-SEC-START, and some markers in mini-buffer."
  (singular-debug 'interactive-filter
		  (message "Post-output filter: (beg %S end %S sss %S) (li %S ci %S lo %S co %S)"
			   beg end simple-sec-start
			   (marker-position singular-last-input-section-start)
			   (marker-position singular-current-input-section-start)
			   (marker-position singular-last-output-section-start)
			   (marker-position singular-current-output-section-start))))

(defun singular-debug-filter-init ()
  "Add debug filters to the necessary hooks.

This function is called at mode initialization time."
  (add-hook 'singular-pre-input-filter-functions
	    'singular-debug-pre-input-filter nil t)
  (add-hook 'singular-post-input-filter-functions
	    'singular-debug-post-input-filter nil t)
  (add-hook 'singular-pre-output-filter-functions
	    'singular-debug-pre-output-filter nil t)
  (add-hook 'singular-post-output-filter-functions
	    'singular-debug-post-output-filter nil t))
;;}}}

;;{{{ Demo mode

;; Note:
;;
;; For documentation on Singular demo mode one should refer to the doc
;; string of `singular-demo-load'.
;; Singular demo mode should have been implemented as a minor mode but it
;; did not seem worth it.

(defcustom singular-demo-chunk-regexp "\\(\n\\s *\n\\)"
  "*Regular expressions to recognize chunks of a demo file.
If there is a subexpression specified its contents is removed after the
chunk has been displayed.
The default value is \"\\\\(\\n\\\\s *\\n\\\\)\" which means that chunks are
separated by one blank line which is removed after the chunks have been
displayed."
  :type 'regexp
  :initialize 'custom-initialize-default
  :group 'singular-demo-mode)

(defcustom singular-demo-print-messages t
  "*If non-nil, print message on how to continue demo mode."
  :type 'boolean
  :initialize 'custom-initialize-default
  :group 'singular-demo-mode)

(defcustom singular-demo-exit-on-load t
  "*If non-nil, an active demo is automatically discarded when a new one is loaded.
Otherwise, the load is aborted with an error."
  :type 'boolean
  :initialize 'custom-initialize-default
  :group 'singular-demo-mode)

(defcustom singular-demo-load-directory nil
  "*Directory where demo files usually reside.
If non-nil, this directory is offered as a starting point to search for
demo files when `singular-demo-load' is called interactively for the first
time.  (In further calls, `singular-demo-load' offers the directory where
the last demo file has been loaded from as starting point).

If this variable equals nil whatever Emacs offers by default is used as
first-time starting point.  In general, this is the directory where
Singular has been started in."
  :type '(choice (const nil) (file))
  :initialize 'custom-initialize-default
  :group 'singular-demo-mode)

(defvar singular-demo-mode nil
  "Non-nil if Singular demo mode is on.

This variable is buffer-local.")

(defvar singular-demo-old-mode-name nil
  "Used to store previous `mode-name' before switching to demo mode.

This variable is buffer-local.")

(defvar singular-demo-end nil
  "Marker pointing to end of demo file.

This variable is buffer-local.")

(defvar singular-demo-last-directory nil
  "If non-nil, directory from which the last demo file has been loaded.

This variable is buffer-local.")

(defun singular-demo-load (demo-file)
  "Load demo file DEMO-FILE and enter Singular demo mode.

The Singular demo mode allows to step conveniently through a prepared demo
file.  The contents of the demo file is made visible and executed in
portions called chunks.  How the chunks have to be marked in the demo file
is described below.

After loading the demo file with this function, \\[singular-send-or-copy-input] displays the first
chunk of the demo file at the Singular prompt.  This chunk may be modified
\(or even deleted) and then sent to Singular entering \\[singular-send-or-copy-input] as any command
would have been sent to Singular.  The next time \\[singular-send-or-copy-input] is entered, the next
chunk of the demo file is displayed, and so on.

One may interrupt this sequence and enter commands at the Singular input
prompt as usual.  As soon as \\[singular-send-or-copy-input] is entered directly after the input
prompt, the next chunk of the demo file is displayed.  Here is the exact
algorithm how this magic works: If point is located at the very end of the
buffer *and* immediately after Singular's last input prompt, the next chunk
of the demo file is displayed.  In particular, if there is any text after
the last input prompt that text is sent to Singular as usual and no new
chunks are displayed.

After displaying the last chunk of DEMO-FILE, Singular demo mode
automatically terminates and normal operation is resumed.  To prematurely
exit Singular demo mode \\[singular-demo-exit] may be used.

DEMO-FILE should consist of regular Singular commands.  Portions of text
separated by a blank line are taken to be the chunks of the demo file.

There is a number of variables to configure Singular demo mode.  Refer to
the `singular-demo-mode' customization group for more information.

Important note: The unprocessed contents of DEMO-FILE is hidden using
buffer narrowing.  Emacs gets terribly confused when during demo mode the
buffer is either narrowed to some other region or if the buffer is widened.
The safest thing to do if that happens by accident is to explicitly exit
the demo by means of \\[singular-demo-exit] and to try to resume somehow
normal operation.

`singular-demo-load' runs the functions on `singular-demo-mode-enter-hook'
just after demo mode has been entered.  The functions on
`singular-demo-mode-exit-hook' are executed after Singular demo mode has
been exited, either prematurely or due to the end of the demo file.
However, it its important to note that in the latter case the last chunk of
the demo file is still waiting to be sent to Singular."
  (interactive
   (list
    (let ((demo-file-name
	   (cond
	    ;; Emacs
	    ((eq singular-emacs-flavor 'emacs)
	     (read-file-name "Load demo file: "
			     (or singular-demo-last-directory
				 singular-demo-load-directory)
			     nil t))
	    ;; XEmacs
	    (t
	     ;; there are some problems with the window being popped up when this
	     ;; function is called from a menu.  It does not display the contents
	     ;; of `singular-demo-load-directory' but of `default-directory'.
	     (let ((default-directory (or singular-demo-last-directory
					  singular-demo-load-directory
					  default-directory)))
	       (read-file-name "Load demo file: "
			       (or singular-demo-last-directory
				   singular-demo-load-directory)
			       nil t))))))

      (setq singular-demo-last-directory (file-name-directory demo-file-name))
      demo-file-name)))

  ;; check for running demo
  (if singular-demo-mode
      (if singular-demo-exit-on-load
	  ;; silently exit running demo
	  (singular-demo-exit t)
	(error "There already is a demo running, exit with `singular-demo-exit' first")))

  ;; load new demo
  (let ((old-point-min (point-min)))
    (unwind-protect
	(progn
	  (goto-char (point-max))
	  (widen)
	  (cond
	   ;; XEmacs
	   ((eq singular-emacs-flavor 'xemacs)
	    ;; load file and remember its end
	    (set-marker singular-demo-end
			(+ (point) (nth 1 (insert-file-contents-literally demo-file)))))
	   ;; Emacs
	   (t
	    ;; Emacs does something like an `insert-before-markers' so
	    ;; save all essential markers
	    (let ((pmark-pos (marker-position (singular-process-mark)))
		  (sliss-pos (marker-position singular-last-input-section-start))
		  (sciss-pos (marker-position singular-current-input-section-start))
		  (sloss-pos (marker-position singular-last-output-section-start))
		  (scoss-pos (marker-position singular-current-output-section-start)))

	      (unwind-protect
		  ;; load file and remember its end
		  (set-marker singular-demo-end
			      (+ (point) (nth 1 (insert-file-contents-literally demo-file))))

		;; restore markers.
		;; This is unwind-protected.
		(set-marker (singular-process-mark) pmark-pos)
		(set-marker singular-last-input-section-start sliss-pos)
		(set-marker singular-current-input-section-start sciss-pos)
		(set-marker singular-last-output-section-start sloss-pos)
		(set-marker singular-current-output-section-start scoss-pos))))))

      ;; completely hide demo file.
      ;; This is unwind-protected.
      (narrow-to-region old-point-min (point))))

  ;; switch demo mode on
  (setq singular-demo-old-mode-name mode-name
	mode-name "Singular Demo"
	singular-demo-mode t)
  (run-hooks 'singular-demo-mode-enter-hook)
  (if singular-demo-print-messages (message "Hit RET to start demo"))
  (force-mode-line-update))

(defun singular-demo-exit-internal ()
  "Exit Singular demo mode.
Recovers the old mode name, sets `singular-demo-mode' to nil, runs
the hooks on `singular-demo-mode-exit-hook'."
  (setq mode-name singular-demo-old-mode-name
	singular-demo-mode nil)
  (run-hooks 'singular-demo-mode-exit-hook)
  (force-mode-line-update))

(defun singular-demo-exit (&optional no-message)
  "Prematurely exit Singular demo mode.
Cleans up everything that is left from the demo.
Runs the hooks on `singular-demo-mode-exit-hook'.
Does nothing when Singular demo mode is not active."
  (interactive)
  (when singular-demo-mode
    ;; clean up hidden rest of demo file
    (let ((old-point-min (point-min))
	  (old-point-max (point-max)))
      (unwind-protect
	  (progn
	    (widen)
	    (delete-region old-point-max singular-demo-end))
	;; this is unwind-protected
	(narrow-to-region old-point-min old-point-max)))
    (singular-demo-exit-internal)
    (or no-message
	(if singular-demo-print-messages (message "Demo exited")))))

(defun singular-demo-show-next-chunk ()
  "Show next chunk of demo file at input prompt.
Assumes that Singular demo mode is active.
Moves point to end of buffer and widenes the buffer such that the next
chunk of the demo file becomes visible.
Finds and removes chunk separators as specified by
`singular-demo-chunk-regexp'.
Leaves demo mode after showing last chunk.  In that case runs hooks on
`singular-demo-mode-exit-hook'."
  (let ((old-point-min (point-min)))
    (unwind-protect
	(progn
	  (goto-char (point-max))
	  (widen)
	  (if (re-search-forward singular-demo-chunk-regexp singular-demo-end 'limit)
	      (if (match-beginning 1)
		  (delete-region (match-beginning 1) (match-end 1)))
	    ;; remove trailing white-space.  We may not use
	    ;; `(skip-syntax-backward "-")' since newline is has no white
	    ;; space syntax.  The solution down below should suffice in
	    ;; almost all cases ...
	    (skip-chars-backward " \t\n\r\f")
	    (delete-region (point) singular-demo-end)
	    (singular-demo-exit-internal)))

      ;; this is unwind-protected
      (narrow-to-region old-point-min (point)))))

(defun singular-demo-mode-init ()
  "Initialize variables belonging to Singular demo mode.
Creates some buffer-local variables and the buffer-local marker
`singular-demo-end'.

This function is called  at mode initialization time."
  (make-local-variable 'singular-demo-mode)
  (make-local-variable 'singular-demo-mode-old-name)
  (make-local-variable 'singular-demo-mode-end)
  (if (not (and (boundp 'singular-demo-end)
		singular-demo-end))
      (setq singular-demo-end (make-marker)))
  (make-local-variable 'singular-demo-last-directory))
;;}}}

;;{{{ Some lengthy notes on input and output

;; NOT READY[so sorry]!

;;}}}

;;{{{ Last input and output section
(defun singular-last-input-section (&optional no-error)
  "Return last input section.
Returns nil if optional argument NO-ERROR is non-nil and there is no
last input section defined, throws an error otherwise."
  (let ((last-input-start (marker-position singular-last-input-section-start))
	(last-input-end (marker-position singular-current-output-section-start)))
    (cond ((and last-input-start last-input-end)
	   (singular-section-create (singular-simple-sec-at last-input-start) 'input
				    last-input-start last-input-end))
	  (no-error nil)
	  (t (error "No last input section defined")))))

(defun singular-current-output-section (&optional no-error)
  "Return current output section.
Returns nil if optional argument NO-ERROR is non-nil and there is no
current output section defined, throws an error otherwise."
  (let ((current-output-start (marker-position singular-current-output-section-start))
	(current-output-end (save-excursion
			      (save-restriction
				(widen)
				(goto-char (singular-process-mark))
				(singular-prompt-skip-backward)
				(and (bolp) (point))))))
    (cond ((and current-output-start current-output-end)
	   (singular-section-create (singular-simple-sec-at current-output-start) 'output
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
	   (singular-section-create (singular-simple-sec-at last-output-start) 'output
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
	  (process-send-string
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
		      simple-sec-start (singular-simple-sec-last-end-position))

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
;;}}}

;;{{{ Sending input interactive
(defcustom singular-move-on-send 'eob
  "*Where to move point before sending input to Singular.
Should be one of:
`eob' which means to move point to end of buffer,
`eol' which means to move point to end of line, or
 nil  which means to not move point at all."
  :type '(choice (const :tag "End of buffer" eob)
		 (const :tag "End of line" eol)
		 (const :tag "Do not move" nil))
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defun singular-get-old-input (get-section)
  "Get and return old input.
Retrivies on a per-section base if GET-SECTION is non-nil, otherwise on a
per-line base."
  (if get-section
      ;; get input from input section
      (let ((section (singular-section-at (point))))
	(if (eq (singular-section-type section) 'input)
	    (singular-white-space-strip (singular-section-to-string section) t)
	  (error "Not on an input section")))
    ;; get input from line
    (save-excursion
      (beginning-of-line)
      (singular-prompt-skip-forward)
      (let ((old-point (point)))
	(end-of-line)
	(buffer-substring old-point (point))))))

(defun singular-send-or-copy-input (get-section)
  "Send input to Singular.

The behavior of this function very much depends on the current position of
point relative to the process mark, that is, the position, where Singular
expects next input.

If point is located before process mark, old input is copied to the process
mark.  With prefix argument, the whole input section point currently is in
is copied, without prefix argument only the current line.  One should note
that the input is *not* sent to Singular, it is only copied to the process
mark.  Another time entering \\[singular-send-or-copy-input] sends it to Singular.

If point is located after process mark, point is moved as determined by the
`singular-move-on-send' variable: either it is moved to the end of the
current line, or to the end of the buffer, or it is not moved at all.  The
default is to move point to the end of the buffer which most closely
resembles regular terminal behaviour.  At last, the text of the region
between process mark and point is sent to Singular.

Any input to Singular is stored in an input history where it may be
retrieved with \\[comint-previous-input] or \\[comint-next-input], respectively.  For more information on the input
history one should refer to the documentation of
`singular-interactive-mode'.

If Singular demo mode is active and point is at process mark and if that
position is at the end of the buffer the next chunk of the demo file is
displayed.  One should refer to the documentation of `singular-demo-load'
for more information on Singular demo mode.

The Singular process should be running."
  (interactive "P")
  (let ((process (singular-process))
	(pmark (singular-process-mark)))
    (cond
     (;; check for demo mode and show next chunk if necessary
      (and singular-demo-mode
	   (= (point) pmark)
	   (= pmark (point-max)))
      (singular-demo-show-next-chunk))

     (;; get old input
      (< (point) pmark)
      (let ((old-input (singular-get-old-input get-section)))
	(goto-char pmark)
	(insert old-input)))

     (;; send input from pmark to point
      t
      ;; print message if demo mode is active.  We print it before we do
      ;; anything else so that the message will not hide any further
      ;; (error) messages.
      (and singular-demo-mode
	   singular-demo-print-messages
	   (message "Hit RET to continue demo"))

      ;; go to desired position
      (cond ((eq singular-move-on-send 'eol)
	     (end-of-line))
	    ((eq singular-move-on-send 'eob)
	     (goto-char (point-max))))

      (let* ((input (buffer-substring pmark (point))))
	;; insert string into history
	(singular-history-insert input)
	;; send string to process
	(singular-send-string process input)
	;; "insert" it into buffer
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
		      simple-sec-start (singular-simple-sec-last-end-position)

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
NOT READY [in particular: input history!]

NOT READY [multiple Singulars]!

\\{singular-interactive-mode-map}


For \"backward compatibility\" with the terminal version of Singular there
is some extra magic built into Singular interactive mode which catches help
commands issued at the command prompt and executes this function instead.
However, this magic is really not too magic and easily may be fooled.  If
this magic if fooled Singular prints some error message starting like this:

// ** Your help command could not be executed. ...

However, the most common case should be recognized: If one issues a help
command to a non-busy Singular, where the help command comes on one line
and is properly terminated with a semicolon.  Like that:

help ring;

Customization: Entry to this mode runs the hooks on
`singular-interactive-mode-hook'.

NOT READY [much more to come.  See shell.el.]!"
  (interactive)

  ;; uh-oh, we have to set `comint-input-ring-size' before we call
  ;; `comint-mode'
  (singular-history-init)

  ;; run comint mode and do basic mode setup
  (let (comint-mode-hook)
    (comint-mode)
    (singular-comint-init))
  (setq major-mode 'singular-interactive-mode)
  (setq mode-name "Singular Interaction")

  ;; some other initialization found in no folding
  (setq comment-start "// ")
  (setq comment-start-skip "// *")
  (setq comment-end "")

  ;; initialize singular input and output filters.  This should be done
  ;; first as the filters are accessed in the following initialization
  ;; functions.  NOT READY [should be moved to the respective foldings]
  (make-local-variable 'singular-pre-input-filter-functions)
  ;;make-local-hook is obsolete in emacs >=21.1
  ;;(make-local-hook 'singular-post-input-filter-functions)
  (make-local-variable 'singular-pre-output-filter-functions)
  ;;(make-local-hook 'singular-post-output-filter-functions)

  (singular-interactive-mode-map-init)
  (singular-mode-syntax-table-init)
  (singular-interactive-mode-menu-init)
  (singular-demo-mode-init)
  (singular-folding-init)
  (singular-help-init)
  (singular-prompt-init)
  (singular-exec-init)

  ;; Font Lock mode initialization for Emacs.  For XEmacs, it is done at
  ;; singular.el loading time.
  (cond
   ;; Emacs
   ((eq singular-emacs-flavor 'emacs)
    (singular-interactive-font-lock-init)))

  ;; debugging filter initialization
  (singular-debug 'interactive-filter
   (singular-debug-filter-init))

  (run-hooks 'singular-interactive-mode-hook))
;;}}}

;;{{{ Starting singular
(defcustom singular-same-window t
  "*Specifies how to open the window for Singular sessions.
If this variable equals t, Singular comes up in the selected window.
If this variable equals nil, Singular comes up in another window.
If this variable equals neither t nor nil, the standard Emacs behaviour to
open the window is adopted (which very much depends on the settings of
`same-window-buffer-names')."
  :initialize 'custom-initialize-default
  :type '(choice (const :tag "This window" t)
		 (const :tag "Other window" nil)
		 (const :tag "Default" default))
  :group 'singular-interactive-miscellaneous)

(defcustom singular-start-file "~/.emacs_singularrc"
  "*Name of start-up file to pass to Singular.
If the file named by this variable exists it is given as initial input
to any Singular process being started.  Note that this may lose due to
a timing error if Singular discards input when it starts up."
  :type 'file
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defcustom singular-executable-default "Singular"
  "*Default name of Singular executable.
Used by `singular' when new Singular processes are started.
If the name is given without path the executable is searched using the
`PATH' environment variable."
  :type 'file
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defvar singular-executable-last singular-executable-default
  "Singular executable name of the last Singular command used.

This variable is buffer-local.")

(defcustom singular-directory-default nil
  "*Default working directory of Singular buffer.
Should be either nil (which means do not set the default directory) or an
existing directory."
  :type '(choice (const nil) (directory :value "~/"))
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defvar singular-directory-last singular-directory-default
  "Working directory of last Singular command used.

This variable is buffer-local.")

;; no singular-directory-history here. Usual file history is used.

(defcustom singular-switches-default '()
  "*List of default switches for Singular processes.
Should be a list of strings, one string for each switch.
Used by `singular' when new Singular processes are started."
  :type '(repeat string)
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defvar singular-switches-last singular-switches-default
  "Switches of last Singular command used.

This variable is buffer-local.")

(defvar singular-switches-history nil
  "History list of Singular switches.")

; (defvar singular-switches-magic '("-t" "--exec" "if (system(\"version\") > 1304){system(\"--emacs\", 1);};")
(defvar singular-switches-magic '("-t" "--emacs")
  "Additional magic switches for Singular process.
List of switch-strings which are automagically added when new Singular
processes are started, one string for each command line argument.
This list should at least contain the options \"--emacs\" and \"-t\". If
you are running a Singular with version < 1.2 , remove option \"--exec\"
from the list.")

(defcustom singular-name-default "singular"
  "*Default process name for Singular process.
Used by `singular' when new Singular processes are started.
This string surrounded by \"*\" will also be the buffer name."
  :type 'string
  :initialize 'custom-initialize-default
  :group 'singular-interactive-miscellaneous)

(defvar singular-name-last singular-name-default
  "process name of the last Singular command used.

This variable is buffer-local.")

(defvar singular-name-history nil
  "History list of Singular process names.")

(defun singular-exec-init ()
  "Initialize defaults for starting Singular.

This function is called at mode initialization time."
  (singular-debug 'interactive (message "Initializing exec"))
  (set (make-local-variable 'singular-executable-last)
       singular-executable-default)
  (set (make-local-variable 'singular-directory-last)
       singular-directory-default)
  (set (make-local-variable 'singular-name-last)
       singular-name-default)
  (set (make-local-variable 'singular-switches-last)
       singular-switches-default)
  (set (make-local-variable 'singular-exit-insert-killed-marker)
       nil)
  (set (make-local-variable 'singular-exit-cleanup-done)
       nil))

(defvar singular-exit-cleanup-done nil
  "Switch indicating if cleanup after Singular exit is already done.
Initial value is nil. Is set to t by `singular-exit-cleanup' and to nil by
`singular-exit-sentinel'.

This variable is buffer-local.")

(defun singular-exit-cleanup ()
  "Clean up after termination of Singular.
Writes back input ring after regular termination of Singular if process
buffer is still alive, deinstalls the library menu und calls several other
exit procedures.
Assumes that the current buffer is a Singular buffer.
Sets the variable `singular-exit-cleanup-done' to t.

This function is called by `singular-kill-singular' or by
`singular-exit-sentinel'."
  (singular-debug 'interactive
		  (message "exit-cleanup called"))
  (singular-demo-exit t)
  (singular-scan-header-exit)
  (singular-menu-deinstall-libraries)
  (singular-history-write)
  (setq singular-exit-cleanup-done t))

(defun singular-exit-sentinel (process message)
  "Clean up after termination of Singular.
Calls `singular-exit-cleanup' if `singular-exit-cleanup-done' is nil."
  (save-excursion
    (singular-debug 'interactive
		    (message "Sentinel: %s" (substring message 0 -1)))

    (if (string-match "finished\\|exited\\|killed" message)
	(let ((process-buffer (process-buffer process)))
	  (if (and (not singular-exit-cleanup-done)
		   process-buffer
		   (buffer-name process-buffer)
		   (set-buffer process-buffer))
	      (singular-exit-cleanup))))
    (setq singular-exit-cleanup-done nil)))

(defun singular-kill-singular ()
  "Delete the Singular process running in the current buffer.
Calls `singular-exit-cleanup' and deletes the Singular process.
Inserts a string indicating that the Singular process is killed."
  (let* ((process (singular-process))
	 (mark (marker-position (process-mark process))))
    (singular-exit-cleanup)
    (delete-process process)
    (save-excursion
      ;; Because of timing problems it would be better if
      ;; singular-exit-sentinel would insert this string (see Version 1.41)
      ;; but this is not possible for XEmacs: The function (process-mark)
      ;; called within singular-exit-sentinel returns a mark with no
      ;; associated buffer!
      (goto-char mark)
      (insert "// ** Singular process killed **\n"))))

(defun singular-control-c (mode)
  "Interrupt the Singular process running in the current buffer.
If called interactiveley, asks whether to (a)bort the current Singular
command, (q)uit or (r) restart the current Singular process, or (c)ontinue
without doing anything (default).

If called non-interactiveley, MODE should be one of 'abort, 'quit, 'restart,
or 'continue."
  (interactive
   (let (answer)
     (while (not answer)
       (setq answer (read-from-minibuffer
		   "(a)bort current command, (q)uit, (r)estart Singular or (c)ontinue? "))
       (setq answer
	     (cond ((equal answer "a") 'abort)
		   ((equal answer "c") 'continue)
		   ((equal answer "r") 'restart)
		   ((equal answer "q") 'quit)
		   ((equal answer "") 'continue) ; default: continue
		   (t nil))))
     (list answer)))
   (cond
    ((eq mode 'quit) (singular-kill-singular))
    ((eq mode 'restart) (singular-restart))
    ((eq mode 'abort) (interrupt-process (singular-process)))))

(defun singular-exec (buffer name executable start-file switches)
  "Start a new Singular process NAME in BUFFER, running EXECUTABLE.
EXECUTABLE should be a string denoting an executable program.
SWITCHES should be a list of strings that are passed as command line
switches.  START-FILE should be the name of a file which contents is
sent to the process.

Deletes any old processes running in that buffer.
Removes any empty string in SWITCHES before passing to Singular.
Moves point to the end of BUFFER.
Initializes all important markers and the simple sections.
Runs the hooks on `singular-exec-hook'.
Returns BUFFER."
  (let ((old-buffer (current-buffer)))
    (unwind-protect
	(progn
	  (set-buffer buffer)

	  ;; delete any old processes
	  (let ((process (get-buffer-process buffer)))
	    (if process (delete-process process)))

	  ;; create new process
	  (singular-debug 'interactive
			  (message "Starting new Singular: %s %s"
				   executable switches))
	  ;; before passing SWITCHES to Singuar we remove any empty strings
	  ;; because otherwise Singular tries to open a file with an empty
	  ;; file name.
	  (let ((process (comint-exec-1 name buffer
					executable (delete "" switches))))
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

	    ;; completion should be initialized before scan header!
	    (singular-completion-init)
	    (singular-scan-header-init)
            (singular-menu-init)

	    ;; feed process with start file and read input ring.  Take
	    ;; care about the undo information.
	    (if start-file
		(let ((buffer-undo-list t) start-string)
		  (singular-debug 'interactive (message "Feeding start file"))
		  (sleep-for 1)			; try to avoid timing errors
		  (insert-file-contents start-file)
		  (setq start-string (buffer-substring (point) (point-max)))
		  (delete-region (point) (point-max))
		  (process-send-string process start-string)))

	    ;; read history if present
	    (singular-history-read)

	    ;; execute hooks
	    (run-hooks 'singular-exec-hook))

	  buffer)
      ;; this code is unwide-protected
      (set-buffer old-buffer))))


;; TODO: Documentation!
;; Note:
;;
;; In contrast to shell.el, `singular' does not run
;; `singular-interactive-mode' every time a new Singular process is
;; started, but only when a new buffer is created.  This behaviour seems
;; more intuitive w.r.t. local variables and hooks.

(defun singular-internal (executable directory switches name)
  "Run an inferior Singular process, with I/O through an Emacs buffer.

Appends `singular-switches-magic' to switches.
Sets default-directory if directory is not-nil.
Sets singular-*-last values."
  (singular-debug 'interactive
		  (message "singular-internal: %s %s %s %s"
			   executable directory name switches))
  (let* ((buffer-name (singular-process-name-to-buffer-name name))
	 ;; buffer associated with Singular, nil if there is none
	 (buffer (get-buffer buffer-name)))

    ;; If directory is set, make sure that it ends in a "/" at the end.
    ;; The check is done on both slash and backslash, but we unconditionally
    ;; insert a slash. Hopefully that works on NT, too.
    (and directory
	 (not (memq (aref directory (1- (length directory))) '(?/ ?\\)))
	 (setq directory (concat directory "/")))

    (if (not buffer)
	(progn
	  ;; create new buffer and call `singular-interactive-mode'
	  (singular-debug 'interactive (message "Creating new buffer"))
	  (setq buffer (get-buffer-create buffer-name))
	  (set-buffer buffer)
	  (and directory
	       (setq default-directory directory))

	  (singular-debug 'interactive (message "Calling `singular-interactive-mode'"))
	  (singular-interactive-mode)))

    (if (not (comint-check-proc buffer))
	;; create new process if there is none
	(singular-exec buffer name executable
		       (if (file-exists-p singular-start-file)
			   singular-start-file)
		       (append switches singular-switches-magic)))

    ;; pop to buffer
    (singular-debug 'interactive (message "Calling `pop-to-buffer'"))
    (singular-pop-to-buffer singular-same-window buffer))

  ;; Set buffer local singular-*-last-values
  (setq singular-executable-last executable)
  (setq singular-directory-last directory)
  (setq singular-switches-last switches)
  (setq singular-name-last name)
  ;; Set global values, too
  (set-default 'singular-executable-last executable)
  (set-default 'singular-directory-last directory)
  (set-default 'singular-switches-last switches)
  (set-default 'singular-name-last name))

(defun singular-generate-new-buffer-name (name)
  "Generate a unique buffer name for a singular interactive buffer.
The string NAME is the desired name for the singular interactive
buffer, without surrounding stars.
The string returned is surrounded by stars.

If no buffer with name \"*NAME*\" exists, return \"*NAME*\".
Otherwise check for buffer called \"*NAME<n>*\" where n is a
increasing number and return \"*NAME<n>*\" if no such buffer
exists."
  (let ((new-name (singular-process-name-to-buffer-name name))
	(count 2))
    (while (get-buffer new-name)
      (setq new-name (singular-process-name-to-buffer-name
		      (concat name "<" (format "%d" count) ">")))
      (setq count (1+ count)))
    new-name))

(defun singular ()
  "Run an inferior Singular process using default arguments.
Starts a Singular process, with I/O through an Emacs buffer, using the
values of `singular-executable-default', `singular-directory-default',
`singular-switches-default', and `singular-name-default'.

For more information on starting a Singular process and on the arguments
see the documentation of `singular-other'. To restart a previously started
Singular process use `singular-restart'.

Every time `singular' starts a new Singular process it runs the hooks
on `singular-exec-hook'.

Type \\[describe-mode] in the Singular buffer for a list of commands."
  (interactive)
  (singular-internal singular-executable-default
		     singular-directory-default
		     singular-switches-default
		     singular-name-default))

(defun singular-restart ()
  "Run an inferior Singular process using the last arguments used.
Starts a Singular process, with I/O through an Emacs buffer, using the
previously used arguments.
If called within a Singular buffer, uses the arguments of the most recent
Singular process started in this buffer. If there is a Singular process
running in this buffer, it is deleted without warning!
If called outside a Singular buffer, uses the arguments of the most recent
Singular process started in any Singular buffer (and does not delete any
Singular process).
If no last values are available, uses the default values (see documentation
of `singular').

For more information on starting a Singular process and on the arguments
see the documentation of `singular-other'.

Every time `singular-restarts' starts a new Singular process it runs the
hooks on `singular-exec-hook'.

Type \\[describe-mode] in the Singular buffer for a list of commands."
  (interactive)

  (if (singular-process t)
      (singular-kill-singular))

  (singular-internal singular-executable-last
		     singular-directory-last
		     singular-switches-last
		     singular-name-last))

(defun singular-other (executable directory switches name)
  "Run an inferior Singular process.
Starts a Singular process, with I/O through an Emacs buffer.

If called interactively, the user is asked in the minibuffer area for an
existing executable (with or without path), an existing directory or nil
(if non-nil, sets the buffers default directory to this directory), the
complete command line arguments to be passed to Singular (as a single
string) and the buffer name of the singular buffer, which is surrounded by
\"*\", if not already. (The process name of the singular process is then
given by the buffer name with the surrounding stars stripped.)

If called non-interactiveley, EXECUTABLE is the name of an existing
Singular executable (with or without path), DIRECTORY is the name of an
existing directory or nil. If non-nil, sets the buffers default directory
to DIRECTORY. SWITCHES is a list of strings where each string contains one
command line argument which is passed to Singular, and NAME is the process
name of the Singular process (that is, the singular buffer name is given by
NAME surrounded by \"*\").

If buffer exists but Singular is not running, starts new Singular.
If buffer exists and Singular is running, just switches to buffer.
If a file `~/.emacs_singularrc' exists, it is given as initial input.
Note that this may lose due to a timing error if Singular discards
input when it starts up.

If a new buffer is created it is put in Singular interactive mode,
giving commands for sending input and handling output of Singular.  See
`singular-interactive-mode'.

Every time `singular-other' starts a new Singular process it runs the hooks
on `singular-exec-hook'.

Type \\[describe-mode] in the Singular buffer for a list of commands."
  (interactive
   (let* ((exec (read-file-name "Singular executable: "))
	 ;; Remark: Do NOT call `expand-file-name' after the
	 ;; above read-file-name! It has to be possible to enter a command
	 ;; without path which should be searched for in $PATH.
	 ;; `start-process' is intelligent enough to start commands with
	 ;; not-expanded name.
	  (dir (file-name-directory (read-file-name "Default directory: "
						    nil
						    (or singular-directory-default
							default-directory)
						    t)))
	 (switch "")
	 (bufname (singular-generate-new-buffer-name
		   (downcase (file-name-nondirectory exec)))))

     ;; Get command line arguments and append magic switches
     ;; TODO: Think about default value: Up to now:
     ;; Use singular-switches-default as init value for read-from-minibuffer
     (let ((switches-default singular-switches-default))
       (while switches-default
	 (setq switch (concat switch (car switches-default) " "))
	 (setq switches-default (cdr switches-default))))
     ;; note: magic switches are appended by `singular-internal'
     (setq switch (split-string (read-from-minibuffer "Singular options: "
						      switch nil nil
						      singular-switches-history)
				" "))
     ;; Generate new buffer name
     (let (done)
       (while (not done)
	 (setq bufname (read-from-minibuffer "Singular buffer name: " bufname))
	 (setq done (or (not (get-buffer bufname))
			(y-or-n-p "Buffer exists. Switch to that buffer? ")))))
     (if (string-match "^\\*\\(.*\\)\\*$" bufname)
	 (setq bufname (substring bufname (match-beginning 1) (match-end 1))))
     (list exec dir switch bufname)))

  (singular-internal executable directory switches name))

(defun singular-exit-singular (&optional kill-singular-buffer)
  "Delete Singular process and kill Singular buffer.
Deletes the buffers Singular process without warning and writes back the input
history to file.
If called with prefix argument, kills the Singular buffer."
  (interactive "P")
  (singular-debug 'interactive
		  (message "exit singular called"))

  (singular-kill-singular)
  (if kill-singular-buffer
      (kill-buffer (current-buffer))))
;;}}}
;;}}}

(provide 'singular)

;;; Local Variables:
;;; fill-column: 75
;;; End:

;;; singular.el ends here.
