#pragma once

namespace dm {
void gl_debug_setup(GLint major, GLint minor, bool enable);
extern void (*gl_debug_insert_marker)(const char *string);
}
