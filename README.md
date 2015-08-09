vbeterm
=======

Custom terminal based on VTE. There are many terminals available. Many
of them are based on VTE, a library implementing a terminal emulator
widget for GTK+. Some are highly configurable, some are small but none
of them match exactly my expectations.

Here is a short survey:

 - [ROXterm](http://roxterm.sourceforge.net/)
 - [Sakura](http://pleyades.net/david/sakura)
 - [Termit](http://github.com/nonstop/termit/wiki)
 - [evilvte][]
 - [gnome-terminal](http://en.wikipedia.org/wiki/gnome_terminal)
 - [lilyterm](http://lilyterm.luna.com.tw/)
 - [st](http://st.suckless.org/)
 - [xfce-terminal](http://www.xfce.org/projects/terminal/)
 - [sterm][]

`vbeterm` is really tailored to my need. You are unlikely to find it
useful. [sterm][] is a derivative with a bit more flexibility. Another
close alternative with a ability to configure before compiling is
[evilvte][].

If you plan to write your own, have a look at the exsting one. It is
likely that [evilvte][] will support what you want. If it is too
"bloated" for your taste, you can start from the
[minimal demo](http://www.calno.com/evilvte/demo.c) from the same
author. If this demo is too simplistic, you can also have a look at
[VTE's own demo](https://github.com/GNOME/vte/blob/master/src/app.vala),
written in Vala.

[sterm]: https://github.com/pyr/sterm
[evilvte]: http://www.calno.com/evilvte/

Features
--------

 - No tab support
 - Use of VTE 2.90 (GTK3)
 - Single instance managing several terminals.
 - dabbrev-expand (mapped on `Alt-/`)

Installation
------------

Execute the following commands:

    $ ./configure
    $ make
    $ sudo make install
