#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

//设置角色帧，作为相机
GLFrame             viewFrame;

//使用GLFrustum类来设置透视投影
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

//标记：背面剔除、深度测试
int iCull = 0;
int iDepth = 0;

//在窗口大小改变时，接收新的宽度&高度。
//使用窗口维度设置视口和投影矩阵
void changeSize(int w,int h) {
    //防止h变为0
    if(h == 0) {
        h = 1;
    }
    
    //设置视口窗口尺寸
    glViewport(0, 0, w, h);
    
    //setPerspective函数的参数是一个从顶点方向看去的视场角度（用角度值表示）
    // 设置透视模式，初始化其透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
    //把透视矩阵加载到透视矩阵对阵中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}


//绘制 渲染
void RenderScene(void) {
    //清空缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    if (iCull) {
        //正背面剔除
        glEnable(GL_CULL_FACE); //开启
        glFrontFace(GL_CCW); //默认值 可以不填
        glCullFace(GL_BACK); //默认值 可以不填
    } else {
        glDisable(GL_CULL_FACE); //关闭  会影响整个工程 用完需要关闭
    }
    
    if (iDepth) {
        //深度测试
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
    } else {
        glDisable(GL_DEPTH_TEST); //关闭  会影响整个工程 用完需要关闭
    }
    
    
    modelViewMatix.PushMatrix(viewFrame);
    
    //设置颜色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    
    //平面着色器
    //参数1：平面着色器
    //参数2：模型视图投影矩阵
    //参数3：颜色
    // shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
    
    //默认光源着色器
    //通过光源、阴影效果跟提现立体效果
    //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
    //参数2：模型视图矩阵
    //参数3：投影矩阵
    //参数4：基本颜色值
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);
    
    torusBatch.Draw();
    modelViewMatix.PopMatrix();
    
    glutSwapBuffers();
}

void setupRC() {
    //设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f );
    
    //初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    //将相机向后移动7个单元：肉眼到物体之间的距离
    viewFrame.MoveForward(7.0);
    
    //创建一个甜甜圈
    //void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
    //参数1：GLTriangleBatch 容器帮助类
    //参数2：外边缘半径
    //参数3：内边缘半径
    //参数4、5：主半径和从半径的细分单元数量
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    //点的大小(方便点填充时,肉眼观察)
    glPointSize(4.0f);
    
}

// 特殊键位处理
void SpecialKeys(int key, int x, int y) {
    //根据方向调整观察者位置
    if (key == GLUT_KEY_UP) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    }
    if (key == 113) {
        //按键"Q" 开启/关闭 正表面剔除
        iCull = !iCull;
    }
    if (key == 119) {
        //按键"W" 开启/关闭 深度测试
        iDepth = !iDepth;
    }
    
    if (key == 49) {
        //按键"1"  正常填充
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
    
    if (key == 50) {
        //按键"2"  线填充
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    
    if (key == 51) {
        //按键"3"  点填充
        glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
    }
    
    
    //重新刷新
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
    glutCreateWindow("GL_Donuts");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(changeSize);
    //特殊键位函数（上下左右）
    glutSpecialFunc(SpecialKeys);
    //渲染函数
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
