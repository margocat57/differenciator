.PHONY: all clean 
# эти цели не являются файлами выполняй их даже если соотв файлы существуют
all: diff
# когда запускаем make без цели, то выполняем первую цель после all, то есть записи make stack make all и make эквивалентны

COMP=clang++

#make DEBUG=1

CFLAGS_DEBUG = -D _DEBUG 
COMMON_CFLAGS = -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -Wno-c++11-extensions -g -fsanitize=address
# CFLAGS = -D _DEBUG
LDFLAGS = -fsanitize=address


CFLAGS = $(COMMON_CFLAGS) $(CFLAGS_DEBUG)

main.o: main.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

common/differenciator.o: common/differenciator.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

dump/graphviz_dump.o: dump/graphviz_dump.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

common/metki.o: common/metki.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

input_and_output/input_output.o: input_and_output/input_output.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

data_struct/tree_func.o: data_struct/tree_func.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

common/calcul_tree.o: common/calcul_tree.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

dump/latex_dump.o: dump/latex_dump.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

data_struct/forest.o: data_struct/forest.cpp
	$(COMP) -c $< -o $@ $(CFLAGS)

diff: main.o common/differenciator.o dump/graphviz_dump.o data_struct/tree_func.o input_and_output/input_output.o common/metki.o common/calcul_tree.o dump/latex_dump.o data_struct/forest.o
	$(COMP) -o $@ $^ $(LDFLAGS)

# $@ имя цели
# $^ все зависимости
# $(COMP) clang++
# clang++ -o stack main.o hash.o log.o my_assert.o stack_func.o

clean:
	rm -f diff common/*.o data_struct/*.o dump/*.o input_and_output/*.o *.o images/*.dot images/*.svg stack_for_diff/*.o
