BISON?=bison
FLEX?=flex

BISON_VERSION=$(shell $(BISON) --version | head -n 1 | grep -o '[0-9]\.[0-9]\+')
BISON_VERSION_SUPPORTED=$(shell awk -v a=$(BISON_VERSION) -v b="3.0" 'BEGIN { print (a >= b) ? 1 : 0 }')
ifneq ($(BISON_VERSION_SUPPORTED), 1)
$(error Bison version $(BISON_VERSION) not supported.)
endif

FLEX_VERSION=$(shell $(FLEX) --version | head -n 1 | grep -o '[0-9]\.[0-9]\+')
FLEX_VERSION_SUPPORTED=$(shell awk -v a=$(FLEX_VERSION) -v b="2.6" 'BEGIN { print (a >= b) ? 1 : 0 }')
ifneq ($(FLEX_VERSION_SUPPORTED), 1)
$(error Flex version $(FLEX_VERSION) not supported.
endif

all: bison_parser.cpp flex_lexer.cpp

bison_parser.cpp: bison_parser.y
	$(BISON) bison_parser.y --output=bison_parser.cpp --defines=bison_parser.hpp

flex_lexer.cpp: flex_lexer.l
	$(FLEX) flex_lexer.l

clean:
	rm -f bison_parser.cpp flex_lexer.cpp bison_parser.h flex_lexer.h *.output

# Tests if the parser builds correctly and doesn't contain conflicts.
test:
	! $(BISON) bison_parser.y -v --output=conflict_test.cpp 2>&1 | grep "conflict" >&2
