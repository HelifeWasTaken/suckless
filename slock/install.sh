if [ -f "/etc/fedora-release" ]; then
	sed -i -e '/s/nogroup/nobody/g' config.def.h
else
	sed -i -e '/s/nobody/nogroup/g' config.def.h
fi
make && sudo make install && make clean
