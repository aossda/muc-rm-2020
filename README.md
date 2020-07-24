# muc-rm-2020
MUC's 2020 code

thanks to the contribution of all team members!

ECO algorithm was forked from https://github.com/rockkingjy/OpenTracker



changed the encoding of some files to UTF-8,

and recovered some comments which are lost because of encoding problems when coding (maybe useless?)

# usage

building your own `.so` file recommended.

make sure OpenCV is installed. 

now tracker part only run successfully on Linux, and we still haven't made it an optional function yet. 

put your video in `./video` and rename it as `1.mp4`, or you may change the macro defined at `others/include/Video_Input.h` .

and then build it

```bash
mkdir build
cd build
cmake ..
make
../bin/findblue
```

