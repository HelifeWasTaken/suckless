#!/bin/bash
# File              : install.sh
# Author            : Mattis DALLEAU <mattisdalleau@gmail.com>
# Date              : 29.05.2021
# Last Modified Date: 29.05.2021
# Last Modified By  : Mattis DALLEAU <mattisdalleau@gmail.com>

DEPENDENCIES=feh\ xcompmgr\ gcc\ make\ ranger\ dash\ w3m\ w3m-img\ libxcb\ libXft\ libX11\ libXft\ libXinerama-devel\ libXinerama\ libX11-devel\ libXft-devel\ alsa-lib-devel

download_package()
{
	echo "Downloading dependencies"

	return { sudo dnf install $DEPENDENCIES; }
}

build_vim_package()
{
	echo "Downloading (n)vim packages"

	sudo dnf copr enable agriffis/neovim-nightly
	sudo dnf install neovim
	mkdir -p ~/.config && cp -r nvim ~/.config
}

check_essential()
{
	if [ ! -f "/usr/bin/xcompmgr" ]; then
		echo "ERROR : You do not have xcompmgr installed"
		exit 1
	fi

	if [ ! -f "/usr/bin/gcc" ]; then
		if [ ! -f "/usr/bin/cc" ]; then
			echo "ERROR : You do not have gcc or cc to compile the build"
			exit 1
		fi
	fi

	if [ ! -f "/usr/bin/make" ]; then
		echo "ERROR : You do not have make to compile the build"
		exit 1
	fi

	if [ ! -f "/usr/bin/dash" ]; then
		echo "ERROR : You do not have Dash to complete the build"
		exit 1
	fi
}

check_optional()
{
	if [ ! -f "/usr/bin/feh" ]; then
		echo "WARNING : You do not have feh installed"
		echo "Background might not be able to show up"
	fi

	if [ ! -f "/usr/bin/ffmpeg" ]; then
		echo "WARNING : You do not have ffmpeg installed"
		echo "You may experience unexpected behaviour with videos"
	fi

	if [ ! -f "~/.config/wall.png" ]; then
		echo "WARNING : Did not found a wallpaper in ~/.config/wall.png setting up the default one of this build"
		cp wall.png ~/.config/wall.png
	fi

	if [ -f "/usr/bin/dash" ]; then
		ln -svf dash /bin/sh
	else
		echo "Consider using dash for faster results with #!/bin/sh"
	fi
}

create_xinitrc()
{
	if [ -f "~/.xinitrc" ]; then
		echo "Overriding ~/.xinitrc setting the original one as ~/.xinitrc_backup"
		mv ~/.xinitrc ~/.xinitrc_backup
	fi
	echo -e "exec slstatus &\nexec dwm" > ~/.xinitrc
}

build()
{
	cd dmenu && sh install.sh && cd ..
	cd dwm && sh install.sh && cd ..
	cd st && sh install.sh && cd ..
	cd slstatus && sh install.sh && cd ..
	cd add_to_xsession/ && sudo make install && cd ..
}

main()
{
	download_package
	if [[ $? -ne 0 ]]; then; exit 1; fi
	check_essential
	build_vim_package
	check_optional
	build
	create_xinitrc
}

main
