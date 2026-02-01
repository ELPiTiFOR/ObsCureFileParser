CC = "C:\Program Files\CodeBlocks\MinGW\bin\gcc.exe"
#WR = "C:\Program Files\CodeBlocks\MinGW\bin\windres.exe"
#CFLAGS = -Wall -Werror -pedantic -std=c99
CPPFLAGS = \
	-Isrc \
	-Isrc/utils \
	-Isrc/it_file \
	-Isrc/tm_file \
	-Isrc/sav_file \
	-Isrc/file_read \
	-Isrc/file_write \
	-Isrc/config \
	-Isrc/obscure_ids \

CFLAGS = -std=c99

SRC = \
	src/main.c \
	src/utils/utils.c \
	src/it_file/it_file.c \
	src/tm_file/tm_file.c \
	src/sav_file/sav_file.c \
	src/sav_file/sav_inv.c \
	src/sav_file/sav_utils.c \
	src/sav_file/sav_pc.c \
	src/obscure_ids/item_id.c \
	src/obscure_ids/document_id.c \
	src/file_read/file_read.c \
	src/file_write/file_write.c \
	src/config/config.c \
	src/commands.c \
	src/business.c \

OBJ = ${SRC:.c=.o}

all: ObsCureFileParser

ObsCureFileParser: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) ObsCureFileParser $(OBJ)