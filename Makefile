BUILD:=build
SRC:=src
CC:=gcc
LD:=gcc
ARCH:=native
CFLAGS:=-Wall -Werror
DEBUG:=-ggdb

ifeq ($(MAKECMDGOALS), $(filter $(MAKECMDGOALS), native-shell build/native-shell.elf))
# 如果是 native-shell，所有头文件采用 gnu-linux 原生
INCLUDE:=
else
# 否则，使用我们自行编写的 C 库
INCLUDE:=-I$(SRC)/include \
	-I$(SRC)/include/user
endif

$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

LDFLAGS:= -lreadline
LDFLAGS:=$(strip ${LDFLAGS})


$(BUILD)/native.elf: \
	$(BUILD)/builtin/cyhsh.o \
	$(BUILD)/arch/native/crt0.o \
	$(BUILD)/lib/user/printf.o \
	$(BUILD)/lib/vsprintf.o \

	$(LD) $^ ${LDFLAGS} -o $@

$(BUILD)/native-shell.elf: \
	$(BUILD)/builtin/cyhsh.o \
	$(BUILD)/arch/native/crt0.o \

	$(LD) $^ ${LDFLAGS} -o $@

# 让 cyhsh 使用我们自己编写的用户文件和系统调用来运行
.PHONY: native
native: $(BUILD)/native.elf
	$(BUILD)/native.elf

# 让 cyhsh 使用 gnu-linux 的库文件和系统调用来运行
.PHONY: native-shell
native-shell: $(BUILD)/native-shell.elf
	$(BUILD)/native-shell.elf

.PHONY: clean
clean:
	rm -rf $(BUILD)
