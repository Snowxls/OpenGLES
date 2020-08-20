//
//  ImageFilterViewController.m
//  GPUImageDemo
//
//  Created by Snow WarLock on 2020/8/18.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ImageFilterViewController.h"
#import "GPUImage.h"
#import "VideoViewController.h"

@interface ImageFilterViewController () {
    UIImageView *imageView;
    UIImage *img;
}

//饱和度滤镜
@property(nonatomic,strong)GPUImageSaturationFilter *disFilter;

@end

@implementation ImageFilterViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
    img = [UIImage imageNamed:@"Snow.jpeg"];
    
    imageView = [[UIImageView alloc] initWithFrame:self.view.bounds];
    [self.view addSubview:imageView];
    imageView.image = img;
    
    UISlider *slider = [[UISlider alloc] initWithFrame:CGRectMake(20, self.view.frame.size.height/2, self.view.frame.size.width-40, 10)];
    slider.value = 1;
    [self.view addSubview:slider];
    [slider addTarget:self action:@selector(SaturationChange:) forControlEvents:UIControlEventValueChanged];
    
    UISlider *slider2 = [[UISlider alloc] initWithFrame:CGRectMake(20, self.view.frame.size.height/2+80, self.view.frame.size.width-40, 10)];
    slider2.value = 1;
    [self.view addSubview:slider2];
    [slider2 addTarget:self action:@selector(SaturationChange2:) forControlEvents:UIControlEventValueChanged];
    
    UIButton *btn2 = [[UIButton alloc]initWithFrame:CGRectMake(self.view.bounds.size.width-100, self.view.bounds.size.height-100, 100, 100)];
    btn2.backgroundColor = [UIColor orangeColor];
    [btn2 setTitle:@"下一个" forState:UIControlStateNormal];
    
    [self.view addSubview:btn2];
    [btn2 addTarget:self action:@selector(next) forControlEvents:UIControlEventTouchUpInside];
}

- (void)SaturationChange:(UISlider *)sender {
    
    //2.选择合适的滤镜
    //饱和度：应用于图像的饱和度或去饱和度（0.0 - 2.0，默认为1.0）
    if (_disFilter == nil) {
        _disFilter = [[GPUImageSaturationFilter alloc]init];
    }
    //设置饱和度值
    _disFilter.saturation = 1.0;
    //设置要渲染的区域 --图片大小
    [_disFilter forceProcessingAtSize:img.size];
    //使用单个滤镜
    [_disFilter useNextFrameForImageCapture];
    //调整饱和度
    _disFilter.saturation = sender.value;
    
    //3.创建图片组件--数据源头(静态图片)
    GPUImagePicture *stillImageSoucer = [[GPUImagePicture alloc]initWithImage:img];
    //为图片添加一个滤镜
    [stillImageSoucer addTarget:_disFilter];
    //处理图片
    [stillImageSoucer processImage];
    
    //4.处理完成,从FrameBuffer帧缓存区中获取图片
    UIImage *newImage = [_disFilter imageFromCurrentFramebuffer];
    
    //更新图片
    imageView.image = newImage;
    
}

- (void)SaturationChange2:(UISlider *)sender {
    
    //2.选择合适的滤镜
    //饱和度：应用于图像的饱和度或去饱和度（0.0 - 2.0，默认为1.0）
    if (_disFilter == nil) {
        _disFilter = [[GPUImageSaturationFilter alloc]init];
    }
    //设置饱和度值
    _disFilter.saturation = 1.0;
 
    ///设置要渲染的区域 --图片大小
    [_disFilter forceProcessingAtSize:img.size];
    
    //使用单个滤镜
    [_disFilter useNextFrameForImageCapture];
    
    //调整饱和度
    _disFilter.saturation = sender.value;
    
    //数据源头(静态图片)
    GPUImagePicture *stillImageSoucer = [[GPUImagePicture alloc]initWithImage:img];
    
    //为图片添加一个滤镜
    [stillImageSoucer addTarget:_disFilter];
    
    //处理图片
    [stillImageSoucer processImage];
    
    //处理完成,从FrameBuffer帧缓存区中获取图片
    UIImage *newImage = [_disFilter imageFromCurrentFramebuffer];
    
    //更新图片
    imageView.image = newImage;
    
}

- (void)next {
    VideoViewController *vvc = [VideoViewController new];
    vvc.modalPresentationStyle = UIModalPresentationFullScreen;
    [self presentViewController:vvc animated:YES completion:nil];
}

@end
