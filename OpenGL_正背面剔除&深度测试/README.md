**首先绘制一个天天圈用于测试**

```
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    if (iCull) {
        //正背面剔除
        glEnable(GL_CULL_FACE); //开启
        glFrontFace(GL_CCW); //默认值 可以不填
        glCullFace(GL_BACK); //默认值 可以不填
    } else {
        glDisable(GL_CULL_FACE); //关闭  会影响整个工程 用完需要关闭
    }
    
    if (iDepth) {
        //深度测试
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST); //关闭  会影响整个工程 用完需要关闭
    }
    
    modelViewMatix.PushMatrix(viewFrame);
    
    //设置颜色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    
    //默认光源着色器
    //通过光源、阴影效果跟提现立体效果
    //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
    //参数2：模型视图矩阵
    //参数3：投影矩阵
    //参数4：基本颜色值
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);
    
    torusBatch.Draw();
    modelViewMatix.PopMatrix();
    
    glutSwapBuffers();
```

![甜甜圈](https://upload-images.jianshu.io/upload_images/8416233-16c8f350bb84b288.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**进行旋转后会出现奇异的现象**

![奇怪的现象](https://upload-images.jianshu.io/upload_images/8416233-7695dfa105fe24b1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

在绘制3D场景的时候,我们需要决定哪些部分是对观察者可⻅的,或者哪些部分是对观察者不可⻅的.对于不可⻅的部分,应该及早丢弃.例如在⼀个不透明的墙壁后,就不应该渲染.这种情况叫做**隐藏⾯消除**

**解决方案**

通过顶点顺序告诉OpenGL正面和背面

![正面/背面区分](https://upload-images.jianshu.io/upload_images/8416233-53e593192606cca7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

![分析](https://upload-images.jianshu.io/upload_images/8416233-afa8464311dd82e3.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**代码调整**

```
//正背面剔除
glEnable(GL_CULL_FACE); //开启
```

![开启正背面剔除](https://upload-images.jianshu.io/upload_images/8416233-837241ec2cae6897.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**这个功能会影响整个功能，使用完需要手动关闭**

```
glDisable(GL_CULL_FACE); //关闭
```

**这样操作后能解决一部分问题，但是会出现另一种现象**

![奇怪的现象2](https://upload-images.jianshu.io/upload_images/8416233-7355c31e41b43bb5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**这是OpenGL将后面方向的面认为是正面，实际后方的那一面也确实是正面，但前方的面也是正面，导致OpenGL渲染出现了视觉上的错误，因此需要使用`深度测试`**

**深度**就是该像素点在3D世界中距离摄像机的距离，Z值。

**深度缓存区**，就是⼀块内存区域,专⻔存储着每个像素点(绘制在屏幕上的)深度值，当观察者在Z轴正方向时，深度（Z值）越大，则离摄像机越近；当观察者在Z轴负方向时，深度（Z值）越小，则离摄像机越近。

**深度缓冲区(DepthBuffer)**和**颜⾊缓存区(ColorBuffer)**是对应的，**颜⾊缓存区**存储像素的颜⾊信息，⽽**深度缓冲区**存储像素的深度信息，在决定是否绘制⼀个物体表⾯时，⾸先要将表⾯对应的像素的深度值与当前**深度缓冲区**中的值进⾏⽐较，如果⼤于深度缓冲区中的值，则丢弃这部分，否则利⽤这个像素对应的深度值和颜⾊值，分别更新**深度缓冲区**和**颜⾊缓存区**。 这个过程称为”**深度测试**”。

开启了深度测试，则在绘制每个像素之前，OpenGL 会把它的深度值与⽬前像素点对应存储的深度值进⾏⽐较。如果像素点新对应深度值 < 像素点对应的深度值 (意思⽐较当前2个图层时， 那个图层更加接近于观察者) 那么此时就会将该像素点的深度值进⾏取⽽代之；反之，如果像素点上的新颜⾊值如果距离观察者更远，则应该会遮挡.。那么此时它所对应的深度值与颜⾊值就会被抛弃，不进⾏绘制。

**深度缓存区的默认值为1.0**
**深度值的范围是[0,1]之间**

代码开启深度测试

```
glEnable(GL_DEPTH_TEST)
```

可以通过`glDepthFunc(GLenum func)`来修改深度测试的测试规则

| 参数 | 说明 |
| :--------:   |  :--------:  | 
| GL_ALWAYS | 总是通过测试 |
| GL_NEVER | 总是不通过测试 |
| GL_LESS | 当前深度值 < 存储的深度值时通过 |
| GL_EQUAL | 当前深度值 = 存储的深度值时通过 |
| GL_LEQUAL | 当前深度值 <= 存储的深度值时通过 |
| GL_GREATER | 当前深度值 > 存储的深度值时通过 |
| GL_NOTEQUAL | 当前深度值 != 存储的深度值时通过 |
| GL_GEQUAL | 当前深度值 >= 存储的深度值时通过 |

同样会影响整个工程，使用后需要关闭

```
glDisable(GL_DEPTH_TEST); //关闭  会影响整个工程 用完需要关闭
```

![结果图](https://upload-images.jianshu.io/upload_images/8416233-c7065a3ff8d32254.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**Z-fighting**

因为开启深度测试后，OpenGL 就不会再去绘制模型被遮挡的部分。这样实现的显示更加真实，但是由于深度缓冲区`精度`的限制对于深度相差⾮常⼩的情况下。(例如在同⼀平⾯上进⾏2次制)，OpenGL 就可能出现不能正确判断两者的深度值，会导致深度测试的结果不可预测，显示出来的现象时交错闪烁.的前⾯2个画⾯，交错出现。

**结局方案 - 多边形偏移（Polygon Offset）**

让深度值之间产生间隔，如果2个图形之间有间隔，就意味着不会产生干涉，可以理解为在执行深度测试前将立方体的深度值做一些细微的增加，就能将重叠的2个图形深度值之间有所区分。

> glEnable(GL_POLYGON_OFFSET_FILL);
>
> 参数列表
> GL_POLYGON_OFFSET_POINT    对应模式：GL_POINT
> GL_POLYGON_OFFSET_LINE  对应模式：GL_LINE
> GL_POLYGON_OFFSET_FILL  对应模式：GL_FILL
>
> glDisable(GL_POLYGON_OFFSET_FILL); //关闭

**指定偏移量** 

- 通过glPolygonOffset 来指定。glPolygonOffset 需要2个参数: factor , units

- 每个Fragment 的深度值都会增加如下所示的偏移量: 
Offset = ( m * factor ) + ( r * units); 
m : 多边形的深度的斜率的最⼤值,理解⼀个多边形越是与近裁剪⾯平⾏,m 就越接近于0. 
r : 能产⽣于窗⼝坐标系的深度值中可分辨的差异最⼩值.
r 是由具体是由具体OpenGL 平台指定的 ⼀个常量.

- ⼀个⼤于0的Offset 会把模型推到离你(摄像机)更远的位置,相应的⼀个⼩于0的Offset 会把模型拉近

- ⼀般⽽⾔,只需要将-1 和 -1 这样简单赋值给glPolygonOffset 基本可以满⾜需求.

**预防Z-fighting**

- 不要将两个物体靠的太近，避免渲染时三⻆形叠在⼀起。这种⽅式要求对场景中物体插⼊⼀个少量的偏 移，那么就可能避免ZFighting现象。例如上⾯的⽴⽅体和平⾯问题中，将平⾯下移0.001f就可以解决 这个问题。当然⼿动去插⼊这个⼩的偏移是要付出代价的。
 
- 尽可能将近裁剪⾯设置得离观察者远⼀些。上⾯我们看到，在近裁剪平⾯附近，深度的精确度是很⾼ 的，因此尽可能让近裁剪⾯远⼀些的话，会使整个裁剪范围内的精确度变⾼⼀些。但是这种⽅式会使离 观察者较近的物体被裁减掉，因此需要调试好裁剪⾯参数。 使⽤更⾼位数的深度缓冲区，通常使⽤的深度缓冲区是24位的，现在有⼀些硬件使⽤使⽤32/64位的缓冲区。

- 使⽤更⾼位数的深度缓冲区，通常使⽤的深度缓冲区是24位的，现在有⼀些硬件使⽤使⽤32/64位的缓 冲区，使精确度得到提⾼。