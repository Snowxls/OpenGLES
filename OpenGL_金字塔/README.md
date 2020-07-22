**先放效果图**

![金字塔](https://upload-images.jianshu.io/upload_images/8416233-2b86123bbce79057.gif?imageMogr2/auto-orient/strip)

**关于纹理的核心知识**

![纹理](https://upload-images.jianshu.io/upload_images/8416233-9eb70c83287fdd92.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**纹理的4个顶点坐标如图所示，使用时将每个顶点对应到纹理的各个顶点上**

![](https://upload-images.jianshu.io/upload_images/8416233-1ad88d66ee6464d8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**每个顶点的对应关系并非唯一，可以交换顶点达到翻转的效果**

![](https://upload-images.jianshu.io/upload_images/8416233-267ac57337420349.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**我们绘制的金字塔是使用6个三角面组合而成的**

![金字塔绘制方案](https://upload-images.jianshu.io/upload_images/8416233-8fe9b2c905a7e2e8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

**代码相关**

```
void MakePyramid(GLBatch& pyramidBatch) {
    /*1、通过pyramidBatch组建三角形批次
      参数1：类型
      参数2：顶点数
      参数3：这个批次中将会应用1个纹理
      注意：如果不写这个参数，默认为0。
     */
    pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    
    /**
     2)设置纹理坐标
     void MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t);
     参数1：texture，纹理层次，对于使用存储着色器来进行渲染，设置为0
     参数2：s：对应顶点坐标中的x坐标
     参数3：t:对应顶点坐标中的y
     (s,t,r,q对应顶点坐标的x,y,z,w)
     
     pyramidBatch.MultiTexCoord2f(0,s,t);
     
     3)void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
      void Vertex3fv(M3DVector3f vVertex);
     向三角形批次类添加顶点数据(x,y,z);
      pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
     */
    
    //塔顶
    M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
    M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
    M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
    M3DVector3f vBackLeft = { -1.0f,  -1.0f, -1.0f };
    M3DVector3f vBackRight = { 1.0f,  -1.0f, -1.0f };
    
    //金字塔底部
    //底部的四边形 = 三角形X + 三角形Y
    //三角形X = (vBackLeft,vBackRight,vFrontRight)
    //vBackLeft
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //vBackRight
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    //vFrontRight
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    
    //三角形Y =(vFrontLeft,vBackLeft,vFrontRight)
    //vFrontLeft
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //vBackLeft
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //vFrontRight
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);

    
    // 金字塔前面
    //三角形：（Apex，vFrontLeft，vFrontRight）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);

    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);

    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    //金字塔左边
    //三角形：（vApex, vBackLeft, vFrontLeft）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //金字塔右边
    //三角形：（vApex, vFrontRight, vBackRight）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);

    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    //金字塔后边
    //三角形：（vApex, vBackRight, vBackLeft）
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //结束批次设置
    pyramidBatch.End();
}
```

**纹理载入**

```
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    //1、读纹理位，读取像素
    //参数1：纹理文件名称
    //参数2：文件宽度地址
    //参数3：文件高度地址
    //参数4：文件组件地址
    //参数5：文件格式地址
    //返回值：pBits,指向图像数据的指针
    
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if(pBits == NULL)
        return false;
    
    //2、设置纹理参数
    //参数1：纹理维度
    //参数2：为S/T坐标设置模式
    //参数3：wrapMode,环绕模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    //参数1：纹理维度
    //参数2：线性过滤
    //参数3: 缩小/放大过滤方式.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    

    //3.载入纹理
    //参数1：纹理维度
    //参数2：mip贴图层次
    //参数3：纹理单元存储的颜色成分（从读取像素图是获得）
    //参数4：加载纹理宽
    //参数5：加载纹理高
    //参数6：加载纹理的深度
    //参数7：像素数据的数据类型（GL_UNSIGNED_BYTE，每个颜色分量都是一个8位无符号整数）
    //参数8：指向纹理图像数据的指针
    
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
                 eFormat, GL_UNSIGNED_BYTE, pBits);
    
    //使用完毕释放pBits
    free(pBits);
    
    //只有minFilter 等于以下四种模式，才可以生成Mip贴图
    //GL_NEAREST_MIPMAP_NEAREST具有非常好的性能，并且闪烁现象非常弱
    //GL_LINEAR_MIPMAP_NEAREST常常用于对游戏进行加速，它使用了高质量的线性过滤器
    //GL_LINEAR_MIPMAP_LINEAR 和GL_NEAREST_MIPMAP_LINEAR 过滤器在Mip层之间执行了一些额外的插值，以消除他们之间的过滤痕迹。
    //GL_LINEAR_MIPMAP_LINEAR 三线性Mip贴图。纹理过滤的黄金准则，具有最高的精度。
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
    //4.纹理生成所有的Mip层
    //参数：GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
    glGenerateMipmap(GL_TEXTURE_2D);
 
    return true;
}
```