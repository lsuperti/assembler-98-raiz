
" Language: A9R

if !exists("main_syntax")
  " quit when a syntax file was already loaded
  if exists("b:current_syntax")
    finish
  endif
  let main_syntax = 'a9r'
elseif exists("b:current_syntax") && b:current_syntax == "a9r"
  finish
endif

au BufRead,BufNewFile *.a9r set filetype=a9r

syntax clear

syn keyword INSTRUCTIONS   LOAD READ ADD STORE BRZERO BRPOS BRNEG WRITE STOP 
syn keyword INSTRUCTIONS   COPY MULT SUB DIV BR CALL PUT RET
syn keyword RESERVED       DATA_REG global STACK GLOBAL

syn match   SUM             '+' 
syn match   SUB             '-' 
syn match   MULT            '*' 
syn match   DIV             '/' 
syn keyword KEYWORDS        section 
syn match   COMMENTS        "--.*$"
syn match   STRINGS         '".*"' 
syn match   IDENTIFIERS     "\v[a-zA-Z_][a-zA-Z0-9_]*"
syn match   DOT_IDENTIFIERS "\v\.[a-zA-Z_][a-zA-Z0-9_]*"
syn match   DIRECT          "&" 
syn match   INDIRECT        "#" 
syn match   LABEL           "[a-zA-Z_][a-zA-Z0-9_]*:"
syn match   NUMBERS         "\<[0-9]\+\(\.[0-9]*\)\?\>" 
syn region  BLOCKCOMMENT   start="\*-" end="-\*" keepend

hi SUM              ctermfg=blue guifg=blue
hi SUB              ctermfg=blue guifg=blue
hi MULT             ctermfg=blue guifg=blue
hi DIV              ctermfg=blue guifg=blue
hi INSTRUCTIONS     ctermfg=lightred guifg=lightred
hi COMMENTS         ctermfg=gray guifg=gray
hi KEYWORDS         ctermfg=lightblue guifg=lightblue
hi STRINGS          ctermfg=green guifg=green
hi DOT_IDENTIFIERS  ctermfg=magenta guifg=magenta
hi IDENTIFIERS      ctermfg=lightyellow guifg=lightyellow
hi DIRECT           ctermfg=lightmagenta guifg=lightmagenta
hi INDIRECT         ctermfg=lightcyan guifg=lightcyan
hi LABEL            ctermfg=lightgray guifg=lightgray
hi RESERVED         ctermfg=red guifg=red
hi NUMBERS          ctermfg=lightgray guifg=lightgray
hi BLOCKCOMMENT    ctermfg=gray guifg=gray

