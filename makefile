.PHONY: all clean run_leak_check
# эти цели не являются файлами выполняй их даже если соотв файлы существуют
all: diff 
# когда запускаем make без цели, то выполняем первую цель после all, то есть записи make stack make all и make эквивалентны

COMP=clang++


CFLAGS_DEBUG = -D_DEBUG -DDEBUG_SMALL_TREE
COMMON_CFLAGS = -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -Wno-c++11-extensions -g -fsanitize=address,leak,undefined  
LDFLAGS = -fsanitize=address,leak,undefined

ifdef DEBUG_TREE
    CFLAGS = $(COMMON_CFLAGS) $(CFLAGS_DEBUG)
else
    CFLAGS = $(COMMON_CFLAGS)
endif

main.o: main.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/differenciation/diff_params.o: src/differenciation/diff_params.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/dump/graphviz_dump.o: src/dump/graphviz_dump.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/utils/metki.o: src/utils/metki.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/io/output_expr_to_file.o: src/io/output_expr_to_file.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/io/read_exp_from_file.o: src/io/read_exp_from_file.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/core/tree_func.o: src/core/tree_func.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/differenciation/differenciator.o: src/differenciation/differenciator.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/calculation_optimization/calcul_tree.o: src/calculation_optimization/calcul_tree.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/dump/latex_dump.o: src/dump/latex_dump.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/dump/gnuplot_graph.o: src/dump/gnuplot_graph.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/matan_book.o: src/matan_book.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/core/forest.o: src/core/forest.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/utils/rofl_matan.o: src/utils/rofl_matan.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

diff: main.o src/differenciation/differenciator.o src/differenciation/diff_params.o src/dump/graphviz_dump.o src/utils/metki.o src/io/output_expr_to_file.o src/io/read_exp_from_file.o src/core/tree_func.o src/calculation_optimization/calcul_tree.o src/dump/latex_dump.o src/matan_book.o src/core/forest.o src/utils/rofl_matan.o src/dump/gnuplot_graph.o
	$(COMP) -o $@ $^ $(LDFLAGS)

# $@ имя цели
# $^ все зависимости
# $(COMP) clang++
# clang++ -o stack main.o hash.o log.o my_assert.o stack_func.o

run_leak_check: diff
	ASAN_OPTIONS="detect_leaks=1:verbosity=1:print_stacktrace=1" ./diff


clean:
	rm -f diff output/plots_gnuplot/*.gp output/plots_gnuplot/*.svg output/*.gp *.toc *.out output/*.toc output/*.out output/*.tex output/*.aux output/*.log *.tex *.aux *.log src/calculation_optimization/*.o src/core/*.o src/differenciation/*.o src/dump/*.o src/utils/*.o src/io/*.o src/*.o *.o output/images/*.dot output/images/*.svg
