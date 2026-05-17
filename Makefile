CC = "C:\Program Files\CodeBlocks\MinGW\bin\gcc.exe"
WR = "C:\Program Files\CodeBlocks\MinGW\bin\windres.exe"
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
	-Isrc/obscure_ids/rooms_names \
	-Isrc/my_crc \
	-Isrc/correct_crc \
	-Isrc/gui \

CFLAGS = -std=c99 #-g
CFLAGS_GUI = -std=c99 -mwindows
LDFLAGS = -lgdi32 -lole32 -luuid

SRC = \
	src/utils/utils.c \
	src/it_file/it_file.c \
	src/tm_file/tm_file.c \
	src/sav_file/sav_file.c \
	src/sav_file/sav_inv.c \
	src/sav_file/sav_utils.c \
	src/sav_file/sav_pc.c \
	src/obscure_ids/item_id.c \
	src/obscure_ids/document_id.c \
	src/obscure_ids/map_id.c \
	src/obscure_ids/rooms_names/rooms_names.c \
	src/file_read/file_read.c \
	src/file_write/file_write.c \
	src/config/config.c \
	src/commands.c \
	src/business.c \
	src/my_crc/my_crc.c \
	src/correct_crc/correct_crc.c \

OBJ = ${SRC:.c=.o}

SRC_CLI = $(SRC) src/main.c
OBJ_CLI = ${SRC_CLI:.c=.o}
SRC_GUI = \
	$(SRC) \
	src/main_gui.c \
	src/gui/it_window.c \
	src/gui/sav_window.c \
	src/gui/item_id_window.c \
	src/gui/add_item_window.c \
	src/gui/document_id_window.c \
	src/gui/choose_format_window.c \
	src/gui/utils_gui.c \
	src/gui/it_mod_list.c \
	src/gui/diff_selector.c \

OBJ_GUI = ${SRC_GUI:.c=.o}

resources/resources.o: resources/resources.rc
	$(WR) $^ -o $@

all: ObsCureFileParser

ObsCureFileParser: $(OBJ_CLI)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

gui: ObsCureFileParserGUI

ObsCureFileParserGUI: $(OBJ_GUI) resources/resources.o
	$(CC) $(CFLAGS_GUI) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) ObsCureFileParser ObsCureFileParserGUI $(OBJ_GUI) $(OBJ_CLI)