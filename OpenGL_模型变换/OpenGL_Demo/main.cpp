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

// 各种需要的类
GLShaderManager shaderManager; //着色管理器
GLMatrixStack modelViewMatrix; //模型视图矩阵  用于变换
GLMatrixStack projectionMatrix; //投影矩阵
GLFrame cameraFrame; //观察者
GLFrame objectFrame; //当前对象 用于进行旋转、移动等操作

GLFrustum viewFrustum; //视景体 投影矩阵

//容器类（7种不同的图元对应7种容器对象） 每个批次类对应一个图形

GLTriangleBatch triangle; //三角形批次类
GLTriangleBatch sphereBatch; //球
GLTriangleBatch torusBatch; //环
GLTriangleBatch cylinderBatch; //圆柱
GLTriangleBatch coneBatch; //锥
GLTriangleBatch diskBatch; //磁盘


//几何变换的管道
GLGeometryTransform transformPipeline;
M3DMatrix44f shadowMatrix;

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// 跟踪效果步骤
int nStep = 0;

//在窗口大小改变时，接收新的宽度&高度。
//使用窗口维度设置视口和投影矩阵
void changeSize(int w,int h) {
    glViewport(0, 0, w, h);
    //透视投影
    //参数描述   视角度   纵横比（宽/高）  最近距离   最远距离
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 500.0f);
    
    //矩阵堆栈 从viewFrustum中获取投影矩阵并存入projectionMatrix
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    //矩阵堆栈 加载单元矩阵
    modelViewMatrix.LoadIdentity();
    
    //通过GLGeometryTransform管理矩阵堆栈
    //使用transformPipeline 管道管理模型视图矩阵堆栈 和 投影矩阵堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

//描边
void DrawWireFramedBatch(GLTriangleBatch* pBatch) {
    //----绘制图形----
    //1.平面着色器，绘制三角形
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
     //传过来的参数，对应不同的图形Batch
    pBatch->Draw();
    
    //---画出黑色轮廓---
    
    //2.开启多边形偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    //多边形模型(背面、线) 将多边形背面设为线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //开启多边形偏移(设置偏移数量)
    glPolygonOffset(-1.0f, -1.0f);
    //线条宽度
    glLineWidth(2.5f);
    
    //3.开启混合功能(颜色混合&抗锯齿功能)
    glEnable(GL_BLEND);
    //开启处理线段抗锯齿功能
    glEnable(GL_LINE_SMOOTH);
    //设置颜色混合因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   //4.平面着色器绘制线条
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    
    pBatch->Draw();
    
    //5.恢复多边形模式和深度测试
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

//绘制 渲染
void RenderScene(void) {
    //清空缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //压栈 记录状态 可以执行回退操作
    modelViewMatrix.PushMatrix(); //单元矩阵入栈
    
    //camera 观察者矩阵
    M3DMatrix44f mCamera;
    //从camereaFrame中获取矩阵到mCamera
    cameraFrame.GetCameraMatrix(mCamera);
    //矩阵相乘 -> 模型视图矩阵  栈顶 * mCamera = newCamera
    modelViewMatrix.MultMatrix(mCamera);
    
    //物体矩阵
    M3DMatrix44f mObjectFrame;
    //从ObjectFrame 获取矩阵到mOjectFrame中
    objectFrame.GetMatrix(mObjectFrame);
    //矩阵相乘 -> 模型视图矩阵  newCamera * mObjectFrame
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    //模型视图矩阵（观察者矩阵，物体变化矩阵） 投影矩阵 mvp(ModelViewProjection)
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlack);
    
#pragma mark RenderSet
    switch (nStep) {
        case 0: {
            DrawWireFramedBatch(&sphereBatch);
            break;
        }
        case 1: {
            DrawWireFramedBatch(&torusBatch);
            break;
        }
        case 2: {
            DrawWireFramedBatch(&cylinderBatch);
            break;
        }
        case 3: {
            DrawWireFramedBatch(&coneBatch);
            break;
        }
        case 4: {
            DrawWireFramedBatch(&diskBatch);
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

void setupRC() {
    //初始化 设置画布背景
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //着色管理器初始化
    shaderManager.InitializeStockShaders();
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    //设置观察者 将Z轴设置成负数 可以看到图像
    cameraFrame.MoveForward(-15.0f);
    
    // 球
    /*
      gltMakeSphere(GLTriangleBatch& sphereBatch, GLfloat fRadius, GLint iSlices, GLint iStacks);
     参数1：sphereBatch，三角形批次类对象
     参数2：fRadius，球体半径
     参数3：iSlices，从球体底部堆叠到顶部的三角形带的数量；其实球体是一圈一圈三角形带组成
     参数4：iStacks，围绕球体一圈排列的三角形对数
     
     建议：一个对称性较好的球体的片段数量是堆叠数量的2倍，就是iStacks = 2 * iSlices;
     绘制球体都是围绕Z轴，这样+z就是球体的顶点，-z就是球体的底部。
     */
    gltMakeSphere(sphereBatch, 3.0, 10, 20);
    
    // 环面
    /*
     gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
     参数1：torusBatch，三角形批次类对象
     参数2：majorRadius,甜甜圈中心到外边缘的半径
     参数3：minorRadius,甜甜圈中心到内边缘的半径
     参数4：numMajor,沿着主半径的三角形数量
     参数5：numMinor,沿着内部较小半径的三角形数量
     */
    gltMakeTorus(torusBatch, 3.0f, 0.75f, 15, 15);
    
    // 圆柱
    /*
     void gltMakeCylinder(GLTriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, GLfloat fLength, GLint numSlices, GLint numStacks);
     参数1：cylinderBatch，三角形批次类对象
     参数2：baseRadius,底部半径
     参数3：topRadius,头部半径
     参数4：fLength,圆形长度
     参数5：numSlices,围绕Z轴的三角形对的数量
     参数6：numStacks,圆柱底部堆叠到顶部圆环的三角形数量
     */
    gltMakeCylinder(cylinderBatch, 2.0f, 2.0f, 3.0f, 15, 2);
    
    //锥
    /*
     void gltMakeCylinder(GLTriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, GLfloat fLength, GLint numSlices, GLint numStacks);
     参数1：cylinderBatch，三角形批次类对象
     参数2：baseRadius,底部半径
     参数3：topRadius,头部半径
     参数4：fLength,圆形长度
     参数5：numSlices,围绕Z轴的三角形对的数量
     参数6：numStacks,圆柱底部堆叠到顶部圆环的三角形数量
     */
    //圆柱体，从0开始向Z轴正方向延伸。
    //圆锥体，是一端的半径为0，另一端半径可指定。
    gltMakeCylinder(coneBatch, 2.0f, 0.0f, 3.0f, 13, 2);
    
    // 磁盘
    /*
    void gltMakeDisk(GLTriangleBatch& diskBatch, GLfloat innerRadius, GLfloat outerRadius, GLint nSlices, GLint nStacks);
     参数1:diskBatch，三角形批次类对象
     参数2:innerRadius,内圆半径
     参数3:outerRadius,外圆半径
     参数4:nSlices,圆盘围绕Z轴的三角形对的数量
     参数5:nStacks,圆盘外网到内围的三角形数量
     */
    gltMakeDisk(diskBatch, 1.5f, 3.0f, 13, 3);
    
}

//根据空格次数。切换不同的窗口
void KeyPressFunc(unsigned char key, int x, int y) {
    //32对应空格的ASCII码
    if (key == 32) {
        nStep++;
        
        if(nStep > 4)
            nStep = 0;
    }
    
    switch(nStep) {
        case 0:
            //设置窗口名称
            glutSetWindowTitle("GL_SPHERE");
            break;
        case 1:
            glutSetWindowTitle("GL_TORUS");
            break;
        case 2:
            glutSetWindowTitle("GL_CYLINDER");
            break;
        case 3:
            glutSetWindowTitle("GL_CONE");
            break;
        case 4:
            glutSetWindowTitle("GL_DISK");
            break;
    }
    
    //改变模式 触发重新渲染
    glutPostRedisplay();
}

// 特殊键位处理（上、下、左、右移动）
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

int main(int argc,char *argv[]) {

    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    //申请一个颜色缓存区、深度缓存区、双缓存区、模板缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置window 的尺寸
    glutInitWindowSize(800, 600);
    //创建window的名称
    glutCreateWindow("GL_SPHERE");
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
