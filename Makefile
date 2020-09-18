
# 目标文件,生成可执行文件a.out
TARGET = a.out

# 编译方式,主要是取恒等号后面的值
CPP := g++

# 编译时候加的一些参数,如动态库等 
CLAGS = -g -lwiringPi -lpthread -std=c++11 \
		-L/usr/lib/mysql/ -lmysqlclient 
# 这个mysql库,需要自己注意一下具体位置稍作修改

# rm -rf的变量
RMRF := rm -rf


# 需要加入的头文件的文件夹地址
INCLUDE_DIR  += Include 
# 源代码,主要是.c/.cpp后缀的文件夹地址
SRC_DIR += Source

# 所有的源文件,用wildcard进行模式匹配的展开,
# 所有匹配上的$(dir)/*.cpp 都会中间添加空格排列
ALL_SRC += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))

#模式转换,把INCLUDE中的所有 符合%的 转换成为 -I% 
ALL_INCLUDE += $(patsubst %, -I%, $(INCLUDE_DIR))

# 所有的对象文件都直接生成在对应源文件目录下
# 可以设置一个OBJDIR,将%.o设置成为$(OBJDIR)/%.o,这样所有的.o文件都在OBJDIR目录下.
ALL_OBJ += $(patsubst %.cpp, %.o, $(ALL_SRC)) 


# 生成TARGET文件,依赖文件为所有的obj
$(TARGET): $(ALL_OBJ)
	$(CPP) $(CLAGS) $^ -o $@

# 生成.o文件,依赖文件为对应的.cpp文件
%.o: %.cpp
	$(CPP) $(CLAGS) $(ALL_INCLUDE) -c $^ -o $@
 


.PHONY:
# cleanall将生成的a.out也一并清除
cleanall: 
	$(RMRF) $(ALL_OBJ) $(TARGET)
# clean只清除生成的.o文件
clean:
	$(RMRF) $(ALL_OBJ)