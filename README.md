# MUC-RM-CV-2020

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5d6d0c0528b4482c873ac9c6d7e2e280)](https://app.codacy.com/manual/tsagaanbar/muc-rm-2020?utm_source=github.com&utm_medium=referral&utm_content=tsagaanbar/muc-rm-2020&utm_campaign=Badge_Grade_Settings)

MUC's 2020 CV project for Robomaster: **findblue** 

(maybe it's just a temporary bin file name, but I regard it kind of poetic.)

thanks to the contribution of all team members!

ECO algorithm was forked from https://github.com/rockkingjy/OpenTracker



## Usage

Now tracker part only run successfully on **Linux**, and you are now supposed to **enable** tracking function by calling `ArmorDetect::enable_tracking()`, and **disable** it by calling `ArmorDetect::disable_tracking()` whenever you want. We use a `bool` variable `if_enable_tracking` to decide whether to initialize tracker and then change an `ArmorDetect`'s instance's status. 

Also added a function `ArmorDetect::clean()`. Remember to call it before you feed `armorfind` with a new frame. 

And, you should ...

- make sure **OpenCV** is installed. 
- install `fftw`
- building your own `.so` file for ecotracker (recommended)
- put your video in `./video` and rename it as `1.mp4`, or you may change the macro defined at `others/include/Video_Input.h` .

and then build it

```bash
mkdir build
cd build
cmake ..
make
../bin/findblue
```



## Notes

when uploaded for the first time:

- changed the encoding of some files to UTF-8

- recovered some lost comments due to encoding problems when coding 