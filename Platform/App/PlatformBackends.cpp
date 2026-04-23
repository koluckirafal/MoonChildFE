#include "PlatformBackends.h"

#ifdef MOONCHILD_WINDOW_SDL3
#include "SDL3Window.h"
#endif

#ifdef MOONCHILD_RENDERER_OPENGL
#include "OpenGLRenderer.h"
#endif

#ifdef MOONCHILD_RENDERER_GLES
#include "GLESRenderer.h"
#endif

#ifdef MOONCHILD_WINDOW_DREAMCAST
#include "DreamcastWindow.h"
#endif

#ifdef MOONCHILD_RENDERER_SDL3
#include "SDL3Renderer.h"
#endif

#ifdef MOONCHILD_RENDERER_DREAMCAST
#include "DreamcastRenderer.h"
#endif

#ifdef MOONCHILD_INPUT_SDL3
#include "SDL3Input.h"
#endif

#ifdef MOONCHILD_INPUT_DREAMCAST
#include "DreamcastInput.h"
#endif

#ifdef MOONCHILD_AUDIO_SDL3
#include "SDL3Audio.h"
#endif

#ifdef MOONCHILD_AUDIO_DREAMCAST
#include "DreamcastAudio.h"
#endif

PlatformBackends MakeDefaultBackends()
{
    PlatformBackends backends;

#ifdef MOONCHILD_WINDOW_SDL3
    backends.Window.reset(new SDL3Window());
#elif MOONCHILD_WINDOW_DREAMCAST
    backends.Window.reset(new DreamcastWindow());
#endif

#if defined(MOONCHILD_RENDERER_OPENGL)
    backends.Renderer.reset(new OpenGLRenderer());
#elif defined(MOONCHILD_RENDERER_GLES)
    backends.Renderer.reset(new GLESRenderer());
#elif defined(MOONCHILD_RENDERER_SDL3)
    backends.Renderer.reset(new SDL3Renderer());
#elif defined(MOONCHILD_RENDERER_DREAMCAST)
    backends.Renderer.reset(new DreamcastRenderer());
#endif

#ifdef MOONCHILD_INPUT_SDL3
    backends.Input.reset(new SDL3Input());
#elif MOONCHILD_INPUT_DREAMCAST
    backends.Input.reset(new DreamcastInput());
#endif

#ifdef MOONCHILD_AUDIO_SDL3
    backends.Audio.reset(new SDL3Audio());
#elif MOONCHILD_AUDIO_DREAMCAST
    backends.Audio.reset(new DreamcastAudio());
#endif

    return backends;
}
