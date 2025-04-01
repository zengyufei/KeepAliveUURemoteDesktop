原文地址 [blog.csdn.net](https://blog.csdn.net/m0_57309959/article/details/139815240?utm_relevant_index=5)

Windows 如何仅安装 MSVC 而不安装 Visual Studio
-----------------------------------------------------------------------------------------------------


### 下载 Visual Studio Installer

**不是安装 Visual Studio**，这个东西相当于一个包管理器

下载地址：[https://visualstudio.microsoft.com/zh-hans/downloads/](https://visualstudio.microsoft.com/zh-hans/downloads/)，依次找到

*   所有下载
    *   用于 Visual Studio 的工具
        *   Visual Studio2022 生成工具

下载得到 `vs_buildTool.exe`

### 安装开发套件

对于我来说，一般只需要在 64 位 Windows 上生成 64 位程序。那么操作如下：

运行上述 exe，依次点击

*   Visual Studio 生成工具 2022
    *   **单个组件**（很多教程这一步就走错了）
        *   MSVC v143 - VS 2022 C++ x86/64 生成工具 (最新)
        *   Windows 10 SDK（找一个最新的版本）

![](https://i-blog.csdnimg.cn/blog_migrate/902c0ae7334a8d3975ce03d0cb2234b1.png)

建议不要修改安装位置。点击右下角的修改，静待安装完毕。

### 修改[环境变量](https://so.csdn.net/so/search?q=%E7%8E%AF%E5%A2%83%E5%8F%98%E9%87%8F&spm=1001.2101.3001.7020)

搜索：查看系统高级设置→环境变量→系统变量

需要注意下面的版本号，把 `10.0.20348.0` 和 `14.40.33807` 改成自己的

选择 Path ，双击添加两条

*   `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.40.33807\bin\Hostx64\x64`
*   `C:\Program Files (x86)\Windows Kits\10\bin\10.0.20348.0\x64`

添加 LIB，添加三条（如果之前没有，那点开之后三条中间用英文分号隔开）

*   `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.40.33807\lib\x64`
*   `C:\Program Files (x86)\Windows Kits\10\Lib\10.0.20348.0\ucrt\x64`
*   `C:\Program Files (x86)\Windows Kits\10\Lib\10.0.20348.0\um\x64`

添加 INCLUDE，添加 6 条

*   `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.40.33807\include`
*   `C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\ucrt`
*   `C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um`
*   `C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\winrt`
*   `C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\cppwinrt`
*   `C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\shared`

![](https://i-blog.csdnimg.cn/blog_migrate/512724234d25d62b4cd492fcfa91ab57.png)

### 测试

确定→确定，**然后**打开自己的编辑器测试

![](https://i-blog.csdnimg.cn/blog_migrate/cc273c306829f541bba44cb3e8b9c714.png)

```
#include <cstdio>
int main() {
	printf("hello world!");
	return 0;
}

```

![](https://i-blog.csdnimg.cn/blog_migrate/d5464f814eb8e40bf3b12f1dabcda76f.png)