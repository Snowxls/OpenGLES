//
//  ShaderView.m
//  Shader
//
//  Created by Snow WarLock on 2020/7/30.
//  Copyright © 2020 Snowxls. All rights reserved.
//


/*
不采样GLKBaseEffect，使用编译链接自定义的着色器（shader）。用简单的glsl语言来实现顶点、片元着色器，并图形进行简单的变换。
思路：
  1.创建图层
  2.创建上下文
  3.清空缓存区
  4.设置RenderBuffer
  5.设置FrameBuffer
  6.开始绘制

*/

#import "ShaderView.h"
#import <OpenGLES/ES2/gl.h>
#import "GLESMath.h"
#import "GLESUtils.h"

@interface ShaderView (){
    float xDegree;
    float yDegree;
    float zDegree;
    BOOL bX;
    BOOL bY;
    BOOL bZ;
    NSTimer* myTimer;
}

//在iOS和tvOS上绘制OpenGL ES内容的图层 继承与CALayer
//核心动画 特殊图层的一种
@property (nonatomic,strong) CAEAGLLayer *myEagLayer;

@property (nonatomic,strong) EAGLContext *myContext;

@property (nonatomic,assign)GLuint myColorRenderBuffer;
@property (nonatomic,assign)GLuint myColorFrameBuffer;

@property (nonatomic,assign)GLuint myPrograme;
@property (nonatomic,assign)GLuint myVertices;

@end

@implementation ShaderView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self createUI];
    }
    return self;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        [self createUI];
    }
    return self;
}

- (void)createUI {
    UIButton *btnX = [[UIButton alloc] initWithFrame:CGRectMake(10, self.frame.size.height-60, 50, 50)];
    [btnX setTitle:@"X" forState:UIControlStateNormal];
    [btnX setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    btnX.backgroundColor = [UIColor yellowColor];
    [btnX addTarget:self action:@selector(XClick:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:btnX];
    
    UIButton *btnY = [[UIButton alloc] initWithFrame:CGRectMake((self.frame.size.width-50)/2, self.frame.size.height-60, 50, 50)];
    [btnY setTitle:@"Y" forState:UIControlStateNormal];
    [btnY setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    btnY.backgroundColor = [UIColor orangeColor];
    [btnY addTarget:self action:@selector(YClick:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:btnY];
    
    UIButton *btnZ = [[UIButton alloc] initWithFrame:CGRectMake(self.frame.size.width-60, self.frame.size.height-60, 50, 50)];
    [btnZ setTitle:@"Z" forState:UIControlStateNormal];
    [btnZ setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    btnZ.backgroundColor = [UIColor blueColor];
    [btnZ addTarget:self action:@selector(ZClick:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:btnZ];
    
}

- (void)layoutSubviews {
    
    //准备工作
    [self createLayer];
    [self createContext];
    [self deleteRenderAndFrameBuffer];
    [self createRenderBuffer];
    [self createFrameBuffer];
    
    //开始绘制
    [self renderLayer];
}

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

//设置图层
- (void)createLayer {
    //创建图层
    self.myEagLayer = (CAEAGLLayer *)self.layer;
    
    //设置scale
    [self setContentScaleFactor:[[UIScreen mainScreen] scale]];
    
    //设置属性
    /*
        kEAGLDrawablePropertyRetainedBacking  表示绘图表面显示后，是否保留其内容。
        kEAGLDrawablePropertyColorFormat
            可绘制表面的内部颜色缓存区格式，这个key对应的值是一个NSString指定特定颜色缓存区对象。默认是kEAGLColorFormatRGBA8；
        
            kEAGLColorFormatRGBA8：32位RGBA的颜色，4*8=32位
            kEAGLColorFormatRGB565：16位RGB的颜色，
            kEAGLColorFormatSRGBA8：sRGB代表了标准的红、绿、蓝，即CRT显示器、LCD显示器、投影机、打印机以及其他设备中色彩再现所使用的三个基本色素。sRGB的色彩空间基于独立的色彩坐标，可以使色彩在不同的设备使用传输中对应于同一个色彩坐标体系，而不受这些设备各自具有的不同色彩坐标的影响。


        */
    self.myEagLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:@false,kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8,kEAGLDrawablePropertyColorFormat,nil];
}

//设置上下文
- (void)createContext {
    //创建context
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (!context) {
        NSLog(@"create context failed");
        return;
    }
    
    //设置当前context
    if (![EAGLContext setCurrentContext:context]) {
        NSLog(@"setCurrentContext failed");
        return;
    }
    self.myContext = context;
}

//清空缓冲区
- (void)deleteRenderAndFrameBuffer {
    //Frame Buffer Object : FBO
    //Render Buffer : 颜色缓冲区，深度缓冲区，模板缓冲区
    glDeleteBuffers(1, &_myColorRenderBuffer);
    self.myColorRenderBuffer = 0;
    
    glDeleteBuffers(1, &_myColorFrameBuffer);
    self.myColorFrameBuffer = 0;
}

//开辟渲染缓冲区
- (void)createRenderBuffer {
    //定义bufferID
    GLuint buffer;
    
    //创建一个缓冲区标记
    glGenRenderbuffers(1, &buffer);
    
    self.myColorRenderBuffer = buffer;
    
    //绑定
    glBindRenderbuffer(GL_RENDERBUFFER, self.myColorRenderBuffer);
    
    //将可绘制对象drawable object's  CAEAGLLayer的存储绑定到OpenGL ES renderBuffer对象
    [self.myContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:self.myEagLayer];
}

//开辟帧缓冲区
- (void)createFrameBuffer {
    //定义一个bufferID
    GLuint buffer;
    
    //创建一个缓冲区标记
    glGenFramebuffers(1, &buffer);
    
    self.myColorFrameBuffer = buffer;
    
    //绑定
    glBindFramebuffer(GL_FRAMEBUFFER, self.myColorFrameBuffer);
    
    /*生成帧缓存区之后，则需要将renderbuffer跟framebuffer进行绑定，
     调用glFramebufferRenderbuffer函数进行绑定到对应的附着点上，后面的绘制才能起作用
     */
    
    //将渲染缓存区myColorRenderBuffer 通过glFramebufferRenderbuffer函数绑定到 GL_COLOR_ATTACHMENT0上
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.myColorRenderBuffer);
}

//开始绘制
- (void)renderLayer {
    //设置清屏颜色
    glClearColor(1, 1, 1, 1.0);
    //清除屏幕
    glClear(GL_COLOR_BUFFER_BIT);
    
    //设置视口大小
    CGFloat scale = [[UIScreen mainScreen] scale];
    glViewport(self.frame.origin.x * scale, self.frame.origin.y * scale, self.frame.size.width * scale, self.frame.size.height * scale);
    
    //读取顶点着色程序、片元着色程序
    NSString *vertFile = [[NSBundle mainBundle] pathForResource:@"shaderv.glsl" ofType:nil];
    NSString *fragFile = [[NSBundle mainBundle] pathForResource:@"shaderf.glsl" ofType:nil];
    
    //判断self.myProgram是否存在，存在则清空其文件
    if (self.myPrograme) {
        glDeleteProgram(self.myPrograme);
        self.myPrograme = 0;
    }
    
    //加载shader
    self.myPrograme = [self loaderShaders:vertFile withFrag:fragFile];
    
    //Program链接
    glLinkProgram(self.myPrograme);
    
    //获取链接的状态
    GLint linkStatus;
    glGetProgramiv(self.myPrograme, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        //link 失败信息
        GLchar message[512]; //先暂定512 可以更多，也可以更少
        glGetProgramInfoLog(self.myPrograme, sizeof(message), 0, &message[0]);
        NSString *messageString = [NSString stringWithUTF8String:message];
        NSLog(@"Programe Link Error: %@",messageString);
        return;
    }
    
    NSLog(@"Program Link Success!");
    
    //使用Program
    glUseProgram(self.myPrograme);
    
    [self setData];
}

//准备顶点数据/纹理坐标
- (void)setData {
    //创建顶点数组 & 索引数组
    //(1)顶点数组 前3顶点值（x,y,z），后3位颜色值(RGB) 默认透明度1 最后2位是纹理坐标
    GLfloat attrArr[] = {
        -0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 1.0f,  1.0f,0.0f,   //左上0
        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 1.0f,  1.0f,1.0f,   //右上1
        -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,  0.0f,0.0f,   //左下2
        
        0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f,  0.0f,1.0f,   //右下3
        0.0f, 0.0f, 1.0f,       0.0f, 1.0f, 0.0f,  0.5f,0.5f,   //顶点4
    };
    
    //(2).索引数组
    GLuint indices[] = {
        0, 3, 2,
        0, 1, 3,
        0, 2, 4,
        0, 4, 1,
        2, 3, 4,
        1, 4, 3,
    };
    
    if (self.myVertices == 0) {
        glGenBuffers(1, &_myVertices);
    }
    
    //将attrBuffer绑定到GL_ARRAY_BUFFER标识符上
    glBindBuffer(GL_ARRAY_BUFFER, _myVertices);
    //把顶点数据从CPU内存复制到GPU上
    glBufferData(GL_ARRAY_BUFFER, sizeof(attrArr), attrArr, GL_DYNAMIC_DRAW);
    
    //打开通道
    
    //将顶点数据通过myPrograme中的传递到顶点着色程序的position
    //glGetAttribLocation,用来获取vertex attribute的入口的.
    //注意：第二参数字符串必须和shaderv.vsh中的输入变量：position保持一致
    GLuint position = glGetAttribLocation(self.myPrograme, "position");
    //告诉OpenGL ES,通过glEnableVertexAttribArray
    glEnableVertexAttribArray(position);
    //最后数据是通过glVertexAttribPointer传递过去的
    //参数1：index,顶点数据的索引
    //参数2：size,每个顶点属性的组件数量，1，2，3，或者4.默认初始值是4.
    //参数3：type,数据中的每个组件的类型，常用的有GL_FLOAT,GL_BYTE,GL_SHORT。默认初始值为GL_FLOAT
    //参数4：normalized,固定点数据值是否应该归一化，或者直接转换为固定值。（GL_FALSE）
    //参数5：stride,连续顶点属性之间的偏移量，默认为0；
    //参数6：指定一个指针，指向数组中的第一个顶点属性的第一个组件。默认为0
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, NULL);
    
    //打开颜色通道
    GLuint positionColor = glGetAttribLocation(self.myPrograme, "positionColor");
    glEnableVertexAttribArray(positionColor);
    glVertexAttribPointer(positionColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (float *)NULL + 3);
    
    //纹理
    GLuint textCoor = glGetAttribLocation(self.myPrograme, "textCoordinate");
    glEnableVertexAttribArray(textCoor);
    glVertexAttribPointer(textCoor, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (float *)NULL + 6);
    
    
    //MVP
    GLuint projectionMatrixSlot = glGetUniformLocation(self.myPrograme, "projectionMatrix");
    GLuint modelViewMatrixSlot = glGetUniformLocation(self.myPrograme, "modelViewMatrix");
    
    float width = self.frame.size.width;
    float height = self.frame.size.height;
    
    //创建4 * 4投影矩阵
    KSMatrix4 _projectionMatrix;
    //(1)获取单元矩阵
    ksMatrixLoadIdentity(&_projectionMatrix);
    //(2)计算纵横比例 = 长/宽
    float aspect = width / height; //长宽比
    //(3)获取透视矩阵
    /*
     参数1：矩阵
     参数2：视角，度数为单位
     参数3：纵横比
     参数4：近平面距离
     参数5：远平面距离
     参考PPT
     */
    ksPerspective(&_projectionMatrix, 30.0, aspect, 5.0f, 20.0f); //透视变换，视角30°
    //(4)将投影矩阵传递到顶点着色器
    /*
     void glUniformMatrix4fv(GLint location,  GLsizei count,  GLboolean transpose,  const GLfloat *value);
     参数列表：
     location:指要更改的uniform变量的位置
     count:更改矩阵的个数
     transpose:是否要转置矩阵，并将它作为uniform变量的值。必须为GL_FALSE
     value:执行count个元素的指针，用来更新指定uniform变量
     */
    glUniformMatrix4fv(projectionMatrixSlot, 1, GL_FALSE, (GLfloat*)&_projectionMatrix.m[0][0]);
    
    //创建一个4 * 4 矩阵，模型视图矩阵
    KSMatrix4 _modelViewMatrix;
    //(1)获取单元矩阵
    ksMatrixLoadIdentity(&_modelViewMatrix);
    //(2)平移，z轴平移-10
    ksTranslate(&_modelViewMatrix, 0.0, 0.0, -10.0);
    //(3)创建一个4 * 4 矩阵，旋转矩阵
    KSMatrix4 _rotationMatrix;
    //(4)初始化为单元矩阵
    ksMatrixLoadIdentity(&_rotationMatrix);
    //(5)旋转
    ksRotate(&_rotationMatrix, xDegree, 1.0, 0.0, 0.0); //绕X轴
    ksRotate(&_rotationMatrix, yDegree, 0.0, 1.0, 0.0); //绕Y轴
    ksRotate(&_rotationMatrix, zDegree, 0.0, 0.0, 1.0); //绕Z轴
    //(6)把变换矩阵相乘.将_modelViewMatrix矩阵与_rotationMatrix矩阵相乘，结合到模型视图
    ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
    //(7)将模型视图矩阵传递到顶点着色器
    /*
     void glUniformMatrix4fv(GLint location,  GLsizei count,  GLboolean transpose,  const GLfloat *value);
     参数列表：
     location:指要更改的uniform变量的位置
     count:更改矩阵的个数
     transpose:是否要转置矩阵，并将它作为uniform变量的值。必须为GL_FALSE
     value:执行count个元素的指针，用来更新指定uniform变量
     */
    glUniformMatrix4fv(modelViewMatrixSlot, 1, GL_FALSE, (GLfloat*)&_modelViewMatrix.m[0][0]);
    
    //正背面剔除
    glEnable(GL_CULL_FACE);
    
    //加载纹理
    [self setTexture:@"Snow.jpeg"];
    
    //设置纹理采样器 sampler2D
    glUniform1i(glGetUniformLocation(self.myPrograme, "colorMap"), 0);
    
    //索引绘制
    /*
    void glDrawElements(GLenum mode,GLsizei count,GLenum type,const GLvoid * indices);
    参数列表：
    mode:要呈现的画图的模型
               GL_POINTS
               GL_LINES
               GL_LINE_LOOP
               GL_LINE_STRIP
               GL_TRIANGLES
               GL_TRIANGLE_STRIP
               GL_TRIANGLE_FAN
    count:绘图个数
    type:类型
            GL_BYTE
            GL_UNSIGNED_BYTE
            GL_SHORT
            GL_UNSIGNED_SHORT
            GL_INT
            GL_UNSIGNED_INT
    indices：绘制索引数组

    */
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, indices);
    
    //从渲染缓存区显示到屏幕上
    [self.myContext presentRenderbuffer:GL_RENDERBUFFER];
}

//加载纹理
- (GLuint)setTexture:(NSString *)fileName {
    //将 UIImage 转换为 CGImageRef
    CGImageRef spriteImage = [UIImage imageNamed:fileName].CGImage;
    
    //判断图片是否获取成功
    if (!spriteImage) {
        NSLog(@"Failed to load image %@", fileName);
        return 1;
    }
    
    //读取图片的大小，宽和高
    size_t width = CGImageGetWidth(spriteImage);
    size_t height = CGImageGetHeight(spriteImage);
    
    //获取图片字节数 宽*高*4（RGBA）
    GLubyte * spriteData = (GLubyte *) calloc(width * height * 4, sizeof(GLubyte));
    
    //创建上下文
    /*
    参数1：data,指向要渲染的绘制图像的内存地址
    参数2：width,bitmap的宽度，单位为像素
    参数3：height,bitmap的高度，单位为像素
    参数4：bitPerComponent,内存中像素的每个组件的位数，比如32位RGBA，就设置为8
    参数5：bytesPerRow,bitmap的没一行的内存所占的比特数
    参数6：colorSpace,bitmap上使用的颜色空间  kCGImageAlphaPremultipliedLast:RGBA
    */
    CGContextRef spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width*4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
    
    //绘制图片
    CGRect rect = CGRectMake(0, 0, width, height);
    //翻转
    CGContextTranslateCTM(spriteContext, 0, height);
    CGContextScaleCTM(spriteContext, 1.0, -1.0);
    CGContextDrawImage(spriteContext, rect, spriteImage);
    
    //绘制完释放
    CGContextRelease(spriteContext);
    
    //绑定纹理到默认的纹理ID(默认纹理ID为0)
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //设置纹理属性
    /*
    参数1：纹理维度
    参数2：线性过滤、为s,t坐标设置模式
    参数3：wrapMode,环绕模式
    */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    //载入纹理2D数据
    /*
    参数1：纹理模式，GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
    参数2：加载的层次，一般设置为0
    参数3：纹理的颜色值GL_RGBA
    参数4：宽
    参数5：高
    参数6：border，边界宽度
    参数7：format
    参数8：type
    参数9：纹理数据
    */
    float fw = width,fh = height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fw, fh, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
    
    //释放spriteData
    free(spriteData);
    
    return 0;
}

#pragma mark shader
- (GLuint)loaderShaders:(NSString *)vert withFrag:(NSString *)frag {
    //定义顶点着色器/片元着色器对象
    GLuint verShader, fragShader;
    //program
    GLuint program = glCreateProgram();
    
    //编辑顶点着色器 片元着色器
    [self compileShader:&verShader type:GL_VERTEX_SHADER file:vert];
    [self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:frag];
    
    //shader 附着到 program
    glAttachShader(program, verShader);
    glAttachShader(program, fragShader);
    
    //用完后删除shader对象
    glDeleteShader(verShader);
    glDeleteShader(fragShader);
    
    return program;
}

//编辑shader
- (void)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file {
    
    //读取路径
    NSString* content = [NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil];
    const GLchar* source = (GLchar *)[content UTF8String];
    
    //创建一个对应类型的shader
    *shader = glCreateShader(type);
    
    //将着色器源码附着到着色器对象上
    //参数1：shader,要编译的着色器对象 *shader
    //参数2：numOfStrings,传递的源码字符串数量 1个
    //参数3：strings,着色器程序的源码（真正的着色器程序源码）
    //参数4：lenOfStrings,长度，具有每个字符串长度的数组，或NULL，这意味着字符串是NULL终止的
    glShaderSource(*shader, 1, &source,NULL);
    
    //编译
    glCompileShader(*shader);
}

#pragma mark - XYClick
- (void)XClick:(UIButton *)sender {
    //开启定时器
    if (!myTimer) {
        myTimer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(reDegree) userInfo:nil repeats:YES];
    }
    //更新的是X还是Y
    bX = !bX;
}

- (void)YClick:(UIButton *)sender {
    //开启定时器
    if (!myTimer) {
        myTimer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(reDegree) userInfo:nil repeats:YES];
    }
    //更新的是X还是Y
    bY = !bY;
}

- (void)ZClick:(UIButton *)sender {
    //开启定时器
    if (!myTimer) {
        myTimer = [NSTimer scheduledTimerWithTimeInterval:0.05 target:self selector:@selector(reDegree) userInfo:nil repeats:YES];
    }
    //更新的是X还是Y
    bZ = !bZ;
}

-(void)reDegree {
    //如果停止X轴旋转，X = 0则度数就停留在暂停前的度数.
    //更新度数
    xDegree += bX * 5;
    yDegree += bY * 5;
    zDegree += bZ * 5;
    //重新渲染
    [self renderLayer];
    
}

@end













