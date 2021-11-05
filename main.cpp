#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sys/time.h>
#include "sample_nnie_api.h"
#include <unistd.h>

using namespace std;
using namespace cv;

int test_image_yuv(const char *yuv420, int width, int height)
{
    FILE *imgyuv;
    struct timeval tv1,tv2;
    long t1, t2, time;
    unsigned char* buffer;

    imgyuv = fopen(yuv420, "rb");
	int size=width*height;
    buffer=(unsigned char *)malloc(size*1.5*sizeof(unsigned char));
	
    int model_load_flag=yolo_init("coco_yolov3_detect.wk");
    printf("model_load_flag : %d\n", model_load_flag);

    int u32RetS = fread(buffer, size*1.5, 1, imgyuv);
    {
        gettimeofday(&tv1, NULL);
        int yolo_run_flag=yolo_run(buffer,width,height);
		printf("yolo_run return flag : %d\n", yolo_run_flag);
        gettimeofday(&tv2, NULL);

        t1 = tv2.tv_sec - tv1.tv_sec;
        t2 = tv2.tv_usec - tv1.tv_usec;
        time = (long)(t1 * 1000 + t2 / 1000);
        printf("yolo_run inference time : %dms\n", time);
        //usleep(300000);
    }

    free(buffer);
    return 0;
}

int main()
{
    //yuv图像格式为420
	const char *image_yuv_demo = "dog_bike_car_416x416.yuv";
    int flag=test_image_yuv( image_yuv_demo,416,416);
    return 0;
}
