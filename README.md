# 位图矢量化工具Potrace的使用
## libpotrace 
由于[Potrace](http://potrace.sourceforge.net/)作者Peter Selinger并未提供官方的Windows下生成libpotrace的方式，所以此处提供出个人创建的Windows下的libpotrace的VS工程
关于详细libpotrace库创建，请参照 [位图矢量化：Potrace的应用](https://blog.csdn.net/xdg_blog)

## libPotraceWrapper
对libpotrace库的浅层次封装，对8bit的cv::Mat进行位图转矢量操作，并通过getpath()接口获得Gdiplus::GraphicsPath对象

## Call Demo
对libPotraceWrapper的简单MFC对话框调用，可以参考此Demo使用