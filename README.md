## Minimote Server

Server written in C for the [Minimote Client](https://github.com/Docheinstein/minimote-client) that allows basic remote control (mouse and keyboard) of hosts.

Works either with X11 or Wayland.

Uses raw linux input /dev/uinput (requires root).

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
* pkg-config
* libxkbcommon

Only for X11:
* libxkbcommon-x11

Only for Wayland:
* libwayland

### Build

```
mkdir build
cd build
cmake ..
make
```

### Run

The server must be started with root privileges in order to interact with uinput.

Furthermore, the user environment should be preserved (`sudo -E`) for access display server information (e.g. keymap).

```
sudo -E ./minimote-server
```

### Options

```
Usage: minimote-server [OPTION...] 
Allow remote keyboard and mouse control of a Linux host

  -m, --mouse-sensibility=SENSIBILITY
                             Mouse sensibility (default: 1.5)
  -p, --port=PORT            Port (default: 50500)
  -s, --scroll-sensibility=SENSIBILITY
                             Scroll sensibility (default: 1)
  -S, --scroll-reverse       Reverse scroll direction
  -v, --verbose              Print more information
  -w, --wayland              Use Wayland as display server
  -x, --x11                  Use X11 as display server
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
```