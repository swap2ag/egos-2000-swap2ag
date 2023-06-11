import re
import os

files = os.listdir("build/debug")

instructions = set()
for file in files:
    print("processing {}".format(file))
    f = open("build/debug/{}".format(file), "r")
    for line in f:
        l = re.split('\t|\n', line)
        if len(l) > 2 and len(l[2]) > 0:
            instructions.add(l[2])
instructions.add("ecall")

print("\033[1;32m[INFO] In total, there are {} instructions.\033[0m".format(len(instructions)))
print(sorted(list(instructions)))

exist = {"csrr", "csrw", 
         "add", "sub", "mul", "and", "or", "xor", "slt", "sltu", "sra", "srl", "sll",
         "addi", "ori", "andi", "xori", "slti", "sltiu", "srai", "srli", "slli", "lui", "auipc",
         "lw", "sw",
         "jal", "jalr",
         "bne", "beq", "blt", "bltu", "bge", "bgeu"}

print("\033[1;32m[INFO] There exist {} instructions.\033[0m".format(len(exist)))
print(sorted(list(exist)))

rest = instructions - exist
#print("\033[1;32m[INFO] There are {} more instructions to be implemented.\033[0m".format(len(rest)))
#print(sorted(list(rest)))

# see Table 20.2: RISC-V pseudoinstructions.
alias = {'beqz': 'beq', 'bgez' : 'bge', 'bgtz' : 'blt', 'blez' : 'bge',
        'bltz' : 'blt', 'bnez' : 'bne', 'j' : 'jal', 'jr' : 'jalr',
        'mv' : 'addi', 'neg' : 'sub', 'nop' : 'addi', 'not' : 'xori',
        'ret' : 'auipc', 'seqz' : 'sltiu', 'snez' : 'sltu', 'li' : 'lui'}

real_rest = set()
for i in rest:
    if not i in alias:
        real_rest.add(i)

print("\033[1;32m[INFO] There are {} more instructions to be implemented.\033[0m".format(len(real_rest)))
print(sorted(list(real_rest)))

f.close()