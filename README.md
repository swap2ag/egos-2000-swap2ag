## Special version for ECE4750

This is a special version of egos-2000 that can run on the teaching processor from [ECE4750 at cornell](https://github.com/cornell-ece4750/). This is a RISC-V processor written in Verilog that runs on the Verilator simulator.

## Step1: Setup the compiler and compile egos-2000

Setup your working directory and name it as `$EGOS`.

```shell
> export EGOS=/home/yunhao/egos
> cd $EGOS
> git clone https://github.com/yhzhang0128/egos-2000.git
# now the code repository is at $EGOS/egos-2000
```

Download the [SiFive riscv-gcc compiler](https://github.com/sifive/freedom-tools/releases/tag/v2020.04.0-Toolchain.Only) to the working directory `$EGOS`.

```shell
> cd $EGOS
> tar -zxvf riscv64-unknown-elf-gcc-8.3.0-2020.04.1-x86_64-xxx-xxx.tar.gz
> export PATH=$PATH:$EGOS/riscv64-unknown...../bin
> cd $EGOS/egos-2000
> make
mkdir -p build/debug build/release
-------- Compile the Apps Layer --------
......
```

## Step2: Create the ELF binary for ECE4750

Make sure you have a C compiler (i.e., the `cc` command) in your shell environment.

```shell
> cd $EGOS/egos-2000
> make install
-------- Create the Disk Image --------
......
[INFO] Finish making the disk image (tools/disk.img)
-------- Create ELF for ECE4750 --------
cp build/release/earth.elf tools/egos-ece4750
riscv64-unknown-elf-objcopy --update-section .image=tools/disk.img tools/egos-ece4750
```

After this step, `tools/egos-ece4750` is the ELF binary for ECE4750.

## Step3: Run egos-2000 on ECE4750

```shell
> cd ece4750
> sim/lab4_sys/sys-sim $EGOS/tools/egos-ece4750
[CRITICAL] ------------- Booting -------------
......
```