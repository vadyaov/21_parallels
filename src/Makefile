CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c++17 #-fsanitize=address -g

ANT_DIR := aco
GAUSS_DIR := gauss
WINOGRAD_DIR := winograd

ANT_SRCS := $(addprefix $(ANT_DIR)/, app.cc console.cc ant.cc)
GAUSS_SRCS := $(addprefix $(GAUSS_DIR)/, app.cc console.cc gauss.cc)
WINOGRAD_SRCS := $(addprefix $(WINOGRAD_DIR)/, app.cc console.cc winograd.cc)

all: aco gauss winograd

ant:
	$(CXX) $(CXXFLAGS) $(ANT_SRCS) -lncursesw -ltinfo -o ant.out
	./ant.out

gauss:
	$(CXX) $(CXXFLAGS) $(GAUSS_SRCS) -lncursesw -ltinfo -o gauss.out
	./gauss.out

winograd:
	$(CXX) $(CXXFLAGS) -c winograd/winograd.cc -o winograd/winograd.o -fopenmp
	$(CXX) $(CXXFLAGS) -c winograd/app.cc -o winograd/app.o -lncursesw -ltinfo
	$(CXX) $(CXXFLAGS) -c winograd/console.cc -o winograd/console.o -lncursesw -ltinfo
	$(CXX) $(CXXFLAGS) -c winograd/helpers.cc -o winograd/helpers.o -lncursesw -ltinfo
	$(CXX) $(CXXFLAGS) winograd/app.o winograd/console.o winograd/winograd.o winograd/helpers.o -o winograd.out -fopenmp -lpthread -lncursesw -ltinfo
	./winograd.out

clean:
	rm -f *.out
	rm -f winograd/*.o

.PHONY: aco gauss winograd
