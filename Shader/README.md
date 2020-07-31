**先放结果图**

![](https://upload-images.jianshu.io/upload_images/8416233-3bb9c326eb0eacce.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**准备shader文件**

shader文件其实只是一段字符串，所以无法在其中下断点，必须书写规范，命名严格统一，否则数据将无法交互

**shaderv.vsh**

```
attribute vec4 position;
attribute vec2 textCoordinate;
varying lowp vec2 varyTextCoord;

void main(){
    varyTextCoord = textCoordinate;
    gl_Position = position;
}
```

**shaderf.fsh**

```
precision highp float;

varying lowp vec2 varyTextCoord;
uniform sampler2D colorMap;

void main (){
    lowp vec4 temp = texture2D(colorMap,varyTextCoord);
    gl_FragColor = temp;
}
```

**注意点**

- shader文件中尽可能避免注释，特别是中文注释，容易引起不必要的错误

**准备工作完成后创建一个UIView，用于加载图片**

**导入OpenGL头文件**

```
#import <OpenGLES/ES2/gl.h>
```

**绘制的主要步骤**

  - 创建图层
  - 创建上下文
  - 清空缓存区
  - 设置`RenderBuffer`
  - 设置`FrameBuffer`
  - 开始绘制

**创建layer**

```
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
```

**创建上下文**

```
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
```

**清空缓冲区**

```
- (void)deleteRenderAndFrameBuffer {
    //Frame Buffer Object : FBO
    //Render Buffer : 颜色缓冲区，深度缓冲区，模板缓冲区
    glDeleteBuffers(1, &_myColorRenderBuffer);
    self.myColorRenderBuffer = 0;
    
    glDeleteBuffers(1, &_myColorFrameBuffer);
    self.myColorFrameBuffer = 0;
}
```

**开辟渲染缓冲区**

```
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
```

**开辟帧缓冲区**

```
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
```

**开始绘制**

```
- (void)renderLayer {
    //设置清屏颜色
    glClearColor(0.3f, 0.45f, 0.5f, 1.0f);
    //清除屏幕
    glClear(GL_COLOR_BUFFER_BIT);
    
    //1.设置视口大小
    CGFloat scale = [[UIScreen mainScreen] scale];
    glViewport(self.frame.origin.x * scale, self.frame.origin.y * scale, self.frame.size.width * scale, self.frame.size.height * scale);
    
    //读取顶点着色程序、片元着色程序
    NSString *vertFile = [[NSBundle mainBundle] pathForResource:@"shaderv.vsh" ofType:nil];
    NSString *fragFile = [[NSBundle mainBundle] pathForResource:@"shaderf.fsh" ofType:nil];
    
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
```

**shader文件的读取于绑定**

```
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
```

**准备顶点数据/纹理坐标**

```
- (void)setData {
    //前3个是顶点坐标，后2个是纹理坐标
    GLfloat attrArr[] = {
        0.5f, -0.5f, -1.0f,     1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,     0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,    0.0f, 0.0f,
        
        0.5f, 0.5f, -1.0f,      1.0f, 1.0f,
        -0.5f, 0.5f, -1.0f,     0.0f, 1.0f,
        0.5f, -0.5f, -1.0f,     1.0f, 0.0f,
    };
    
    //顶点数据 copy 顶点缓冲区
    GLuint attrBuffer;
    //申请一个缓存区标识符
    glGenBuffers(1, &attrBuffer);
    //将attrBuffer绑定到GL_ARRAY_BUFFER标识符上
    glBindBuffer(GL_ARRAY_BUFFER, attrBuffer);
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
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, NULL);
    
    //打开纹理通道
    GLuint textCoor = glGetAttribLocation(self.myPrograme, "textCoordinate");
    glEnableVertexAttribArray(textCoor);
    glVertexAttribPointer(textCoor, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, (float *)NULL + 3);
    
    //加载纹理
    [self setTexture:@"Snow.jpeg"];
    
    //设置纹理采样器 sampler2D
    glUniform1i(glGetUniformLocation(self.myPrograme, "colorMap"), 0);
    
    //绘图
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //从渲染缓存区显示到屏幕上
    [self.myContext presentRenderbuffer:GL_RENDERBUFFER];
}
```

**加载纹理**

```
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
```
