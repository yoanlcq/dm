#pragma once

// Un fourre-tout des en-têtes dont on a généralement
// besoin. Pas forcément une bonne pratique, mais
// ça fait gagner du temps.

#include <dm/stdcxx.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <dm/gl.hpp>
#include <dm/gl_debug.hpp>
#include <dm/math_utils.hpp>
#include <dm/rgba32.hpp>

#include <dm/Res.hpp>
#include <dm/GLProgram.hpp>
#include <dm/GLTexture.hpp>
#include <dm/GLQuadBatch.hpp>
#include <dm/GLMonospaceText.hpp>
#include <dm/Camera.hpp>
#include <dm/Input.hpp>
#include <dm/FpsLimiter.hpp>
#include <dm/Dungeon.hpp>
#include <dm/WorldMap.hpp>
#include <dm/WelcomeScreen.hpp>
#include <dm/Game.hpp>

// Macro for when we need a condition to be true, and its falsehood
// would not be caused by a programming error.
#define hope(...) assert(__VA_ARGS__)
#ifdef NDEBUG
#error "You've defined NDEBUG - the hope() macro needs to be reimplemented."
#endif

