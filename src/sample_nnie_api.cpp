#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>
#include "ins_nnie_interface.h"
#include "Tensor.h"
#include "util.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/dnn.hpp"
#define _BSD_SOURCE
#include "sample_nnie_api.h"

#define debugflag 0
#define LimitTime 40*60*3

using namespace cv;
using namespace std;

void* ppHandle; 
float ConfidenceThreshold=0.5;
char *cls_names[] = {"background", "person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat",
                         "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep",
                         "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase",
                         "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket",
                         "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "donut", "apple", "sandwich",
                         "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant",
                         "bed", "diningtable", "toilet", "vmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",
						 "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"};


int yolo_init(const char *yolo_model_path)
{
    if (access(yolo_model_path,F_OK)==-1)
    {
        printf("yolov3_model is not exists!\n");
        return -1;
    }
    ppHandle = new NNIE();
    NNIE* yolov3_mnas=(NNIE *)ppHandle;
    yolov3_mnas->init(yolo_model_path);
    return 0;
}

int yolov3_inference(void* pHandle, Mat& imgRGB)
{
    NNIE* yolov3_mnas=(NNIE *)pHandle;
    int img_width = imgRGB.cols;
    int img_height = imgRGB.rows;
    int step = imgRGB.step;
    int c = imgRGB.channels();
    int num_classes = 80;
    int kBoxPerCell = 3;

    unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * img_width * img_height * c);
    unsigned char *data1 = (unsigned char *)imgRGB.data;
    int count = 0;

    for (int k = 0; k < c; k++)
    {
        for (int i = 0; i < img_height; i++)
        {
            for (int j = 0; j < img_width; j++)
            {
                data[count++] = data1[i * step + j * c + k];
            }
        }
    }

    yolov3_mnas->run(data);
    Tensor output0 = yolov3_mnas->getOutputTensor(0);
    Tensor output1 = yolov3_mnas->getOutputTensor(1);
    Tensor output2 = yolov3_mnas->getOutputTensor(2);

    int feature_index0 = 0;
    int feature_index1 = 1;
    int feature_index2 = 2;

    float conf_threshold = 0.1;
    float nms_threshold = 0.4;
    int is_nms = 1;

    std::vector<int> ids;
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;

    const std::vector<std::vector<cv::Size2f>> anchors = {
        {{116, 90}, {156, 198}, {373, 326}},
        {{30, 61}, {62, 45}, {59, 119}},
        {{10, 13}, {16, 30}, {33, 23}}};

    parseYolov3Feature(img_width,
                       img_height,
                       num_classes,
                       kBoxPerCell,
                       feature_index0,
                       conf_threshold,
                       anchors[0],
                       output0,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(img_width,
                       img_height,
                       num_classes,
                       kBoxPerCell,
                       feature_index1,
                       conf_threshold,
                       anchors[1],
                       output1,
                       ids,
                       boxes,
                       confidences);

    parseYolov3Feature(img_width,
                       img_height,
                       num_classes,
                       kBoxPerCell,
                       feature_index2,
                       conf_threshold,
                       anchors[2],
                       output2,
                       ids,
                       boxes,
                       confidences);

    std::vector<int> indices;

    std::vector<ObjectDetection> detection_results;

    if (is_nms)
    {
        cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, nms_threshold, indices);
    }
    else
    {
        for (int i = 0; i < boxes.size(); ++i)
        {
            indices.push_back(i);
        }
    }

    //{"data":["class":"dog","score":,0,4,"x":1,"y":2,"w":1,"h":2],[]}
	char temp[4096]={0};
	std::string r=temp;
	
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];

        // remap box in src input size.
        auto remap_box = RemapBoxOnSrc(cv::Rect2d(box), img_width, img_height);
        ObjectDetection object_detection;
        object_detection.box = remap_box;
        object_detection.cls_id = ids[idx] + 1;  //
        object_detection.confidence = confidences[idx];
        detection_results.push_back(std::move(object_detection));

        float xmin = object_detection.box.x;
        float ymin = object_detection.box.y;
        float xmax = object_detection.box.x + object_detection.box.width;
        float ymax = object_detection.box.y + object_detection.box.height;
        float confidence = object_detection.confidence;
        int cls_id = object_detection.cls_id;
        
		char *cls_name = cls_names[cls_id];
		if(confidence>ConfidenceThreshold)
		{
			printf("%d %s %.3f %.3f %.3f %.3f %.3f\n", cls_id, cls_name, confidence, xmin, ymin, xmax, ymax);
		}
		memset(temp,0,sizeof(temp));
		r+=temp;
    	if (i!= indices.size()-1)
    	{
    		r+=",";
    	}
		cv::rectangle(imgRGB, cv::Point(xmin,ymin),cv::Point(xmax,ymax),cv::Scalar(255,0,0), 3);		
    }
	r+="]}";
	
    free(data),data=NULL;
    return 0;
}

int yolo_run(unsigned char *input_yuv420_data, int input_w,int input_h)
{
    Mat imgRGB;
    Mat imgRect;
    Mat imgYUV(input_h + input_h/2, input_w, CV_8UC1, input_yuv420_data);
    cvtColor(imgYUV,imgRGB,COLOR_YUV2BGR_I420);
    resize(imgRGB, imgRGB, cv::Size(416, 416));
	
    std::vector<Rect> obj_post;
    yolov3_inference(ppHandle, imgRGB);
	
    return 0;
}


int yolo_unit()
{
    if(ppHandle)
    {
         delete((NNIE*)ppHandle); ppHandle = NULL;
    }
    return 0; 
}