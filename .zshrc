export ZSH="/home/mattis/.oh-my-zsh"

ZSH_THEME="robbyrussell"

plugins=(git vi-mode zsh-autosuggestions zsh-syntax-highlighting)

source $ZSH/oh-my-zsh.sh

if [[ -n $SSH_CONNECTION ]]; then
	export EDITOR='nvim'
fi

transfer(){ if [ $# -eq 0 ];then echo "No arguments specified.\nUsage:\n  transfer <file|directory>\n  ... | transfer <file_name>">&2;return 1;fi;if tty -s;then file="$1";file_name=$(basename "$file");if [ ! -e "$file" ];then echo "$file: No such file or directory">&2;return 1;fi;if [ -d "$file" ];then file_name="$file_name.zip" ,;(cd "$file"&&zip -r -q - .)|curl --progress-bar --upload-file "-" "http://transfer.sh/$file_name"|tee /dev/null,;else cat "$file"|curl --progress-bar --upload-file "-" "http://transfer.sh/$file_name"|tee /dev/null;fi;else file_name=$1;curl --progress-bar --upload-file "-" "http://transfer.sh/$file_name"|tee /dev/null;fi;}
alias vi="nvim"
alias vim="vi"
alias rls="clear && ls"
alias cat="bat"
mytmp() {if [ $# -eq 0 ];then;echo "No arguments specified.";return 1;fi;mkdir -p ~/test;mv $1 ~/test}
export FZF_DEFAULT_COMMAND='find * -type f \( ! -iname "*.o" \)'
export PATH="$PATH:/home/mattis/.cargo/bin"
