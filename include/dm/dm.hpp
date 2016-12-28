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


// Macro for when we need a condition to be true, and its falsehood
// would not be caused by a programming error.
// hope() must be guaranteed to have an effect even in release builds.
#define hope(...) assert(__VA_ARGS__)
#ifdef NDEBUG
#error "You've defined NDEBUG - Please reimplement the hope() macro accordingly."
#endif


#include <dm/gl.hpp>
#include <dm/gl_debug.hpp>
#include <dm/clamp.hpp>
#include <dm/rgba32.hpp>
#include <dm/Image.hpp>
#include <dm/PPMImage.hpp>
#include <dm/Lerp.hpp>

#include <dm/PerspectiveView.hpp>
#include <dm/OrthographicView.hpp>
#include <dm/TextureUnit.hpp>
#include <dm/GLProgram.hpp>
#include <dm/GLTexture.hpp>
#include <dm/GLQuadBatch.hpp>
#include <dm/GLMonospaceText.hpp>
#include <dm/Input.hpp>
#include <dm/FpsLimiter.hpp>
#include <dm/GameplayType.hpp>
#include <dm/Dungeon.hpp>
#include <dm/WorldMap.hpp>
#include <dm/WelcomeScreen.hpp>
#include <dm/Game.hpp>

