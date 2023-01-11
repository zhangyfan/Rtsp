1. 准备环境

+ 编译程序需要用build.sh buildroot构建buildroot环境
  
2. 创建目录build_linux，并cd
3. 根据实际修改cross.cmake文件中的路径
```
set(sysroot_target /home/z/rv1126/buildroot/output/rockchip_rv1126_rv1109_spi_nand/host/arm-buildroot-linux-gnueabihf/sysroot)
set(tools /home/z/rv1126/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin)
```
4. 执行cmake，其中sysroot路径根据实际修改
```
cmake ../ -DCMAKE_TOOLCHAIN_FILE=../cross.cmake
```

5. 执行make命令
6. 执行完成在项目的bin目录下可得到.so和可执行文件

-------
+ 运行
  
bin/Release上传了Windows下的可执行文件，执行命令需要两个参数
```
RtspProxy.exe rtsp://admin:!QAZ2wsx@192.168.1.103/ rtmp://127.0.0.1:1935/1
```

第一个参数是设备RTSP链接，第二个参数是RTMP推流地址
RTSP链接格式为: rtps://账号:密码@IP[:PORT]/
-------
TODO:

+ linux下目前为了调试方便解码使用的ffmpeg软解（硬解接口一样，但是AVFrame是MPP自己的数据格式）