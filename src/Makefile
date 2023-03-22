.PHONY: all clean check
CC = gcc -Wall -Wextra -Werror -std=c11
GCOV = --coverage
PROJECTNAME= s21_decimal
GCOVFLAGS = -fprofile-arcs -ftest-coverage
LINUXFLAGS = -lsubunit -lrt -lpthread -lm
LIBS= -lcheck
OS := $(shell uname -s)

ifeq ($(OS), Darwin)
 CC += -D OS_MAC
 TEST_FLAGS = $(LIBS)
endif
ifeq ($(OS), Linux)
 CC += -D OS_LINUX
 TEST_FLAGS =-lpthread  $(LIBS) -pthread -lrt -lm -lsubunit
endif

all: $(PROJECTNAME).a

$(PROJECTNAME).a:
	${CC} -c *.c
	ar rc $(PROJECTNAME).a *.o
	ranlib $(PROJECTNAME).a

test: clean s21_decimal.a
ifeq ($(OS), Darwin)
	$(CC) tests/test.c s21_decimal.a -o test  $(LIBS)
else
	$(CC) tests/test.c s21_decimal.a -o test  $(LIBS) $(LINUXFLAGS)
endif
	./test

gcov_report:
	@gcc $(GCOVFLAGS) *.c tests/test.c  $(TEST_FLAGS) -o test -lcheck
	@./test
	@lcov -t test -o test.info -c -d .
	@genhtml -o report test.info
	@open ./report/index-sort-f.html

check:
ifeq ("","$(wildcard ./.clang-format)")
	cp -f ../materials/linters/.clang-format ./.clang-format
endif
	clang-format -style=Google -n *.c *.h

rebuild: clean all

clean:
	-rm -rf ${PROJECTNAME}
	-rm -rf unit_test
	-rm -rf unit_tests
	-rm -rf *.o && rm -rf *.gcno
	-rm -rf *.a && rm -rf *.gcda
	-rm -rf *.info && rm -rf *.gcov
	-rm -rf ./test && rm -rf ./gcov_report
	-rm -rf ./report/
	-rm -f .clang-format
	-rm -rf print

push: clean
	git add .
	git commit -m 'Abobas'
	git push origin develop

print: clean
	${CC} s21_decimal_*.c -o print
	./print

clang:
	cp ../materials/linters/.clang-format .
	clang-format -n *.c *.h