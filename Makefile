all: apps servers
	@echo "$(GREEN)-------- Compile the Grass Layer --------$(END)"
	$(RISCV_CC) $(COMMON) $(GRASS_SRCS) $(GRASS_LD) -o $(RELEASE)/grass.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(RELEASE)/grass.elf > $(DEBUG)/grass.lst
	@echo "$(YELLOW)-------- Compile the Earth Layer --------$(END)"
	$(RISCV_CC) $(COMMON) $(EARTH_SRCS) $(EARTH_LD) -o $(RELEASE)/earth.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(RELEASE)/earth.elf > $(DEBUG)/earth.lst

.PHONY: apps
apps: apps/system/*.c apps/user/*.c
	mkdir -p $(DEBUG) $(RELEASE)
	@echo "$(CYAN)-------- Compile the Apps Layer --------$(END)"
	for FILE in $^ ; do \
	  export APP=$$(basename $${FILE} .c);\
	  echo "Compile" $${FILE} "=>" $(RELEASE)/$${APP}.elf;\
	  $(RISCV_CC) $(COMMON) $(APPS_SRCS) $${FILE} $(APPS_LD) -Iapps -o $(RELEASE)/$${APP}.elf || exit 1 ;\
	  echo "Compile" $${FILE} "=>" $(DEBUG)/$${APP}.lst;\
	  $(OBJDUMP) $(OBJDUMP_FLAGS) $(RELEASE)/$${APP}.elf > $(DEBUG)/$${APP}.lst;\
	done

servers:
	# sys_proc
	$(RISCV_CC) $(COMMON) $(APPS_SRCS) apps/system/sys_proc.c -Tapps/system/sys_proc.lds -lc -lgcc -Iapps -o $(RELEASE)/sys_proc.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(RELEASE)/sys_proc.elf > $(DEBUG)/sys_proc.lst
	# sys_file
	$(RISCV_CC) $(COMMON) $(APPS_SRCS) apps/system/sys_file.c -Tapps/system/sys_file.lds -lc -lgcc -Iapps -o $(RELEASE)/sys_file.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(RELEASE)/sys_file.elf > $(DEBUG)/sys_file.lst
	# sys_dir
	$(RISCV_CC) $(COMMON) $(APPS_SRCS) apps/system/sys_dir.c -Tapps/system/sys_dir.lds -lc -lgcc -Iapps -o $(RELEASE)/sys_dir.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(RELEASE)/sys_dir.elf > $(DEBUG)/sys_dir.lst
	# sys_shell
	$(RISCV_CC) $(COMMON) $(APPS_SRCS) apps/system/sys_shell.c -Tapps/system/sys_shell.lds -lc -lgcc -Iapps -o $(RELEASE)/sys_shell.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(RELEASE)/sys_shell.elf > $(DEBUG)/sys_shell.lst

install:
	@echo "$(YELLOW)-------- Create the Disk Image --------$(END)"
	$(CC) $(TOOLS)/mkfs.c library/file/file.c -DMKFS $(INCLUDE) -o $(TOOLS)/mkfs
	cd $(TOOLS); ./mkfs
	@echo "$(YELLOW)-------- Create ELF for ECE4750 --------$(END)"
	cp $(RELEASE)/earth.elf $(TOOLS)/egos-ece4750
	$(OBJCOPY) --update-section .image=$(TOOLS)/disk.img $(TOOLS)/egos-ece4750
	cp tools/egos-ece4750 ../ece4750/app

instructions:
	python3 tools/instructions.py

clean:
	rm -rf build
	rm -rf $(TOOLS)/qemu/qemu.elf
	rm -rf $(TOOLS)/mkfs $(TOOLS)/mkrom
	rm -rf $(TOOLS)/disk.img $(TOOLS)/bootROM.bin $(TOOLS)/bootROM.mcs

RISCV_QEMU = qemu-system-riscv32
RISCV_CC = riscv64-unknown-elf-gcc
OBJDUMP = riscv64-unknown-elf-objdump
OBJCOPY = riscv64-unknown-elf-objcopy

APPS_SRCS = apps/app.S library/*/*.c grass/context.S
GRASS_SRCS = grass/grass.S grass/context.S grass/*.c library/elf/*.c library/libc/memory.c
EARTH_SRCS = earth/earth.S earth/*.c library/elf/*.c library/libc/*.c

CFLAGS = -march=rv32i -mabi=ilp32 -mcmodel=medlow -ffunction-sections -fdata-sections -fPIC
LDFLAGS = -Wl,--gc-sections -nostartfiles -nostdlib
INCLUDE = -Ilibrary -Ilibrary/elf -Ilibrary/libc -Ilibrary/file -Ilibrary/servers -Ilibrary/queue

COMMON = $(CFLAGS) $(LDFLAGS) $(INCLUDE) -D CPU_CLOCK_RATE=65000000

APPS_LD = -Tapps/app.lds -lc -lgcc
GRASS_LD = -Tgrass/grass.lds -lc -lgcc
EARTH_LD = -Tearth/earth.lds -lc -lgcc

TOOLS = tools
QEMU = tools/qemu
DEBUG = build/debug
RELEASE = build/release
OBJDUMP_FLAGS =  --source --all-headers --demangle --line-numbers --wide
#OBJDUMP_FLAGS =  --source --demangle --line-numbers --wide

GREEN = \033[1;32m
YELLOW = \033[1;33m
CYAN = \033[1;36m
END = \033[0m
