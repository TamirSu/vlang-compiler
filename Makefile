# ====== כלי קומפילציה ======
CC      ?= gcc
FLEX    ?= flex
BISON   ?= bison

CFLAGS  ?= -std=c11 -Wall -Wextra -O2 -Iinclude -Igenerated/parser
LDFLAGS ?=

# ====== תיקיות תוצרים ======
BUILD   := generated/build
PARSER  := generated/parser

# ====== יעד ברירת מחדל ======
all: $(BUILD)/vlangc

# ====== לינק בינארי הקומפיילר ======
$(BUILD)/vlangc: $(BUILD)/main.o $(BUILD)/runtime.o $(BUILD)/lex.yy.o $(BUILD)/vlang.tab.o
	$(CC) $^ -o $@ $(LDFLAGS)

# ====== אובייקטים מקבצי C שלנו ======
$(BUILD)/main.o: src/main.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/runtime.o: src/runtime/runtime.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# ====== אובייקטים של Flex/Bison ======
$(BUILD)/lex.yy.o: $(PARSER)/lex.yy.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/vlang.tab.o: $(PARSER)/vlang.tab.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# ====== יצירת קבצים מ-Flex/Bison ======
$(PARSER)/lex.yy.c: src/lexer/vlang.l $(PARSER)/vlang.tab.h | $(PARSER)
	$(FLEX) -o $@ $<

$(PARSER)/vlang.tab.c $(PARSER)/vlang.tab.h: src/parser/vlang.y | $(PARSER)
	$(BISON) -d -o $(PARSER)/vlang.tab.c $<

# ====== יצירת התיקיות אם לא קיימות ======
$(BUILD) $(PARSER):
	mkdir -p $@

# ====== יעדי עזר ======
.PHONY: clean run-empty
clean:
	rm -rf generated/*

run-empty: $(BUILD)/vlangc
	./$(BUILD)/vlangc examples/empty.vl
