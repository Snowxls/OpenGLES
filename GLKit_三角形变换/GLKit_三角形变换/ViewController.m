//
//  ViewController.m
//  GLKit_三角形变换
//
//  Created by Snow WarLock on 2020/8/1.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ViewController.h"

@interface ViewController () {
    dispatch_source_t timer;
}

@property(nonatomic,strong)EAGLContext *mContext;
@property(nonatomic,strong)GLKBaseEffect *mEffect;

@property(nonatomic,assign)int count;

//旋转的度数
@property(nonatomic,assign)float XDegree;
@property(nonatomic,assign)float YDegree;
@property(nonatomic,assign)float ZDegree;

//是否旋转X,Y,Z
@property(nonatomic,assign) BOOL XB;
@property(nonatomic,assign) BOOL YB;
@property(nonatomic,assign) BOOL ZB;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self createUI];
    
    //新建图层
    [self setupContext];
    
    //渲染图形
    [self render];
}

- (void)createUI {
    UIButton *btnX = [[UIButton alloc] initWithFrame:CGRectMake(10, self.view.frame.size.height-60, 50, 50)];
    [btnX setTitle:@"X" forState:UIControlStateNormal];
    [btnX setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    btnX.backgroundColor = [UIColor yellowColor];
    [btnX addTarget:self action:@selector(XClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btnX];
    
    UIButton *btnY = [[UIButton alloc] initWithFrame:CGRectMake((self.view.frame.size.width-50)/2, self.view.frame.size.height-60, 50, 50)];
    [btnY setTitle:@"Y" forState:UIControlStateNormal];
    [btnY setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    btnY.backgroundColor = [UIColor orangeColor];
    [btnY addTarget:self action:@selector(YClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btnY];
    
    UIButton *btnZ = [[UIButton alloc] initWithFrame:CGRectMake(self.view.frame.size.width-60, self.view.frame.size.height-60, 50, 50)];
    [btnZ setTitle:@"Z" forState:UIControlStateNormal];
    [btnZ setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    btnZ.backgroundColor = [UIColor blueColor];
    [btnZ addTarget:self action:@selector(ZClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btnZ];
}

- (void)setupContext {
    //新建OpenGL ES上下文
    self.mContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.mContext;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [EAGLContext setCurrentContext:self.mContext];
    glEnable(GL_DEPTH_TEST);
}


-(void)render {
    //顶点数据
    //前3个元素，是顶点数据；中间3个元素，是顶点颜色值，最后2个是纹理坐标
    GLfloat attrArr[] = {
        -0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 1.0f,  1.0f,0.0f,   //左上0
        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 1.0f,  1.0f,1.0f,   //右上1
        -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,  0.0f,0.0f,   //左下2
        
        0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f,  0.0f,1.0f,   //右下3
        0.0f, 0.0f, 1.0f,       0.0f, 1.0f, 0.0f,  0.5f,0.5f,   //顶点4
    };
    
    //绘图索引
    GLuint indices[] = {
        0, 3, 2,
        0, 1, 3,
        0, 2, 4,
        0, 4, 1,
        2, 3, 4,
        1, 4, 3,
    };
    
    //顶点个数
    self.count = sizeof(indices) /sizeof(GLuint);

    //将顶点数组放入数组缓冲区中 GL_ARRAY_BUFFER
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(attrArr), attrArr, GL_STATIC_DRAW);
    
    //将索引数组存储到索引缓冲区 GL_ELEMENT_ARRAY_BUFFER
    GLuint index;
    glGenBuffers(1, &index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //使用顶点数据
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, NULL);
    
    //使用颜色数据
    glEnableVertexAttribArray(GLKVertexAttribColor);
    glVertexAttribPointer(GLKVertexAttribColor, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLfloat *)NULL + 3);
    
    //使用纹理
    //载入图片
    NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"Snow.jpeg" ofType:nil];
    UIImage *image = [UIImage imageWithContentsOfFile:imagePath];
    
    //纹理转换 GLKit
    NSDictionary *options = @{GLKTextureLoaderOriginBottomLeft:@(YES)};
    GLKTextureInfo *textureInfo = [GLKTextureLoader textureWithCGImage:image.CGImage options:options error:nil];
    //打开通道
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLfloat *)NULL + 6);

    //着色器
    self.mEffect = [[GLKBaseEffect alloc] init];
    self.mEffect.texture2d0.name = textureInfo.name;
    self.mEffect.texture2d0.target = textureInfo.target;

    //投影视图
    CGSize size = self.view.bounds.size;
    float aspect = fabs(size.width / size.height);
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(90.0), aspect, 0.1f, 100.0);
    projectionMatrix = GLKMatrix4Scale(projectionMatrix, 1.0f, 1.0f, 1.0f);
    self.mEffect.transform.projectionMatrix = projectionMatrix;
    
    //模型视图
    GLKMatrix4 modelViewMatrix = GLKMatrix4Translate(GLKMatrix4Identity, 0.0f, 0.0f, -2.0f);
    self.mEffect.transform.modelviewMatrix = modelViewMatrix;
    
    
    //定时器
    double seconds = 0.1;
    timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
    dispatch_source_set_timer(timer, DISPATCH_TIME_NOW, seconds * NSEC_PER_SEC, 0.0);
    dispatch_source_set_event_handler(timer, ^{
       
        self.XDegree += 0.1f * self.XB;
        self.YDegree += 0.1f * self.YB;
        self.ZDegree += 0.1f * self.ZB ;
        
    });
    dispatch_resume(timer);

}

//场景数据变化
-(void)update {
    GLKMatrix4 modelViewMatrix = GLKMatrix4Translate(GLKMatrix4Identity, 0.0f, 0.0f, -2.5);
    
    modelViewMatrix = GLKMatrix4RotateX(modelViewMatrix, self.XDegree);
    modelViewMatrix = GLKMatrix4RotateY(modelViewMatrix, self.YDegree);
    modelViewMatrix = GLKMatrix4RotateZ(modelViewMatrix, self.ZDegree);
    
    self.mEffect.transform.modelviewMatrix = modelViewMatrix;
    
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClearColor(1, 1, 1, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    [self.mEffect prepareToDraw];
    glDrawElements(GL_TRIANGLES, self.count, GL_UNSIGNED_INT, 0);
}

#pragma mark -XYZClick
- (void)XClick:(UIButton *)sender {
    _XB = !_XB;
}
- (void)YClick:(UIButton *)sender {
    _YB = !_YB;
}
- (void)ZClick:(UIButton *)sender {
    _ZB = !_ZB;
}

@end
