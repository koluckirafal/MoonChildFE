#include "DreamcastWindow.h"

#include "DisplayBridge.h"
#include "IInput.h"

#include <cstdio>
#include <kos.h>

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
    // TODO: Handle:
    // - CD tray state, if not handled automatically (exit when opened to prevent any crashes)

    maple_device_t *dev;
    cont_state_t *state;

    // Access first controller
    dev = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if (!dev) return;

    // Fetch controller state
    state = static_cast<cont_state_t *>(maple_dev_status(dev));
    if (!state) return;

    // Iterate over all buttons
    sink->OnGamepadButton(0, CONT_DPAD_UP,    (state->buttons & CONT_DPAD_UP));
    sink->OnGamepadButton(0, CONT_DPAD_DOWN,  (state->buttons & CONT_DPAD_DOWN));
    sink->OnGamepadButton(0, CONT_DPAD_LEFT,  (state->buttons & CONT_DPAD_LEFT));
    sink->OnGamepadButton(0, CONT_DPAD_RIGHT, (state->buttons & CONT_DPAD_RIGHT));
    sink->OnGamepadButton(0, CONT_A,          (state->buttons & CONT_A));
    sink->OnGamepadButton(0, CONT_B,          (state->buttons & CONT_B));
    sink->OnGamepadButton(0, CONT_X,          (state->buttons & CONT_X));
    sink->OnGamepadButton(0, CONT_Y,          (state->buttons & CONT_Y));
    sink->OnGamepadButton(0, CONT_START,      (state->buttons & CONT_START));

    // Iterate over both axises of first analog joystick
    sink->OnGamepadAxis(0, CONT_CAPABILITY_ANALOG_X, state->joyx);
    sink->OnGamepadAxis(0, CONT_CAPABILITY_ANALOG_Y, state->joyy);

    return;
}
