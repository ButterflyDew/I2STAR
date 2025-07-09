CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -g
INCLUDES = -Iinclude
SRCS = main.cpp \
       src/GlobalUtils.cpp \
       src/Log.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(SRCS)

test_heap_all: tests/test_Heap_all.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o test_heap_all tests/test_Heap_all.cpp

test_graph_query: tests/test_graph_and_query.cpp src/GlobalUtils.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o test_graph_query tests/test_graph_and_query.cpp src/GlobalUtils.cpp

test_gstcover: tests/test_GSTCover.cpp src/GlobalUtils.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DDEBUG -o test_gstcover tests/test_GSTCover.cpp src/GlobalUtils.cpp src/Log.cpp

test_gstcover_v2: tests/test_GSTCover_v2.cpp src/GlobalUtils.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DDEBUG -o test_gstcover_v2 tests/test_GSTCover_v2.cpp src/GlobalUtils.cpp src/Log.cpp

test_log: tests/test_Log.cpp src/Log.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DDEBUG -o test_log tests/test_Log.cpp src/Log.cpp

clean:
	rm -f $(TARGET) *.o src/*.o test_heap_all test_graph_query test_gstcover test_log 