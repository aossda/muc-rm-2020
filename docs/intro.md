# 视觉模板-2020-总工程介绍

2017级 徐珲灿



## 一、总工程介绍：

总工程大致结构图：https://www.processon.com/view/link/5eb8aa6b0791290fe0550e37

<img src=".\images\main-structure.jpg" alt="main-structure"  />

总工程预计**两个线程**进行，结构如图所示。

运行流程图预计如图所示：https://www.processon.com/view/link/5eb8a9e37d9c08156c331a89

<img src=".\images\program-flow-chart.jpg" alt="program-flow-chart" style="zoom:50%;" />

（注：两图使用ProcessOn在线工具完成）

```
muc-rm-2020
│
│  CMakeLists.txt
│  main.cpp
│  main_single.cpp
│  README.md
│  
├─armor //装甲识别
│  ├─include
│  │      ArmorClass.h
│  │      Armor_Detect.h
│  │      Img_Manage.h
│  │      Num_Select.h
│  │      
│  └─src
│          Armor_Detect.cpp
│          Img_Manage.cpp
│          Num_Select.cpp
│          
├─eco //ECO算法相关，详细略
│              
├─energy //能量机关
│  ├─include
│  │      Energy.h
│  └─src
│          Energy.cpp
│          
├─lib
│      libecotracker.so
│      libecotracker_single.so
│      libMVSDK.so
│      MVCAMSDK.lib
│      MVCAMSDK_X64.lib
│      
├─others
│  ├─include
│  │      CameraApi.h
│  │      CameraDefine.h
│  │      CameraStatus.h
│  │      Camera_Input.h
│  │      config.h
│  │      Image_Input.h
│  │      Mes_Manage.h
│  │      Video_Input.h
│  │      
│  └─src
│          Camera_Input.cpp
│          Mes_Manage.cpp
│          Video_Input.cpp
│          
└─xml
        2020svm_v2_123458.xml
```



## 二、构成的各种大模块介绍：

（会对核心算法进行讲解）

### armor

```
├─armor
│  ├─include
│  │      ArmorClass.h
│  │      Armor_Detect.h
│  │      Img_Manage.h
│  │      Num_Select.h
│  │      
│  └─src
│          Armor_Detect.cpp
│          Img_Manage.cpp
│          Num_Select.cpp
```

#### 1. Img_Manage.h

处理获取的图像,进行各种图像预处理。最终获取到二值图与其对应的黑白灯条组。

有两个组成部分：

1. **总控制函数**

   ```cpp
   void Pre_Manage(cv::Mat &input);
   ```

2. **图像预处理，进行开闭运算，获取三种输入图像**

   ```cpp
   void input_operation(cv::Mat& input, cv::Mat& BinaryImg, cv::Mat& colorImg);
   ```
   
   - 目的是生成两个过程图：`BinaryImg`和`colorImg`。
   
   - 对原图像分为三通道split，为检测红色，将r通道减去b通道（检测蓝色则相反：将b通道减去r通道），得到的结果再进行二值化处理得到二值图（负值自动归零）。
   
   - 对图像进行灰度处理bgr2gray，然后进行归一化normalize。对归一化后图像进行二值化处理，并进行一次闭运算，除去近距离的小黑洞（务必不要使用开运算，删去远距离灯条！）。
   - 流程结束。
   

3. **获取部分轮廓，并使用过程图区分红蓝**

   ```cpp
   cv::Mat get_dstimg(cv::Mat& input, cv::Mat& BinaryImg, cv::Mat& colorImg);
   ```

   1. 寻找两个过程图`BinaryImg`和`colorImg`的轮廓，并使用下面算法进行筛选灯条。

      ```cpp
      Void lightExam(vector<vector<Point> > fgcontours, vector<vector<Point> > bluecontoursl, Mat* dst, Mat fgImg);
      ```

      `binarycontours`是找到的所有颜色灯条结果，`colorcontours`是`colorImg`找到的通道相减轮廓。

   2. 进行`for`循环遍历，使用了omp的`for`循环加速，效果没有最初（去年时）明显，可能有继续优化的方法。

   3. 遍历所有轮廓，并进行&运算，将运算后轮廓结果与`binaryimg`的轮廓进行匹配，相同则写入新的结果图像中。

   关于`rect`类的运用，可看这篇：https://blog.csdn.net/qq_41204464/article/details/89736793

   使用到的函数：

   ```cpp
   bool isInside(Rect rect1, Rect rect2)
   {
       return (rect1 == (rect1&rect2));
       //return (rect1 == (rect1&rect2))||(rect2 == (rect1&rect2));
   }
   ```

   **注释的一条是新修改的，因为在查找过程中可能因为提取红色的原因出现匹配失误，可通过让红色区在内的形式去进行匹配。（可能存在bug，未在实地测试过，录像效果还不错）。**

   

   在运算符的问题上，可通过对有疑问的运算符使用"**go to...**"选项去寻找详细定义；比如`Rect`类之间的运算符有：

   ```cpp
   template<typename _Tp> static inline
   bool operator == (const Rect_<_Tp>& a, const Rect_<_Tp>& b)
   {
       return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
   }
   
   template<typename _Tp> static inline
   bool operator != (const Rect_<_Tp>& a, const Rect_<_Tp>& b)
   {
       return a.x != b.x || a.y != b.y || a.width != b.width || a.height != b.height;
   }
   //这里是Rect类中的对全等与全不等的重构运算符操作。
   ```

4. **通过算法，快速找结果灯条：**

   ```cpp
   void get_Lightbar();
   ```

   该函数流程为：

   1. 查找轮廓

   2. 循环遍历所有轮廓，调整轮廓角度信息，进行信息归一化处理。

   3. 对长宽比例通过参数进行一次筛选。

   4. 将结果返回到vector变量中。

5. **留存函数：**

   ```cpp
   void LightBarExtract(Rect LightBarRect, Mat BlueImg, Mat Binaryimg, Mat& DstImg); //2018年使用程序，思路和上面一致
   void ContrastAndBright(Mat& img, Mat& g_dstImage);//调整亮度与对比度函数
   ```

   

#### 2. 装甲检测 Armor_Detect.h

**控制总模块**

```cpp
void Armor_Contours(vector<RotatedRect>& LightBar, cv::Mat & frame);
```

流程为：

1. 使用装甲检测，调用数字分类器进行分类。

2. 对分类结果进行打分。

3. 做出最终选择（补偿）。

4. 返回装甲结果。



**装甲检测**

```cpp
vector<Armor> Detect(vector<RotatedRect> &LightBar, vector<Armor>&armor);
```

流程：

1. 遍历所有灯条进行匹配（通过各类参数，包括长宽比限定，长长，宽宽限定，角度差值限定。）

2. 对已通过筛选灯条进行装甲组合，并赋值（可优化）。

3. 对结果使用分类器进行分类（如SVM，欧氏距离）。

4. 对分类后装甲板返回结果。



**装甲打分**

一张图片中，会存在多个装甲板，但是我们能同时击打的装甲板只有一个，在识别到多个装甲板时，只能选择最优先的装甲板。

- 函数结构：

**输入参数**：`Vector<Armor>` （相当于动态数组）

**返回值**：`Armor` （单一装甲板）



- 算法结构：

给装甲板打分：

>score =（装甲板号固定分）+ （面积分）+（角度分）+（距离？）
>
>装甲板号固定分要求（暂定）：8>1=6>3=4=5>2
>
>面积分：为占比最大分数，建议 ln(x+a) +b  **（学弟注：实际采用函数为x^2）**
>
>角度分：占比受面积影响，但是角度大时接近无效，建议 a/x（最方便），或-ax+b
>
>距离分：暂定

然后使用快速排序，然后**选择第一个目标**。

> 学弟注：其实可以不用排序，省一些时间，但是排序貌似也有好处……

**注：完成后不知道是否考虑上一帧结果的分值情况？可优化，float浮点**

> 学弟注：目前实现方式为，判断与上一次最优装甲板位置的差别，如偏移不大，可视为同一块，加相应分数）

```cpp
void Choice(vector<Armor>& armor);//2019
void final_choice(vector<Armor>& armor);//2020
void GetScore(vector<Armor> &armor);//2020
void ArmorGrading(vector<Armor>& armors);//2019
```



**角度转换函数**，从360度单位值转换为0-90

与两个角度转换函数（`RotatedRect`类缺陷问题处理）

```cpp
float AngleConvert(float RowAngle);  //convert to poleAngle
float AngleConvert2(float poleAngle);  //convert poleAngle to rotatedRect angle

void AngleSolver(Armor armor, float& YawAngle, float& PitchAngle);
//该函数目的是将xy轴坐标转换为py轴坐标。（与发送信息有关）
```



**绘图函数**

```cpp
void DrawArmor(Mat& frame, Point2f pt[]);
```



#### 3. Num_Select.h 模板匹配

**SVM：**

```cpp
Ptr<ml::SVM> svm = Algorithm::load<ml::SVM>(SVM_PATH);
HOGDescriptor* hog = new HOGDescriptor(Size(40, 40), Size(20, 20), Size(10, 10), Size(10, 10), 9);
//HOGDescriptor* hog = new HOGDescriptor(Size(9, 9), Size(6, 6), Size(3, 3), Size(3, 3), 9);
//HOGDescriptor* hog = new HOGDescriptor(Size(6, 6), Size(6, 6), Size(3, 3), Size(3, 3), 9);
vector<float> descriptors;
```

SVM模板匹配流程：

1.使用`depart_img`，通过透射变换将目标装甲转换为正常`rect`图像（40x40）.

2.使用hog提取特征。较好的参数已在函数注释内。其中较小的尺寸也可以达到相同的匹配度，但是负样本可能会暴露更多问题。如何优化交给其他组员了。（注：如果hog尺寸与以前训练的xml文件不同则一定会报错！）

3.转换为svm所需的单列图像后，进行匹配。

4.返回匹配结果。 

**Euclidean Distance：**

```cpp
Mat tem_image;   //匹配图像
Mat tmp[8];      //预留匹配图像
string filename = "..//img//"; //路径

bool Euclidean_distance(Mat& src, Armor& armor);
//使用欧氏距离算法进行匹配。（18级方嘉慧颖同学完成）
//但由于经验与实际等原因，准确度无法达到其他学校预想的好，只能暂时放弃。
//Armor armor;
Armor lastArmor; //last target

```



#### 4. ArmorClass.h

装甲类，保存所有装甲相关内容。相关信息应该比较好懂。



### energy

```cpp
Energy();
//初始化类，将卡尔曼相关内容实例化
void start(cv::Mat &input);
//进行图像预处理
cv::Point2d get_target_min2X();
//通过最小二乘法实现寻找目标
cv::Point2d get_target_Kalman();
//通过卡尔曼滤波实现寻找目标
int LeastSquaresCircleFitting(vector<cv::Point2d>& m_Points, cv::Point2d& Centroid, double& dRadius);
//通过最小二乘法拟合圆形
```

具体细节请看2018级姚涵同学文档《RM2020 视觉 能量机关1.0》



### other

```
├─others
│  ├─include
│  │      CameraApi.h
│  │      CameraDefine.h
│  │      CameraStatus.h
│  │      Camera_Input.h
│  │      config.h
│  │      Image_Input.h
│  │      Mes_Manage.h
│  │      Video_Input.h
│  │      
│  └─src
│          Camera_Input.cpp
│          Mes_Manage.cpp
│          Video_Input.cpp
```

#### 1. `ImageHead.h` 读取图像的基类

```cpp
virtual ~InputHead() = default;
virtual bool init() = 0;
virtual bool read(cv::Mat &src) = 0;
```

#### 2. `Camera_Input.h` 工业摄像头读取

```cpp
private:
    unsigned char*          g_pRgbBuffer;      //处理后图像数据缓存区地址
    int                     iCameraCounts = 1; //摄像头数量
    int                     iStatus = -1;      //相机初始化返回值
    tSdkCameraDevInfo       tCameraEnumList;   //相机列表
    int                     hCamera;           //相机句柄
    tSdkCameraCapbility     tCapability;       //设备描述信息
    tSdkFrameHead           sFrameInfo;        //输入图像的zhen头信息
    BYTE*                   pbyBuffer;         //输出图像数据的缓冲区地址
    IplImage*               iplImage = NULL;
    int                     channel = 3;
public:
    CameraInput();
    bool init() final;
    bool read(cv::Mat &src) final;
    ~CameraInput();
```

#### 3. `Video_Input.h`  视频读取

```cpp
public:
    VideoInput(std::string &input);//初始化
    //虚函数实例化
    bool init() final;
    bool read(cv::Mat& src) final;
private:
    //内存变量，保存函数内容
    cv::Mat tmp;
    cv::VideoCapture origin;
    int camera_mode;
```

#### 4. `config.h` 

由CMake生成的提供cmake变量的文件，用于由cmake来控制具体流程。

提供特殊变量为：`USE_INDUSTRY_CAMERA`

#### 5. `Mes_Manage.h`

放置发送接收数据相关内容2020  ——by 李安

* 角度补偿：

  由于最终发送信息为角度值，所以预测部分由角度方面展开，预测角度。

  参数仍需调整。

  如果跟踪模块无法完成可尝试该算法，不过一定要多测试补偿！！



> 2019年函数：
>
> ```
> set_Mes()设置发送信息
> 
> void Send_Message(); //通过已经打开的串口发送信息
> 
> void Set_Miss();//无目标时发送信息
> void saveVideo(cv::Mat frame); // 保存视频函数（独）
> void set_Mes(Armor &target); 
> 
> void AngleSolver(Armor armor, float& YawAngle, float& PitchAngle);
> //转换角度为py轴角度
> ```



#### 6. `CameraApi.h`, `CameraStatus.h`, `CameraDefine.h`

属于MindVision工业摄像头相关调用头文件。



### video

存放视频



### lib

存放外部库

```
├─lib
│      libecotracker.so
│      libecotracker_single.so
│      libMVSDK.so
│      MVCAMSDK.lib
│      MVCAMSDK_X64.lib
```





## 三、有待优化处：

### 1. 系统部分：

1）工业摄像机库的x86与x64环境不能兼容，cmake的变量在组长（2017级徐珲灿）处无法识别系统环境，有待优化。

2）使用了cmake构成的变量，可以尝试进行删除，让其在其他IDE处同样可以运行。

3）未使用ini来导入参数，可进行修改。

4）未使用log文件跟踪写入问题日志，可以由此方便调试。

### 2. 读取视频部分：

1）工业摄像头可修改分辨率提高画面帧率，由于2020疫情原因无法进行，交给后面的同学来处理。之后可修改`resize()`函数处。（李安）

2）正常读取视频时会报警告信息，但不影响调试与最终结果。

### 3. 图像预处理部分：

1）现在的闭运算只使用一次，能否再进行删减？

2）`normalize()`函数可操作性太多，还可以进一步确定光照环境，减少调光情况。

3）hsv进行颜色识别已由2019年队长（研究生周旺生）完成，可进行参考，拉入代码库。

4）蓝色识别参数与红色识别效果有一定区别，能否减少该情况？（未知）

5）若发现参数无论怎么调整都没办法筛选出正常结果，可尝试将注释掉的两次闭运算恢复，再去尝试。

6）关于红蓝模式方面，使用的是最保守的`if`判断，优化如下：

​    （1）第一次执行时给相关参数赋值。

​    （2）每次执行给相关参数赋值。

7）**目前算法缺陷：对光照环境要求较高，并且对通道相减得到结果有明确范围，而若参数不好时则结果会有纰漏。因此我们的算法蓝色效果远好于红色。**

注：目前的参数都是宏定义，是调试后的结果，可进行多次尝试，找最佳值，我们没有别的学校的运算硬件，只能手动来。

### 4. 装甲检测部分：

1）svm尺寸可调整，可调整，可调整，注意训练结果！！能否调高识别率？同时降低负样本影响？

2）**目前比赛可用样本：3,4,5,8，其他样本需要重新采样。1.2需要调整。去年大部分样本不能使用，未考虑到负样本的影响，导致的后果。**

3）**欧氏距离参数与回避负样本影响的方法是否还能优化？**

4）打分系统，能否更简单？是否考虑了前一帧图像位置的情况？能否不使用浮点运算，以整数达成目标，提高效率？

### 5. 自瞄部分：

已知自身算法不够可靠，今年跟踪能否可使用？或者还是进行简单自瞄预测？

### 6. 其他：

**目前追踪模块与信息发送模块未安装到总工程中！切记必须完善！**

**信息发送模块与电控同学对接未完成！**

**守护进程未完成！！**