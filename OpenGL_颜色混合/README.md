**我们把OpenGL 渲染时会把颜⾊值存在颜⾊缓存区中，每个⽚段的深度值也是放在深度缓冲区。当深度缓冲区被关闭时，新的颜⾊将简单的覆盖原来颜⾊缓存区存在的颜⾊值，当深度缓冲区再次打开时，新的颜⾊⽚段只是当它们⽐原来的值更接近邻近的裁剪平⾯才会替换原来的颜⾊⽚段。**

![颜色混合](https://upload-images.jianshu.io/upload_images/8416233-774f28375ae13954.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

- ⽬标颜⾊：已经存储在颜⾊缓存区的颜⾊值
- 源颜⾊：作为当前渲染命令结果进⼊颜⾊缓存区的颜⾊值

**代码开启方式**

```
glEnable(GL_BlEND);
```

固定着⾊器/可编程着⾊器-> 使⽤开关⽅式 ->颜⾊混合(单纯的2个图层重叠进⾏混合) 

可编程着⾊器->⽚元着⾊器 -> 处理图⽚原图颜⾊+另一个颜⾊值 -> 进⾏颜⾊混合⽅程式计算 -> 套⽤公式

**当混合功能被启动时，源颜⾊和⽬标颜⾊的组合⽅式是混合⽅程式控制的。在默认情况下，混合⽅程式如下所示：**

> Cf = (Cs * S) + (Cd * D)
>
> Cf : 最终计算参数的颜⾊
> Cs : 源颜⾊
> Cd : ⽬标颜⾊
> S : 源混合因⼦
> D : ⽬标混合因⼦

**常用的混合代码**

```
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
```

如果颜⾊缓存区已经有⼀种颜⾊红⾊（1.0f,0.0f,0.0f,0.0f）这个⽬标颜⾊Cd，如果在这上⾯⽤⼀种 alpha为0.6的蓝⾊（0.0f,0.0f,1.0f,0.6f） 

Cd (⽬标颜⾊) = （1.0f,0.0f,0.0f,0.0f）
Cs (源颜⾊) = （0.0f,0.0f,1.0f,0.6f）
S = 源alpha值 = 0.6f 
D = 1 - 源alpha值= 1-0.6f = 0.4f 
⽅程式Cf = (Cs * S) + (Cd * D) 
等价于 = （Blue * 0.6f） + (Red * 0.4f)

**最终颜⾊是以原先的红⾊（⽬标颜⾊）与 后来的蓝⾊（源颜⾊）进⾏组合。 源颜⾊的alpha值越⾼，添加的蓝⾊颜⾊成分越⾼，⽬标颜⾊所保留的成分就会越少。 混合函数经常⽤于实现在其他⼀些不透明的物体前⾯绘制⼀个透明物体的效果。**

实际上远不⽌这⼀种混合⽅程式，我们可以从5个不同的⽅程式中进⾏选择

```
//选择混合⽅程式的函数： 
glbBlendEquation(GLenum mode);
```

| 模式 | 函数 |
|:-----:|:-----:|
| GL_FUNC_ADD | Cf = (Cs * S) + (Cd * D) |
| GL_FUNC_SUBTRACT | Cf = (Cs * S) - (Cd * D) |
| GL_FUNC_REVERSE_SUBTRACT | Cf = (Cd * D) - (Cs * S) |
| GL_MIN | Cf = min(Cs,Cd) |
| GL_MAX | Cf = max(Cs,Cd) |

**常量混合颜⾊，默认初始化为⿊⾊（0.0f,0.0f,0.0f,0.0f），但是还是可以修改这个常量混 合颜⾊。**

```
void glBlendColor(GLclampf red ,GLclampf green ,GLclampf blue ,GLclam pf alpha );
```