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
