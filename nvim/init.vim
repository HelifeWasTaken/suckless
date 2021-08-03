" File              : .vimrc
" Date              : 26.05.2021
" Last Modified Date: 26.05.2021
call plug#begin('~/.vim/plugged')
Plug 'Heliferepo/VimTek'
Plug 'Heliferepo/VimUtils'
Plug 'mbbill/undotree'
Plug 'neoclide/coc.nvim', {'branch': 'release'}
Plug 'justinmk/vim-syntax-extra'
Plug 'alec-gibson/nvim-tetris'
Plug 'f-person/git-blame.nvim'
Plug 'akinsho/nvim-toggleterm.lua'
Plug 'kyazdani42/nvim-tree.lua'
Plug 'preservim/nerdcommenter'
Plug 'jbyuki/instant.nvim'
Plug 'sakhnik/nvim-gdb', { 'do': ':!./install.sh' }
Plug 'junegunn/fzf', { 'do': { -> fzf#install() } }
Plug 'alpertuna/vim-header'
call plug#end()

let mapleader = " "

nnoremap <leader>u :UndotreeShow<CR><bar>:wincmd h<CR>

if has('persistent_undo')
	silent !mkdir ~/.vim/undodir > /dev/null 2>&1
	set undodir=$HOME/.vim/undodir
	set undolevels=5000
	set undofile
endif

set nobackup
set noswapfile

function! SetupCommandAbbrs(from, to)
	exec 'cnoreabbrev <expr> '.a:from
				\ .' ((getcmdtype() ==# ":" && getcmdline() ==# "'.a:from.'")'
				\ .'? ("'.a:to.'") : ("'.a:from.'"))'
endfunction

autocmd FileType json syntax match Comment +\/\/.\+$+
let g:coc_global_config="$HOME/.config/nvim/coc-settings.json"

call SetupCommandAbbrs('C', 'CocConfig')
function! s:check_back_space() abort
  let col = col('.') - 1
  return !col || getline('.')[col - 1]  =~ '\s'
endfunction

inoremap <expr><cr> pumvisible() ? "\<C-y>" : "\<C-g>u\<CR>"
inoremap <silent><expr> <Tab>
      \ pumvisible() ? "\<C-n>" :
      \ <SID>check_back_space() ? "\<Tab>" :
      \ coc#refresh()

imap <ESC> <nop>
inoremap jj <ESC>
inoremap jk <ESC>
inoremap kj <ESC>
inoremap kk <ESC>

nnoremap v <C-V>
nnoremap <C-V> v
xnoremap v <C-V>
xnoremap <C-V> v

set nowrap

set clipboard+=unnamedplus

nnoremap <leader>h :wincmd h<CR>
nnoremap <leader>j :wincmd j<CR>
nnoremap <leader>k :wincmd k<CR>
nnoremap <leader>l :wincmd l<CR>

let g:toggleterm_terminal_mapping = '<C-j>'
autocmd TermEnter term://*toggleterm#*
      \ tnoremap <silent><c-j> <C-\><C-n>:exe v:count1 . "ToggleTerm"<CR>

inoremap <silent><c-j> <Esc>:<c-u>exe v:count1 . "ToggleTerm"<CR>
nnoremap <silent><c-j> <Esc>:<c-u>exe v:count1 . "ToggleTerm"<CR>

set hidden

let g:nvim_tree_show_icons = { 'git': 1 }
let g:nvim_tree_icons = {
    \ 'git': {
    \   'unstaged': "[UNSTAGED] ",
    \   'staged': "[STAGED] ",
    \   'unmerged': "[UMERGE] ",
    \   'renamed': "[RENAME] ",
    \   'untracked': "[UNTRACKED] ",
    \   'deleted': "[DELETED] ",
    \   'ignored': "[IGNORED] "
    \   }
    \ }

nnoremap <C-n> :NvimTreeToggle<CR>
"set termguicolors " this variable must be enabled for colors to be applied properly

nnoremap <C-f> :FZF<CR>
set wildignore+=*.a,*.o,*.so,*.pyc,.git
set wildignore+=*.tmp,*.swp

nmap <silent> gd <Plug>(coc-definition)
nmap <silent> gD <Plug>(coc-implementation)
nmap <silent> gr <Plug>(coc-references)

let g:header_field_author = 'Mattis DALLEAU'
let g:header_field_author_email = 'mattisdalleau@gmail.com'
let g:header_field_timestamp = 1
let g:header_auto_add_header = 0
