# 计算机图形学实验课堂作业
## **学生编号27号**
## **实验代码使用总体说明**
1. 所有课堂实验作业文件均在主目录下
2. 文件名前的数字代表实验序号

   <code>例如：
   [07_anti_aliasing.cpp](07_anti_aliasing.cpp)
   表示实验七中的反走样实验</code>
3. 实验所有原文件为GBK编码方式，如出现乱码请切换打开方式
（本文件为utf-8格式）
4. 所有源文件编译为二进制的方式为

   ```bash
   g++ fileName.cpp -lglut32 -lglu32 -lgdi32 -lopengl32 -o fileName.exe
   ```
   运行方式为
   ```bash
   .\fileName.exe
   ```
5. 有命令行提示的实验直接按照提示进行

- **实验一：**
[调用OpenGL程序生成直线段](01_text_line.cpp)
- **实验二：**
[DDA画直线段](02_dda_line.cpp)
- **实验三：**
[Bresenham画直线段](03_bresenham_line.cpp)
- **实验四：**
[扫描线填充算法](04_scanline_fill.cpp)

  <code>运行程序后,鼠标左键依次单击产生多边形,鼠标右键单击进行扫描线填充</code>
- **实验五：**
  - [Bresenham画圆(圆的扫描转换)](05_bresenham_circle.cpp)
  - [种子点填充](05_seed_point_fill.cpp)
    ```
    点击鼠标左键画点
    点击鼠标右键结束画点，形成多边形
    点击鼠标中键确定区域填充种子点位置
    ```
    <code>因为涉及到递归,画多边形的时候请画小一点</code>
- **实验六：**
[二维几何变换](06_2D_transform.cpp)

  <code>到时候请直接输入二维变换矩阵来进行几何变换
  
  屏幕中心的绿点为原点

  红色的为变换后图形

  浅蓝色的为变换前图形</code>
  ```
  放大两倍
  2 0 0
  0 2 0
  0 0 1
  缩小两倍
  1 0 0
  0 1 0
  0 0 2
  平移(x轴平移x,y轴平移y)
  1 0 0
  0 1 0
  x y 1
  ```
- **实验七：**
[反走样](07_anti_aliasing.cpp)

  <code>线条更光滑
  
  可以试试按一下键盘r键</code>
- **实验八：**
  - [裁剪算法](08_cohen_sutherland.cpp)
  <code>鼠标操作同实验四</code>
  - [鼠标函数](08_mouse_func.cpp)
  <code>鼠标点击生成红点</code>
- **实验九：**
[三维几何变换](09_3D_transform.cpp)
  
  <code>在代码108-114行中,可以修改注释内容再次运行来观看不同的三维变换效果</code>
- **实验十：**
[颜色编码建模](10_color_encode.cpp)
- **实验十一：**
  - [旋转的茶壶](11_rotate_teapot.cpp)
  - [旋转的三角形](11_rotate_triangle.cpp)
    
    <code>鼠标左键或键盘a键逆时针转

    鼠标右键或键盘d键顺时针转</code>
- **实验十二：**
[3D旋转金字塔](12_3D_pyramid.cpp)
<code>wasd键控制旋转</code>
- **实验十三：**
  
  <code>红绿蓝为xyz轴</code>
  - [3D模型](13_3D_model.cpp)
  - [3D二次曲面](13_3D_quadric_surfaces.cpp)
- **实验十四：**
  - [Bezier曲线](14_bezier_curve.cpp)
  - [鼠标控制版Bezier曲线](14_advanced_bezier_curve.cpp)
  <code>鼠标左键依次点击四个点自动生成Bezier曲线</code>
- **实验十五：**
[消隐算法](15_blanking.cpp)
  <code>通过Z-Buffer双缓存消隐,例如甜甜圈前面遮挡后面的物体</code>

# **期末实验**
## **[一个猴头](./learn_final/NeHe/simple_monkey_head.cpp)**

### **运行方法：** 

```
cd ".\learn_final\NeHe\" ; if ($?) { g++ simple_monkey_head.cpp -lglut32 -lglu32 -lglaux -lgdi32 -lopengl32 -o simple_monkey_head } ; if ($?) { .\simple_monkey_head }
```

**注：运行时需要有OpenGL的环境（采用的是glut不是glfw），以及[simple_monkey_head.cpp](./learn_final/NeHe/simple_monkey_head.cpp)文件中所引用到的头文件（部分头文件在同级目录下）**

### **控制方法（仔细看一下）：** 
- 通过方向键来控制视角的前后左右平移

  **注：移动时镜头会有上下移动，感觉十分真实**
- 通过PageUp和PageDown来控制视角的上下视角
- 通过W键来控制视角的上移
- 通过S键来控制视角的下移
- 通过A键来控制视角的左移
- 通过F键来控制视角的右移

  注：在视角左右移动后，再按前进后退键可以向该方向进行前进和后退
- 通过C键来控制猴头的纹理贴图

  注：
  1.因为纹理贴图太难找，我用的纹理贴图有的不是特别适合猴头

  2.在控制台会提醒当前是第几个纹理贴图（一共有11个）

  3.第十一个纹理贴图是空白纹理，所以不开光照效果就是一片白
- 通过L键来控制是否开启灯光效果
  
  注：光源在猴头的右侧，具体的坐标为（5，0，0）
- 通过空格键来控制切换颜色
  注：此时切换的是灯光的颜色，进而达到了给贴上了纹理的物体进行换色的效果
- 通过B键来控制是否显示半透明效果
- 通过F1键可以尝试切换全屏效果

  注：可能会失败
- 通过返回键（Esc键）退出程序

### **关于learn_final文件夹的说明：**
- 该文件夹下是我在写期末实验过程中所尝试和学习过的所有代码，是我努力的过程性证明
- 里面的NeHe文件夹中从01下标开始的带数字下标的文件是我学习[NeHe的OpenGL教程](./learn_final/NeHe/NeHe.OpenGL%E6%95%99%E7%A8%8B%E4%B8%AD%E6%96%87%E7%89%88.CHM)中所写的代码
- 而其它部分的代码如[ObjLoader.cpp](./learn_final/NeHe/ObjLoader.cpp)则是在学习加载模型时所写的代码，其它的文件是学习测试时所用
- 在NeHe文件夹之外的文件同样是进行构建3D世界的一些探索和尝试用的代码

## **写在最后**
**经过此次期末实验，学到了很多东西，比如**
1. 纹理可以自己进行构建（见[texsmpl.cpp](./learn_final/texsmpl.cpp)中的```makeImage()```方法）
2. 纹理映射的方法（一定要按照逆时针的顺序）
3. 多个纹理映射不同对象的方法
4. 开启光源后颜色设置就会失效，因为通过光就可以生成不同的颜色，同时也可以通过设置材质颜色来实现
5. 如果把一个空的纹理进行映射，就会得到一个空白纹理
6. 光照的设置方法
7. Obj文件的格式
8. 通过文件格式来设计对应的读取方法（网上的代码基本都不适用我的情况，还是通过修改来得到的）
9. 如何实现半透明的效果
10. 如何实现键盘的消抖控制
11. 如何实现人物的走路效果
12. 如何将jpg png bmp等格式的图片读入生成纹理
13. 如何实现人物移动效果（通过移动整个场景）
14. 模型的List预加载
15. 如何通过Blender导出我需要的Obj模型
16. 如何按照指定的格式读取文件