//
//  ViewController.m
//  CAEmitterLayerDemo
//
//  Created by Snow WarLock on 2020/8/4.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ViewController.h"

@interface ViewController () {
    
}

@property (nonatomic, strong) CAEmitterLayer * colorBallLayer;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupEmitter];
}

- (void)setupEmitter {
    
    /*
    emitterShape: 形状:
    1. 点;kCAEmitterLayerPoint .
    2. 线;kCAEmitterLayerLine
    3. 矩形框: kCAEmitterLayerRectangle
    4. 立体矩形框: kCAEmitterLayerCuboid
    5. 圆形: kCAEmitterLayerCircle
    6. 立体圆形: kCAEmitterLayerSphere

    emitterMode:
    kCAEmitterLayerPoints
    kCAEmitterLayerOutline
    kCAEmitterLayerSurface
    kCAEmitterLayerVolume
    
    */
    //设置CAEmitterLayer
    CAEmitterLayer *colorBallLayer = [CAEmitterLayer layer];
    [self.view.layer addSublayer:colorBallLayer];
    self.colorBallLayer = colorBallLayer;
    
    //发射源尺寸大小
    colorBallLayer.emitterSize = self.view.frame.size;
    //发射源形状
    colorBallLayer.emitterShape = kCAEmitterLayerPoint;
    //发射模式
    colorBallLayer.emitterMode = kCAEmitterLayerPoints;
    //粒子发射形状的中心点
    colorBallLayer.emitterPosition = CGPointMake(self.view.layer.bounds.size.width, 0.0f);
    
    //配置CAEmitterCell
    CAEmitterCell *colorBarCell = [CAEmitterCell emitterCell];
    //粒子名称
    colorBarCell.name = @"colorBarCell";
    //粒子产生率 默认为0
    colorBarCell.birthRate = 20.0f;
    //粒子生命周期
    colorBarCell.lifetime = 10.0f;
    //粒子速度 默认为0
    colorBarCell.velocity = 40.0f;
    //粒子速度平均量
    colorBarCell.velocityRange = 100.0f;
    //x,y,z方向上的加速度分量 三者默认为0
    colorBarCell.yAcceleration = 15.0f;
    //指定纬度 纬度角代表了在x-z轴平面坐标系中与x轴之间的夹角 默认为0
    colorBarCell.emissionLatitude = M_PI; //向左
    //发射角度范围 默认为0 以锥形分布的发射角度 角度用弧度制 粒子均匀分布在这个锥形范围
    colorBarCell.emissionRange = M_PI_4; //围绕x轴向左90度
    //缩放比例 默认1
    colorBarCell.scale = 0.2;
    //缩放比例范围 默认0
    colorBarCell.scaleRange = 0.1;
    //在生命周期内的缩放速度 默认0
    colorBarCell.scaleSpeed = 0.02;
    //粒子的内容 CGImageRef对象
    colorBarCell.contents = (id)[[UIImage imageNamed:@"circle_white"] CGImage];
    //颜色
    colorBarCell.color = [[UIColor colorWithRed:0.5 green:0.0f blue:0.5f alpha:1.0f] CGColor];
    //粒子颜色 RGBA能改变的范围 默认0
    colorBarCell.redRange = 1.0f;
    colorBarCell.greenRange = 1.0f;
    colorBarCell.alphaRange = 0.8f;
    //粒子颜色 RGBA在生命周期内的改变速度 默认0
    colorBarCell.blueSpeed = 1.0f;
    colorBarCell.alphaSpeed = -0.1f;
    
    //添加
    colorBallLayer.emitterCells = @[colorBarCell];
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    
    CGPoint point = [self locationFromTouchEvent:event];
    [self setBallInPsition:point];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    CGPoint point = [self locationFromTouchEvent:event];
    [self setBallInPsition:point];
}

/**
 * 获取手指所在点
 */
- (CGPoint)locationFromTouchEvent:(UIEvent *)event{
    UITouch * touch = [[event allTouches] anyObject];
    return [touch locationInView:self.view];
}

/**
 * 移动发射源到某个点上
 */
- (void)setBallInPsition:(CGPoint)position{
    //创建基础动画
    CABasicAnimation *anim = [CABasicAnimation animationWithKeyPath:@"emitterCells.colorBallCell.scale"];
    anim.fromValue = @0.2f;
    anim.toValue = @0.5f;
    anim.duration = 1.1f;
    //线型起搏 是动画在其持续时间内均匀的发生
    anim.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionLinear];
    
    //用事物包装隐式动画
    [CATransaction begin];
    //设置是否禁止由于该事务组内的属性更改而触发的操作
    [CATransaction setDisableActions:YES];
    //为colorBallLayer 添加动画
    [self.colorBallLayer addAnimation:anim forKey:nil];
    //为colorBallLayer 添加指定动画效果
    [self.colorBallLayer setValue:[NSValue valueWithCGPoint:position] forKeyPath:@"emitterPosition"];
    //提交动画
    [CATransaction commit];
}

@end
