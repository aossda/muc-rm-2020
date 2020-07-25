# muc-rm-2020
MUC's 2020 code project: **findblue** 

(maybe it's just a temporary bin file name, but I regard it as poetic. and it can be translated into Chinese as "觅蓝")

thanks to the contribution of all team members!

ECO algorithm was forked from https://github.com/rockkingjy/OpenTracker



changed the encoding of some files to UTF-8,

and recovered some comments which are lost because of encoding problems when coding (maybe useless?)

# usage

building your own `.so` file recommended.

Now tracker part only run successfully on **Linux**, and you are now supposed to **enable** tracking function by calling `ArmorDetect::enable_tracking()`, and **disable** it by calling `ArmorDetect::disable_tracking()` whenever you want. We use a `bool` variable `if_enable_tracking` to decide whether to initialize tracker and then change an `ArmorDetect`'s instance's status. 

Also added a function `ArmorDetect::clean()`. Remember to call it before you feed `armorfind` with a new frame. 

And, you should ...

- make sure OpenCV is installed. 

- put your video in `./video` and rename it as `1.mp4`, or you may change the macro defined at `others/include/Video_Input.h` .

and then build it

```bash
mkdir build
cd build
cmake ..
make
../bin/findblue
```

