/*
 3种修饰类型
 uniform,attribute,varying
 
 uniform -> 传递到vertex,fragment 变量
 glUniform...
 
 vertex,fragment -> 常量~
 uniform -> 客户端 -> 顶点/片元着色器
 在顶点/片元着色器 进行一样的声明 -> 都传
 用途: 视图矩阵,投影矩阵,投影视图矩阵...
 
 uniform mat4 viewProMatrix;
 
 glUniform...
 
 attribute 特点: 客户端 -> 顶点着色器
 修饰顶点,纹理坐标,颜色,法线...  坐标颜色相关
 glVertex...
 打开开关
 
 纹理坐标 二维 vec2

 attribute vec4 position;
 attribute vec4 color;
 attribute vec2 texCoord;
 
 纹理坐标 -> 传递
 attribute 传递到片元着色器
 varying 修饰符
 
 
 
 */


