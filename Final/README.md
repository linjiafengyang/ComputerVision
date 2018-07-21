# 计算机视觉期末作业

## 文件夹结构

```
.
├─ testdata
│   └── 测试数据集
├─ result
│   └── warping
│         └── A4纸校正结果
│   └── singleNumImg
│         └── 数字字符分割
├─ mlp
│   └── data
│         └── mnist
│   └── model
│         └── 数字识别模型
└─ excel
    └── 模型预测结果
``` 

## 项目运行

首先使用下面命令编译：

```
g++ -std=c++11 -o main.exe main.cpp Process.cpp Canny.cpp Hough.cpp ImageWarping.cpp ImageSegmentation.cpp -O2 -lgdi32
```

PS：这里由于数据集bmp图像太大，我没传上来！

然后在Windows Powershell下运行：


```
./main.exe
```

或在Windows cmd下运行：

```
main
```

此时可以在result文件夹中查看是否已生成结果。

接着，运行classify.py：

```
python classify.py
```

此时可以在excel文件夹中查看是否已得到预测的Excel文件。
