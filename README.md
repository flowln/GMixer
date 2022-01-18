# GMixer
Graphical frontend for GStreamer pipeline editing. Still very incomplete, but I'm working on it!
![Current state](https://user-images.githubusercontent.com/9145768/150025718-a3bdaa52-b413-4397-aa96-073aec760b3f.png)

## Dependencies
This project uses [GStreamer](https://gstreamer.freedesktop.org/) and GTK4 (the C++ [gtkmm](https://www.gtkmm.org/en/) bindings to be exact). GStreamer plugins are dynamically discovered. Building is done via [meson](https://mesonbuild.com/), so it should take care of finding all the dependencies in your system, if they are installed. 

## Building from source
On Linux, so the following to build the project in release mode:
```bash
  git clone https://github.com/flowln/GMixer
  cd GMixer
  meson build
  cd build
  meson configure -Dbuildtype=release
  ninja
```
You will end up with a `gmixer` file in the build directory. To install it on your system, you can either copy the binary to `/usr/bin` (or other folter in your PATH variable), or you can do `meson install` right after the `ninja` command. 

## TODO
- Save pipeline to file
- Handle ghost bins (e.g. decodebin's source pads)
- Add keyboard shortcuts (quit, save, import, select node, move node, etc)
- Specialize property edit fields (e.g. checkbox for gboolean properties)
- Improve graph visuals
- Optimize memory usage
- Add DBus support? (for starting pipelines from other processes)
- Generalize basic components (allowing for non-GTK implementations, like a console one)
- Simplify code (since it's my first time using C++ and doing a graphical application, it's highly possible that most things are over-complicated and _bad_)
