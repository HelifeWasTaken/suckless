#!/usr/bin/sh

if [ ! -f "/usr/bin/xcompmgr" ]; then
	echo "WARNING : You do not have xcompmgr installed"
	echo "You may have unexpected behaviour"
fi

if [ ! -f "/usr/bin/nitrogen" ]; then
	echo "WARNING : You do not have nitrogen installed"
	echo "You may have unexpected behaviour"
fi

sh dmenu/install.sh
sh dwm/install.sh

if [ -f "/etc/fedora-release" ]; then
	echo "This slock build is not compatible with fedora but I will download the base one for you"
	sudo dnf install slock -y
else
	sh slock/install.sh
fi

sh st/install.sh
sh slstatus/install.sh
