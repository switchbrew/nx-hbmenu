### Usage
See [Homebrew_Applications](https://switchbrew.org/wiki/Homebrew_Applications) for SD layout and applications, etc. See [Switchbrew](https://switchbrew.org/wiki/Homebrew_Menu) for hbmenu docs.

### Download
The latest release is available from the [releases](https://github.com/switchbrew/nx-hbmenu/releases/latest) page.

### Building
Build for the Nintendo Switch with ```make nx``` and for the PC with ```make pc```.
Running ```make``` builds for both systems.

The following [pacman packages](https://devkitpro.org/wiki/devkitPro_pacman) are required to build:
- `switch-dev`
- `switch-freetype`
- `switch-libconfig`
- `switch-libjpeg-turbo`
- `switch-zlib`
- `zip` (on Windows)

For the PC build, `libminizip` is required (for the Switch build, the `switch-zlib` package includes this).

Since C11 threads are used, building for the PC may fail if C11 threads are not available.

#### Credits

* This uses code based on 3DS [new-hbmenu](https://github.com/fincs/new-hbmenu).
* `libjpeg-turbo` is used for handling JPEG icons. This library doesn't support lossless JPEG (likewise for official sw which uses `libjpeg-turbo`).
