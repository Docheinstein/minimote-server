## Minimote Server

Server written in C for the [Minimote Client](https://github.com/Docheinstein/minimote-client-kotlin) that allows basic remote control (mouse and keyboard) of hosts.

Uses libx11 for interact with the X server.

The protocol supports a discover mechanism that allows the client to find active servers on the same network automatically.

### Why

Mostly for control my laptop from my bed without standing up.

### Control functionalities

#### Mouse
* Movement
* Scroll
* Drag & Drop
* Left Click / Left Down / Left Up
* Right Click / Right Down / Right Up
* Middle Click / Middle Down / Middle Up

#### Keyboard
* Unicode typing
* Key Click / Key Down / Key Up
* Hotkey (e.g. Alt+F4, Ctrl+Alt+Canc, ...)


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
