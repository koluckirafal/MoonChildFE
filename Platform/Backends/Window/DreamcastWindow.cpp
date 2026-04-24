#include "DreamcastWindow.h"

#include "DisplayBridge.h"
#include "IInput.h"

#include <cstdio>

DreamcastWindow::DreamcastWindow() = default;

DreamcastWindow::~DreamcastWindow()
{
    Destroy();
}

bool DreamcastWindow::Create(const char* title, int width, int height)
{
    if (title != nullptr)
    {
        printf("Window title: %s\n", title);
    }

    if ((width != WIDTH) || (height != HEIGHT))
    {
        puts("Requested resolution other than 640x480");
        return false;
    }

    vid_set_mode(DM_640x480 | DM_MULTIBUFFER, PM_RGB888);

    return true;
}

void DreamcastWindow::Destroy()
{
    return;
}

WindowSize DreamcastWindow::GetPixelSize() const
{
    WindowSize size;
    size.Width = WIDTH;
    size.Height = HEIGHT;
    return size;
}

void DreamcastWindow::DisplaySetFullscreen(bool enabled)
{
    return;
}

void DreamcastWindow::PumpOSEvents(IInput* sink, bool& outExitRequested)
{
    puts("NOOP: DreamcastWindow::PumpOSEvents");
    /*
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        if (sdlEvent.type == SDL_EVENT_QUIT)
        {
            outExitRequested = true;
            continue;
        }

        switch (sdlEvent.type)
        {
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                DisplayBridge::NotifyFullscreenChange(1);
                break;

            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                DisplayBridge::NotifyFullscreenChange(0);
                break;

            case SDL_EVENT_WINDOW_FOCUS_LOST:
#ifdef MOONCHILD_HAS_DISPLAY_OPTIONS
                SwallowEnterKey = false;
#endif
                sink->OnFocusLost();
                break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
#ifdef MOONCHILD_HAS_DISPLAY_OPTIONS
                if (HandleFullscreenHotkey(sdlEvent))
                {
                    break;
                }
#endif
                sink->OnKeyEvent(static_cast<int>(sdlEvent.key.key),
                                 sdlEvent.type == SDL_EVENT_KEY_DOWN,
                                 sdlEvent.key.repeat != 0);
                break;

            case SDL_EVENT_GAMEPAD_ADDED:
                sink->OnGamepadConnected(static_cast<int>(sdlEvent.gdevice.which));
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                sink->OnGamepadDisconnected(static_cast<int>(sdlEvent.gdevice.which));
                break;

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                sink->OnGamepadButton(static_cast<int>(sdlEvent.gbutton.which),
                                      sdlEvent.gbutton.button,
                                      sdlEvent.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN);
                break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                sink->OnGamepadAxis(static_cast<int>(sdlEvent.gaxis.which),
                                    sdlEvent.gaxis.axis,
                                    sdlEvent.gaxis.value);
                break;

            default:
                break;
        }
    }
    */
}
