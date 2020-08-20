//
//  SWNoticeLabel.h
//  GPUImageDemo
//
//  Created by Snow WarLock on 2020/8/18.
//  Copyright © 2020 Snowxls. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SWNoticeLabel : UIView

/**
 *  功能：提示框
 *  用法： [self.view addSubView:[LMNoticeLabel message:@"我是测试文字" delaySecond:2.0]];
 *  param
 *  message: 要显示的文字信息
 *  second:  显示文字的时间
 */
+(instancetype)message:(NSString *)message delaySecond:(CGFloat)second;

@end
