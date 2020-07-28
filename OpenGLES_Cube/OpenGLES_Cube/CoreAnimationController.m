//
//  CoreAnimationController.m
//  OpenGLES_Cube
//
//  Created by Snow WarLock on 2020/7/28.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "CoreAnimationController.h"

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
