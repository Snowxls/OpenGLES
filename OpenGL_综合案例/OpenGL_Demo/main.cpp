#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
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

//**4、添加附加随机球
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

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

//绘制 渲染
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
 
    return  0;
    
}
