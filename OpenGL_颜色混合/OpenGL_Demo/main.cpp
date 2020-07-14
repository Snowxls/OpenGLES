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

GLBatch squareBatch;
GLBatch greenBatch;
GLBatch redBatch;
GLBatch blueBatch;
GLBatch blackBatch;

GLShaderManager shaderManager;

GLfloat blockSize = 0.2f;

GLfloat xPos = 0.0f; //记录图形在X轴的偏移数值 用于矩阵移动
GLfloat yPos = 0.0f; //记录图形在Y轴的偏移数值 用于矩阵移动

GLfloat vVerts[] = { -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize,  blockSize, 0.0f,
    -blockSize,  blockSize, 0.0f};

void changeSize(int w,int h) {
    glViewport(0, 0, w, h);
}

//绘制 渲染
void RenderScene(void) {
    //清空缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //开启混合
    glEnable(GL_BLEND);
    //开启组合函数 计算混合颜色因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //定义5种颜色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat vRed1[] = { 1.0f, 0.0f, 0.0f, 0.5f };
    GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 0.5f };
    GLfloat vBlue[] = { 0.0f, 0.0f, 1.0f, 0.5f };
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 0.5f };
    
    
    //使用矩阵
    M3DMatrix44f mTransfromMatrix;
    //平移矩阵
    m3dTranslationMatrix44(mTransfromMatrix, xPos, yPos, 0);
    //平面着色器 （固定着色器）
    shaderManager.UseStockShader(GLT_SHADER_FLAT,mTransfromMatrix,vRed);
    //容器类开始绘制
    squareBatch.Draw();
    
    //渲染场景的时候，将4个固定矩形绘制好
    //使用 单位着色器
    //参数1：简单的使用默认笛卡尔坐标系（-1，1），所有片段都应用一种颜色。GLT_SHADER_IDENTITY
    //参数2：着色器颜色
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vGreen);
    greenBatch.Draw();
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed1);
    redBatch.Draw();
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlue);
    blueBatch.Draw();
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlack);
    blackBatch.Draw();
    
    //关闭混合功能
    glDisable(GL_BLEND);
    
    //同步绘制命令
    glutSwapBuffers();
}

void setupRC() {
    //清屏
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
    //初始化着色管理器
    shaderManager.InitializeStockShaders();
    
    //绘制1个移动矩形
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
    
    //绘制4个固定矩形
    GLfloat vBlock[] = { 0.25f, 0.25f, 0.0f,
        0.75f, 0.25f, 0.0f,
        0.75f, 0.75f, 0.0f,
        0.25f, 0.75f, 0.0f};
    
    greenBatch.Begin(GL_TRIANGLE_FAN, 4);
    greenBatch.CopyVertexData3f(vBlock);
    greenBatch.End();
    
    GLfloat vBlock2[] = { -0.75f, 0.25f, 0.0f,
        -0.25f, 0.25f, 0.0f,
        -0.25f, 0.75f, 0.0f,
        -0.75f, 0.75f, 0.0f};
    
    redBatch.Begin(GL_TRIANGLE_FAN, 4);
    redBatch.CopyVertexData3f(vBlock2);
    redBatch.End();
    
    GLfloat vBlock3[] = { -0.75f, -0.75f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        -0.25f, -0.25f, 0.0f,
        -0.75f, -0.25f, 0.0f};
    
    blueBatch.Begin(GL_TRIANGLE_FAN, 4);
    blueBatch.CopyVertexData3f(vBlock3);
    blueBatch.End();
    
    GLfloat vBlock4[] = { 0.25f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.75f, -0.25f, 0.0f,
        0.25f, -0.25f, 0.0f};
    
    blackBatch.Begin(GL_TRIANGLE_FAN, 4);
    blackBatch.CopyVertexData3f(vBlock4);
    blackBatch.End();
    
}

//上下左右键位控制移动
void SpecialKeys(int key, int x, int y) {
    
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
    glutCreateWindow("颜色混合");
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
