**基础的图元连接方式**

![图元连接方式](https://upload-images.jianshu.io/upload_images/8416233-8a7b56c256dcdfdc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**基础设置**

```
// 各种需要的类
GLShaderManager shaderManager; //着色管理器
GLMatrixStack modelViewMatrix; //模型视图矩阵  用于变换
GLMatrixStack projectionMatrix; //投影矩阵
GLFrame cameraFrame; //观察者
GLFrame objectFrame; //当前对象 用于进行旋转、移动等操作
//投影矩阵
GLFrustum viewFrustum;

//容器类（7种不同的图元对应7种容器对象） 每个批次类对应一个图形
GLBatch pointBatch;
GLBatch lineBatch;
GLBatch lineStripBatch;
GLBatch lineLoopBatch;
GLBatch triangleBatch;
GLBatch triangleFanBatch;
GLBatch triangleStripBatch;


//几何变换的管道
GLGeometryTransform    transformPipeline;

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// 跟踪效果步骤
int nStep = 0;
```

**main函数设置**

```
int main(int argc,char *argv[]) {

    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    //申请一个颜色缓存区、深度缓存区、双缓存区、模板缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置window 的尺寸
    glutInitWindowSize(800, 600);
    //创建window的名称
    glutCreateWindow("GL_POINTS");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(changeSize);
    //点击空格时，调用的函数
    glutKeyboardFunc(KeyPressFunc);
    //特殊键位函数（上下左右）
    glutSpecialFunc(SpecialKeys);
    //显示函数
    glutDisplayFunc(RenderScene);
    
    //判断一下是否能初始化glew库，确保项目能正常使用OpenGL 框架
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    //绘制
    setupRC();
    
    //runloop运行循环
    glutMainLoop();
 
    return  0;
    
}
```
**特殊键位处理（上、下、左、右移动）**

用来控制图像的旋转变换

```
void SpecialKeys(int key, int x, int y) {
    //围绕一个指定的X,Y,Z轴旋转
    //m3dDegToRad 度数转弧度
    if(key == GLUT_KEY_UP) {
        //围绕X轴旋转
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if(key == GLUT_KEY_DOWN) {
        //围绕X轴旋转
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if(key == GLUT_KEY_LEFT) {
       //围绕Y轴旋转
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    if(key == GLUT_KEY_RIGHT) {
        //围绕Y轴旋转
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    //重新渲染
    glutPostRedisplay();
}
```

**根据空格次数。切换不同的窗口**

```
void KeyPressFunc(unsigned char key, int x, int y) {
    //32对应空格的ASCII码
    if (key == 32) {
        nStep++;
        
        if(nStep > 6)
            nStep = 0;
    }
    
    switch(nStep) {
        case 0:
            //设置窗口名称
            glutSetWindowTitle("GL_POINTS");
            break;
        case 1:
            glutSetWindowTitle("GL_LINES");
            break;
        case 2:
            glutSetWindowTitle("GL_LINE_STRIP");
            break;
        case 3:
            glutSetWindowTitle("GL_LINE_LOOP");
            break;
        case 4:
            glutSetWindowTitle("GL_TRIANGLES");
            break;
        case 5:
            glutSetWindowTitle("GL_TRIANGLE_FAN");
            break;
        case 6:
            glutSetWindowTitle("GL_TRIANGLE_STRIP");
            break;
    }
    
    //改变模式 触发重新渲染
    glutPostRedisplay();
}
```

**setupRC()**

```
void setupRC() {
    //初始化 设置画布背景
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //着色管理器初始化
    shaderManager.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);
    //设置变换管线以使用两个矩阵堆栈
    //投影变化、移动变换 ——> 变换管道（矩阵运算）
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    //设置观察者 将Z轴设置成负数 可以看到图像
    cameraFrame.MoveForward(-15.0f);
    
    //设置顶点 物体坐标系（基于物体本身） 使用时需要转换成规范坐标系
    GLfloat vCoast[9] = {
        3,3,0,
        0,3,0,
        3,0,0
    };
    
#pragma mark PointSet
    //通过线的形式
    pointBatch.Begin(GL_POINTS, 3);
    pointBatch.CopyVertexData3f(vCoast);
    pointBatch.End();
    
#pragma mark LineSet
    //提交批次类
    lineBatch.Begin(GL_LINES, 3);
    lineBatch.CopyVertexData3f(vCoast);
    lineBatch.End();
    
#pragma mark Line_STRIP
    //通过线段的形式
    lineStripBatch.Begin(GL_LINE_STRIP, 3);
    lineStripBatch.CopyVertexData3f(vCoast);
    lineStripBatch.End();
    
#pragma mark LINE_LOOP
    //通过线环的形式
    lineLoopBatch.Begin(GL_LINE_LOOP, 3);
    lineLoopBatch.CopyVertexData3f(vCoast);
    lineLoopBatch.End();
    
#pragma mark Pyramid
    //通过三角形创建金字塔
    GLfloat vPyramid[12][3] = {
        -2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        -2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f
    };
    
    //GL_TRIANGLES 每3个顶点定义一个新的三角形
    triangleBatch.Begin(GL_TRIANGLES, 12);
    triangleBatch.CopyVertexData3f(vPyramid);
    triangleBatch.End();
    
#pragma mark 三角形扇形--六边形
    GLfloat vPoints[100][3];
    int nVerts = 0;
    //半径
    GLfloat r = 3.0f;
    //原点(x,y,z) = (0,0,0);
    vPoints[nVerts][0] = 0.0f;
    vPoints[nVerts][1] = 0.0f;
    vPoints[nVerts][2] = 0.0f;
    
    
    //M3D_2PI 就是2Pi 的意思，就一个圆的意思。 绘制圆形
    //M3D_2PI / 6.0f 可以控制三角形的数量
    for(GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI / 6.0f) {
        
        //数组下标自增（每自增1次就表示一个顶点）
        nVerts++;
        /*
         弧长=半径*角度,这里的角度是弧度制,不是平时的角度制
         既然知道了cos值,那么角度=arccos,求一个反三角函数就行了
         */
        //x点坐标 cos(angle) * 半径
        vPoints[nVerts][0] = float(cos(angle)) * r;
        //y点坐标 sin(angle) * 半径
        vPoints[nVerts][1] = float(sin(angle)) * r;
        //z点的坐标
        vPoints[nVerts][2] = -0.5f;
    }
    
    // 结束扇形 前面一共绘制7个顶点（包括圆心）
    //添加闭合的终点
    //课程添加演示：屏蔽177-180行代码，并把绘制节点改为7.则三角形扇形是无法闭合的。
    nVerts++;
    vPoints[nVerts][0] = r;
    vPoints[nVerts][1] = 0;
    vPoints[nVerts][2] = 0.0f;
    
    // 加载！
    //GL_TRIANGLE_FAN 以一个圆心为中心呈扇形排列，共用相邻顶点的一组三角形
    triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
    triangleFanBatch.CopyVertexData3f(vPoints);
    triangleFanBatch.End();
    
#pragma mark 三角形条带
    //三角形条带，一个小环或圆柱段
    //顶点下标
    int iCounter = 0;
    //半径
    GLfloat radius = 3.0f;
    //从0度~360度，以0.3弧度为步长
    for(GLfloat angle = 0.0f; angle <= (2.0f*M3D_PI); angle += 0.3f) {
        //或许圆形的顶点的X,Y
        GLfloat x = radius * sin(angle);
        GLfloat y = radius * cos(angle);
        
        //绘制2个三角形（他们的x,y顶点一样，只是z点不一样）
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = -0.5;
        iCounter++;
        
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = 0.5;
        iCounter++;
    }
    
    // 关闭循环
    printf("三角形带的顶点数：%d\n",iCounter);
    //结束循环，在循环位置生成2个三角形
    vPoints[iCounter][0] = vPoints[0][0];
    vPoints[iCounter][1] = vPoints[0][1];
    vPoints[iCounter][2] = -0.5;
    iCounter++;
    
    vPoints[iCounter][0] = vPoints[1][0];
    vPoints[iCounter][1] = vPoints[1][1];
    vPoints[iCounter][2] = 0.5;
    iCounter++;
    
    // GL_TRIANGLE_STRIP 共用一个条带（strip）上的顶点的一组三角形
    triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    triangleStripBatch.CopyVertexData3f(vPoints);
    triangleStripBatch.End();
}
```

**RenderScene()**
```
void RenderScene(void) {
    //清空缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //压栈 记录状态 可以执行回退操作
    modelViewMatrix.PushMatrix(); //单元矩阵入栈 这是栈中有 原始单元矩阵 & 当前会修改的单元矩阵
    
    //camera 观察者矩阵
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    
    //矩阵相乘 -> 模型视图矩阵  栈顶 * mCamera = newCamera
    modelViewMatrix.MultMatrix(mCamera);
    
    //物体矩阵
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    
    //矩阵相乘 -> 模型视图矩阵  newCamera * mObjectFrame
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    //模型视图矩阵（观察者矩阵，物体变化矩阵） 投影矩阵 mvp(ModelViewProjection)
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlack);
    
#pragma mark RenderSet
    switch (nStep) {
        case 0: {
            //Point
            //设置点的大小
            glPointSize(6.0f);
            pointBatch.Draw();
            glPointSize(1.0f); //因为有其他图形需要绘制 所以更改完后需要还原
            break;
        }
        case 1: {
            //Line
            //设置线宽
            glLineWidth(4.0f);
            lineBatch.Draw();
            glLineWidth(1.0f); //因为有其他图形需要绘制 所以更改完后需要还原
            break;
        }
        case 2: {
            //Line_Strip
            //设置线宽
            glLineWidth(2.0f);
            lineStripBatch.Draw();
            glLineWidth(1.0f); //因为有其他图形需要绘制 所以更改完后需要还原
            break;
        }
        case 3: {
            //Line_Loop
            //设置线宽
            glLineWidth(2.0f);
            lineLoopBatch.Draw();
            glLineWidth(1.0f); //因为有其他图形需要绘制 所以更改完后需要还原
            break;
        }
        case 4: {
            DrawWireFramedBatch(&triangleBatch);//描边
            break;
        }
        case 5: {
            DrawWireFramedBatch(&triangleFanBatch);//描边
            break;
        }
        case 6: {
            DrawWireFramedBatch(&triangleStripBatch);//描边
            break;
        }
            
        default:
            break;
    }
    
    //还原到以前的模型视图矩阵（单位矩阵）
    modelViewMatrix.PopMatrix(); //出栈后  栈中剩余一个原始单元矩阵 以供后续修改
    
    // 进行缓冲区交换
    glutSwapBuffers();
}
```

**使用栈的机制完成回退的功能**

![栈机制](https://upload-images.jianshu.io/upload_images/8416233-d8a3a407b6e7f494.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**这样做的优势在于可以不用重复创建单元矩阵来进行各种变换操作，同时具有了回退功能**

**DrawWireFramedBatch(GLBatch* pBatch)**

描边的核心代码

```
void DrawWireFramedBatch(GLBatch* pBatch) {
    /*------------画绿色部分----------------*/
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
          --transformPipeline 变换管线（指定了2个矩阵堆栈）
     参数3：颜色值
    */
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    /*-----------边框部分-------------------*/
    /*
        glEnable(GLenum mode); 用于启用各种功能。功能由参数决定
        参数列表：http://blog.csdn.net/augusdi/article/details/23747081
        注意：glEnable() 不能写在glBegin() 和 glEnd()中间
        GL_POLYGON_OFFSET_LINE  根据函数glPolygonOffset的设置，启用线的深度偏移
        GL_LINE_SMOOTH          执行后，过虑线点的锯齿
        GL_BLEND                启用颜色混合。例如实现半透明效果
        GL_DEPTH_TEST           启用深度测试 根据坐标的远近自动隐藏被遮住的图形（材料
     
     
        glDisable(GLenum mode); 用于关闭指定的功能 功能由参数决定
     
     */
    
    //画黑色边框
    glPolygonOffset(-1.0f, -1.0f);// 偏移深度，在同一位置要绘制填充和边线，会产生z冲突，所以要偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    // 画反锯齿，让黑边好看些
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //绘制线框几何黑色版 三种模式，实心，边框，点，可以作用在正面，背面，或者两面
    //通过调用glPolygonMode将多边形正面或者背面设为线框模式，实现线框渲染
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //设置线条宽度
    glLineWidth(2.5f);
    
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
         --transformPipeline.GetModelViewProjectionMatrix() 获取的
          GetMatrix函数就可以获得矩阵堆栈顶部的值
     参数3：颜色值（黑色）
     */
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();

    // 复原原本的设置
    //通过调用glPolygonMode将多边形正面或者背面设为全部填充模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}
```

**运行效果**

![图元绘制.gif](https://upload-images.jianshu.io/upload_images/8416233-58ed820e6231cfdc.gif?imageMogr2/auto-orient/strip)




