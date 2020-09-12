#!/usr/bin/sh

DEPENDENCIES=feh\ xcompmgr\ gcc\ make\ ranger\ dash\ w3m\ w3m-img\ libxcb\ libXft\ libX11\ libXft\ libXinerama-devel\ libXinerama\ libX11-devel\ libXft-devel\ alsa-lib-devel

download_package()
{
	echo "Downloading dependencies"

	which zypper > /dev/null 2>&1 && { sudo zypper install $DEPENDENCIES; return; }
	which apt-get > /dev/null 2>&1 && { sudo apt-get install $DEPENDENCIES; return; }
	which pacman > /dev/null 2>&1 && { sudo pacman -S $DEPENDENCIES; return; }
	which dnf > /dev/null 2>&1 && { sudo dnf install $DEPENDENCIES; return; }
	echo "Did not found your package manager to check the dependencies"
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
}

check_optional()
{
	if [ ! -f "/usr/bin/feh" ]; then
		echo "WARNING : You do not have feh installed"
		echo "You may experience unexpected behaviour"
	fi
		
	if [ ! -f "/usr/bin/ffmpeg" ]; then
		echo "WARNING : You do not have ffmpeg installed"
		echo "You may experience unexpected behaviour"
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
	touch ~/.xinitrc
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
	check_essential
	check_optional
	build
	create_xinitrc
}

main
