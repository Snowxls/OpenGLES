#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager shaderManager; // 着色器管理器
GLMatrixStack modelViewMatrix; // 模型视图矩阵堆栈
GLMatrixStack projectionMatrix; // 投影矩阵堆栈
GLFrustum viewFrustum; // 视景体
GLGeometryTransform transformPipeline; // 几何图形变换管道

GLTriangleBatch torusBatch; //大球
GLTriangleBatch sphereBatch; //小球
GLBatch floorBatch; //地板

//角色帧 照相机角色帧
GLFrame cameraFrame;
GLFrame objectFrame;

//纹理标记数组
GLuint uiTextures[3];

//添加附加随机球
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

void changeSize(int w,int h) {
    //设置视口
    glViewport(0, 0, w, h);
    
    //创建投影矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
    //把投影矩阵压栈
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelViewMatrix.LoadIdentity();
    
    //为管道提供堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void drawSomething(GLfloat yRot) {
    //定义光源位置&漫反射颜色
    static GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static GLfloat vLightPos[] = { 0.0f, 3.0f, 0.0f, 1.0f };
    
    //绘制悬浮小球
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    for (int i = 0; i < NUM_SPHERES; i++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,modelViewMatrix.GetMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vWhite,0);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    
    //绘制大球
    modelViewMatrix.Translate(0.0f, 0.2f, -2.5f);
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,modelViewMatrix.GetMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vWhite,0);
    torusBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    //绘制公转小球
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot * (-2.0f), 0, 1, 0);
    modelViewMatrix.Translate(0.8, 0, 0);
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,modelViewMatrix.GetMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vWhite,0);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix();
}
 
//绘制 渲染
void RenderScene(void) {
    
    //设置地板颜色
    static GLfloat vFloorColor[] = { 1.0f, 1.0f, 0.0f, 0.75f};
    
    //设置动画计时器
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60;
    
    //清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    modelViewMatrix.PushMatrix();
    
    //设置相机
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);
    //因为相机伴随观察着整个变换过程 所以不能设置完直接出栈 否则将不能观察到后续的变换内容
    
    //镜面部分 压栈
    modelViewMatrix.PushMatrix();
    
    //翻转 围绕Y轴 用缩放函数实现
    modelViewMatrix.Scale(1, -1, 1);
    //Y轴移动一段距离 产生镜面效果
    modelViewMatrix.Translate(0, 0.8, 0);
    
    //指定顺时针为正面
    glFrontFace(GL_CW);
    
    //绘制镜面效果
    drawSomething(yRot);
    
    //恢复为逆时针为正面
    glFrontFace(GL_CCW);
    
    //镜面绘制完 恢复矩阵
    modelViewMatrix.PopMatrix();
    
    //开启混合功能(绘制地板)
    glEnable(GL_BLEND);
    //指定glBlendFunc 颜色混合方程式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //绑定地面纹理
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    
    /*.
     纹理调整着色器(将一个基本色乘以一个取自纹理的单元nTextureUnit的纹理)
     参数1：GLT_SHADER_TEXTURE_MODULATE
     参数2：模型视图投影矩阵
     参数3：颜色
     参数4：纹理单元（第0层的纹理单元）
     
     */
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,transformPipeline.GetModelViewProjectionMatrix(),vFloorColor,0);
//    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,transformPipeline.GetModelViewProjectionMatrix(),0);
    
    //开始绘制
    floorBatch.Draw();
    //取消混合
    glDisable(GL_BLEND);
    
    //绘制地面上方的球
    drawSomething(yRot);
    
    //绘制玩 恢复矩阵
    modelViewMatrix.PopMatrix();
    
    //观察者矩阵 pop
    modelViewMatrix.PopMatrix();
    
    //交换缓存区
    glutSwapBuffers();
    
    //提交重新渲染
    glutPostRedisplay();
}

bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {

    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    //1.读取纹理数据
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if(pBits == NULL)
        return false;
    
    //2、设置纹理参数
    //参数1：纹理维度
    //参数2：为S/T坐标设置模式
    //参数3：wrapMode,环绕模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    //参数1：纹理维度
    //参数2：线性过滤
    //参数3：wrapMode,环绕模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    //3.载入纹理
    //参数1：纹理维度
    //参数2：mip贴图层次
    //参数3：纹理单元存储的颜色成分（从读取像素图是获得）-将内部参数nComponents改为了通用压缩纹理格式GL_COMPRESSED_RGB
    //参数4：加载纹理宽
    //参数5：加载纹理高
    //参数6：加载纹理的深度
    //参数7：像素数据的数据类型（GL_UNSIGNED_BYTE，每个颜色分量都是一个8位无符号整数）
    //参数8：指向纹理图像数据的指针
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, nWidth, nHeight, 0,
                 eFormat, GL_UNSIGNED_BYTE, pBits);
    
    //使用完毕释放pBits
    free(pBits);
    
    //只有minFilter 等于以下四种模式，才可以生成Mip贴图
    //GL_NEAREST_MIPMAP_NEAREST具有非常好的性能，并且闪烁现象非常弱
    //GL_LINEAR_MIPMAP_NEAREST常常用于对游戏进行加速，它使用了高质量的线性过滤器
    //GL_LINEAR_MIPMAP_LINEAR 和GL_NEAREST_MIPMAP_LINEAR 过滤器在Mip层之间执行了一些额外的插值，以消除他们之间的过滤痕迹。
    //GL_LINEAR_MIPMAP_LINEAR 三线性Mip贴图。纹理过滤的黄金准则，具有最高的精度。
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
    //4.加载Mip,纹理生成所有的Mip层
    //参数：GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    return true;
}

void setupRC() {
    //清除缓存
    glClearColor(0, 0, 0, 1);
    //初始化
    shaderManager.InitializeStockShaders();
    
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    //开启背面剔除
    glEnable(GL_CULL_FACE);
    
    //地板创建(物体坐标系)
    GLfloat texSize = 10.0f;
    floorBatch.Begin(GL_TRIANGLE_FAN, 4,1);
    floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    floorBatch.Vertex3f(-20.f, -0.41f, 20.0f);
    
    floorBatch.MultiTexCoord2f(0, texSize, 0.0f);
    floorBatch.Vertex3f(20.0f, -0.41f, 20.f);
    
    floorBatch.MultiTexCoord2f(0, texSize, texSize);
    floorBatch.Vertex3f(20.0f, -0.41f, -20.0f);
    
    floorBatch.MultiTexCoord2f(0, 0.0f, texSize);
    floorBatch.Vertex3f(-20.0f, -0.41f, -20.0f);
    floorBatch.End();
    
    //初始化大球
    gltMakeSphere(torusBatch, 0.4f, 40, 80);
    
    //初始化小球
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
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
    
    //命名纹理对象
    glGenTextures(3, uiTextures);
    
    //加载tga文件
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    LoadTGATexture("Marble.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    LoadTGATexture("Marslike.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    LoadTGATexture("MoonLike.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
}

// 特殊键位处理（上、下、左、右移动）
void SpecialKeys(int key, int x, int y) {
    
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));
    
    if(key == GLUT_KEY_UP) {
        cameraFrame.MoveForward(linear);
    }
    if(key == GLUT_KEY_DOWN) {
        cameraFrame.MoveForward(-linear);
    }
    if(key == GLUT_KEY_LEFT) {
        cameraFrame.RotateWorld(angular, 0, 1, 0);
    }
    if(key == GLUT_KEY_RIGHT) {
        cameraFrame.RotateWorld(-angular, 0, 1, 0);
    }
    
    //重新渲染
    glutPostRedisplay();
}

//删除纹理
void ShutdownRC(void) {
    glDeleteTextures(3, uiTextures);
}

int main(int argc,char *argv[]) {

    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    //申请一个颜色缓存区、深度缓存区、双缓存区、模板缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置window 的尺寸
    glutInitWindowSize(800, 600);
    //创建window的名称
    glutCreateWindow("SphereWorld");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(changeSize);
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
    ShutdownRC();
    
    return  0;
    
}
