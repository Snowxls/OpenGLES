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

//4个批次容器类
GLBatch floorBatch;//地面
GLBatch ceilingBatch;//天花板
GLBatch leftWallBatch;//左墙面
GLBatch rightWallBatch;//右墙面

//深度初始值，-65。
GLfloat viewZ = -65.0f;

// 纹理标识符号
#define TEXTURE_BRICK   0 //墙面
#define TEXTURE_FLOOR   1 //地板
#define TEXTURE_CEILING 2 //纹理天花板
#define TEXTURE_COUNT   3 //纹理个数

GLuint  textures[TEXTURE_COUNT];//纹理标记数组
//文件tag名字数组
const char *szTextureFiles[TEXTURE_COUNT] = { "brick.tga", "floor.tga", "ceiling.tga" };

// 将TGA文件加载为2D纹理。
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    //1、读纹理位，读取像素
    //参数1：纹理文件名称
    //参数2：文件宽度地址
    //参数3：文件高度地址
    //参数4：文件组件地址
    //参数5：文件格式地址
    //返回值：pBits,指向图像数据的指针
    
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
    //参数3: 缩小/放大过滤方式.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    
    //3.载入纹理
    //参数1：纹理维度
    //参数2：mip贴图层次
    //参数3：纹理单元存储的颜色成分（从读取像素图是获得）
    //参数4：加载纹理宽
    //参数5：加载纹理高
    //参数6：加载纹理的深度
    //参数7：像素数据的数据类型（GL_UNSIGNED_BYTE，每个颜色分量都是一个8位无符号整数）
    //参数8：指向纹理图像数据的指针
    
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
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
        //4.纹理生成所有的Mip层
        //参数：GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
        glGenerateMipmap(GL_TEXTURE_2D);
    
    return true;
}

void changeSize(int w,int h) {
    //设置视口
    glViewport(0, 0, w, h);
    
    //创建投影矩阵
    viewFrustum.SetPerspective(80.0f, float(w)/float(h), 1.0f, 120.0f);
    
    //把投影矩阵压栈
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //设置变换管道以使用两个矩阵堆栈（变换矩阵modelViewMatrix ，投影矩阵projectionMatrix）
    //初始化GLGeometryTransform 的实例transformPipeline.通过将它的内部指针设置为模型视图矩阵堆栈 和 投影矩阵堆栈实例，来完成初始化
    //当然这个操作也可以在SetupRC 函数中完成，但是在窗口大小改变时或者窗口创建时设置它们并没有坏处。而且这样可以一次性完成矩阵和管线的设置。
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

//绘制 渲染
void RenderScene(void) {
    //清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //当前模型视频压栈
    modelViewMatrix.PushMatrix();
    
    //Z轴平移viewZ 距离
    modelViewMatrix.Translate(0.0f, 0.0f, viewZ);
    
    //纹理替换矩阵着色器
    /*
     参数1：GLT_SHADER_TEXTURE_REPLACE（着色器标签）
     参数2：模型视图投影矩阵
     参数3：纹理层
     */
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, transformPipeline.GetModelViewProjectionMatrix(), 0);
    
    //绑定纹理
    /*
     参数1：纹理模式，GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
     参数2：需要绑定的纹理
     */
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FLOOR]);
    floorBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CEILING]);
    ceilingBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
    leftWallBatch.Draw();
    rightWallBatch.Draw();
    
    //pop
    modelViewMatrix.PopMatrix();
    
    //交换缓存区
    glutSwapBuffers();
}

void setupRC() {
    //清除缓存
    glClearColor(0, 0, 0, 1);
    //初始化
    shaderManager.InitializeStockShaders();
    
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    GLbyte *pBytes;
    GLint iWidth, iHeight, iComponents;
    GLenum eFormat;
    GLint iLoop;
    
    //生成纹理标记
    /** 分配纹理对象 glGenTextures
     参数1：纹理对象的数量
     参数2：纹理对象标识数组
     */
    glGenTextures(TEXTURE_COUNT, textures);
    
    //4. 循环设置纹理数组的纹理参数
    for(iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++) {
        /**绑定纹理对象 glBindTexture
         参数1：纹理模式，GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_3D
         参数2：需要绑定的纹理对象
         */
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        
        /**加载tga文件
         参数1：纹理文件名称
         参数2：文件宽度变量地址
         参数3：文件高度变量地址
         参数4：文件组件变量地址
         参数5：文件格式变量地址
         返回值：pBytes，指向图像数据的指针
         */
        
        pBytes = gltReadTGABits(szTextureFiles[iLoop],&iWidth, &iHeight,&iComponents, &eFormat);
        
        //加载纹理、设置过滤器和包装模式
        //GL_TEXTURE_MAG_FILTER（放大过滤器,GL_NEAREST(最邻近过滤)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //GL_TEXTURE_MIN_FILTER(缩小过滤器),GL_NEAREST(最邻近过滤)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_TEXTURE_WRAP_S(s轴环绕),GL_CLAMP_TO_EDGE(环绕模式强制对范围之外的纹理坐标沿着合法的纹理单元的最后一行或一列进行采样)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //GL_TEXTURE_WRAP_T(t轴环绕)，GL_CLAMP_TO_EDGE(环绕模式强制对范围之外的纹理坐标沿着合法的纹理单元的最后一行或一列进行采样)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        
        /**载入纹理 glTexImage2D
         参数1：纹理维度，GL_TEXTURE_2D
         参数2：mip贴图层次
         参数3：纹理单元存储的颜色成分（从读取像素图中获得）
         参数4：加载纹理宽度
         参数5：加载纹理的高度
         参数6：加载纹理的深度
         参数7：像素数据的数据类型,GL_UNSIGNED_BYTE无符号整型
         参数8：指向纹理图像数据的指针
         */
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        
        /**为纹理对象生成一组完整的mipmap glGenerateMipmap
         参数1：纹理维度，GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_2D
         */
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //释放原始纹理数据，不在需要纹理原始数据了
        free(pBytes);
    }
    
    //设置几何图形顶点/纹理坐标(上.下.左.右)
    GLfloat z;
    
    /*
     GLTools库中的容器类，GBatch，
     void GLBatch::Begin(GLenum primitive,GLuint nVerts,GLuint nTextureUnits = 0);
     参数1：图元枚举值
     参数2：顶点数
     参数3：1组或者2组纹理坐标
     */
    
    //地板
    floorBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    //Z表示深度，隧道的深度
    for(z = 60.0f; z >= 0.0f; z -=10.0f) {
        floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        floorBatch.Vertex3f(-10.0f, -10.0f, z);
        
        floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z);
        
        floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        floorBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
    }
    floorBatch.End();
    
    //天花板
    ceilingBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f) {
        ceilingBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        ceilingBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
        
        ceilingBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
        
        ceilingBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        ceilingBatch.Vertex3f(-10.0f, 10.0f, z);
        
        ceilingBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z);
    }
    ceilingBatch.End();
    
    //左墙
    leftWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f) {
        leftWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        leftWallBatch.Vertex3f(-10.0f, -10.0f, z);
        
        leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z);
        
        leftWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        leftWallBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        leftWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
    }
    leftWallBatch.End();
    
    //右墙
    rightWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f) {
        rightWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        rightWallBatch.Vertex3f(10.0f, -10.0f, z);
        
        rightWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        rightWallBatch.Vertex3f(10.0f, 10.0f, z);
        
        rightWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        rightWallBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
        
        rightWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        rightWallBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
    }
    rightWallBatch.End();
    
}

//关闭渲染环境
void ShutdownRC(void) {
    //删除纹理
    glDeleteTextures(TEXTURE_COUNT, textures);
}

// 特殊键位处理（上、下、左、右移动）
void SpecialKeys(int key, int x, int y) {
    
    if(key == GLUT_KEY_UP) {
        //移动的是深度值，Z
        viewZ += 0.5f;
    }
    if(key == GLUT_KEY_DOWN) {
        //移动的是深度值，Z
        viewZ -= 0.5f;
    }
    //按键 1 ~ 8
    if (key == 49 || key == 50 || key == 51 || key == 52 || key == 53 || key == 54 || key == 55 || key == 56) {
        GLint iLoop;
        
        for(iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++) {
            /**绑定纹理 glBindTexture
             参数1：GL_TEXTURE_2D
             参数2：需要绑定的纹理对象
             */
            glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
            
            /**配置纹理参数 glTexParameteri
             参数1：纹理模式
             参数2：纹理参数
             参数3：特定纹理参数
             
             */
            switch(key) {
                case 49:
                    //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST（最邻近过滤）
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    break;
                    
                case 50:
                    //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR（线性过滤）
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    break;
                    
                case 51:
                    //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_NEAREST（选择最邻近的Mip层，并执行最邻近过滤）
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    break;
                    
                case 52:
                    //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行最邻近过滤）
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                    break;
                    
                case 53:
                    //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（选择最邻近Mip层，并执行线性过滤）
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                    break;
                    
                case 54:
                    //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行线性过滤，又称为三线性过滤）
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    break;
                    
                case 55:
                
                    //设置各向异性过滤
                    GLfloat fLargest;
                    //获取各向异性过滤的最大数量
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
                    //设置纹理参数(各向异性采样)
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
                    break;
            
                case 56:
                    //设置各向同性过滤，数量为1.0表示(各向同性采样)
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
                    break;
                    
            }
        }
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
    glutCreateWindow("Tunnel");
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
    
    //关闭渲染环境
    ShutdownRC();
    
    return  0;
    
}
