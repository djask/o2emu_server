CXX := g++
CXXFLAGS := -lboost_system -lboost_thread -lpthread

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TARGET=O2ServerEmu

O2ServerEmu: $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm $(TARGET) *.o
