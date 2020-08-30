#!/usr/bin/sh

DEPENDENCIES=feh\ xcompmgr\ gcc\ make\ ranger\ dash\ w3m\ w3m-img\ ffmpeg\ libxcb\ libXft\ libX11\ libXft\ libXinerama

download_package()
{
	echo "Downloading dependencies"

	which zypper > /dev/null 2>&1 && { sudo zypper install $DEPENDENCIES; return; }
	which apt-get > /dev/null 2>&1 && { sudo apt-get install $DEPENDENCIES; return; }
	which pacman > /dev/null 2>&1 && { sudo pacman -S $DEPENDENCIES; return; }
	which dnf > /dev/null 2>&1 && { sudo dnf install $DEPENDENCIES; return; }
	echo "Did not found your package manager to check the dependencies"
}  

check_package()
{
	if [ ! -f "/usr/bin/xcompmgr" ]; then
		echo "WARNING : You do not have xcompmgr installed"
		echo "You may have unexpected behaviour"
	fi

	if [ ! -f "/usr/bin/nitrogen" ]; then
		echo "WARNING : You do not have nitrogen installed"
		echo "You may have unexpected behaviour"
	fi

	if [ ! -f "/usr/bin/gcc" ]; then
		if [ ! -f "/usr/bin/cc" ]; then
			echo "ERROR : You do not have gcc or cc to compile the build"
			exit 1
		fi
	fi

	if [ ! -f "/usr/bin/make" ]; then
		echo "ERROR : You do not have make to link the build automatically"
		exit 1
	fi
	
	if [ ! -f "~/.config/wall.png" ];
	then
		echo "WARNING : Did not found a wallpaper in ~/.config/wall.png setting up the default one of this build"
		mv wall.png ~/.config/wall.png
	fi

	if [ -f "/usr/bin/dash" ];
	then
		ln -svf dash /bin/sh
	fi
}

build()
{

	cd dmenu && sh install.sh && cd ..
	cd dwm && sh install.sh && cd ..
	cd st && sh install.sh && cd ..
	cd slstatus && sh install.sh && cd ..
}

main()
{
	download_package
	check_package
	build
}

main
