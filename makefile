CXX = g++
# Agregamos -Wall para que el compilador nos avise de posibles errores
CXXFLAGS = -Iinclude -Wall 

# IMPORTANTE: El orden de los flags en LDFLAGS a veces afecta en Windows (MinGW)
# Añadimos -lfreeglut y -lglu32
LDFLAGS = -Llib -lglfw3 -lfreeglut -lopengl32 -lgdi32 -lglu32

SRC = src/main.cpp src/glad.c
OUT = bin/app.exe

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(OUT) $(LDFLAGS)

run:
	./$(OUT)

clean:
	if exist bin\app.exe del bin\app.exe