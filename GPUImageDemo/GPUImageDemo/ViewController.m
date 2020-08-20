//
//  ViewController.m
//  GPUImageDemo
//
//  Created by Snow WarLock on 2020/8/18.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import "ViewController.h"
#import "GPUImage.h"
#import <PhotosUI/PhotosUI.h>
#import <AssetsLibrary/ALAssetsLibrary.h>
#import "ImageFilterViewController.h"

@interface ViewController () {
    
}

@property (strong,nonatomic)GPUImageVideoCamera *vCamera;

@property (strong,nonatomic)GPUImageStillCamera *mCamera;
@property (strong,nonatomic)GPUImageFilter *mFilter;
@property (strong,nonatomic)GPUImageView *mGPUImgView;


@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    //添加GPUImage
    [self addFiterCamera];
    
    //添加一个按钮触发拍照
    UIButton *btn = [[UIButton alloc]initWithFrame:CGRectMake((self.view.bounds.size.width-80)*0.5, self.view.bounds.size.height-120, 100, 100)];
    btn.backgroundColor = [UIColor redColor];
    [btn setTitle:@"拍照" forState:UIControlStateNormal];
    
    [self.view addSubview:btn];
    [btn addTarget:self action:@selector(takePhoto) forControlEvents:UIControlEventTouchUpInside];
    
    UIButton *btn2 = [[UIButton alloc]initWithFrame:CGRectMake(self.view.bounds.size.width-100, self.view.bounds.size.height-100, 100, 100)];
    btn2.backgroundColor = [UIColor orangeColor];
    [btn2 setTitle:@"下一个" forState:UIControlStateNormal];
    
    [self.view addSubview:btn2];
    [btn2 addTarget:self action:@selector(next) forControlEvents:UIControlEventTouchUpInside];
}

- (void)addFiterCamera {
    //1.
    //第一个参数表示相片的尺寸，第二个参数表示前、后摄像头 AVCaptureDevicePositionFront/AVCaptureDevicePositionBack
    _mCamera = [[GPUImageStillCamera alloc] initWithSessionPreset:AVCaptureSessionPreset640x480 cameraPosition:AVCaptureDevicePositionFront];
    
    //2.切换摄像头
    [_mCamera rotateCamera];
    
    //3.竖屏方向
    _mCamera.outputImageOrientation = UIInterfaceOrientationPortrait;
    
    //4.设置滤镜对象
    //这个滤镜你可以换其它的，官方给出了不少滤镜
    _mFilter = [[GPUImageGrayscaleFilter alloc] init];
    
    //5.view
    _mGPUImgView = [[GPUImageView alloc]initWithFrame:self.view.bounds];
    
    [_mCamera addTarget:_mFilter];
    [_mFilter addTarget:_mGPUImgView];
    [self.view addSubview:_mGPUImgView];
    
    //6.拍摄
    [_mCamera startCameraCapture];
    
}

- (void)takePhoto {
    
    //7.将图片通过PhotoKit add 相册中
    [_mCamera capturePhotoAsJPEGProcessedUpToFilter:_mFilter withCompletionHandler:^(NSData *processedJPEG, NSError *error){
        
        [[PHPhotoLibrary sharedPhotoLibrary] performChanges:^{
            
            [[PHAssetCreationRequest creationRequestForAsset] addResourceWithType:PHAssetResourceTypePhoto data:processedJPEG options:nil];
            
        } completionHandler:^(BOOL success, NSError * _Nullable error) {
        }];
        
        //获取拍摄的图片
        UIImage * image = [UIImage imageWithData:processedJPEG];
        
    }];
    
    
}

- (void)next {
    ImageFilterViewController *ifvc = [ImageFilterViewController new];
    ifvc.modalPresentationStyle = UIModalPresentationFullScreen;
    [self presentViewController:ifvc animated:YES completion:nil];
}

@end
