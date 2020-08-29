#!/usr/bin/sh

if [ ! -f "/usr/bin/xcompmgr" ]; then
	echo "WARNING : You do not have xcompmgr installed"
	echo "You may have unexpected behaviour"
fi

if [ ! -f "/usr/bin/nitrogen" ]; then
	echo "WARNING : You do not have nitrogen installed"
	echo "You may have unexpected behaviour"
fi

cd dmenu && sh install.sh && cd ..
pwd
cd dwm && sh install.sh && cd ..
pwd

if [ -f "/etc/fedora-release" ]; then
	echo "This slock build is not compatible with fedora but I will download the base one for you"
	sudo dnf install slock -y
else
	cd slock && sh install.sh && cd ..
fi
pwd

cd st && sh install.sh && cd ..
cd slstatus && sh install.sh && cd ..
