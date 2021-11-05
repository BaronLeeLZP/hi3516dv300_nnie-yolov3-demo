CFLAGS += -I./src
CFLAGS += -I./libhisi/include
CFLAGS += -I./libopencv/include
CFLAGS += -L./libhisi/lib
CFLAGS += -L./libopencv/lib

CXXFLAGS += -I./src
CXXFLAGS += -I./libhisi/include
CXXFLAGS += -I./libopencv/include
CXXFLAGS += -L./libhisi/lib
CXXFLAGS += -L./libopencv/lib


LD_HISI_LIBS +=  -ldnvqe  -lhdmi -lhi_cipher -lhifisheyecalibrate -live -lmd -lmpi -lsecurec  -lsvpruntime -ltde -lupvqe -lVoiceEngine -lnnie

LD_OPENCV_LIBS += -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_videoio -lopencv_dnn -lopencv_ml -lopencv_photo -lopencv_objdetect -lopencv_stitching -lopencv_flann -lopencv_imgcodecs  -lopencv_core -lopencv_calib3d -lopencv_features2d


CXXFLAGS += ${LD_HISI_LIBS}
CXXFLAGS += ${LD_OPENCV_LIBS}
CXXFLAGS += -std=c++11 
CXXFLAGS += -Wno-error
CXXFLAGS += -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math  -fpermissive -fpic 
CXXFLAGS += -O3

SRCS1 := $(wildcard ./src/*.c)
SRCS2 := $(wildcard ./src/*.cpp)
TARGET:= test

# target source

OBJS1  = $(SRCS1:%.c=%.o)
OBJS2  = $(SRCS2:%.cpp=%.o)

CXX = arm-himix200-linux-g++
CC  = arm-himix200-linux-gcc
.PHONY : clean all

all: $(TARGET)

$(TARGET):   ./main.o  $(OBJS1) $(OBJS2) 
	$(CXX) $(CXXFLAGS)  -lpthread -lm -ldl -o $@ $^ -Wl,--start-group ${LD_HISI_LIBS} -Wl,--end-group
arm_lib:
	arm-himix200-linux-ar rcs libyolo.a $(OBJS1) $(OBJS2) 
	arm-himix200-linux-gcc  -shared -fPIC -lstdc++ -o libyolo.so  $(OBJS1) $(OBJS2)   

clean:
	@rm -f $(TARGET) ./*.o ./src/*.o
	@rm -f  libyolo.a libyolo.so
	
	
	
