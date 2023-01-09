TARGET := alg_demo

CROSS_COMPILER:=arm-linux-gnueabihf-

CC := $(CROSS_COMPILER)gcc
CXX := $(CROSS_COMPILER)g++
AR := $(CROSS_COMPILER)ar
LD := $(CROSS_COMPILER)g++

#####指定编译选项
CXXFlAGS += -O2 -g 
CXXFlAGS += -Wall -W 
CXXFlAGS += -Wno-unused-parameter
#-fvisibility=hidden设置符号默认隐藏，针对特定变量和函数，在代码中使用__attribute__ ((visibility("default")))另该符号外部可见，这种方法可用有效避免so之间的符号冲突。
CXXFlAGS += -fvisibility=hidden
#####指定链接选项
#LDFLAGS := -shared 
#所有使用的符号均有定义
#LDFLAGS += -Wl,--no-undefined 
#LDFLAGS += -Wl,-soname,$(TARGET)
#-Wl,--rpath=程序运行时动态库搜索路径
LDFLAGS += -Wl,--rpath=./alg_lib:./opencv_lib

######指定源文件所在目录
SRC_DIR := .

######指定头文件所在目录
INCLUDE_DIR := $(SRC_DIR)
INCLUDE_DIR += ./alg_inc
#INCLUDE_DIR += ./opencv_inc/

#INCLUDE_DIR += ./relyLib/xxx/inc
CXXFlAGS += $(addprefix -I,$(INCLUDE_DIR))

#####指定所依赖的动态库或静态库。-Wl,-Bstatic和 -Wl,-Bdynamic分别指定
#LIBS += -L./relyLib/xxx -lxxx
#LIBS += -L./relyLib/xxx -lpthread
#LIBS += -Wl,-Bstatic 
#LIBS += -L./opencv_lib/ -lopencv_highgui -lopencv_video -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -llibjpeg-turbo -llibpng -lzlib 

LIBS += -Wl,-Bdynamic
LIBS += -lpthread -ldl
LIBS += -L./alg_lib -lrwalg_core -lrwalg_reasoning
#LIBS += -L./opencv_lib -lopencv_photo -lopencv_features2d -lopencv_video -lopencv_highgui -lopencv_flann -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_core
# CSrcs := $(foreach CurDir,$(SRC_DIR),$(wildcard $(CurDir)/*.c))
# CppSrcs := $(foreach CurDir,$(SRC_DIR),$(wildcard $(CurDir)/*.cpp))
# OBJECTS := $(patsubst %.c,%.o,$(CSrcs))
# OBJECTS += $(patsubst %.cpp,%.o,$(CppSrcs))

SOURCES := $(foreach CurDir,$(SRC_DIR),$(wildcard $(CurDir)/*.c) $(wildcard $(CurDir)/*.cpp))
OBJECTS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

all:$(TARGET)

$(TARGET):$(OBJECTS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS) 

# %.o: %.c*
%.o:%.c
	$(CXX) -c $(CXXFlAGS) $< -o $@ 
%.o:%.cpp
	$(CXX) -c $(CXXFlAGS) $< -o $@ 

install:all
	@echo "install $(TARGET)"

.PHONY: clean
clean:
	-rm $(OBJECTS) $(TARGET) -rf

#####需要强制执行的目标可以添加FORCE依赖
FORCE:
