
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

int yolo_init(const char *yolo_model_path);
int yolo_run(unsigned char *input_yuv420_data, int input_w, int input_h);
int yolo_unit();

#ifdef __cplusplus
}
#endif //__cplusplus
