" Vim syntax file
" Language:     Singular
" Maintainer:   Henning Kopp <kopp@mathematik.uni-kl.de>
" Last Change:  2011 Nov 09
"
" HOW TO USE THIS FILE
" Put this file into ~/.vim/syntax/singular.vim or
" in your vim installation directory under syntax/singular.vim
"
" Then you can use the commands :syntax=singular
" or :set filetype=singular to load the syntax highlighting for your
" Singular file
" If you are lazy, you should consider adding
" map <special> <F12> :set filetype=singular<CR>
" to your vimrc. Now you can enable singular syntax by pressing F12
"
" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn sync fromstart
"start highlighting from the start of the document

"COMMENT
syn region      Comment        start="//" end="$"
" stuff behind a // up to the newline
syn region      Comment        start="/\*" end="\*/"
" stuff between /* and */  including newlines

" CONSTANTS
" Strings
syn region      String         start=+"+ end=+"+ contains=infostring
" infostring are special keywords in the library
syn keyword     infostring     AUTHORS OVERVIEW REFERENCES KEYWORDS SEE\ ALSO PROCEDURES USAGE RETURN NOTE ASSUME EXAMPLE LIBRARY contained=String
"contained=String
hi  link        infostring     SpecialComment
" Highlight infostring as if it is a special comment
" Library specific stuff
syn keyword     Label          version category info
"Numbers
syn match       Number         "\d\+"
"Floats
syn match       Float          "[+-]\?\d\+\.\d*" " stuff like -12.05
syn match       Float          "[+-]\?\d\+\.\d*e[+-]\?\d\+" " stuff like +0.3e-2 or even 1.e-02

" IDENTIFIER
" Functions, up to now just basic commands, no library functions
syn keyword     Function        attrib
syn keyword     Function        bareiss betti
syn keyword     Function        char char_series charstr chinrem cleardenom close coef coeffs contract
syn keyword     Function        datetime dbprint defined deg degree delete det diff dim division dump
syn keyword     Function        eliminate eval ERROR example execute extgcd
syn keyword     Function        facstd factmodd factorize farey fetch fglm fglmquot find finduni fprintf freemodule frwalk
syn keyword     Function        gcd gen getdump groebner
syn keyword     Function        help highcorner hilb homog hres
syn keyword     Function        imap impart indepSet insert interpolation interred intersect
syn keyword     Function        jacob janet jet
syn keyword     Function        kbase kernel kill killattrib koszul
syn keyword     Function        laguerre lead leadcoef leadexp leadmonom LIB lift liftstd listvar lres ludecomp luinverse lusolve
syn keyword     Function        maxideal memory minbase minor minres modulo monitor monomial mpresmat mres mstd mult
syn keyword     Function        nameof names ncols npars nres nrows nvars
syn keyword     Function        open option ord ordstr
syn keyword     Function        par pardeg parstr preimage prime primefactors print printf prune
syn keyword     Function        qhweight qrds quote quotient
syn keyword     Function        random rank read reduce regularity repart res reservedName resultant ringlist rvar
syn keyword     Function        setring simplex simplify size slimgb sortvec sqrfree sprintf sres status std stdfglm stdhilb subst system syz
syn keyword     Function        trace transpose type typeof
syn keyword     Function        univariate uressolve
syn keyword     Function        vandermonde var variables varstr vdim
syn keyword     Function        waitall waitfirst wedge weight weightKB write

" STATEMENT
" Conditional
syn keyword     Conditional    if else
" Repeat
syn keyword     Repeat         for while
"Keyword, also known as stuff that fits nowhere else (within statements)
syn keyword     Keyword        break breakpoint continue export exportto importfrom keepring load quit return ~

" TYPE
syn keyword     Type           bigint def ideal int intmat intvec link list map matrix module number package poly proc qring resolution ring string vector pyobject


syn match       Error          "\*/"
" For wrong comments

let b:current_syntax = "singular"
