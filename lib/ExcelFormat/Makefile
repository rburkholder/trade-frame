
CFLAGS = -O2
#CFLAGS = -m32 -g
#CFLAGS = -m64 -g

all: excelformat

HEADERS = basicExcel.hpp Excelformat.h
SRC = BasicExcel.cpp ExcelFormat.cpp examples.cpp

clean:
	rm -f excelformat

excelformat: $(SRC) $(HEADERS)
	g++ $(CFLAGS) -o $@ $(SRC)

