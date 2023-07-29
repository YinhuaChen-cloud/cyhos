BUILD:=build
SRC:=src
CC:=gcc
LD:=gcc
ARCH:=native
CFLAGS:=-Wall -Werror

$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

LDFLAGS:= -lreadline
LDFLAGS:=$(strip ${LDFLAGS})

$(BUILD)/native.elf: \
	$(BUILD)/builtin/cyhsh.o \
	$(BUILD)/arch/native/crt0.o \

	$(LD) $^ ${LDFLAGS} -o $@

.PHONY: native
native: $(BUILD)/native.elf
	$(BUILD)/native.elf

.PHONY: clean
clean:
	rm -rf $(BUILD)
