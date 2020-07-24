# Mindvision工业摄像头配置

2017级 徐辉灿

由于没看到网上有类似的教程，节约一下大家时间。

## Visual Studio 2019安装教程：

登录**迈德威视**官网 http://www.mindvision.com.cn/index.aspx

进入"服务支持"->"文档下载"/"软件下载"

建议下载文档与软件。

软件下载：（windows包）[http://www.mindvision.com.cn/uploadfiles/SDK/MindVision%20Camera%20Platform%20Setup(2.1.10.124).exe](http://www.mindvision.com.cn/uploadfiles/SDK/MindVision Camera Platform Setup(2.1.10.124).exe)

然后进行安装。记住安装位置。

打开Visual Studio 2019，

在属性管理器中：

新建一个属性表，以便之后再次调用。

和opencv配置比较类似：

1. 打开属性表，选择“**包含目录**”并添加：

   ```
   <你的windows包安装位置>\Demo\VC++\OpenCV\Include
   ```

   如：

   ```
   E:\SourceFile\MindVision\Demo\VC++\OpenCV\Include
   ```

2. 打开库目录并添加：

   ```
   <你的windows包安装位置>\MindVision\Demo\VC++\OpenCV
   ```

   如：

   ```
   E:\SourceFile\MindVision\Demo\VC++\OpenCV
   ```

   

3. 打开 **链接器** -> **输入**：

   添加：`MVCAMSDK.lib`、`MVCAMSDK_X64.lib`

   注意，以英文分号`;`为间隔。

   之后就配置好了。

 

4. 测试时可以试试直接运行模板，也可以打开：

   ```
   <你的windows包安装位置>\Demo\VC++\OpenCV\
   ```

   主要看编译是否有问题。



## Linux配置教程：

1. 下载好官网的或我修改过demo的sdk包

2. 解压

   ```bash
   tar -xzvf mimd***
   ```

   

3. 进入文件夹

   ```bash
   sudo install.sh
   ```

4. **（这一步对于Jetson开发才需要，正常的PC链接库就不用替换了）**更改so文件，由于`install.sh`复制的都是`x86`或`x64`平台的链接库，这里改成`arm64`的

   命令行输入

   ```bash
   sudo rm /lib/libMVSDK.so
   sudo cp lib/arm64/libMVSDK.so /lib
   ```

   至此已经安装完成！以下demo自选做不做

5. 运行demo

   由于nano是qt5，所以有些代码要改，这里改好了，还有gtk要安装依赖

   1. 安装gtk

      ```bash
      sudo apt-get install build-essential
      sudo apt-get install pkg-config
      sudo apt-get install glade libglade2-dev
      sudo apt-get install libgtk2.0-dev
      ```

      

   2. GTK_Demo编译

      ```bash
      cd demo
      cd GTK_Demo
      ./configure
      make
      ```

      

   3. 运行

      在windows或其他电脑上配置相机ip，nano也配置成同一网段就行了

      将相机接上nano网口

      ```bash
      ./GTK_demo
      ```

   4. 其他例子编译,进入相应例子目录

      如OpenCV

      ```bash
      make
      ./main
      ```

      如QT_Demo

      ```bash
      qmake
      make
      ./QT_Demo
      ```

      如Sampli_Save_Demo

      ```bash
      ./configure
      make
      ./demo
      ```

