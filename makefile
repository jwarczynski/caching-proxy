CPP_SOURCES = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp)
HEADERS = $(wildcard src/*.hpp) $(wildcard src/**/*.hpp) $(wildcard src/**/**/*.hpp)
OBJ = ${patsubst src/%.cpp, obj/%.o, $(CPP_SOURCES)}
OUTPUT = bin/http-cache

all: folders ${OUTPUT}
run: all
	./${OUTPUT}

${OUTPUT}: ${OBJ}
	g++ -o $@ $^ -Wall -Wextra

obj/%.o: src/%.cpp ${HEADERS}
	g++ -c $< -o $@ -Wall -Wextra

clean:
	rm -rf obj bin

# Tworzy foldery dla plików obiektowych i wykonywalnych
# Kopiuje strukturę katalogów z src, aby kompilator nie narzekał
folders:
	mkdir obj 2> /dev/null || (exit 0)
	mkdir bin 2> /dev/null || (exit 0)
	(cd src && find -type d | xargs -I{} mkdir -p "../obj/{}")

# Komendy pod Windows:
# clean:
#	rmdir /q /s obj

# Ignoruje błędy, kiedy foldery bin i obj istnieją
# 2> NUL znika komunikat błędu
# || (exit 0) zapewnia traktowanie komendy jako udanej
folders:
#	mkdir obj 2> NUL || (exit 0)
#	mkdir bin 2> NUL || (exit 0)
#	xcopy src obj /t /e