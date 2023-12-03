#include "pico_pio_loader.h"

#include "hardware/pio.h"

namespace {

constexpr int MAX_PROGRAMS_PER_PIO = 8;

struct AddedProgram {
  const pio_program_t* program;
  uint offset;
};

AddedProgram pio0_added_programs[MAX_PROGRAMS_PER_PIO];
int pio0_added_program_count = 0;
AddedProgram pio1_added_programs[MAX_PROGRAMS_PER_PIO];
int pio1_added_program_count = 0;

} // namespace

bool pio_loader_add_or_get_offset(PIO pio, const pio_program_t* program, uint* offset) {
  AddedProgram* added_programs;
  int* added_program_count;
  if (pio == pio0) {
    added_programs = pio0_added_programs;
    added_program_count = &pio0_added_program_count;
  } else if (pio == pio1) {
    added_programs = pio1_added_programs;
    added_program_count = &pio1_added_program_count;
  } else {
    return false;
  }

  for (int i = 0; i < *added_program_count; i++) {
    if (added_programs[i].program == program) {
      *offset = added_programs[i].offset;
      return true;
    }
  }

  if (*added_program_count >= MAX_PROGRAMS_PER_PIO) {
    return false;
  }
  if (!pio_can_add_program(pio, program)) {
    return false;
  }

  AddedProgram* added_program = &added_programs[(*added_program_count)++];
  added_program->program = program;
  added_program->offset = pio_add_program(pio, program);

  *offset = added_program->offset;
  return true;
}