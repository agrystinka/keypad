# The main (project top) file without .c
TARGET_DYNAMIC = libbuff.so
TARGET_STATIC= libbuff.a
# buff_lib sourses
SRC += fifo.c errors.c

CC = gcc
CFLAGS = -std=gnu18 -O2 -Wall -Wextra -Wpedantic

BUILDDIR_D = ./build_dynamic
BUILDDIR_S = ./build_static
SRCDIR = ./src
INCDIR = ./inc

.PHONY: all dynamic static clean tidy

all: $(BUILDDIR_D) $(TARGET_DYNAMIC) $(BUILDDIR_S) $(TARGET_STATIC)

dynamic: $(BUILDDIR_D) $(TARGET_DYNAMIC)

static: $(BUILDDIR_S) $(TARGET_STATIC)

$(BUILDDIR_D)/%.o: $(SRCDIR)/%.c | $(BUILDDIR_D)
	$(CC) $(CFLAGS) -fPIC $(addprefix -I,$(INCDIR)) -c $< -o $@

$(TARGET_DYNAMIC): $(addprefix $(BUILDDIR_D)/,$(SRC:.c=.o))
	$(CC) $(CFLAGS) -shared $^ -o $@

$(BUILDDIR_D):
	mkdir -p $@

$(BUILDDIR_S)/%.o: $(SRCDIR)/%.c | $(BUILDDIR_S)
	$(CC) $(CFLAGS) $(addprefix -I,$(INCDIR)) -c $< -o $@

$(TARGET_STATIC): $(addprefix $(BUILDDIR_S)/,$(SRC:.c=.o))
	ar -crs $(TARGET_STATIC) $@

$(BUILDDIR_S):
	mkdir -p $@

clean:
	rm -rf $(BUILDDIR_D)
	rm -rf $(BUILDDIR_S)

tidy: clean
	rm -f *.so
	rm -f *.a

lint: lint-all

lint-all: | lint-clang-tidy lint-splint lint-oclint lint-cppcheck
_oclint := oclint
_splint := splint +checks $(addprefix -I ,$(INCDIR))
_cppcheck := cppcheck -q -j$$(($$(nproc)+1)) $(addprefix -I,$(INCDIR)) \
	--platform=unix64 \
	--enable=warning,style,performance,portability,information \
	--std=c11 --language=c --verbose --inconclusive
_clang-tidy := clang-tidy --quiet --checks='*'

lint-oclint: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> OCLint report\e[38;5;130m"
	$(_oclint) 2>/dev/null $^ | head -n -2 | tail -n +1
	@echo -en "\e[0m"

lint-clang-tidy: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> Clang-Tidy report\e[38;5;130m"
	$(_clang-tidy) $^ -- $(addprefix -I,$(INCDIR)) $(CFLAGS) 2>/dev/null | cat
	@echo -en "\e[0m"

lint-splint: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> SPLint report\e[38;5;130m"
	$(_splint) 2>&1 $^ | tail -n +2
	@echo -en "\e[0m"

lint-cppcheck: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> CPPCheck report\e[38;5;130m"
	$(_cppcheck) $^ && echo
	@echo -en "\e[0m"
