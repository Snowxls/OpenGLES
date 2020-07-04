####核心框架导入
```
#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>
```
`#include<GLShaderManager.h>` 移入了GLTool 着色器管理器（shader Mananger）类。没有着色器，我们就不能在OpenGL（核心框架）进行着色。着色器管理器不仅允许我们创建并管理着色器，还提供一组“存储着色器”，他们能够进行一些初步䄦基本的渲染操作。

`#include<GLTools.h>`  GLTool.h头文件包含了大部分GLTool中类似C语言的独立函数

在Mac 系统下，`#include<glut/glut.h>`
 在Windows 和 Linux上，我们使用freeglut的静态库版本并且需要添加一个宏

#####定义一个，着色管理器&批次容器
```
GLShaderManager shaderManager;
GLBatch triangleBatch
```
#####初始化GLUT库,这个函数只是传说命令参数并且初始化glut库
```
glutInit(&argc, argv);
```
#####初始化双缓冲窗口
```
glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
```
#####GLUT设置窗口大小、窗口标题
```
glutInitWindowSize(800, 600);
glutCreateWindow("Triangle");
```
#####注册重塑函数
```
glutReshapeFunc(changeSize);
```
#####注册显示函数
```
glutDisplayFunc(RenderScene);
```
**初始化一个GLEW库,确保OpenGL API对程序完全可用。
     在试图做任何渲染之前，要检查确定驱动程序的初始化过程中没有任何问题。**
```
GLenum status = glewInit();
    if (GLEW_OK != status) {
        
        printf("GLEW Error:%s\n",glewGetErrorString(status));
        return 1;
        
    }
```
#####设置我们的渲染环境并开启
```
setupRC();
glutMainLoop();
```
#####void setupRC()  初始化设置
```
void setupRC() {
    //设置清屏颜色（背景颜色）
    glClearColor(0.0f, 1.0f, 0.0f, 1);
    
    
    //没有着色器，在OpenGL 核心框架中是无法进行任何渲染的。初始化一个渲染管理器。
    //在前面的课程，我们会采用固管线渲染，后面会学着用OpenGL着色语言来写着色器
    shaderManager.InitializeStockShaders();
    
    
    //指定顶点
    //在OpenGL中，三角形是一种基本的3D图元绘图原素。
    GLfloat vVerts[] = {
        -1.0f,0.0f,0.0f,
        1.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f
    };
    
    triangleBatch.Begin(GL_TRIANGLES, 3);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
    
}
```
#####void changeSize(int w,int h)  在窗口大小改变时会调用，接收新的宽度&高度
```
void changeSize(int w,int h) {
    /*
      x,y 参数代表窗口中视图的左下角坐标，而宽度、高度是像素为表示，通常x,y 都是为0
     */
    glViewport(0, 0, w, h);
    
}
```
#####void RenderScene(void)  渲染
```
void RenderScene(void) {

    //1.清除一个或者一组特定的缓存区
    /*
     缓冲区是一块存在图像信息的储存空间，红色、绿色、蓝色和alpha分量通常一起分量通常一起作为颜色缓存区或像素缓存区引用。
     OpenGL 中不止一种缓冲区（颜色缓存区、深度缓存区和模板缓存区）
      清除缓存区对数值进行预置
     参数：指定将要清除的缓存的
     GL_COLOR_BUFFER_BIT :指示当前激活的用来进行颜色写入缓冲区
     GL_DEPTH_BUFFER_BIT :指示深度缓存区
     GL_STENCIL_BUFFER_BIT:指示模板缓冲区
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //2.设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0,1.00,0.0,0.5f};
    
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //在开始的设置openGL 窗口的时候，我们指定要一个双缓冲区的渲染环境。这就意味着将在后台缓冲区进行渲染，渲染结束后交换给前台。这种方式可以防止观察者看到可能伴随着动画帧与动画帧之间的闪烁的渲染过程。缓冲区交换平台将以平台特定的方式进行。
    //将后台缓冲区进行渲染，然后结束后交换给前台
    glutSwapBuffers();
    
}
```
![运行结果](https://upload-images.jianshu.io/upload_images/8416233-a528cda8e78ed64b.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)