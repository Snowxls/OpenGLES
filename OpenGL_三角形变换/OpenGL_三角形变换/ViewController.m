//
//  ViewController.m
//  OpenGL_三角形变换
//
//  Created by Snow WarLock on 2020/8/1.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ViewController.h"
#import "ShaderView.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    ShaderView *sv = [[ShaderView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:sv];
}


@end
