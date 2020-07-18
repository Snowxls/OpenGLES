**先看结果**

![](https://upload-images.jianshu.io/upload_images/8416233-1ade0d9dcc5837cc.gif?imageMogr2/auto-orient/strip)

**核心代码**

```
void changeSize(int w,int h) {
    //设置视口
    glViewport(0, 0, w, h);
    
    //创建投影矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
    //把投影矩阵压栈
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //为管道提供堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void RenderScene(void) {
    //清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //设置地板颜色
    static GLfloat vFloorColor[] = {0,1,0,1};
    
    //设置大球的颜色
    static GLfloat vTorusColor[] = {1,0,0,1};
    
    //设置小球的颜色
    static GLfloat vSphereColor[] = {0,0,1,1};
    
    //设置动画计时器
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60;
    
    //设置相机
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.PushMatrix(mCamera); //camera入栈
    //默认在原点 相机会在大圆的中心 为了看清整体 Z轴移动
    modelViewMatrix.Translate(0, 0, -3);
    
    //因为相机伴随观察着整个变换过程 所以不能设置完直接出栈 否则将不能观察到后续的变换内容
    
    
    //地板
    modelViewMatrix.PushMatrix(); //地板操作 入栈
    //着色器设置
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vFloorColor);
    floorBatch.Draw(); //地板绘制
    modelViewMatrix.PopMatrix(); //地板操作 出栈
    
    //随机设置一个光源点
    M3DVector4f vLightPos = {0,10,5,1};
    
    //大球
    modelViewMatrix.PushMatrix(); //大红球操作 入栈
    //自转
    modelViewMatrix.Rotate(yRot, 0, 1, 0);
    //着色器设置
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vTorusColor);
    torusBatch.Draw(); //大红球绘制
    modelViewMatrix.PopMatrix(); //大红球操作 出栈
    
    //画50个小球
    for (int i = 0;i < NUM_SPHERES;i++) {
        modelViewMatrix.PushMatrix(); // 每个小球单独入栈
        modelViewMatrix.MultMatrix(spheres[i]); //移动小球到设定的位置
        //着色器设置
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vSphereColor);
        sphereBatch.Draw(); //小球绘制
        modelViewMatrix.PopMatrix(); //每个小球单独出栈
    }
    
    //小蓝球围绕大红球转 围绕Y轴转
    modelViewMatrix.PushMatrix(); //小篮球操作 入栈
    modelViewMatrix.Rotate(yRot * -2.0f, 0, 1, 0);
    //在X轴上平移一个距离 使查看时能分辨
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vSphereColor);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix(); //小篮球操作 出栈
    
    modelViewMatrix.PopMatrix(); //camera 出栈
    
    glutSwapBuffers();
    
    //重新绘制
    glutPostRedisplay();
}

void setupRC() {
    //清除缓存
    glClearColor(0, 0, 0, 1);
    //初始化
    shaderManager.InitializeStockShaders();
    
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    //地板创建(物体坐标系)
    floorBatch.Begin(GL_LINES, 324);
    for(GLfloat x = -20.0; x <= 20.0f; x+= 0.5) {
        floorBatch.Vertex3f(x, -0.55f, 20.0f);
        floorBatch.Vertex3f(x, -0.55f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.55f, x);
        floorBatch.Vertex3f(-20.0f, -0.55f, x);
    }
    floorBatch.End();
    
    //初始化大球
    gltMakeSphere(torusBatch, 0.4f, 40, 80);
    
    //初始化小球
    gltMakeSphere(sphereBatch, 0.1f, 13, 26);
    //周围的小球
    for (int i = 0;i < NUM_SPHERES;i++) {
        //统一平面下 Y值一样
        //X,Y生成随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        //在y方向，将球体设置为0.0的位置，这使得它们看起来是飘浮在眼睛的高度
        //对spheres数组中的每一个顶点，设置顶点数据
        spheres[i].SetOrigin(x, 0.0f, z);
    }
    
}
```

**栈的机制**

在`changeSize()`函数中，我们加载了投影矩阵，并把投影矩阵压入管道`transformPipeline`的栈中。因此我们的后续操作其实都是对栈顶的内容操作。

首先需要设置观察者`camera`，默认设置的地点在原点，为了更好的观察整个场景，需要在Z轴负方向移动一段距离。同时，为了观察后续的模型变换，入栈后暂时不能出栈，需要等模型变换全部完成后出栈。

之后的地板绘制，大球绘制，小球绘制，都需要进行堆栈的入栈/出栈，每次绘制前入栈，绘制结束出栈，为的是后续的模型变换不会被之前的变换所影响。

**CStopWatch**

这是一个计时器，可以记录当前运行的时间。为了做成有动画的效果，需要在`RenderScene()`渲染函数中调用`glutPostRedisplay()`方法，用以每次都会重新渲染画面，并触发计时器，达到动画的效果。

**按键操作**

```
void SpeacialKeys(int key,int x,int y){
    
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));
    
    if (key == GLUT_KEY_UP) {
        cameraFrame.MoveForward(linear);
    }
    if (key == GLUT_KEY_DOWN) {
        cameraFrame.MoveForward(-linear);
    }
    
    if (key == GLUT_KEY_LEFT) {
        cameraFrame.RotateWorld(angular, 0, 1, 0);
    }
    if (key == GLUT_KEY_RIGHT) {
        cameraFrame.RotateWorld(-angular, 0, 1, 0);
    }
}
```

对相机进行移动达到观察不同画面的效果，值得注意的是，这里的移动都是基于世界坐标系的，并且由于在`RenderScene()`中调用了重绘方法，其实每一帧都在重绘，所以这里可以省略重绘的函数调用。