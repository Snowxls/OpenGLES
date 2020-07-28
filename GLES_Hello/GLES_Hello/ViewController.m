//
//  ViewController.m
//  GLES_Hello
//
//  Created by Snow WarLock on 2020/7/27.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ViewController.h"
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

@interface ViewController () {
    EAGLContext *context; //上下文
    
    GLKBaseEffect *mEffect;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // 初始化设置
    [self setUpConfig];
    
    //顶点数据
    [self setupVertexData];
    
    //纹理读取
    [self setupTexture];
    
}

//初始化设置
- (void)setUpConfig {
    //初始化 上下文
    /*
    EAGLContext 是苹果iOS平台下实现OpenGLES 渲染层.
    kEAGLRenderingAPIOpenGLES1 = 1, 固定管线
    kEAGLRenderingAPIOpenGLES2 = 2,
    kEAGLRenderingAPIOpenGLES3 = 3,
    */
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if (!context) {
        NSLog(@"Create ES context Failed !");
    }
    
    //可以有多个上下文 当前只有一个
    [EAGLContext setCurrentContext:context];
    
    //创建GLKView
    GLKView *view = (GLKView *)self.view;
    view.context = context;
    
    /*配置视图创建的渲染缓存区.
    
    (1). drawableColorFormat: 颜色缓存区格式.
    简介:  OpenGL ES 有一个缓存区，它用以存储将在屏幕中显示的颜色。你可以使用其属性来设置缓冲区中的每个像素的颜色格式。
    
    GLKViewDrawableColorFormatRGBA8888 = 0,
    默认.缓存区的每个像素的最小组成部分（RGBA）使用8个bit，（所以每个像素4个字节，4*8个bit）。
    
    GLKViewDrawableColorFormatRGB565,
    如果你的APP允许更小范围的颜色，即可设置这个。会让你的APP消耗更小的资源（内存和处理时间）
    
    (2). drawableDepthFormat: 深度缓存区格式
    
    GLKViewDrawableDepthFormatNone = 0,意味着完全没有深度缓冲区
    GLKViewDrawableDepthFormat16,
    GLKViewDrawableDepthFormat24,
    如果你要使用这个属性（一般用于3D游戏），你应该选择GLKViewDrawableDepthFormat16
    或GLKViewDrawableDepthFormat24。这里的差别是使用GLKViewDrawableDepthFormat16
    将消耗更少的资源
    
    */
    
    //设置颜色缓冲区
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    //设置深度缓冲区
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    //设置背景色
    glClearColor(0, 1, 1, 1);
}

//顶点数据
- (void)setupVertexData {
    /*
     一个正方形由2个三角组成
     前3个为顶点xyz  后2个为纹理坐标st
     
     纹理坐标系
     
     0,1    1,1
     
     0,0    1,0
     
     */
    GLfloat vertextData[] = {
        0.5,-0.5,0.0,   1.0,0.0, //右下
        0.5,0.5,0.0,    1.0,1.0, //右上
        -0.5,0.5,0.0,   0.0,1.0, //左上
        
        0.5,-0.5,0.0,   1.0,0.0, //右下
        -0.5,0.5,0.0,   0.0,1.0, //左上
        -0.5,-0.5,0.0,  0.0,0.0, //左下
    };
    
    //顶点缓冲区  GPU
    GLuint bufferID;
    glGenBuffers(1, &bufferID); //1表示纹理个数 1个纹理
    
    //绑定顶点缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    
    //将顶点数据从 内存 复制到 GPU 中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertextData), vertextData, GL_STATIC_DRAW);
    
    /*
      (1)在iOS中, 默认情况下，出于性能考虑，所有顶点着色器的属性（Attribute）变量都是关闭的.
      意味着,顶点数据在着色器端(服务端)是不可用的. 即使你已经使用glBufferData方法,将顶点数据从内存拷贝到顶点缓存区中(GPU显存中).
      所以, 必须由glEnableVertexAttribArray 方法打开通道.指定访问属性.才能让顶点着色器能够访问到从CPU复制到GPU的数据.
      注意: 数据在GPU端是否可见，即，着色器能否读取到数据，由是否启用了对应的属性决定，这就是glEnableVertexAttribArray的功能，允许顶点着色器读取GPU（服务器端）数据。
    
     (2)方法简介
     glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
    
     功能: 上传顶点数据到显存的方法（设置合适的方式从buffer里面读取数据）
     参数列表:
         index,指定要修改的顶点属性的索引值,例如
         size, 每次读取数量。（如position是由3个（x,y,z）组成，而颜色是4个（r,g,b,a）,纹理则是2个.）
         type,指定数组中每个组件的数据类型。可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。
         normalized,指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）
         stride,指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0. 比如第一个x坐标到第二个x坐标之间的步长
         ptr指定一个指针，指向数组中第一个顶点属性的第一个组件。初始值为0
      */
    
    
    //打开顶点坐标数据的通道
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    //读取顶点数据
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE,sizeof(GLfloat) * 5 , (GLfloat *)NULL + 0);
    
    //打开纹理坐标数据的通道
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    //读取纹理数据
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE,sizeof(GLfloat) * 5 , (GLfloat *)NULL + 3);
}

//纹理读取
- (void)setupTexture {
    //获取图片路径
    NSString *path = [[NSBundle mainBundle] pathForResource:@"Snow.jpeg" ofType:nil];
    
    //设置纹理参数
    
    /*
     纹理坐标原点：左下角
     图片显示原点：左上角
    */
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:@(1),GLKTextureLoaderOriginBottomLeft, nil];
    
    GLKTextureInfo *textureInfo = [GLKTextureLoader textureWithContentsOfFile:path options:options error:nil];
    
    //GLKit BaseRffect 加载图片
    mEffect = [[GLKBaseEffect alloc] init];
    mEffect.texture2d0.enabled = GL_TRUE;
    mEffect.texture2d0.name = textureInfo.name;
    
    
    //投影矩阵设置
    CGFloat aspect = self.view.bounds.size.width / self.view.bounds.size.height;
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(90), aspect, 0.1, 100.0);
    mEffect.transform.projectionMatrix = projectionMatrix;

    //默认在原点 看不到图片 观察者Z轴后退2个单位 用以观察到图片
    GLKMatrix4 modelviewMatrix = GLKMatrix4Translate(GLKMatrix4Identity, 0, 0, -2.0);
    mEffect.transform.modelviewMatrix = modelviewMatrix;
    
}


#pragma mark GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClear(GL_COLOR_BUFFER_BIT);
    
    //准备绘制
    [mEffect prepareToDraw];
    
    //开始绘制
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

@end
