//
//  ViewController.m
//  Shader
//
//  Created by Snow WarLock on 2020/7/28.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ViewController.h"
#import "ShaderView.h"

@interface ViewController () {
    
}

@property(nonatomic,strong)ShaderView *myView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.myView = (ShaderView *)self.view;
    
//    self.myView = [[UIView alloc] initWithFrame:self.view.frame];
//    [self.view addSubview:self.myView];
}


@end
