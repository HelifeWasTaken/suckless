#!/usr/bin/sh

DEPENDENCIES=nitrogen\ xcompmgr\ gcc\ make

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
}

build()
{

	cd dmenu && sh install.sh && cd ..
	cd dwm && sh install.sh && cd ..

	if [ -f "/etc/fedora-release" ]; then
		echo "This slock build is not compatible with fedora but I will download the base one for you"
		sudo dnf install slock -y
	else
		cd slock && sh install.sh && cd ..
	fi

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
