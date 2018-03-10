CC := gcc
FLAG := -w
BIN := bin
BUILD := build
INC := include
#INC := src
SRC := src
INCLUDE := -I./$(INC)

# by setting target name here and define value in setting.h to make different version
# candidate target name: least_8 least_128
$(BIN)/least_128: $(BUILD)/main.o $(BUILD)/buffer_manager.o $(BUILD)/IO_manager.o \
			$(BUILD)/page.o $(BUILD)/record.o $(BUILD)/work.o \
			$(BUILD)/setting.o $(BUILD)/hash.o $(BUILD)/tracker.o
	$(CC) $(FLAG) $(INCLUDE) $^ -o $@

$(BUILD)/main.o: $(SRC)/main.c $(INC)/work.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/main.c -o $@
$(BUILD)/buffer_manager.o: $(SRC)/buffer_manager.c $(INC)/buffer_manager.h \
		$(INC)/IO_manager.h $(INC)/page.h $(INC)/setting.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/buffer_manager.c -o $@
$(BUILD)/IO_manager.o: $(SRC)/IO_manager.c $(INC)/IO_manager.h \
		$(INC)/buffer_manager.h $(INC)/page.h $(INC)/hash.h \
		$(INC)/record.h $(INC)/tracker.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/IO_manager.c -o $@
$(BUILD)/page.o: $(SRC)/page.c $(INC)/page.h \
		$(INC)/buffer_manager.h $(INC)/record.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/page.c -o $@
$(BUILD)/record.o: $(SRC)/record.c $(INC)/record.h \
		$(INC)/buffer_manager.h $(INC)/IO_manager.h \
		$(INC)/page.h $(INC)/hash.h $(INC)/setting.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/record.c -o $@
$(BUILD)/work.o: $(SRC)/work.c $(INC)/work.h \
		$(INC)/buffer_manager.h $(INC)/page.h $(INC)/hash.h \
		$(INC)/record.h $(INC)/tracker.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/work.c -o $@
$(BUILD)/setting.o: $(SRC)/setting.c $(INC)/setting.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/setting.c -o $@
$(BUILD)/hash.o: $(SRC)/hash.c $(INC)/hash.h \
	$(INC)/setting.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/hash.c -o $@
$(BUILD)/tracker.o: $(SRC)/tracker.c $(INC)/tracker.h
	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/tracker.c -o $@

# $(BUILD)/IO_manager.o: $(SRC)/IO_manager.c
# 	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/IO_manager.c -o $@
# $(BUILD)/page.o: $(SRC)/page.c
# 	$(CC) $(FLAG) $(INCLUDE) -g -c $(SRC)/page.c -o $@




#$(BUILD)/%.o: $(SRC)/%.c
#	$(CC) $(FLAG) $(INCLUDE) -g -c $< -o $@

clean:
	@rm -f $(BUILD)/*
