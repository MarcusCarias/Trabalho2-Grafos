CXX      := g++
CXXFLAGS := -std=c++17 -O2

SRCS := main.cpp grafo.cpp guloso.cpp mst.cpp poda.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := pcstp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
