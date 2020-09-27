# OpenCV-MinGW环境配置

> 创建日期：2020/3/13
>
> 修改日期：2020/3/22



---

参考链接：

[msys2配置记录 - 简书](https://www.jianshu.com/p/c740b71e7775)

[Windows下使用msys2打造OpenCV开发环境 - 简书](https://www.jianshu.com/p/038ceee1c7af)

---



自行编译的方法：

一、预先安装与准备软件：

(1) 64位Windows系统（XP系统以上）

(2) **MSYS2**（模拟Linux环境命令行）

​     官网地址：[http://www.msys2.org/](http://www.msys2.org/)

​     本次下载的是`msys2-x86_64-xxxxx.exe`

​     建议使用迅雷或者比特彗星等进行下载。

​     （如未使用外网或者校园网，建议断开网络安装，避免卡死在  “==>Update trust Database”处）



1. 双击`msys2-x86_64-20180531.exe`，并点击下一步选择安装文件夹

   路径要短,不能有空格,只能用ASCII字符,不能用单引号

   然后下一步直到开始安装

2. 安装完成后点击 完成 退出安装程序

   注意：此处不要勾选"立即运行 MSYS2 64bit", 因为接下来要配置国内更新源

3. 配置国内更新源

   这里选用的是中科大的源,也可以搜索其他的源,如阿里云,清华大学等的源

   修改`安装位置\etc\pacman.d`中的三个配置文件

   ```bash
   mirrorlist.msys
   
   mirrorlist.mingw32
   
   mirrorlist.mingw64
   ```

   务必放在配置文件的第一个网页处上！(建议全部换源，因为网络不好时会切换到下一个网站)

   在`mirrorlist.msys`中添加

   ```
   Server = http://mirrors.ustc.edu.cn/msys2/msys/$arch/
   Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/msys/$arch
   ```

   在`mirrorlist.mingw32`中添加

   ```
   Server = http://mirrors.ustc.edu.cn/msys2/mingw/i686/ 
   Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/mingw/i686/
   ```

   在`mirrorlist.mingw64`中添加

   ```
   Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/mingw/x86_64
   Server = http://mirrors.ustc.edu.cn/msys2/mingw/x86_64/
   ```

   

4. 打开菜单中的 **MSYS2 MinGW 64-bit**

   习惯了打开64位的图标,所以此处打开 **MSYS2 MinGW 64-bit**，以后的使用也一直使用 **MSYS2 MinGW 64-bit** 就好了

   安装需要的包：

   ```bash
   pacman -S gcc make 
   pacman -S mingw-w64-x86_64-gcc
   ```

   非必要：

   ```bash
   pacman -S base-devel git mercurial cvs wget p7zip perl ruby python2
   pacman -S mingw-w64-x86_64-toolchain
   pacman -Syu
   ```

   

执行命令：

```
mount -fo binary,noacl,posix=0,user <挂载文件所在地址 为主目录> <挂载后文件夹名>
```

如 :

```bash
mount -fo binary,noacl,posix=0,user E:\SourceFile\OpenSource\ \opensource
```

就可简单挂载目录



OpenCV版本包以及对应的contrib包

>OpenCV代码地址为https://github.com/opencv/opencv/
>
>3.4.1：https://github.com/opencv/opencv/tree/3.4.1
>
>opencv_contrib代码地址是：https://github.com/opencv/opencv_contrib
>
>3.4.1版本代码地址是：https://github.com/opencv/opencv_contrib/tree/3.4.1
>
>如果连接太慢，建议使用gitee码云fork连接到github网络



