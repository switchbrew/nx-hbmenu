#### Usage
See [Homebrew_Applications](http://switchbrew.org/index.php?title=Homebrew_Applications) for SD layout and applications, etc. See [Switchbrew](http://switchbrew.org/index.php?title=Homebrew_Menu) for hbmenu docs.

#### Download
The latest release is available from the [releases](https://github.com/switchbrew/nx-hbmenu/releases/latest) page.

#### Building
Build with ```make nx``` or just run ```make```.

The following is required to build: libfreetype (switch-freetype), libconfig (switch-libconfig), and libjpeg-turbo (switch-libjpeg-turbo). Where "({name})" is the pacman package.

C11-threads are used, hence building for the pc-build may fail if C11-threads are not available.

#### Credits

* This uses code based on 3DS [new-hbmenu](https://github.com/fincs/new-hbmenu).
* libjpeg-turbo is used for handling JPEG icons. This library doesn't support lossless JPEG (likewise for official sw which uses libjpeg-turbo).
