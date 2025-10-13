CC = "C:\Program Files\CodeBlocks\MinGW\bin\gcc.exe"
#WR = "C:\Program Files\CodeBlocks\MinGW\bin\windres.exe"
#CFLAGS = -Wall -Werror -pedantic -std=c99
CPPFLAGS = \
	-Isrc \
	-Isrc/utils \
	-Isrc/it_file \
	-Isrc/file_read \
	-Isrc/file_write \

CFLAGS = -std=c99

SRC = \
	src/main.c \
	src/utils/utils.c \
	src/it_file/it_file.c \
	src/file_read/file_read.c \
	src/file_write/file_write.c \
	src/commands.c \
	src/business.c \

OBJ = ${SRC:.c=.o}

all: ObsCureFileParser

ObsCureFileParser: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) ObsCureFileParser $(OBJ)