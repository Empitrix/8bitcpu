# 8-Bit CPU
This project implements a simplified 8-bit CPU architecture in C, along with an assembler, emulator, and a TUI-based visualization tool. It serves as an educational tool for understanding computer architecture, instruction sets, and low-level programming.

## Preview
![8bitcpu](./assets/clip.gif)


## Build
Clone the project by:
```bash
git clone https://github.com/empitrix/8bitcpu
cd ./8bitcpu
```

To build the project using `gnu make` run:
```bash
make
```
After that you will have `./cpu`.

> [!NOTE]
> currently this project is for unix-like operating systems only.


## Usage
For example to use the project run:
```bash
./cpu -p ./examples/pattern.bin
```

Which executes the file [`pattern.asm`](https://github.com/Empitrix/assembler/blob/master/examples/pattern.asm) that generated by [**Assembler**](https://github.com/Empitrix/assembler).

Or if you wish to see the instructions step by step use `-s` flag:
```bash
./cpu -p ./examples/pattern.bin -s
```
In stepping mode if you press `q` the program will end.


## CPU Specifications:
### Memory
- 16 bytes of RAM (Addresses `0x10` to `0x1F`)
- 256 words of ROM (Addresses `0x00` to `0xFF`)

### Registers
- 10 special-purpose registers (Addresses `0x00` to `0x09`)
  - Register `0x06`: GPIO (General Purpose Input/Output)

### Opcodes
- `BSF f, b`: Bit set forward (`0101 bbb fffff`)
- `BCF f, b`: Bit clear file (`0100 bbb fffff`)
- `GOTO k`: Unconditional Branch (`101k kkkk kkkk`)
- `NOP`: No Operation (`0000 0000 0000`)

### Opcode Details
- `BSF`: Bit clear forward: set given bit from given register to 1
- `BCF`: Bit clear file: set given bit from given register to 0
- `GOTO`: Goto given lable address
- `NOP`: No Operation (small delay)


## Flags
| Flag        | Name           | Description                       |
|-------------|----------------|-----------------------------------|
| `-s`        | Stepping Mode  | Clock pulse with keyboard keys    |
| `-f <num>`  | Frequency      | Clock frequency from 1 to 1000000 |
| `-p <path>` | Program        | Path to `.bin` file from [`assembler`](https://github.com/empitrix/assembler) |
| `-l <path>` | Load CPU state | Path to a `.txt` file that contains CPU's state |

- If CPU state is loaded with `-l` don't need to use `-p` to a program to CPU, but make sure that the program is exists for CPU to load it.


## Actions
- Press `s` to save the cpu's state to a `cpu_state.txt` file!
- Press `<space>` to move to the next step in `stepping mode`.
- Press `<space>` to pause/unpause cpu in `auto mode`.


## TODO
* Control Functions:
- [x] Save: Implement the functionality to save the current CPU state to a file.
- [x] Run/Pause: Allow the user to start/stop continuous execution of the CPU.
- [ ] Add more opcodes: Implement additional instructions to enhance the CPU's capabilities (e.g., arithmetic, logic, control flow).

