####基础设置
- 初始化GLUT库，初始化双缓冲窗口

```
glutInit(&argc, argv);
glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
```

- GLUT窗口大小、窗口标题

```
glutInitWindowSize(500, 500);
glutCreateWindow("Square");
```

- 注册重塑函数，显示函数

```
//注册重塑函数
glutReshapeFunc(changeSize);
//注册显示函数
glutDisplayFunc(RenderScene);
```

- 初始化一个GLEW库
```
GLenum status = glewInit();
if (GLEW_OK != status) {
    printf("GLEW Error:%s\n",glewGetErrorString(status));
        return 1;
}
```

- 设置我们的渲染环境并开启

```
setupRC();
glutMainLoop();
```


`changeSize()`：首次加载或者窗口发生变化时会调用，触发重绘

```
void changeSize(int w,int h) {
    //x,y 参数代表窗口中视图的左下角坐标，而宽度、高度是像素为表示，通常x,y 都是为0
    glViewport(0, 0, w, h);
}
```

`RenderScene()`：渲染出屏幕上需要显示的内容

`setupRC()`：初始化设置

####画一个正方形
**设置到原点的距离**

```
GLfloat blockSize = 0.1f;
```

**设置4个顶点坐标**

```
GLfloat vVerts[] = {
        -blockSize,-blockSize,0.0f, //A
        blockSize,-blockSize,0.0f,  //B
        blockSize,blockSize,0.0f,  //C
        -blockSize,blockSize,0.0f //D
};
```

**初始化设置**

```
void setupRC() {
    //设置清屏颜色（背景颜色）
    glClearColor(0.2f, 1.0f, 0.0f, 1);

    //初始化一个渲染管理器。
    shaderManager.InitializeStockShaders();
    
    //GL_TRIANGLE_FAN ，4个顶点
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}
```

**完成渲染逻辑**

```
void RenderScene(void) {

    //清除一个或者一组特定的缓存区
    /*
     GL_COLOR_BUFFER_BIT :指示当前激活的用来进行颜色写入缓冲区
     GL_DEPTH_BUFFER_BIT :指示深度缓存区
     GL_STENCIL_BUFFER_BIT:指示模板缓冲区
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //设置一组浮点数来表示颜色
    GLfloat vRed[] = {1.0,1.0,0.0,0.5f};
    
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    //提交着色器
    triangleBatch.Draw();
    //将后台缓冲区进行渲染，然后结束后交换给前台
    glutSwapBuffers();
}
```

**运行后能得到一个正方形**

![正方形](https://upload-images.jianshu.io/upload_images/8416233-a224710f12a06f24.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

####通过键盘控制操作移动图形

**注册一个特殊函**

```
glutSpecialFunc(SpecialKeys);
```

**这个函数能获取键盘输入的内容，通过获取的键位来实现图像的移动**

**设置步长**

```
GLfloat stepSize = 0.025f;
```

**取出其中一个顶点作为相对顶点使用**

```
    /*
     顶点顺序
     D  C
     
     A  B
     */
    
    //取其中一个点作为相对顶点  使用D点
    GLfloat blockX = vVerts[9];
    GLfloat blockY = vVerts[10];
```

**根据键盘输入移动参数点**

```
    if (key == GLUT_KEY_UP) {
        blockY += stepSize;
    }
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepSize;
    }
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepSize;
    }
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepSize;
    }
```

**根据参数点，更新所有顶点信息**

```
    //更新其他顶点
    //A
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize * 2;
    
    //B
    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY - blockSize * 2;
    
    //C
    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY;
    
    //D
    vVerts[9] = blockX;
    vVerts[10] = blockY;
```

**提交更新点并提交**

```
    //提交更新点
    triangleBatch.CopyVertexData3f(vVerts);
    //重新渲染
    glutPostRedisplay();
```

**自此，我们可以通过键盘的上下左右来移动图形**

**但是还是存在一个问题，图形会超过边界，为了防止这个问题，需要在更新所有顶点前做边界判断**

```
    //边界检测
    //左边界限制
    if (blockX < -1.0) {
        blockX = -1.0;
    }
    //右边界限制  因为取的是D点 所以还要加上图形的边长
    if (blockX > 1.0 - blockSize * 2) {
        blockX = 1.0 - blockSize * 2;
    }
    //下边界限制  因为取的是D点 所以还要加上图形的边长
    if (blockY < -1.0 + blockSize * 2) {
        blockY = -1.0 + blockSize * 2;
    }
    //上边界限制
    if (blockX > 1.0) {
        blockY = 1.0;
    }
```
**完成了边界检测后就能满足用键盘移动图形的需求，其中`glutPostRedisplay()`会触发`RenderScene()`进行重新渲染**

####通过矩阵的方式进行图形移动

通过对所有顶点的更新能完成图形移动的效果，但是如果碰到了多边形，顶点处很多的情况下，通过更新顶点的方式代码量和计算量过大，可以使用**矩阵平移**的形式进行移动的操作

**定义2个参数用于记录偏移量**

```
GLfloat xPos = 0.0f; //记录图形在X轴的偏移数值 用于矩阵移动
GLfloat yPos = 0.0f; //记录图形在Y轴的偏移数值 用于矩阵移动
```

**注册一个新的键盘监听函数**

```
glutSpecialFunc(SpecialKeysWithMatrix);
```

**重写`RenderScene()`函数，使用平面着色器**

```
void RenderScene(void) {

    //清除一个或者一组特定的缓存区
    /*
     GL_COLOR_BUFFER_BIT :指示当前激活的用来进行颜色写入缓冲区
     GL_DEPTH_BUFFER_BIT :指示深度缓存区
     GL_STENCIL_BUFFER_BIT:指示模板缓冲区
     */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //设置一组浮点数来表示颜色
    GLfloat vRed[] = {1.0,1.0,0.0,0.5f};
    
    //使用矩阵
    M3DMatrix44f mTransfromMatrix;
    
    //平移矩阵
    m3dTranslationMatrix44(mTransfromMatrix, xPos, yPos, 0);
    
    //平面着色器 （固定着色器）
    shaderManager.UseStockShader(GLT_SHADER_FLAT,mTransfromMatrix,vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //在开始的设置openGL 窗口的时候，我们指定要一个双缓冲区的渲染环境。这就意味着将在后台缓冲区进行渲染，渲染结束后交换给前台。这种方式可以防止观察者看到可能伴随着动画帧与动画帧之间的闪烁的渲染过程。缓冲区交换平台将以平台特定的方式进行。
    //将后台缓冲区进行渲染，然后结束后交换给前台
    glutSwapBuffers();
    
}
```

**编写移动逻辑**

```
void SpecialKeysWithMatrix(int key, int x, int y) {
    //设置步长
    GLfloat stepSize = 0.025f;
    
    if (key == GLUT_KEY_UP) {
        yPos += stepSize;
    }
    if (key == GLUT_KEY_DOWN) {
        yPos -= stepSize;
    }
    if (key == GLUT_KEY_LEFT) {
        xPos -= stepSize;
    }
    if (key == GLUT_KEY_RIGHT) {
        xPos += stepSize;
    }
    
    //边界检测 现在控制的是偏移量 可以认为是中心点 需要加上边长
    //左边界限制
    if (xPos < -1.0 + blockSize) {
        xPos = -1.0 + blockSize;
    }
    //右边界限制
    if (xPos > 1.0 - blockSize) {
        xPos = 1.0 - blockSize;
    }
    //下边界限制
    if (yPos < -1.0 + blockSize) {
        yPos = -1.0 + blockSize;
    }
    //上边界限制
    if (yPos > 1.0 - blockSize) {
        yPos = 1.0 - blockSize;
    }
    
    //重新渲染
    glutPostRedisplay();
}
```

**矩阵移动使用是偏移量，在边界检测时可以认为是中心点，所以需要额外计算一个顶点到原点的距离**
