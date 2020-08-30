# Helife suckless 0.9

This is my package of all my suckless softwares
> There might be some other software that might not be directly related to suckless softwares in the future
> But I try to keep the same philosophy of minimalism

## Dependencies :

To compile :

- Make
- cc or gcc
- base-devel 
> (no need for now but I might add python or other stuff in it to be sure)

Apps that I use at dwm startup : 
- feh (Gives me my background)
- xcompmgr (Enable RGBA)

Dependencies are automatically built with Fedora 30 and higher I still need to check for the other distributions

In order to change the wallpaper you can change the path or command in `suckless/dwm/dwm.c` search for `system ("feh --bg-fill ~/.config/wall.png &");` or just change/add the image and reload the wm
