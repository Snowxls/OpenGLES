**先放结果图**

![](https://upload-images.jianshu.io/upload_images/8416233-ec0c1b16c7c1971c.gif?imageMogr2/auto-orient/strip)

**为了更好的存放顶点数据、纹理数据和法线数据，创建一个结构体**

```
typedef struct {
    GLKVector3 positionCoord; //顶点坐标
    GLKVector3 textureCoord; //纹理坐标
    GLKVector3 normal; //法线 (计算光照 叉乘结果)
} SNVertex;
```

**创建context并设置当前context**

```
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    [EAGLContext setCurrentContext:context];
```

**创建glkView并添加到视图中**

```
    //创建glkView
    self.glkView = [[GLKView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height) context:context];
    self.glkView.backgroundColor = [UIColor clearColor];
    self.glkView.delegate = self;

    //深度缓冲区
    self.glkView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    //设置深度缓冲区范围
    glDepthRangef(1, 0);
    
    [self.view addSubview:self.glkView];
```

**纹理载入**

```
    //载入图片
    NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"Snow.jpeg" ofType:nil];
    UIImage *image = [UIImage imageWithContentsOfFile:imagePath];
    
    //纹理转换 GLKit
    NSDictionary *options = @{GLKTextureLoaderOriginBottomLeft:@(YES)};
    GLKTextureInfo *textureInfo = [GLKTextureLoader textureWithCGImage:image.CGImage options:options error:nil];
```

**BaseEffect创建并添加光照**

```
    //baseEffect创建
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.texture2d0.name = textureInfo.name;
    self.baseEffect.texture2d0.target = textureInfo.target;
    
    //开启光照
    self.baseEffect.light0.enabled = YES;
    //设置漫反射颜色
    self.baseEffect.light0.diffuseColor = GLKVector4Make(1, 1, 1, 1);
    //光照的位置
    self.baseEffect.light0.position = GLKVector4Make(-0.5, -0.5, 5, 1);
```

**正方体的顶点坐标数据**

```
    //正方体顶点数据创建 一个面由2个三角形组成 6个顶点 共6面 36个顶点
    self.vertices = malloc(sizeof(SNVertex) * kCoordCount);
    
    // 前面
    self.vertices[0] = (SNVertex){{-0.5, 0.5, 0.5}, {0, 1}, {0, 0, 1}};
    self.vertices[1] = (SNVertex){{-0.5, -0.5, 0.5}, {0, 0}, {0, 0, 1}};
    self.vertices[2] = (SNVertex){{0.5, 0.5, 0.5}, {1, 1}, {0, 0, 1}};
    self.vertices[3] = (SNVertex){{-0.5, -0.5, 0.5}, {0, 0}, {0, 0, 1}};
    self.vertices[4] = (SNVertex){{0.5, 0.5, 0.5}, {1, 1}, {0, 0, 1}};
    self.vertices[5] = (SNVertex){{0.5, -0.5, 0.5}, {1, 0}, {0, 0, 1}};
    
    // 上面
    self.vertices[6] = (SNVertex){{0.5, 0.5, 0.5}, {1, 1}, {0, 1, 0}};
    self.vertices[7] = (SNVertex){{-0.5, 0.5, 0.5}, {0, 1}, {0, 1, 0}};
    self.vertices[8] = (SNVertex){{0.5, 0.5, -0.5}, {1, 0}, {0, 1, 0}};
    self.vertices[9] = (SNVertex){{-0.5, 0.5, 0.5}, {0, 1}, {0, 1, 0}};
    self.vertices[10] = (SNVertex){{0.5, 0.5, -0.5}, {1, 0}, {0, 1, 0}};
    self.vertices[11] = (SNVertex){{-0.5, 0.5, -0.5}, {0, 0}, {0, 1, 0}};
    
    // 下面
    self.vertices[12] = (SNVertex){{0.5, -0.5, 0.5}, {1, 1}, {0, -1, 0}};
    self.vertices[13] = (SNVertex){{-0.5, -0.5, 0.5}, {0, 1}, {0, -1, 0}};
    self.vertices[14] = (SNVertex){{0.5, -0.5, -0.5}, {1, 0}, {0, -1, 0}};
    self.vertices[15] = (SNVertex){{-0.5, -0.5, 0.5}, {0, 1}, {0, -1, 0}};
    self.vertices[16] = (SNVertex){{0.5, -0.5, -0.5}, {1, 0}, {0, -1, 0}};
    self.vertices[17] = (SNVertex){{-0.5, -0.5, -0.5}, {0, 0}, {0, -1, 0}};
    
    // 左面
    self.vertices[18] = (SNVertex){{-0.5, 0.5, 0.5}, {1, 1}, {-1, 0, 0}};
    self.vertices[19] = (SNVertex){{-0.5, -0.5, 0.5}, {0, 1}, {-1, 0, 0}};
    self.vertices[20] = (SNVertex){{-0.5, 0.5, -0.5}, {1, 0}, {-1, 0, 0}};
    self.vertices[21] = (SNVertex){{-0.5, -0.5, 0.5}, {0, 1}, {-1, 0, 0}};
    self.vertices[22] = (SNVertex){{-0.5, 0.5, -0.5}, {1, 0}, {-1, 0, 0}};
    self.vertices[23] = (SNVertex){{-0.5, -0.5, -0.5}, {0, 0}, {-1, 0, 0}};
    
    // 右面
    self.vertices[24] = (SNVertex){{0.5, 0.5, 0.5}, {1, 1}, {1, 0, 0}};
    self.vertices[25] = (SNVertex){{0.5, -0.5, 0.5}, {0, 1}, {1, 0, 0}};
    self.vertices[26] = (SNVertex){{0.5, 0.5, -0.5}, {1, 0}, {1, 0, 0}};
    self.vertices[27] = (SNVertex){{0.5, -0.5, 0.5}, {0, 1}, {1, 0, 0}};
    self.vertices[28] = (SNVertex){{0.5, 0.5, -0.5}, {1, 0}, {1, 0, 0}};
    self.vertices[29] = (SNVertex){{0.5, -0.5, -0.5}, {0, 0}, {1, 0, 0}};
    
    // 后面
    self.vertices[30] = (SNVertex){{-0.5, 0.5, -0.5}, {0, 1}, {0, 0, -1}};
    self.vertices[31] = (SNVertex){{-0.5, -0.5, -0.5}, {0, 0}, {0, 0, -1}};
    self.vertices[32] = (SNVertex){{0.5, 0.5, -0.5}, {1, 1}, {0, 0, -1}};
    self.vertices[33] = (SNVertex){{-0.5, -0.5, -0.5}, {0, 0}, {0, 0, -1}};
    self.vertices[34] = (SNVertex){{0.5, 0.5, -0.5}, {1, 1}, {0, 0, -1}};
    self.vertices[35] = (SNVertex){{0.5, -0.5, -0.5}, {1, 0}, {0, 0, -1}};
```

**将坐标数据从内存读取到GPU**

```
    //坐标拷贝 内存->GPU
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    GLsizeiptr bufferSizeBytes = sizeof(SNVertex) * kCoordCount;
    glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, self.vertices, GL_STATIC_DRAW);
```

**由于iOS默认不开启Attrib，因此需要手动开启通道**

```
    //打开通道 GPU->GLKit->着色器
    
    //顶点数据
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(SNVertex), NULL + offsetof(SNVertex, positionCoord));
    
    //纹理数据
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(SNVertex), NULL + offsetof(SNVertex, textureCoord));
    
    //法线数据
    glEnableVertexAttribArray(GLKVertexAttribNormal);
    glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(SNVertex), NULL + offsetof(SNVertex, normal));
```

**完善GLKViewDelegate方法**

```
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    //清空缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //准备绘制
    [self.baseEffect prepareToDraw];
    //开始绘制
    glDrawArrays(GL_TRIANGLES, 0, kCoordCount);
}
```

**添加定时器**

```
//定时器
- (void)addCADisplayLink {
    self.angle = 0;
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(update)];
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)update {
    //计算旋转角度
    self.angle = (self.angle + 5) % 360;
    self.baseEffect.transform.modelviewMatrix = GLKMatrix4MakeRotation(GLKMathDegreesToRadians(self.angle), 0.3, 1, 0.7);
    //刷新
    [self.glkView display];
}
```

**除了使用OpenGL外，还能使用CoreAnimation实现相同的效果**

```
@interface CoreAnimationController () {
    UIView *view0;
    UIView *view1;
    UIView *view2;
    UIView *view3;
    UIView *view4;
    UIView *view5;
    
    UIView *containerView;
    
    NSArray *faces;
    
    CADisplayLink *displayLink;
    NSInteger angle;
}

@end

@implementation CoreAnimationController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    //添加面
    [self addCFaces];
    //添加CADisplayLink
    [self addCADisplayLink];
    
}

-(void)addCFaces {
    view0 = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 200)];
    view0.layer.contents = (id)[UIImage imageNamed:@"Snow.jpeg"].CGImage;
    view1 = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 200)];
    view1.layer.contents = (id)[UIImage imageNamed:@"Snow.jpeg"].CGImage;
    view2 = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 200)];
    view2.layer.contents = (id)[UIImage imageNamed:@"Snow.jpeg"].CGImage;
    view3 = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 200)];
    view3.layer.contents = (id)[UIImage imageNamed:@"Snow.jpeg"].CGImage;
    view4 = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 200)];
    view4.layer.contents = (id)[UIImage imageNamed:@"Snow.jpeg"].CGImage;
    view5 = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 200, 200)];
    view5.layer.contents = (id)[UIImage imageNamed:@"Snow.jpeg"].CGImage;
    
    containerView = [[UIView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:containerView];
    
    faces = @[view0,view1,view2,view3,view4,view5];
    
    //父View的layer图层
    CATransform3D perspective = CATransform3DIdentity;
    perspective.m34 = -1.0 / 500.0;
    perspective = CATransform3DRotate(perspective, -M_PI_4, 1, 0, 0);
    perspective = CATransform3DRotate(perspective, -M_PI_4, 0, 1, 0);
    containerView.layer.sublayerTransform = perspective;
    
    //add cube face 1
    CATransform3D transform = CATransform3DMakeTranslation(0, 0, 100);
    [self addFace:0 withTransform:transform];
    
    //add cube face 2
    transform = CATransform3DMakeTranslation(100, 0, 0);
    transform = CATransform3DRotate(transform, M_PI_2, 0, 1, 0);
    [self addFace:1 withTransform:transform];
    
    //add cube face 3
    transform = CATransform3DMakeTranslation(0, -100, 0);
    transform = CATransform3DRotate(transform, M_PI_2, 1, 0, 0);
    [self addFace:2 withTransform:transform];
    
    //add cube face 4
    transform = CATransform3DMakeTranslation(0, 100, 0);
    transform = CATransform3DRotate(transform, -M_PI_2, 1, 0, 0);
    [self addFace:3 withTransform:transform];
    
    //add cube face 5
    transform = CATransform3DMakeTranslation(-100, 0, 0);
    transform = CATransform3DRotate(transform, -M_PI_2, 0, 1, 0);
    [self addFace:4 withTransform:transform];
    
    //add cube face 6
    transform = CATransform3DMakeTranslation(0, 0, -100);
    transform = CATransform3DRotate(transform, M_PI, 0, 1, 0);
    [self addFace:5 withTransform:transform];
}

- (void)addFace:(NSInteger)index withTransform:(CATransform3D)transform {
    //获取face视图并将其添加到容器中
    UIView *face = faces[index];
    [containerView addSubview:face];
    
    //将face视图放在容器的中心
    CGSize containerSize = containerView.bounds.size;
    face.center = CGPointMake(containerSize.width / 2.0, containerSize.height / 2.0);
    
    //添加transform
    face.layer.transform = transform;
}

- (void)addCADisplayLink {
    angle = 0;
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(update)];
    [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)update {
    //计算旋转度数
    angle = (angle + 5) % 360;
    float deg = angle * (M_PI / 180);
    CATransform3D temp = CATransform3DIdentity;
    temp = CATransform3DRotate(temp, deg, 0.3, 1, 0.7);
    containerView.layer.sublayerTransform = temp;
}

@end
```