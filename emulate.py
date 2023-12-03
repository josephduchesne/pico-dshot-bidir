# pip install rp2040-pio-emulator adafruit-circuitpython-pioasm

from pioemu import emulate, State
from adafruit_pioasm import Program

from collections import deque

from io import StringIO
import sys

f = open("dshot_orig.pio", "r")
reading_c = False
pio_asm = ""
pio_c = ""
for line in f:
    if line == '% c-sdk {\n':
        reading_c = True
    if reading_c:
        if line != '% c-sdk {\n' and line[0:2] != '%}':
            pio_c += line
    else:
        pio_asm += line
f.close()
print(f"ASM:\n{pio_asm}\n\n")
print(f"C:\n{pio_c}\n\n")


prog = Program(pio_asm, build_debuginfo=True)

old_stdout = sys.stdout
sys.stdout = captured_c = StringIO()
prog_name = "dshot_bidir_300"
prog.print_c_program(prog_name, "static const")
sys.stdout = old_stdout

print(f"captured C:\n{captured_c.getvalue()}\n\n")

from string import Template

template_vars = {
    'PROG_NAME': prog_name,
    'PROG_GEN_C': captured_c.getvalue(),
    'PROG_INLINE_C': pio_c
}

with open('pio_c_template.txt', 'r') as f:
    src = Template(f.read())
    result = src.substitute(template_vars)
    print(result)
    with open(f"{prog_name}.pio.h", "w") as out:
        out.write(result);

generator = emulate(prog.assembled, shift_osr_right=False, shift_isr_right=False, initial_state=State(transmit_fifo=deque([0b0000011000001001])), stop_when=lambda _, state: state.y_register < 0)

linemap = prog.debuginfo[0][:]  # Use a copy since we destroy it
program_lines = prog.debuginfo[1].split("\n")
print(linemap)

start_clock = 0
for before, after in generator:
    line = program_lines[linemap[before.program_counter]]
    if before.pin_values != after.pin_values:
        total_time = (before.clock - start_clock)*.105/2 # .105ms per clock
        print(f"{before.program_counter:02d}[{before.clock:03d}]: {before.output_shift_register.contents:032b} \tx:{after.x_register} p:{after.pin_values:b} {before.pin_values} {total_time:.2f}us\t{line}")
        start_clock = before.clock
    else:
        print(f"{before.program_counter:02d}[{before.clock:03d}]: {before.output_shift_register.contents:032b} \tx:{after.x_register} p:{after.pin_values:b}\t\t{line}")