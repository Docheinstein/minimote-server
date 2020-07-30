## Minimote Server

Server for the Minimote Client that allows remote control of Linux hosts.  
Written in C.  
Uses libx11 for interact with the X server.

### Why

So that I can use the Minimote Client for perform basic actions from my bed.

### Supported functionalities

1. Mouse movement
2. Unicode keys typing (limited to characters in the current keyboard layout)
3. Hotkeys (e.g. Alt+F4, Ctrl+Alt+Canc, ...)

### Requirements
* glib2
* libx11
* libxtst

*Arch Linux*
```
sudo pacman -Syu glib2 libx11 libxtst
```

*Debian/Ubuntu*
```
sudo apt install libglib2.0-dev libx11-dev libxtst-dev
```

### Build

```
mkdir build
cd build
cmake ..
make
```
