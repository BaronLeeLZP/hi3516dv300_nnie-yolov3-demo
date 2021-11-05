# nnie_yolo
**海思Hi3516DV300移植YOLOv3**
在海思Hisilicon的Hi3516dv300芯片上，利用nnie和opencv库，简洁了官方yolov3用例中各种复杂的嵌套调用/复杂编译，提供了交叉编译后可成功上板部署运行的demo。
同时，对在海思Hisilicon的其他AI芯片上利用nnie部署CNN模型提供参考和借鉴。
以下为demo工程说明：

## 1.libopencv
交叉编译时所依赖的opencv库，已经打包好了；
亦可根据自己项目需要选择不同版本/不同阉割程度的opencv依赖库，需自行编译。

## 2.libhisi
交叉编译时所依赖的nnie库，海思官方提供，未作处理全部堆上，lib静态库，亦可换为动态库。

## 3.image
demo输入原始图像dog_bike_car.jpg；dog_bike_car_416x416.jpg；
官方输入原始图像dog_bike_car_416x416.bgr；
项目使用原始图像dog_bike_car_416x416.yuv。**嵌入式搬砖哥提供的图像格式，YUV420；最终转换后输入模型的图像格式仍为BGR**


## 4.model
**coco_yolov3_detect.wk**官方提供的COCO预训练模型，classes number 80, transformed by RuyiStudio(Windows System)。Linux System(for me, Ubuntu18.04) can use ruyicmd tool.
模型转换这块坑多且深，由于海思只支持caffe模型转自家wk模型，所以通过tensorflow/pytorch/keras/darknet等深度学习框架训练出来sample_nnie_api.cpp的自定义模型需要先转caffe模型，再转wk模型。

## 5.src
### sample_nnie_api.h
yolov3 wk模型接口函数：
1. int yolo_init(const char *yolo_model_path);
2. int yolo_run(unsigned char *input_yuv420_data, int input_w, int input_h);
3. int yolo_unit();
可封装为标准的输入输出接口。

### sample_nnie_api.cpp
yolov3 wk模型后处理过程详细的代码实现。


## 6.main.cpp
demo主函数代码实现，可了解物体检测时接口函数调用过程。

## 7.交叉编译环境
交叉编译工具见海思Hisilicon官方提供的SDK，即**arm-himix200-linux.tgz**，3.6个GB。
手里拿有板子的码友应该都有SDK的，对吧。依照文档配置好交叉编译环境。

## 8.编译文件
**Makefile**较为简洁直观的编译文件

## demo使用步骤如下：
1. 工程编译生成test可执行文件；
  命令**make**
2. 工程编译生成libyolo库文件；
  命令**make arm_lib**
3. 在hi3516dv300上运行test文件可得yolov3-demo运行结果如图。
![result](https://github.com/BaronLeeLZP/hi3516dv300_nnie-yolov3-demo/blob/master/image/result.png)
4. 命令**make clean**可清理原有的编译输出对象，之后再修改文件重新编译。

如有技术问题欢迎交流！
如有版权问题请及时告知！
