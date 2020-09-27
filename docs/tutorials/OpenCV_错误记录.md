# OpenCV 错误记录

> 创建日期：2020/4/9

## OpenCV 4

1. `IplImage`被包含在 

   ```cpp
   #include "opencv2/imgproc/imgproc_c.h"
   ```

 

2. `CV_RGB2GRAY`等一众宏定义无法被直接调用需包含头文件：`type_c.h`

   ```cpp
   #include <opencv2/imgproc/types_c.h>
   ```

   （在opencv3中应是自动调用的。）

   > 链接跳转：[学长博客](https://blog.csdn.net/weixin_42374556/article/details/105422790)

 

3. 