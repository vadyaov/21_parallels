CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -Wpedantic -std=c++17 #-fsanitize=address -g

ANT_SRCS = s21_graph.cc

# GCOV_TARGET = ./coverage
# COVERFLAGS = -fprofile-arcs -ftest-coverage

all: aco gauss #test

# test: lib
# 	@mkdir -p examples/dots/
# 	@cmake -DCMAKE_CXX_FLAGS="$(CXXFLAGS)" -S ./tests/ -B ./tests/build
# 	@cmake --build ./tests/build
# 	@./tests/build/test_graph

# gcov_report: $(GCOV_TARGET)

# $(GCOV_TARGET): CXXFLAGS += $(COVERFLAGS)
# $(GCOV_TARGET): test Makefile
# 	lcov --rc lcov_branch_coverage=1 --capture --directory . --output-file coverage.info \
# 		--no-external --exclude "$(CURDIR)/tests/*"
# 	genhtml coverage.info --output-directory coverage_report
# 	open ./coverage_report/index.html
#
aco:
	$(CXX) $(CXXFLAGS) aco/*.cc -lncursesw -ltinfo -o ant.out
	./ant.out

gauss:
	$(CXX) $(CXXFLAGS) gauss/*cc -lncursesw -ltinfo -o gauss.out
	./gauss.out

# s21_graph.a: $(GRAPH_OBJ)
# 	ar -rcs $@ $< 

# s21_graph_algorithms.a: $(ALGO_OBJ)
# 	ar -rcs $@ $<

# $(GRAPH_OBJ): $(GRAPH_SRC)
# 	$(CXX) -c $(CXXFLAGS) $< -o $@

# $(ALGO_OBJ): $(ALGO_SRC)
# 	$(CXX) -c $(CXXFLAGS) $< -o $@

# app: s21_graph.a s21_graph_algorithms.a
# 	$(CXX) $(CXXFLAGS) console/*.cc $? -lncursesw -ltinfo -o $@.out
# 	./$@.out

clean:
	rm -f *.out

.PHONY: aco gauss
