1. 准备环境

+ 编译程序需要用build.sh buildroot构建buildroot环境
  
2. 创建目录build_linux，并cd
3. 设置环境变量，下面第一个环境变量路径根据实际修改
```
export PATH=rv1126/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin:$PATH
export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++
```
4. 执行cmake，其中sysroot路径根据实际修改
```
cmake ../ -DCMAKE_SYSROOT=/home/z/rv1126/buildroot/output/rockchip_rv1126_rv1109_spi_nand/staging
```

5. 执行make命令
6. 执行完成在项目的bin目录下可得到.so和可执行文件

TODO:

+ Linux下使用mpi进行编解码