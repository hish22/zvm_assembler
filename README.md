# zvm_assembler

This program is a simple assembler that converts human-readable assembly code into binary machine instructions. It tokenizes source text into instructions, registers, and numbers, packs them into a 3-integer binary format, and saves the output to a `.zx` file. Additionally, it generates a syntax-highlighted HTML file to provide a visual representation of the source code for easier debugging.

```bash
gcc zasm.c -o zasm
zasm -a program.zasm
zasm -c program.zasm
```