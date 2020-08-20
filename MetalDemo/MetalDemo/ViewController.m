//
//  ViewController.m
//  MetalDemo
//
//  Created by Snow WarLock on 2020/8/20.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ViewController.h"
#import "Render.h"


@interface ViewController () {
    MTKView *mView;
    Render *mRender;
    
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    mView = [[MTKView alloc] initWithFrame:self.view.frame device:MTLCreateSystemDefaultDevice()];
    [self.view addSubview:mView];
    
    if (!mView.device) {
        return;
    }
    
    //分开渲染循环
//    mRender = [[Render alloc] initWithNormalMetalKitView:mView];
    mRender = [[Render alloc] initWithMetalKitView:mView];
    if (!mRender) {
        return;
    }
    
    [mRender mtkView:mView drawableSizeWillChange:mView.drawableSize];
    
    //设置代理
    mView.delegate = mRender;
    
    //设置刷新率 默认60
    mView.preferredFramesPerSecond = 60;
    
}


@end
