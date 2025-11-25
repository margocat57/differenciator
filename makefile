.PHONY: all clean 
# эти цели не являются файлами выполняй их даже если соотв файлы существуют
all: diff
# когда запускаем make без цели, то выполняем первую цель после all, то есть записи make stack make all и make эквивалентны

COMP=clang++


CFLAGS_DEBUG = -D _DEBUG 
COMMON_CFLAGS = -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -Wno-c++11-extensions -g -fsanitize=address
LDFLAGS = -fsanitize=address


CFLAGS = $(COMMON_CFLAGS) $(CFLAGS_DEBUG)

main.o: main.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/differenciation/differenciator.o: src/differenciation/differenciator.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/dump/graphviz_dump.o: src/dump/graphviz_dump.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/utils/metki.o: src/utils/metki.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/io/input_output.o: src/io/input_output.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/core/tree_func.o: src/core/tree_func.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/calculation_optimization/calcul_tree.o: src/calculation_optimization/calcul_tree.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/dump/latex_dump.o: src/dump/latex_dump.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/matan_book.o: src/matan_book.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

src/core/forest.o: src/core/forest.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

diff: main.o src/differenciation/differenciator.o src/dump/graphviz_dump.o src/utils/metki.o src/io/input_output.o src/core/tree_func.o src/calculation_optimization/calcul_tree.o src/dump/latex_dump.o src/matan_book.o src/core/forest.o
	$(COMP) -o $@ $^ $(LDFLAGS)

# $@ имя цели
# $^ все зависимости
# $(COMP) clang++
# clang++ -o stack main.o hash.o log.o my_assert.o stack_func.o

clean:
	rm -f diff output/diff.tex output/diff.aux output/diff.log src/calculation_optimization/*.o src/core/*.o src/differenciation/*.o src/dump/*.o src/utils/*.o src/io/*.o src/*.o *.o output/images/*.dot output/images/*.svg
