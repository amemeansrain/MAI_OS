CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -ldl

all: libimpl1.so libimpl2.so static_program dynamic_program

# Компиляция динамических библиотек
libimpl1.so: libimpl1.o
	$(CC) -shared -o libimpl1.so libimpl1.o

libimpl2.so: libimpl2.o
	$(CC) -shared -o libimpl2.so libimpl2.o

# Компиляция объектных файлов
libimpl1.o: libimpl1.c contract.h
	$(CC) $(CFLAGS) -c libimpl1.c

libimpl2.o: libimpl2.c contract.h
	$(CC) $(CFLAGS) -c libimpl2.c

# Компиляция статической программы
static_program: static_program.c libimpl1.so
	$(CC) -o static_program static_program.c -L. -limpl1 -Wl,-rpath,.

# Компиляция динамической программы
dynamic_program: dynamic_program.c
	$(CC) -o dynamic_program dynamic_program.c $(LDFLAGS)

# Тестовый запуск
test: all
	@echo "=== Тестирование статической программы ==="
	@echo "1 1 100" | ./static_program 2>/dev/null | grep -A1 ">"
	@echo ""
	@echo "=== Тестирование динамической программы ==="
	@echo -e "1 1 100\n0\n1 1 100" | ./dynamic_program 2>/dev/null | grep -A1 "\[lib"

# Очистка
clean:
	rm -f *.o *.so static_program dynamic_program

.PHONY: all test clean