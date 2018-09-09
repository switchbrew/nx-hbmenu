#### Theming
See the [example](https://github.com/friedkeenan/nx-hbmenu/releases/download/2.0.1/hbtheme.cfg) in the releases for the correct structure. In short, there are two seperate entries for the dark and light system themes, but you only need to make a them for one of them for it to work. If there are any colors missing, the missing color is set to #800080 (purple), and if anything else is wrong the theme won't take effect. You place the hbtheme.cfg file on the root of the SD card and then you should be good to go.

#### Usage
See [Homebrew_Applications](http://switchbrew.org/index.php?title=Homebrew_Applications) for SD layout and applications, etc. See [Switchbrew](http://switchbrew.org/index.php?title=Homebrew_Menu) for hbmenu docs.

#### Download
The latest release is available from the [releases](https://github.com/friedkeenan/nx-hbmenu/releases/latest) page.

#### Building
Build with ```make nx``` or just run ```make```.

#### Credits

* This uses code based on 3DS [new-hbmenu](https://github.com/fincs/new-hbmenu).
* [nanojpeg](https://svn.emphy.de/nanojpeg/trunk/nanojpeg/nanojpeg.c) is used for handling JPEG icons. This library doesn't support lossless JPEG.
