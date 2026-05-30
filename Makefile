# This Makefile was made for Windows, change it if needed (only if you know what
# you're doing)
CC = gcc.exe
WR = windres.exe

CPPFLAGS = \
	-Ilibs/ObsCureInfoHeader \
	-Isrc \
	-Isrc/utils \
	-Isrc/it_file \
	-Isrc/tm_file \
	-Isrc/sav_file \
	-Isrc/hoe_file \
	-Isrc/file_read \
	-Isrc/file_write \
	-Isrc/config \
	-Isrc/obscure_ids/rooms_names \
	-Isrc/my_crc \
	-Isrc/correct_crc \
	-Isrc/gui \
	-Isrc/types \

HEAD = \
	src/utils/utils.h \
	src/it_file/it_file.h \
	src/tm_file/tm_file.h \
	src/sav_file/sav_file.h \
	src/sav_file/sav_inv.h \
	src/sav_file/sav_utils.h \
	src/sav_file/sav_pc.h \
	src/hoe_file/hoe_file.h \
	src/hoe_file/hoe_event.h \
	src/obscure_ids/rooms_names/rooms_names.h \
	src/file_read/file_read.h \
	src/file_write/file_write.h \
	src/config/config.h \
	src/commands.h \
	src/business.h \
	src/my_crc/my_crc.h \
	src/correct_crc/correct_crc.h \
	src/types/lstring.h \

LDFLAGS = -Llibs/
LDLIBS = -lObsCureInfo

CFLAGS = -std=c99 #-g
CFLAGS_GUI = -std=c99 -mwindows
LDFLAGS_GUI = -lgdi32 -lole32 -luuid

SRC = \
	src/utils/utils.c \
	src/it_file/it_file.c \
	src/tm_file/tm_file.c \
	src/sav_file/sav_file.c \
	src/sav_file/sav_inv.c \
	src/sav_file/sav_utils.c \
	src/sav_file/sav_pc.c \
	src/hoe_file/hoe_file.c \
	src/hoe_file/hoe_event.c \
	src/obscure_ids/rooms_names/rooms_names.c \
	src/file_read/file_read.c \
	src/file_write/file_write.c \
	src/config/config.c \
	src/commands.c \
	src/business.c \
	src/my_crc/my_crc.c \
	src/correct_crc/correct_crc.c \
	src/types/lstring.c \
	src/types/simple_map.c \

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

# Build CLI, GUI and DLL
all: cli gui lib

# Build CLI
cli: ObsCureFileParser

ObsCureFileParser: $(OBJ_CLI)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_GUI) $(LDFLAGS) $(LDLIBS)

# Build GUI
gui: ObsCureFileParserGUI

resources/resources.o: resources/resources.rc
	$(WR) $^ -o $@

ObsCureFileParserGUI: $(OBJ_GUI) resources/resources.o
	$(CC) $(CFLAGS_GUI) -o $@ $^ $(LDFLAGS_GUI) $(LDFLAGS) $(LDLIBS)

# Build DLL
lib: ObsCureFileParser.dll

ObsCureFileParser.dll: $(OBJ)
	mkdir lib\ObsCureFileParserHeader
	$(CC) -shared -o lib\$@ $^ $(LDFLAGS) $(LDLIBS) -Wl,--out-implib,lib\libObsCureFileParser.dll.a
	cp $(HEAD) lib\ObsCureFileParserHeader
	cp "resources\Room IDs.csv" lib

clean:
	$(RM) ObsCureFileParser ObsCureFileParserGUI ObsCureFileParser.dll libObsCureFileParser.dll.a \
	$(OBJ_GUI) $(OBJ_CLI) resources/resources.o
	$(RM) -r lib