// Catalog of the text the host shows the player (ui_text.h). One array per catalog language, indexed by UiTextId: a new
// string gets an id in ui_text.h and an entry in every language that has one (a missing entry falls back to English).
// The software keyboard has every language; the menu has English, Russian and Simplified Chinese.
#include <stdatomic.h>
#include <string.h>

#include "cfg.h"
#include "ov.h"
#include "platform/platform.h"
#include "ui_text.h"

typedef struct { const char *lang; const char *const *text; } Row;

// The language names are the same in every row.
#define LANG_NAMES \
    [UI_MENU_LANGNAME_EN] = "English", [UI_MENU_LANGNAME_JA] = "日本語", [UI_MENU_LANGNAME_FR] = "Français", \
    [UI_MENU_LANGNAME_DE] = "Deutsch", [UI_MENU_LANGNAME_IT] = "Italiano", [UI_MENU_LANGNAME_ES] = "Español", \
    [UI_MENU_LANGNAME_NL] = "Nederlands", [UI_MENU_LANGNAME_RU] = "Русский", \
    [UI_MENU_LANGNAME_ZH_HANS] = "简体中文", [UI_MENU_LANGNAME_ZH_HANT] = "繁體中文", [UI_MENU_LANGNAME_KO] = "한국어"

// SWKBD: TITLE, PROMPT, DEFAULT_NAME (at most 9 UTF-16 units, no '@' '%' '\'), OK, CANCEL, PAD_HINT.
#define SWKBD(title, prompt, name, ok, cancel, hint) \
    [UI_SWKBD_TITLE] = title, [UI_SWKBD_PROMPT] = prompt, [UI_SWKBD_DEFAULT_NAME] = name, [UI_SWKBD_OK] = ok, \
    [UI_SWKBD_CANCEL] = cancel, [UI_SWKBD_PAD_HINT] = hint, LANG_NAMES

static const char *const text_en[UI_TEXT_COUNT] = {
    [UI_MENU_PAD_DEVICE] = "Controller Device",
    [UI_MENU_PAD_DEVICE_DESC] = "Choose which controller to use. Changes take effect immediately.\n\n"
        "Newly connected controllers take over automatically. If the active controller disconnects, another available controller takes over.\n\n"
        "Manual choices apply immediately and are saved for startup. Auto starts with the first available controller. Later connections can still take over.\n\n"
        "Keyboard and mouse remain available.",
    [UI_MENU_PAD_FULLSCREEN_HOLD] = "Fullscreen hold shortcut",
    [UI_MENU_PAD_FULLSCREEN_HOLD_DESC] = "Off: Plus + Minus opens or closes settings.\n\nOn: hold Plus + Minus for 0.5 seconds to toggle fullscreen instead.\n\nGuide/Home and keyboard F1 remain available in either mode.",
    [UI_MENU_SHORTCUT_SETTINGS_COMBO_DESC] = "Press both displayed Plus and Minus bindings together to open or close settings.\n\nThis shortcut follows the current button bindings.\n\nKeyboard equivalent: F1.",
    [UI_MENU_SHORTCUTS] = "Shortcuts",
    [UI_MENU_SHORTCUT_READONLY] = "Read-only shortcuts",
    [UI_MENU_SHORTCUT_SETTINGS] = "Open settings menu",
    [UI_MENU_SHORTCUT_SETTINGS_DESC] = "Press the Guide or Home button to open or close settings.\n\nKeyboard equivalent: F1.\n\nThe client or operating system must pass this button to the game.",
    [UI_MENU_SHORTCUT_FULLSCREEN] = "Toggle fullscreen",
    [UI_MENU_SHORTCUT_FULLSCREEN_DESC] = "Hold both displayed buttons together for 0.5 seconds to toggle fullscreen.\n\nRelease both before repeating.\n\nKeyboard equivalent: F11.",
    [UI_MENU_SHORTCUT_PERFORMANCE] = "Toggle performance data",
    [UI_MENU_SHORTCUT_PERFORMANCE_DESC] = "Press both stick-click bindings together to cycle performance data: Off, FPS, then Full.\n\nThis shortcut follows the current stick-click bindings.\n\nKeyboard equivalent: F10.",
    [UI_MENU_PAD_CONNECTED] = "Connected",
    [UI_MENU_PAD_WAITING] = "Connect a controller to resume",
    [UI_MENU_PAD_NONE] = "No controller connected",
    [UI_MENU_PAD_SAVED] = "Saved controller",
    SWKBD("Text entry", "Enter a name", "Horse", "OK", "Cancel", "Controller: A = OK, B = Cancel"),

    [UI_MENU_TITLE] = "Settings",
    [UI_MENU_TAB_DISPLAY] = "Display", [UI_MENU_TAB_GRAPHICS] = "Graphics",
    [UI_MENU_TAB_GAME] = "Game", [UI_MENU_TAB_SOUND] = "Sound",
    [UI_MENU_TAB_CONTROLS] = "Controls", [UI_MENU_TAB_MENU] = "Menu", [UI_MENU_TAB_ADVANCED] = "Advanced",
    [UI_MENU_CONTINUE] = "Continue", [UI_MENU_QUIT] = "Quit game",
    [UI_MENU_ON] = "On", [UI_MENU_OFF] = "Off", [UI_MENU_AUTO] = "Automatic", [UI_MENU_IS_DEFAULT] = "Default",
    [UI_MENU_AFTER_RESTART] = "Restart required", [UI_MENU_SET_BY_ENV] = "Locked by environment variable",
    [UI_MENU_SAVE_FAILED] = "could not save settings",
    [UI_MENU_VERSION] = "DKB Recomp %s, game %s",

    [UI_MENU_FULLSCREEN] = "Fullscreen", [UI_MENU_FULLSCREEN_DESC] = "Fills the screen with the game; F11 toggles it.",
    [UI_MENU_WINDOW_MODE] = "Window mode",
    [UI_MENU_WINDOW_MODE_DESC] = "F11 switches between Windowed and the last fullscreen mode you used.",
    [UI_MENU_WINDOW_MODE_WINDOWED] = "Windowed", [UI_MENU_WINDOW_MODE_BORDERLESS] = "Borderless",
    [UI_MENU_WINDOW_MODE_FULLSCREEN] = "Fullscreen",
    [UI_MENU_WINDOW_SIZE] = "Window size", [UI_MENU_WINDOW_SIZE_DESC] = "Sets the size of the desktop window in Windowed mode.",
    [UI_MENU_WINDOW_SIZE_CUSTOM] = "Custom",
    [UI_MENU_WINDOWED_ONLY] = "Only available in windowed mode",
    [UI_MENU_PRESENT] = "Frame sync", [UI_MENU_PRESENT_DESC] = "VSync waits for the display to refresh.\n"
                             "Mailbox shows the latest finished frame at each refresh when supported.\n"
                             "Immediate sends frames as soon as they are ready and can cause tearing.",
    [UI_MENU_PRESENT_MAILBOX] = "Mailbox", [UI_MENU_PRESENT_VSYNC] = "VSync", [UI_MENU_PRESENT_IMMEDIATE] = "Immediate",
    [UI_MENU_PRESENT_IN_USE] = "in use: %s",
    [UI_MENU_GPU] = "Graphics card", [UI_MENU_GPU_DESC] = "Choose which graphics card runs the game. Use Automatic unless you need to select a specific card.",
    [UI_MENU_GPU_AUTO] = "Automatic",
    [UI_MENU_PERF_OVERLAY] = "Performance overlay",
    [UI_MENU_PERF_OVERLAY_DESC] = "Shows FPS in the top right. Full also shows a frame time graph and hardware statistics. Press F10 to cycle through the modes.",
    [UI_MENU_PERF_OFF] = "Off", [UI_MENU_PERF_FPS] = "FPS", [UI_MENU_PERF_FULL] = "Full",
    // printf format of the overlay line in FPS mode: the frame rate
    [UI_PERF_LINE_FPS] = "%.0f fps",
    [UI_MENU_RESOLUTION_GROUP] = "Resolution",
    [UI_MENU_DISPLAY_OUTPUT] = "Display output",
    [UI_MENU_RENDER_OPTIONS] = "Render Options",
    [UI_MENU_OUTPUT_RES] = "Render resolution",
    [UI_MENU_ULTRAWIDE] = "Ultrawide",
    [UI_MENU_ENABLE_ULTRAWIDE] = "Enable ultrawide",
    [UI_MENU_ULTRAWIDE_DESC] = "Widens the game view for 21:9 and 32:9 displays.",
    [UI_MENU_WIDE_RES] = "Render resolution",
    [UI_MENU_WIDE_RES_DESC] = "Sets the resolution of the game image before it is scaled to your display. "
                             "Higher resolutions can look sharper and use more GPU power and video memory.\n\n"
                             "Enable Ultrawide for 21:9 and 32:9 options.",
    [UI_MENU_HUD_POSITION] = "HUD position",
    [UI_MENU_HUD_POSITION_DESC] = "Place corner displays near the screen edges, or keep them inside a centered 16:9 "
                                 "area. Menus and aiming markers stay centered.",
    [UI_MENU_HUD_EDGES] = "Screen edges", [UI_MENU_HUD_CENTERED] = "Centered (16:9)",
    [UI_MENU_WIDE_DISABLED] = "Enable Ultrawide first",
    [UI_MENU_OUTPUT_RES_DESC] = "The size the game draws its picture and interface in. Above 1920 x 1080 the game needs "
                                "more video memory and GPU time. A wider size (21:9) widens the camera's view and moves "
                                "the HUD to the screen's corners; a taller one (16:10, Steam Deck) moves it to the top "
                                "and bottom edges.",
    [UI_MENU_RENDER_SCALE] = "3D render scale",
    [UI_MENU_RENDER_SCALE_DESC] = "Size of the 3D scene relative to the game resolution; the game's FSR scales it to "
                                  "that size. Above 100 % smooths jagged edges (supersampling) at a high GPU cost.",
    [UI_MENU_RENDER_SCALE_GAME] = "Game (83 %)",
    [UI_MENU_SCENE_RES] = "3D scene resolution",
    [UI_MENU_SCENE_RES_DESC] = "Auto lets the game scale its 3D scene by load, usually to two thirds of the render resolution "
                               "on each side, then upscale it. Full draws the 3D scene at the whole render resolution: a "
                               "sharper picture at a higher GPU cost, and the upscaler and DLSS are not used. Applies at the next start.",
    [UI_MENU_SCENE_RES_AUTO] = "Auto (game)", [UI_MENU_SCENE_RES_FULL] = "Full",
    [UI_MENU_UPSCALER] = "Upscaling method",
    [UI_MENU_UPSCALER_DESC] = "This applies when the game image is smaller than the area it fills on your screen or in your window. "
                              "AMD FSR 1 can keep edges sharper than Bilinear.",
    [UI_MENU_UPSCALER_FSR] = "AMD FSR 1", [UI_MENU_UPSCALER_BILINEAR] = "Bilinear", [UI_MENU_UPSCALER_DLSS] = "NVIDIA DLSS",
    [UI_MENU_UPSCALER_FSR3] = "AMD FSR 3.1",
    [UI_MENU_ASPECT] = "Display scaling",
    [UI_MENU_ASPECT_DESC] = "Default keeps the game's shape and adds black bars if needed.\n"
                            "Stretch to fill uses the whole window or screen. The image stretches only if the shapes differ.",
    [UI_MENU_ASPECT_FIT] = "Default", [UI_MENU_ASPECT_STRETCH] = "Stretch to fill",
    [UI_MENU_FOV] = "Field of view",
    [UI_MENU_FOV_DESC] = "Adjusts the camera's view as a percentage of the original angle. 100% uses the game's default view.",
    [UI_MENU_SHARPNESS] = "FSR sharpness",
    [UI_MENU_SHARPNESS_DESC] = "Sharpening after FSR scaling; 0 turns it off.",
    [UI_MENU_NO_UPSCALING] = "Requires a smaller game image",
    [UI_MENU_SHARPNESS_NEEDS_FSR] = "Requires AMD FSR 1",
    [UI_MENU_SMAA] = "Anti-aliasing (SMAA)",
    [UI_MENU_SMAA_DESC] = "Smooths jagged edges in the game image and HUD. Higher settings use more GPU time.",
    [UI_MENU_SMAA_LOW] = "Low", [UI_MENU_SMAA_MEDIUM] = "Medium", [UI_MENU_SMAA_HIGH] = "High",
    [UI_MENU_COLOR] = "Color",
    [UI_MENU_COLOR_PRESET] = "Color profile",
    [UI_MENU_COLOR_PRESET_DESC] = "Game default uses the original colors.\nVivid (OLED) boosts contrast and color.\n"
                                  "Clarity makes the image a little brighter and more vivid.\n\n"
                                  "Adjusting any slider switches the profile to Custom.",
    [UI_MENU_COLOR_DEFAULT] = "Game default", [UI_MENU_COLOR_VIVID] = "Vivid (OLED)", [UI_MENU_COLOR_CLARITY] = "Clarity",
    [UI_MENU_COLOR_CUSTOM] = "Custom",
    [UI_MENU_COLOR_BRIGHTNESS] = "Brightness", [UI_MENU_COLOR_BRIGHTNESS_DESC] = "Makes the image brighter or darker. The default is 0%.",
    [UI_MENU_COLOR_CONTRAST] = "Contrast",
    [UI_MENU_COLOR_CONTRAST_DESC] = "Higher values make dark areas darker and bright areas brighter. The default is 100%.",
    [UI_MENU_COLOR_SATURATION] = "Saturation",
    [UI_MENU_COLOR_SATURATION_DESC] = "Higher values make colors more vivid. The default is 100%.",
    [UI_MENU_COLOR_GAMMA] = "Gamma",
    [UI_MENU_COLOR_GAMMA_DESC] = "Below 1.00 brightens shadows and midtones. Above 1.00 darkens them.",
    [UI_MENU_HDR_GROUP] = "HDR (experimental)",
    [UI_MENU_HDR] = "HDR output",
    [UI_MENU_HDR_DESC] = "Brightens highlights in the game's SDR image on an HDR display.\n\n"
                         "Enable HDR in your system display settings first.",
    [UI_MENU_HDR_NATIVE] = "Game's own HDR",
    [UI_MENU_HDR_NATIVE_DESC] = "Uses the game’s own HDR on update 3.2.0. Highlights follow Peak brightness up to 1400 nits. "
                                "This mode disables FSR 1, SMAA, and color adjustments.",
    [UI_MENU_HDR_PEAK] = "Peak brightness",
    [UI_MENU_HDR_PEAK_DESC] = "Set the brightest highlights to match your display's peak brightness in nits.",
    [UI_MENU_HDR_GAME] = "Game brightness",
    [UI_MENU_HDR_GAME_DESC] = "Sets the brightness of ordinary whites in the game and HUD. Raise it if the picture looks too dim.",
    [UI_MENU_HDR_UI] = "Settings menu brightness",
    [UI_MENU_HDR_UI_DESC] = "Sets the brightness of whites in this settings menu. The game HUD follows Game brightness.",

    [UI_MENU_FRAME_RATE_GROUP] = "Frame rate",
    [UI_MENU_FPS_LIMIT] = "Frame rate limit",
    [UI_MENU_DKB_FPS_LIMIT_DESC] = "Caps the game’s frame rate. Monitor refresh uses the current refresh rate of your display.",
    [UI_MENU_FPS_LIMIT_DESC] = "The most frames per second the game draws. Above 60 is experimental and needs Game speed "
                               "follows the clock. Unlimited is experimental.",
    [UI_MENU_FPS_30] = "30 FPS", [UI_MENU_FPS_40] = "40 FPS", [UI_MENU_FPS_45] = "45 FPS",
    [UI_MENU_FPS_60] = "60 FPS", [UI_MENU_FPS_90] = "90 FPS", [UI_MENU_FPS_120] = "120 FPS", [UI_MENU_FPS_144] = "144 FPS",
    // the monitor's refresh rate; printf format with the rate in Hz when it is known
    [UI_MENU_FPS_DISPLAY] = "Monitor refresh", [UI_MENU_FPS_DISPLAY_HZ] = "Monitor refresh (%.0f Hz)",
    [UI_MENU_FPS_UNLIMITED] = "Unlimited",
    [UI_MENU_FPS_NEEDS_REALTIME] = "Enable \"Allow higher frame rates\" and restart",
    [UI_MENU_REALTIME] = "Game speed follows the clock",
    [UI_MENU_REALTIME_DESC] = "Steps the game by real time, so a slow frame does not slow the game down.",
    [UI_MENU_DKB_REALTIME] = "Allow higher frame rates",
    [UI_MENU_DKB_REALTIME_DESC] = "Unlocks the Frame rate limit setting, including rates above 60 FPS. "
                                "Game timing adapts as FPS changes. This mode is experimental.",
    [UI_MENU_CLOTH_REALTIME] = "Cloth on the clock",
    [UI_MENU_CLOTH_REALTIME_DESC] = "Steps cloth and hair by real time as well.",
    [UI_MENU_HIGHFPS_FIX] = "High frame rate fixes",
    [UI_MENU_HIGHFPS_FIX_DESC] = "Corrects physics that misbehave above 30 frames per second, and objects that stay see-through or invisible above 60.",
    [UI_MENU_GAME_LANG] = "Game language", [UI_MENU_GAME_LANG_DESC] = "Language of the game's text and voices.",
    [UI_MENU_GAME_LANG_AUTO] = "Automatic",
    [UI_MENU_OPERATION_MODE] = "Console mode",
    [UI_MENU_OPERATION_MODE_DESC] = "Choose which Switch mode the game uses. Docked is the TV profile. Handheld uses the portable profile.",
    [UI_MENU_MODE_DOCKED] = "Docked", [UI_MENU_MODE_HANDHELD] = "Handheld",
    [UI_MENU_ASSUME_FOCUS] = "Keep running in background",
    [UI_MENU_ASSUME_FOCUS_DESC] = "Keeps the game running while you use another window. Turn it off to pause when you switch away.\n\n"
                                  "Pausing during a load can prevent it from completing.",

    [UI_MENU_VOLUME] = "Volume", [UI_MENU_VOLUME_DESC] = "Adjusts the overall game volume. Set it to 0% to mute the game.",
    [UI_MENU_AUDIO_CHANNELS] = "Audio output",
    [UI_MENU_AUDIO_CHANNELS_DESC] = "Choose Stereo or 5.1 surround. Automatic uses 5.1 when your default audio device supports at least six channels.",
    [UI_MENU_CHANNELS_AUTO] = "Automatic", [UI_MENU_CHANNELS_STEREO] = "Stereo", [UI_MENU_CHANNELS_51] = "5.1 surround",

    [UI_MENU_CONTROLLER] = "Controller", [UI_MENU_MOUSE_KEYBOARD] = "Mouse & Keyboard",
    [UI_MENU_MOTION] = "Motion controls", [UI_MENU_MOTION_DESC] = "Aiming with the controller's gyroscope.",
    [UI_MENU_RUMBLE_STRENGTH] = "Vibration strength",
    [UI_MENU_RUMBLE_STRENGTH_DESC] = "How strong the controller vibrates.",
    [UI_MENU_RUMBLE_CURVE] = "Vibration curve",
    [UI_MENU_RUMBLE_CURVE_DESC] = "Lower values make subtle vibration easier to feel.",
    [UI_MENU_RUMBLE_TRIGGERS] = "Trigger vibration (Xbox)",
    [UI_MENU_RUMBLE_TRIGGERS_DESC] = "Enables vibration in the triggers on supported Xbox controllers.",
    [UI_MENU_PAD_LAYOUT] = "Button layout",
    [UI_MENU_PAD_LAYOUT_DESC] = "Automatic picks Nintendo for Nintendo-style controllers and Xbox otherwise.\nNintendo uses the right button for A.\n"
                                "Xbox uses the bottom button for A.\nXbox prompts uses Nintendo positions with Xbox labels.\n\n"
                                "The game still shows the original prompt for any button you remap.",
    [UI_MENU_PAD_LAYOUT_AUTO] = "Automatic", [UI_MENU_PAD_LAYOUT_NINTENDO] = "Nintendo (A right)",
    [UI_MENU_PAD_LAYOUT_XBOX] = "Xbox (A bottom)", [UI_MENU_PAD_LAYOUT_XBOX_PROMPTS] = "Xbox prompts (A right)",
    [UI_MENU_PAD_LAYOUT_IN_USE] = "in use: %s",
    [UI_MENU_PAD_LAYOUT_PROMPTS_MISSING] = "prompt files missing: run the setup again",
    [UI_MENU_PAD_LAYOUT_PROMPTS_RESTART] = "prompts change after restart",
    [UI_MENU_STICK_DEADZONE] = "Stick dead zone",
    [UI_MENU_STICK_DEADZONE_DESC] = "Sets how far either stick must move before it registers. Raise it if the character or camera drifts.",
    [UI_MENU_MOUSE_CAMERA] = "Direct mouse camera",
    [UI_MENU_MOUSE_CAMERA_DESC] = "Mouse movement turns the follow camera directly. Other camera modes interpret mouse movement as stick input.",
    [UI_MENU_MOUSE_SENS] = "Mouse sensitivity",
    [UI_MENU_MOUSE_SENS_DESC] = "Sets how far the camera turns when you move the mouse.",

    [UI_BINDS_HELP_KEYS] = "Click a binding or press Enter, then press a key or mouse button. Delete or Backspace clears it. "
                           "A key already in use moves to the new action.",
    [UI_BINDS_HELP_PAD] = "Press A on a binding, then press a controller button. X clears it. Buttons already in use "
                          "swap. The left stick moves and the right stick controls the camera.",
    [UI_BINDS_COL_KEY1] = "Primary", [UI_BINDS_COL_KEY2] = "Secondary",
    [UI_BINDS_GROUP_BUTTONS] = "Buttons", [UI_BINDS_GROUP_MOVE] = "Movement", [UI_BINDS_GROUP_CAMERA] = "Camera",
    [UI_BINDS_PRESS_KEY] = "Press a key or mouse button for %s",
    [UI_BINDS_PRESS_PAD] = "Press a controller button for %s",
    [UI_BINDS_CANCEL_HINT] = "Esc cancels, or wait %d s",
    [UI_BINDS_TAKEN] = "%s was taken from %s", [UI_BINDS_SWAPPED] = "%s swapped with %s",
    [UI_BINDS_RESET] = "Reset all bindings",
    [UI_BINDS_RESET_DESC] = "Back to the default keys and controller buttons. The button layout above applies again: remapped controller buttons stay where you put them until this reset.",
    [UI_BIND_A] = "A", [UI_BIND_B] = "B", [UI_BIND_X] = "X", [UI_BIND_Y] = "Y",
    [UI_BIND_LCLICK] = "Left stick click", [UI_BIND_RCLICK] = "Right stick click", [UI_BIND_L] = "L",
    [UI_BIND_R] = "R", [UI_BIND_ZL] = "ZL", [UI_BIND_ZR] = "ZR", [UI_BIND_PLUS] = "Plus", [UI_BIND_MINUS] = "Minus",
    [UI_BIND_LEFT] = "D-pad left", [UI_BIND_UP] = "D-pad up", [UI_BIND_RIGHT] = "D-pad right",
    [UI_BIND_DOWN] = "D-pad down",
    [UI_BIND_LSTICK_LEFT] = "Move left", [UI_BIND_LSTICK_UP] = "Move forward", [UI_BIND_LSTICK_RIGHT] = "Move right",
    [UI_BIND_LSTICK_DOWN] = "Move back",
    [UI_BIND_RSTICK_LEFT] = "Camera left", [UI_BIND_RSTICK_UP] = "Camera up", [UI_BIND_RSTICK_RIGHT] = "Camera right",
    [UI_BIND_RSTICK_DOWN] = "Camera down",
    [UI_BIND_DESC_A] = "Interact, pick up, talk.", [UI_BIND_DESC_B] = "Run, cancel, go back.",
    [UI_BIND_DESC_X] = "Jump.", [UI_BIND_DESC_Y] = "Attack.", [UI_BIND_DESC_LCLICK] = "Crouch.",
    [UI_BIND_DESC_RCLICK] = "First-person view.", [UI_BIND_DESC_L] = "Abilities.",
    [UI_BIND_DESC_R] = "Throw a weapon.", [UI_BIND_DESC_ZL] = "Shield, lock on.", [UI_BIND_DESC_ZR] = "Bow.",
    [UI_BIND_DESC_PLUS] = "Menu, inventory.", [UI_BIND_DESC_MINUS] = "Map.",
    [UI_BIND_DESC_DPAD] = "Quick-select.",
    [UI_BIND_DESC_MOVE] = "Moves Link like the left stick. The controller's left stick always works too.",
    [UI_BIND_DESC_CAMERA] = "Turns the camera like the right stick. The mouse and the controller's right stick always "
                            "work too.",
    [UI_DKB_BIND_A] = "Jump. Confirms selections in menus.",
    [UI_DKB_BIND_B] = "Punch downward. Goes back in menus.",
    [UI_DKB_BIND_X] = "Punch upward.",
    [UI_DKB_BIND_Y] = "Punch forward or swing the chunk you're holding.",
    [UI_DKB_BIND_LCLICK] = "Use the left stick press action assigned in Bananza's controls.",
    [UI_DKB_BIND_RCLICK] = "Use the right stick press action assigned in Bananza's controls.",
    [UI_DKB_BIND_L] = "Hold to whistle and reveal the route to your objective.",
    [UI_DKB_BIND_R] = "Use Hand Slap to collect nearby items and reveal hidden objects.",
    [UI_DKB_BIND_ZL] = "Roll forward or Turf Surf while holding a chunk.",
    [UI_DKB_BIND_ZR] = "Grab a chunk of terrain or throw the chunk you're holding.",
    [UI_DKB_BIND_PLUS] = "Open the pause menu.",
    [UI_DKB_BIND_MINUS] = "Open the map.",
    [UI_DKB_BIND_LEFT] = "Cycle to the previous unlocked Bananza form.",
    [UI_DKB_BIND_UP] = "Open the Skills menu.",
    [UI_DKB_BIND_RIGHT] = "Cycle to the next unlocked Bananza form.",
    [UI_DKB_BIND_DOWN] = "Open Photo Mode when available.",
    [UI_DKB_BIND_MOVE] = "Move Donkey Kong in this direction.",
    [UI_DKB_BIND_CAMERA] = "Turn the camera in this direction.",
    [UI_KEY_SPACE] = "Space", [UI_KEY_TAB] = "Tab", [UI_KEY_LSHIFT] = "Left Shift", [UI_KEY_LCTRL] = "Left Ctrl",
    [UI_KEY_UP] = "Up arrow", [UI_KEY_DOWN] = "Down arrow", [UI_KEY_LEFT] = "Left arrow",
    [UI_KEY_RIGHT] = "Right arrow",
    [UI_KEY_MOUSE_L] = "Left mouse", [UI_KEY_MOUSE_R] = "Right mouse", [UI_KEY_MOUSE_M] = "Middle mouse",
    [UI_KEY_MOUSE_X1] = "Mouse 4", [UI_KEY_MOUSE_X2] = "Mouse 5",
    [UI_KEY_ENTER] = "Enter", [UI_KEY_BACKSPACE] = "Backspace", [UI_KEY_RSHIFT] = "Right Shift",
    [UI_KEY_RCTRL] = "Right Ctrl", [UI_KEY_LALT] = "Alt", [UI_KEY_CAPSLOCK] = "Caps Lock", [UI_KEY_INSERT] = "Insert",
    [UI_KEY_DELETE] = "Delete", [UI_KEY_HOME] = "Home", [UI_KEY_END] = "End", [UI_KEY_PAGEUP] = "Page Up",
    [UI_KEY_PAGEDOWN] = "Page Down", [UI_KEY_NUM_FMT] = "Num %s",
    [UI_PAD_SOUTH] = "Bottom button", [UI_PAD_EAST] = "Right button", [UI_PAD_WEST] = "Left button",
    [UI_PAD_NORTH] = "Top button", [UI_PAD_LSTICK] = "Left stick", [UI_PAD_RSTICK] = "Right stick",
    [UI_PAD_L] = "Left bumper", [UI_PAD_R] = "Right bumper", [UI_PAD_START] = "Start", [UI_PAD_BACK] = "Back",
    [UI_PAD_DPAD_LEFT] = "D-pad left", [UI_PAD_DPAD_UP] = "D-pad up", [UI_PAD_DPAD_RIGHT] = "D-pad right",
    [UI_PAD_DPAD_DOWN] = "D-pad down", [UI_PAD_ZL] = "Left trigger", [UI_PAD_ZR] = "Right trigger",
    [UI_PAD_LPADDLE1] = "Left back button", [UI_PAD_RPADDLE1] = "Right back button",
    [UI_PAD_LPADDLE2] = "Left back button 2", [UI_PAD_RPADDLE2] = "Right back button 2", [UI_PAD_TOUCHPAD] = "Touchpad",
    [UI_PAD_CROSS] = "Cross", [UI_PAD_CIRCLE] = "Circle", [UI_PAD_SQUARE] = "Square", [UI_PAD_TRIANGLE] = "Triangle",

    [UI_MENU_MENU_LANG] = "Settings language", [UI_MENU_MENU_LANG_DESC] = "Sets the language of the settings menu and performance overlay. System default uses "
                             "Russian when your system language is Russian, Chinese when it is Chinese, and English "
                             "otherwise.",
    [UI_MENU_MENU_LANG_AUTO] = "System default",
    [UI_MENU_THEME] = "Menu theme", [UI_MENU_THEME_DK] = "Donkey Kong", [UI_MENU_THEME_CLASSIC] = "Classic",
    [UI_MENU_THEME_DESC] = "Choose the Donkey Kong theme or the classic green and gold theme.",

    [UI_MENU_ADV_WARNING] = "These settings are for testing and troubleshooting. Change them only when you know why.",
    [UI_MENU_GROUP_SCHED] = "Scheduling", [UI_MENU_GROUP_SHADERS] = "Shaders and pipelines",
    [UI_MENU_GROUP_TEXMEM] = "Textures and memory", [UI_MENU_GROUP_FIXES] = "Render fixes",
    [UI_MENU_GROUP_VULKAN] = "Vulkan", [UI_MENU_GROUP_DIAG] = "Diagnostics",
    [UI_MENU_ASYNC_PREEMPT] = "Virtual core preemption",
    [UI_MENU_ASYNC_PREEMPT_DESC] = "Lets a game thread interrupt another one on the same virtual core.",
    [UI_MENU_PREEMPT_OFF] = "Off", [UI_MENU_PREEMPT_ON] = "On", [UI_MENU_PREEMPT_INHERIT] = "Inheritance only",
    [UI_MENU_REPLAY_ASYNC] = "GPU frame prep on its own thread",
    [UI_MENU_REPLAY_ASYNC_DESC] = "Prepares frames for the GPU on a separate thread instead of the game thread. Adds up to one frame of latency.",
    [UI_MENU_REPLAY_THREADS] = "Threads recording big render passes",
    [UI_MENU_REPLAY_THREADS_DESC] = "Records the draws of big render passes on this many threads at once. Automatic uses 6 threads on 12 or more logical processors and 4 on 8 to 11 (not on AMD); Off records them on one thread. Faster in busy areas on CPUs with many cores.",
    [UI_MENU_TIMER_1MS] = "1 ms Windows timer",
    [UI_MENU_TIMER_1MS_DESC] = "Raises the system timer resolution to 1 ms for steadier frame pacing.",
    [UI_MENU_PIPE_WAIT] = "Waiting draw handling",
    [UI_MENU_PIPE_WAIT_DESC] = "Default keeps the current scheduling with a reserved worker for waiting draws; Old (draw) uses the previous scheduling.",
    [UI_MENU_PIPE_WAIT_DEFAULT] = "Default", [UI_MENU_PIPE_WAIT_DRAW] = "Old (draw)",
    [UI_MENU_PIPE_SKIP] = "Draws skipped while a pipeline builds",
    [UI_MENU_PIPE_SKIP_DESC] = "Which draws are dropped instead of waiting for their pipeline.",
    [UI_MENU_PIPE_SKIP_STRICT] = "Strict", [UI_MENU_PIPE_SKIP_ANY] = "Any",
    [UI_MENU_PIPE_SKIP_FULLSCREEN] = "Fullscreen passes",
    [UI_MENU_LIB_PREBUILD] = "Stage library prebuild",
    [UI_MENU_LIB_PREBUILD_DESC] = "Builds shader stage libraries ahead of time in the background.",
    [UI_MENU_LIB_PREBUILD_THREADS] = "Stage library prebuild threads",
    [UI_MENU_LIB_PREBUILD_THREADS_DESC] = "Threads used for the prebuild; Automatic picks a number from your CPU.",
    [UI_MENU_PIPE_PREBUILD] = "Pipeline prebuild",
    [UI_MENU_PIPE_PREBUILD_DESC] = "Builds known pipelines ahead of time.",
    [UI_MENU_PREWARM_XLATE] = "Translator prewarm",
    [UI_MENU_PREWARM_XLATE_DESC] = "Threads that translate known shaders at start; Automatic takes half the CPU threads, at most 16.",
    [UI_MENU_GPL] = "Graphics pipeline library",
    [UI_MENU_GPL_DESC] = "Uses the Vulkan pipeline library extension; turn off if your driver misbehaves with it.",
    [UI_MENU_ASTC] = "ASTC texture decoding", [UI_MENU_ASTC_DESC] = "Where compressed ASTC textures are decoded.",
    [UI_MENU_ASTC_AUTO] = "Automatic", [UI_MENU_ASTC_GPU] = "GPU shader", [UI_MENU_ASTC_NATIVE] = "Native",
    [UI_MENU_ASTC_CPU] = "CPU",
    [UI_MENU_MEMORY] = "GPU memory mode", [UI_MENU_MEMORY_DESC] = "How game memory is shared with the GPU.",
    [UI_MENU_MEMORY_AUTO] = "Automatic", [UI_MENU_MEMORY_IMPORT] = "Import", [UI_MENU_MEMORY_MIRROR] = "Mirror",
    [UI_MENU_MEMORY_VERIFY] = "Verify",
    [UI_MENU_TEX_ALIAS] = "Texture aliasing", [UI_MENU_TEX_ALIAS_DESC] = "A new texture created over a live texture's memory starts with that content instead of empty.",
    [UI_MENU_TEX_VRAM_PRESSURE] = "Texture memory limit", [UI_MENU_TEX_VRAM_PRESSURE_DESC] = "Percent of video memory above which textures the game released are dropped (they come back from storage when used again). 0 keeps them.",
    [UI_MENU_ASTC_BC] = "Compress ASTC to BC3", [UI_MENU_ASTC_BC_DESC] = "With the GPU decoder: re-encodes decoded ASTC textures to BC3, a quarter of the video memory. For GPUs with 4 GB or less; textures look slightly blockier.",
    [UI_MENU_LOD_GUARD] = "LOD guard", [UI_MENU_LOD_GUARD_DESC] = "Prevents objects flashing in the wrong place or a flat far field for a frame, by waiting for the game to finish rewriting level-of-detail data before the GPU reads it.",
    [UI_MENU_FIX_C0] = "Constant buffer fix",
    [UI_MENU_FIX_C0_DESC] = "Fills the driver-reserved constant bank c0 (storage sizes, dispatch size, point size, base instance) instead of reading zeros.",
    [UI_MENU_FIX_ZERO] = "Zero new GPU memory",
    [UI_MENU_FIX_ZERO_DESC] = "New GPU memory pools start zero-filled, as Switch memory does.",
    [UI_MENU_LIST_RESTART] = "Primitive restart on lists",
    [UI_MENU_LIST_RESTART_DESC] = "Applies the game's primitive restart to triangle-list draws, as the Switch does. Without it the restart indices are drawn as vertices: long stretched triangles, for example on the map. Needs a GPU with primitiveTopologyListRestart.",
    [UI_MENU_BIND_SKIP] = "Skip repeated binds",
    [UI_MENU_BIND_SKIP_DESC] = "Optimisation: skips bindings that did not change.",
    [UI_MENU_SHADER_MEMO] = "Shader memo", [UI_MENU_SHADER_MEMO_DESC] = "Optimisation: remembers shader lookups.",
    [UI_MENU_PIPE_MEMO] = "Pipeline memo", [UI_MENU_PIPE_MEMO_DESC] = "Optimisation: reuses the previous draw's pipeline when its state repeats.",
    [UI_MENU_TEX_MEMO] = "Texture binding memo", [UI_MENU_TEX_MEMO_DESC] = "Optimisation: remembers texture descriptors between draws.",
    [UI_MENU_DESC_TEMPLATE] = "Descriptor templates",
    [UI_MENU_DESC_TEMPLATE_DESC] = "Optimisation: updates descriptors in bulk.",
    [UI_MENU_PUSH_SKIP] = "Skip repeated push constants",
    [UI_MENU_PUSH_SKIP_DESC] = "Optimisation: skips push constants that did not change.",
    [UI_MENU_SPLIT_DRAWS] = "Early GPU submission",
    [UI_MENU_SPLIT_DRAWS_DESC] = "Submits GPU work in parts every N draws between passes so the GPU starts earlier; 0 turns it off.",
    [UI_MENU_VK_VALIDATION] = "Validation layer",
    [UI_MENU_VK_VALIDATION_DESC] = "Checks every Vulkan call and slows the game down a lot.",
    [UI_MENU_VK_VALIDATION_MISSING] = "The validation layer is not installed.",
    [UI_MENU_VK_DYN_VERTEX] = "Dynamic vertex state",
    [UI_MENU_VK_DYN_VERTEX_DESC] = "Forces dynamic vertex input on or off; the default depends on the GPU.",
    [UI_MENU_VK_DYN_A2C] = "Dynamic alpha to coverage",
    [UI_MENU_VK_DYN_A2C_DESC] = "Forces dynamic alpha to coverage on or off. Default: off on AMD's driver for RDNA 2 and "
                                "older, which draws characters black with it, on elsewhere.",
    [UI_MENU_VK_SUBGROUP_PIN] = "Pin subgroup size", [UI_MENU_VK_SUBGROUP_PIN_DESC] = "Fixes the shader subgroup size.",
    [UI_MENU_TRACE] = "Performance log", [UI_MENU_TRACE_DESC] = "Writes frame timings to a file.",
    [UI_MENU_FLIGHT] = "Flight recorder",
    [UI_MENU_FLIGHT_DESC] = "Keeps a short history and saves it when a frame freezes.",
    [UI_MENU_FLIGHT_SPIKE_MS] = "Freeze threshold (ms)",
    [UI_MENU_FLIGHT_SPIKE_MS_DESC] = "Frame time that counts as a freeze.",
    [UI_MENU_INPUT_RECORD] = "Input recording",
    [UI_MENU_INPUT_RECORD_DESC] = "Writes your inputs to a file for bug reports.",
    [UI_MENU_OPEN_LOG] = "Open log folder", [UI_MENU_OPEN_LOG_DESC] = "Shows the folder with the log files.",
    [UI_MENU_COPY_INFO] = "Copy system info",
    [UI_MENU_COPY_INFO_DESC] = "Copies GPU, driver, CPU, RAM and changed settings to the clipboard.",
    [UI_MENU_COPY_INFO_DONE] = "Copied", [UI_MENU_ACTION_FAILED] = "did not work",
    [UI_MENU_RESET] = "Reset all settings to defaults", [UI_MENU_RESET_DESC] = "Applies to every tab.",
    [UI_MENU_RESET_CONFIRM] = "Reset every setting to its default?", [UI_MENU_RESET_YES] = "Reset",
    [UI_MENU_RESET_NO] = "Cancel",

    [UI_MENU_OV_FPS] = "%.0f FPS", [UI_MENU_OV_FRAME_MS] = "%.1f ms", [UI_MENU_OV_GAME_PCT] = "game %.0f%%",
    [UI_MENU_OV_MAX_MS] = "max %.1f ms",
    [UI_MENU_OV_GPU] = "GPU", [UI_MENU_OV_CPU] = "CPU", [UI_MENU_OV_RAM] = "RAM", [UI_MENU_OV_VRAM] = "VRAM",
    [UI_MENU_OV_TEMP] = "Temperature", [UI_MENU_OV_POWER] = "Power", [UI_MENU_OV_CLOCK] = "Clock",
    [UI_MENU_OV_NONE] = "-",
    [UI_MENU_OV_REPLAY] = "GPU frame prep", [UI_MENU_OV_GPU_TIME] = "GPU busy per frame",
    [UI_MENU_OV_PIPE_WAIT] = "Pipeline waits",
    [UI_MENU_OV_SHADER_WAIT] = "Shader waits", [UI_MENU_OV_TEX_UPLOADS] = "Texture uploads",
    [UI_MENU_OV_FMT_PCT] = "%.0f%%", [UI_MENU_OV_FMT_TEMP] = "%.0f °C", [UI_MENU_OV_FMT_POWER] = "%.0f W",
    [UI_MENU_OV_FMT_MHZ] = "%.0f MHz", [UI_MENU_OV_FMT_GB] = "%.1f GB", [UI_MENU_OV_FMT_GB_OF] = "/ %.1f GB",
    [UI_MENU_OV_GAME_GB] = "game %.1f GB", [UI_MENU_OV_FMT_MS] = "%.2f ms", [UI_MENU_OV_PER_FRAME] = "%.1f per frame",
    [UI_MENU_OV_WINDOW] = "Window", [UI_MENU_OV_OUTPUT] = "Game image", [UI_MENU_OV_SCENE] = "3D scene",
    [UI_MENU_OV_SCALING] = "Scaling", [UI_MENU_OV_LIMIT] = "Frame limit", [UI_MENU_OV_PRESENT] = "Presentation",
    [UI_MENU_OV_FMT_SIZE] = "%ux%u", [UI_MENU_OV_FMT_SCENE] = "%ux%u, %.0f%%",
    [UI_MENU_OV_FMT_UPSCALE] = "%ux%u -> %ux%u",
    [UI_MENU_OV_NO_UPSCALER] = "no upscaler",
    [UI_MENU_OV_FMT_FSR] = "AMD FSR 1, sharpness %d%%", [UI_MENU_OV_SCALE_NONE] = "none (1:1)",
    [UI_MENU_OV_SCALE_BILINEAR] = "bilinear",
};

static const char *const text_ru[UI_TEXT_COUNT] = {
    SWKBD("Ввод текста", "Введите имя", "Лошадь", "ОК", "Отмена", "Контроллер: A — ОК, B — отмена"),

    [UI_MENU_TITLE] = "Настройки",
    [UI_MENU_TAB_DISPLAY] = "Экран", [UI_MENU_TAB_GRAPHICS] = "Графика",
    [UI_MENU_TAB_GAME] = "Игра", [UI_MENU_TAB_SOUND] = "Звук",
    [UI_MENU_TAB_CONTROLS] = "Управление", [UI_MENU_TAB_MENU] = "Меню", [UI_MENU_TAB_ADVANCED] = "Дополнительно",
    [UI_MENU_CONTINUE] = "Закрыть настройки", [UI_MENU_QUIT] = "Выйти из игры",
    [UI_MENU_ON] = "Вкл.", [UI_MENU_OFF] = "Выкл.", [UI_MENU_AUTO] = "Автоматически",
    [UI_MENU_IS_DEFAULT] = "По умолчанию",
    [UI_MENU_AFTER_RESTART] = "Требуется перезапуск", [UI_MENU_SET_BY_ENV] = "Задано переменной окружения",
    [UI_MENU_SAVE_FAILED] = "не удалось сохранить настройки",
    [UI_MENU_VERSION] = "DKB Recomp %s, игра %s",

    [UI_MENU_FULLSCREEN] = "Полный экран", [UI_MENU_FULLSCREEN_DESC] = "Игра на весь экран; F11 переключает режим.",
    [UI_MENU_WINDOW_MODE] = "Режим окна",
    [UI_MENU_WINDOW_MODE_DESC] = "F11 переключает между оконным и последним выбранным полноэкранным режимом.",
    [UI_MENU_WINDOW_MODE_WINDOWED] = "В окне", [UI_MENU_WINDOW_MODE_BORDERLESS] = "Без рамки",
    [UI_MENU_WINDOW_MODE_FULLSCREEN] = "Полный экран",
    [UI_MENU_WINDOW_SIZE] = "Размер окна", [UI_MENU_WINDOW_SIZE_DESC] = "Размер окна игры вне полноэкранного режима.",
    [UI_MENU_WINDOW_SIZE_CUSTOM] = "Другой",
    [UI_MENU_WINDOWED_ONLY] = "Доступно только в оконном режиме",
    [UI_MENU_PRESENT] = "Синхронизация кадров", [UI_MENU_PRESENT_DESC] = "Вертикальная синхронизация ждёт обновления экрана.\n"
                             "Mailbox показывает последний готовый кадр при обновлении, если режим поддерживается.\n"
                             "Без ожидания кадры выводятся сразу, но возможны разрывы изображения.",
    [UI_MENU_PRESENT_MAILBOX] = "Mailbox", [UI_MENU_PRESENT_VSYNC] = "Вертикальная синхронизация",
    [UI_MENU_PRESENT_IMMEDIATE] = "Без ожидания",
    [UI_MENU_PRESENT_IN_USE] = "сейчас: %s",
    [UI_MENU_GPU] = "Видеокарта", [UI_MENU_GPU_DESC] = "Устройство Vulkan, которое рисует игру.",
    [UI_MENU_GPU_AUTO] = "Автоматически",
    [UI_MENU_PERF_OVERLAY] = "Индикатор производительности",
    [UI_MENU_PERF_OVERLAY_DESC] = "Частота кадров в правом верхнем углу. «Полный» добавляет время кадров с графиком, "
                                  "загрузку ЦП, GPU и памяти, температуры и ожидания потока отрисовки. F10 переключает "
                                  "режимы.",
    [UI_MENU_PERF_OFF] = "Выкл.", [UI_MENU_PERF_FPS] = "FPS", [UI_MENU_PERF_FULL] = "Полный",
    [UI_PERF_LINE_FPS] = "%.0f к/с",
    [UI_MENU_RESOLUTION_GROUP] = "Разрешение",
    [UI_MENU_DISPLAY_OUTPUT] = "Вывод изображения",
    [UI_MENU_RENDER_OPTIONS] = "Настройки рендеринга",
    [UI_MENU_OUTPUT_RES] = "Разрешение рендеринга",
    [UI_MENU_ULTRAWIDE] = "Ультраширокий экран",
    [UI_MENU_ENABLE_ULTRAWIDE] = "Включить ультраширокий режим",
    [UI_MENU_ULTRAWIDE_DESC] = "Расширяет обзор в игре на экранах 21:9 и 32:9.",
    [UI_MENU_WIDE_RES] = "Разрешение отрисовки",
    [UI_MENU_WIDE_RES_DESC] = "Разрешение игры до масштабирования под окно или экран. Более высокое улучшает "
                             "детализацию, но требует больше ресурсов GPU и видеопамяти. "
                             "Ультраширокий режим добавляет варианты 21:9 и 32:9.",
    [UI_MENU_HUD_POSITION] = "Положение интерфейса",
    [UI_MENU_HUD_POSITION_DESC] = "Расположить угловые индикаторы у краёв экрана или в центральной области 16:9. "
                                 "Меню и прицел остаются в центре.",
    [UI_MENU_HUD_EDGES] = "У краёв экрана", [UI_MENU_HUD_CENTERED] = "По центру (16:9)",
    [UI_MENU_WIDE_DISABLED] = "Сначала включите ультраширокий экран",
    [UI_MENU_OUTPUT_RES_DESC] = "Размер, в котором игра рисует картинку и интерфейс. Выше 1920 × 1080 игре нужно больше "
                                "видеопамяти и времени GPU. Более широкий размер (21:9) расширяет обзор камеры и "
                                "переносит интерфейс в углы экрана, более высокий (16:10, Steam Deck) переносит его к "
                                "верхнему и нижнему краям.",
    [UI_MENU_RENDER_SCALE] = "Масштаб 3D-отрисовки",
    [UI_MENU_RENDER_SCALE_DESC] = "Размер 3D-сцены относительно разрешения игры; FSR игры доводит её до этого размера. "
                                  "Выше 100 % сглаживает лесенки на краях (суперсэмплинг) ценой большой нагрузки на GPU.",
    [UI_MENU_RENDER_SCALE_GAME] = "Как в игре (83 %)",
    [UI_MENU_SCENE_RES] = "Разрешение 3D-сцены",
    [UI_MENU_SCENE_RES_DESC] = "Авто: игра сама меняет размер 3D-сцены по нагрузке, обычно до двух третей разрешения рендеринга "
                               "по каждой стороне, и затем растягивает её. Полное: 3D-сцена рисуется в полном разрешении "
                               "рендеринга, картинка чётче, но нагрузка на GPU выше; апскейлер и DLSS не работают. Действует после перезапуска.",
    [UI_MENU_SCENE_RES_AUTO] = "Авто (игра)", [UI_MENU_SCENE_RES_FULL] = "Полное",
    [UI_MENU_UPSCALER] = "Масштабирование под окно",
    [UI_MENU_UPSCALER_DESC] = "Этот параметр действует, когда изображение игры меньше области вывода на экране или в окне. "
                              "AMD FSR 1 может сохранять края более чёткими, чем билинейное масштабирование.",
    [UI_MENU_UPSCALER_FSR] = "AMD FSR 1", [UI_MENU_UPSCALER_BILINEAR] = "Билинейное", [UI_MENU_UPSCALER_DLSS] = "NVIDIA DLSS",
    [UI_MENU_UPSCALER_FSR3] = "AMD FSR 3.1",
    [UI_MENU_ASPECT] = "Масштабирование вывода",
    [UI_MENU_ASPECT_DESC] = "По умолчанию пропорции игры сохраняются, при необходимости появляются чёрные полосы.\n"
                            "В режиме «Растянуть» изображение заполняет всё окно или экран. Оно растягивается, только "
                            "если пропорции изображения и окна или экрана различаются.",
    [UI_MENU_ASPECT_FIT] = "По умолчанию", [UI_MENU_ASPECT_STRETCH] = "Растянуть",
    [UI_MENU_FOV] = "Поле зрения",
    [UI_MENU_FOV_DESC] = "Меняет угол обзора камеры в процентах от исходного. При 100 % используется исходный угол.",
    [UI_MENU_SHARPNESS] = "Резкость FSR",
    [UI_MENU_SHARPNESS_DESC] = "Повышение резкости после масштабирования FSR; 0 выключает его.",
    [UI_MENU_NO_UPSCALING] = "Нужно меньшее разрешение игры",
    [UI_MENU_SHARPNESS_NEEDS_FSR] = "Требуется AMD FSR 1",
    [UI_MENU_SMAA] = "Сглаживание (SMAA)",
    [UI_MENU_SMAA_DESC] = "Сглаживает лесенки на готовой картинке после масштабирования под окно и до отрисовки этого "
                          "меню. Интерфейс игры тоже сглаживается. Чем выше качество, тем больше нагрузка на GPU.",
    [UI_MENU_SMAA_LOW] = "Низкое", [UI_MENU_SMAA_MEDIUM] = "Среднее", [UI_MENU_SMAA_HIGH] = "Высокое",
    [UI_MENU_COLOR] = "Цвет",
    [UI_MENU_COLOR_PRESET] = "Цветовой профиль",
    [UI_MENU_COLOR_PRESET_DESC] = "Задаёт четыре значения ниже. «Яркий»: больше контраста и насыщенности, тени немного "
                                  "светлее (для OLED-экранов). «Чёткость»: немного больше контраста, насыщенности и "
                                  "яркости. Изменение любого значения делает профиль «Своим». Интерфейс игры меняется "
                                  "вместе с картинкой, это меню — нет.",
    [UI_MENU_COLOR_DEFAULT] = "Как в игре", [UI_MENU_COLOR_VIVID] = "Яркий (OLED)", [UI_MENU_COLOR_CLARITY] = "Чёткость",
    [UI_MENU_COLOR_CUSTOM] = "Свой",
    [UI_MENU_COLOR_BRIGHTNESS] = "Яркость", [UI_MENU_COLOR_BRIGHTNESS_DESC] = "Прибавляется к каждому цвету; 0 — без изменений.",
    [UI_MENU_COLOR_CONTRAST] = "Контраст",
    [UI_MENU_COLOR_CONTRAST_DESC] = "Отдаляет цвета от среднего серого (выше 100 %) или приближает к нему; 100 % — без "
                                    "изменений.",
    [UI_MENU_COLOR_SATURATION] = "Насыщенность",
    [UI_MENU_COLOR_SATURATION_DESC] = "Сила цвета относительно серого; 100 % — без изменений.",
    [UI_MENU_COLOR_GAMMA] = "Гамма",
    [UI_MENU_COLOR_GAMMA_DESC] = "Ниже 1 осветляет тёмные и средние тона, выше 1 затемняет их; 1 — без изменений.",
    [UI_MENU_HDR_GROUP] = "HDR (экспериментально)",
    [UI_MENU_HDR] = "Вывод HDR",
    [UI_MENU_HDR_DESC] = "Делает светлые участки SDR-изображения ярче на HDR-дисплее.\n\n"
                         "Сначала включите HDR в настройках экрана системы.",
    [UI_MENU_HDR_NATIVE] = "Собственный HDR игры",
    [UI_MENU_HDR_NATIVE_DESC] = "Включает собственный HDR игры в версии 3.2.0. Яркость бликов ограничена пиковой яркостью и 1400 нит. "
                                "В этом режиме отключены FSR 1, SMAA и цветокоррекция.",
    [UI_MENU_HDR_PEAK] = "Пиковая яркость",
    [UI_MENU_HDR_PEAK_DESC] = "Пик вашего дисплея в нитах; самые яркие блики останавливаются на нём.",
    [UI_MENU_HDR_GAME] = "Яркость игры",
    [UI_MENU_HDR_GAME_DESC] = "Ниты белого цвета картинки (paper white). 203 — эталон HDR; в светлой комнате "
                              "поднимите.",
    [UI_MENU_HDR_UI] = "Яркость меню",
    [UI_MENU_HDR_UI_DESC] = "Ниты белого цвета этого меню. Интерфейс игры — часть картинки и следует яркости игры.",

    [UI_MENU_FRAME_RATE_GROUP] = "Частота кадров",
    [UI_MENU_FPS_LIMIT] = "Ограничение частоты кадров",
    [UI_MENU_DKB_FPS_LIMIT_DESC] = "Ограничивает частоту кадров игры. Вариант «Частота экрана» использует текущую частоту экрана.",
    [UI_MENU_FPS_LIMIT_DESC] = "Наибольшее число кадров в секунду. Выше 60 — экспериментальный режим, нужна «Скорость "
                               "игры по часам». «Без ограничения» — экспериментально.",
    [UI_MENU_FPS_30] = "30 FPS", [UI_MENU_FPS_40] = "40 FPS", [UI_MENU_FPS_45] = "45 FPS",
    [UI_MENU_FPS_60] = "60 FPS", [UI_MENU_FPS_90] = "90 FPS", [UI_MENU_FPS_120] = "120 FPS", [UI_MENU_FPS_144] = "144 FPS",
    [UI_MENU_FPS_DISPLAY] = "Частота экрана", [UI_MENU_FPS_DISPLAY_HZ] = "Частота экрана (%.0f Гц)",
    [UI_MENU_FPS_UNLIMITED] = "Без ограничения",
    [UI_MENU_FPS_NEEDS_REALTIME] = "Разрешите высокий FPS и перезапустите игру",
    [UI_MENU_REALTIME] = "Скорость игры по часам",
    [UI_MENU_REALTIME_DESC] = "Игра идёт по реальному времени, медленный кадр её не замедляет.",
    [UI_MENU_DKB_REALTIME] = "Разрешить высокий FPS",
    [UI_MENU_DKB_REALTIME_DESC] = "Позволяет выбрать лимит кадров, в том числе выше 60 FPS. "
                                "Игровое время подстраивается под частоту кадров. Режим экспериментальный.",
    [UI_MENU_CLOTH_REALTIME] = "Ткань по часам",
    [UI_MENU_CLOTH_REALTIME_DESC] = "Ткань и волосы тоже идут по реальному времени.",
    [UI_MENU_HIGHFPS_FIX] = "Исправления для высокого FPS",
    [UI_MENU_HIGHFPS_FIX_DESC] = "Исправляет физику, которая ломается при частоте выше 30 кадров в секунду, и объекты, которые выше 60 остаются полупрозрачными или невидимыми.",
    [UI_MENU_GAME_LANG] = "Язык игры", [UI_MENU_GAME_LANG_DESC] = "Язык текста и озвучки игры.",
    [UI_MENU_GAME_LANG_AUTO] = "Автоматически",
    [UI_MENU_OPERATION_MODE] = "Режим консоли",
    [UI_MENU_OPERATION_MODE_DESC] = "Игра считает, что консоль в док-станции или в портативном режиме.",
    [UI_MENU_MODE_DOCKED] = "В док-станции", [UI_MENU_MODE_HANDHELD] = "Портативный",
    [UI_MENU_ASSUME_FOCUS] = "Работать без фокуса",
    [UI_MENU_ASSUME_FOCUS_DESC] = "Включено (по умолчанию): игра идёт, пока вы в другом окне, а клавиатура не "
                                  "действует, пока окно не вернётся на передний план. Выключено: игра встаёт на паузу, "
                                  "когда окно теряет фокус, как на Switch при нажатии Home; прерванная так загрузка "
                                  "может остановиться насовсем.",

    [UI_MENU_VOLUME] = "Громкость", [UI_MENU_VOLUME_DESC] = "Общая громкость игры.",
    [UI_MENU_AUDIO_CHANNELS] = "Вывод звука",
    [UI_MENU_AUDIO_CHANNELS_DESC] = "Стерео или объёмный звук 5.1. Автоматически выбирается 5.1, если у устройства "
                                    "вывода по умолчанию шесть каналов или больше.",
    [UI_MENU_CHANNELS_AUTO] = "Автоматически", [UI_MENU_CHANNELS_STEREO] = "Стерео", [UI_MENU_CHANNELS_51] = "Объёмный 5.1",

    [UI_MENU_CONTROLLER] = "Контроллер", [UI_MENU_MOUSE_KEYBOARD] = "Мышь и клавиатура",
    [UI_MENU_MOTION] = "Управление движением", [UI_MENU_MOTION_DESC] = "Прицеливание гироскопом контроллера.",
    [UI_MENU_RUMBLE_STRENGTH] = "Сила вибрации",
    [UI_MENU_RUMBLE_STRENGTH_DESC] = "Насколько сильно вибрирует контроллер.",
    [UI_MENU_RUMBLE_CURVE] = "Кривая вибрации",
    [UI_MENU_RUMBLE_CURVE_DESC] = "Чем ниже значение, тем сильнее слабая вибрация.",
    [UI_MENU_RUMBLE_TRIGGERS] = "Вибрация триггеров (Xbox)",
    [UI_MENU_RUMBLE_TRIGGERS_DESC] = "Вибрация и в триггерах контроллера Xbox.",
    [UI_MENU_PAD_LAYOUT] = "Раскладка кнопок",
    [UI_MENU_PAD_LAYOUT_DESC] = "Какая кнопка работает как A. Nintendo: правая, как на Switch. Xbox: нижняя, как "
                                "подписано на контроллере Xbox (крест на контроллере PlayStation). Автоматически: "
                                "Nintendo для контроллеров с подписями Nintendo, Xbox для остальных. Подсказки Xbox: "
                                "A справа, как в Nintendo, а игра показывает надписи Xbox (B вместо A, A вместо B, Y "
                                "вместо X, X вместо Y); подсказки меняются после перезапуска. Буквы записаны в файлах "
                                 "игры, поэтому у кнопки, переназначенной на вкладке управления, на экране остаётся "
                                 "прежняя буква.",
    [UI_MENU_PAD_LAYOUT_AUTO] = "Автоматически", [UI_MENU_PAD_LAYOUT_NINTENDO] = "Nintendo (A справа)",
    [UI_MENU_PAD_LAYOUT_XBOX] = "Xbox (A снизу)", [UI_MENU_PAD_LAYOUT_XBOX_PROMPTS] = "Подсказки Xbox (A справа)",
    [UI_MENU_PAD_LAYOUT_IN_USE] = "сейчас: %s",
    [UI_MENU_PAD_LAYOUT_PROMPTS_MISSING] = "нет файлов подсказок: запустите установку снова",
    [UI_MENU_PAD_LAYOUT_PROMPTS_RESTART] = "подсказки сменятся после перезапуска",
    [UI_MENU_STICK_DEADZONE] = "Мёртвая зона стиков",
    [UI_MENU_STICK_DEADZONE_DESC] = "Насколько нужно отклонить стик геймпада, чтобы игра это заметила. Увеличьте, если персонаж или камера двигаются сами.",
    [UI_MENU_MOUSE_CAMERA] = "Прямое управление камерой мышью",
    [UI_MENU_MOUSE_CAMERA_DESC] = "Движение мыши напрямую поворачивает камеру слежения. "
                                 "В других режимах камеры мышь имитирует правый стик.",
    [UI_MENU_MOUSE_SENS] = "Чувствительность мыши",
    [UI_MENU_MOUSE_SENS_DESC] = "Как быстро мышь поворачивает камеру.",

    [UI_BINDS_HELP_KEYS] = "Щёлкните назначение или нажмите Enter, затем нажмите клавишу или кнопку мыши. Delete или Backspace очищает ячейку. "
                           "Занятая клавиша переходит к новому действию.",
    [UI_BINDS_HELP_PAD] = "Нажмите A на назначении, затем нажмите кнопку контроллера. X очищает ячейку. Занятые кнопки "
                          "меняются местами. Левый стик отвечает за движение, правый — за камеру.",
    [UI_BINDS_COL_KEY1] = "Основное", [UI_BINDS_COL_KEY2] = "Дополнительное",
    [UI_BINDS_GROUP_BUTTONS] = "Кнопки", [UI_BINDS_GROUP_MOVE] = "Движение", [UI_BINDS_GROUP_CAMERA] = "Камера",
    [UI_BINDS_PRESS_KEY] = "Нажмите клавишу или кнопку мыши для «%s»",
    [UI_BINDS_PRESS_PAD] = "Нажмите кнопку контроллера для «%s»",
    [UI_BINDS_CANCEL_HINT] = "Esc отменяет, или подождите %d с",
    [UI_BINDS_TAKEN] = "%s: убрано с «%s»", [UI_BINDS_SWAPPED] = "«%s» и «%s» поменялись местами",
    [UI_BINDS_RESET] = "Сбросить все привязки",
    [UI_BINDS_RESET_DESC] = "Вернуть клавиши и кнопки контроллера по умолчанию. Снова действует раскладка кнопок выше: "
                            "переназначенные кнопки контроллера остаются на своих местах до "
                            "этого сброса.",
    [UI_BIND_A] = "A", [UI_BIND_B] = "B", [UI_BIND_X] = "X", [UI_BIND_Y] = "Y",
    [UI_BIND_LCLICK] = "Нажатие левого стика", [UI_BIND_RCLICK] = "Нажатие правого стика", [UI_BIND_L] = "L",
    [UI_BIND_R] = "R", [UI_BIND_ZL] = "ZL", [UI_BIND_ZR] = "ZR", [UI_BIND_PLUS] = "Плюс", [UI_BIND_MINUS] = "Минус",
    [UI_BIND_LEFT] = "Крестовина влево", [UI_BIND_UP] = "Крестовина вверх", [UI_BIND_RIGHT] = "Крестовина вправо",
    [UI_BIND_DOWN] = "Крестовина вниз",
    [UI_BIND_LSTICK_LEFT] = "Влево", [UI_BIND_LSTICK_UP] = "Вперёд", [UI_BIND_LSTICK_RIGHT] = "Вправо",
    [UI_BIND_LSTICK_DOWN] = "Назад",
    [UI_BIND_RSTICK_LEFT] = "Камера влево", [UI_BIND_RSTICK_UP] = "Камера вверх",
    [UI_BIND_RSTICK_RIGHT] = "Камера вправо", [UI_BIND_RSTICK_DOWN] = "Камера вниз",
    [UI_BIND_DESC_A] = "Взаимодействие, подобрать, поговорить.", [UI_BIND_DESC_B] = "Бег, отмена, назад.",
    [UI_BIND_DESC_X] = "Прыжок.", [UI_BIND_DESC_Y] = "Атака.", [UI_BIND_DESC_LCLICK] = "Присесть.",
    [UI_BIND_DESC_RCLICK] = "Вид от первого лица.", [UI_BIND_DESC_L] = "Способности.",
    [UI_BIND_DESC_R] = "Бросок оружия.", [UI_BIND_DESC_ZL] = "Щит, захват цели.", [UI_BIND_DESC_ZR] = "Лук.",
    [UI_BIND_DESC_PLUS] = "Меню, инвентарь.", [UI_BIND_DESC_MINUS] = "Карта.",
    [UI_BIND_DESC_DPAD] = "Быстрый выбор.",
    [UI_BIND_DESC_MOVE] = "Двигает Линка, как левый стик. Левый стик контроллера тоже всегда работает.",
    [UI_BIND_DESC_CAMERA] = "Поворачивает камеру, как правый стик. Мышь и правый стик контроллера тоже всегда "
                            "работают.",
    [UI_DKB_BIND_A] = "Прыжок. Подтверждение в меню.",
    [UI_DKB_BIND_B] = "Удар вниз. Возврат в меню.",
    [UI_DKB_BIND_X] = "Удар вверх.",
    [UI_DKB_BIND_Y] = "Удар вперёд или взмах куском породы в руках.",
    [UI_DKB_BIND_LCLICK] = "Действие, назначенное нажатию левого стика в настройках Bananza.",
    [UI_DKB_BIND_RCLICK] = "Действие, назначенное нажатию правого стика в настройках Bananza.",
    [UI_DKB_BIND_L] = "Удерживайте, чтобы свистом указать путь к цели.",
    [UI_DKB_BIND_R] = "Хлопок по земле собирает предметы поблизости и помогает найти скрытые объекты.",
    [UI_DKB_BIND_ZL] = "Кувырок вперёд или скольжение на куске породы в руках.",
    [UI_DKB_BIND_ZR] = "Взять кусок породы или бросить тот, что уже в руках.",
    [UI_DKB_BIND_PLUS] = "Открыть меню паузы.",
    [UI_DKB_BIND_MINUS] = "Открыть карту.",
    [UI_DKB_BIND_LEFT] = "Выбрать предыдущую открытую форму Bananza.",
    [UI_DKB_BIND_UP] = "Открыть меню навыков.",
    [UI_DKB_BIND_RIGHT] = "Выбрать следующую открытую форму Bananza.",
    [UI_DKB_BIND_DOWN] = "Открыть фоторежим, если он доступен.",
    [UI_DKB_BIND_MOVE] = "Двигать Донки Конга в этом направлении.",
    [UI_DKB_BIND_CAMERA] = "Повернуть камеру в этом направлении.",
    [UI_KEY_SPACE] = "Пробел", [UI_KEY_TAB] = "Tab", [UI_KEY_LSHIFT] = "Левый Shift", [UI_KEY_LCTRL] = "Левый Ctrl",
    [UI_KEY_UP] = "Стрелка вверх", [UI_KEY_DOWN] = "Стрелка вниз", [UI_KEY_LEFT] = "Стрелка влево",
    [UI_KEY_RIGHT] = "Стрелка вправо",
    [UI_KEY_MOUSE_L] = "ЛКМ", [UI_KEY_MOUSE_R] = "ПКМ", [UI_KEY_MOUSE_M] = "СКМ", [UI_KEY_MOUSE_X1] = "Мышь 4",
    [UI_KEY_MOUSE_X2] = "Мышь 5",
    [UI_KEY_ENTER] = "Enter", [UI_KEY_BACKSPACE] = "Backspace", [UI_KEY_RSHIFT] = "Правый Shift",
    [UI_KEY_RCTRL] = "Правый Ctrl", [UI_KEY_LALT] = "Alt", [UI_KEY_CAPSLOCK] = "Caps Lock", [UI_KEY_INSERT] = "Insert",
    [UI_KEY_DELETE] = "Delete", [UI_KEY_HOME] = "Home", [UI_KEY_END] = "End", [UI_KEY_PAGEUP] = "Page Up",
    [UI_KEY_PAGEDOWN] = "Page Down", [UI_KEY_NUM_FMT] = "Num %s",
    [UI_PAD_SOUTH] = "Нижняя кнопка", [UI_PAD_EAST] = "Правая кнопка", [UI_PAD_WEST] = "Левая кнопка",
    [UI_PAD_NORTH] = "Верхняя кнопка", [UI_PAD_LSTICK] = "Левый стик", [UI_PAD_RSTICK] = "Правый стик",
    [UI_PAD_L] = "Левый бампер", [UI_PAD_R] = "Правый бампер", [UI_PAD_START] = "Start", [UI_PAD_BACK] = "Back",
    [UI_PAD_DPAD_LEFT] = "Крестовина влево", [UI_PAD_DPAD_UP] = "Крестовина вверх",
    [UI_PAD_DPAD_RIGHT] = "Крестовина вправо", [UI_PAD_DPAD_DOWN] = "Крестовина вниз", [UI_PAD_ZL] = "Левый курок",
    [UI_PAD_ZR] = "Правый курок",
    [UI_PAD_LPADDLE1] = "Левая задняя кнопка", [UI_PAD_RPADDLE1] = "Правая задняя кнопка",
    [UI_PAD_LPADDLE2] = "Левая задняя кнопка 2", [UI_PAD_RPADDLE2] = "Правая задняя кнопка 2", [UI_PAD_TOUCHPAD] = "Сенсорная панель",
    [UI_PAD_CROSS] = "Крест", [UI_PAD_CIRCLE] = "Круг", [UI_PAD_SQUARE] = "Квадрат", [UI_PAD_TRIANGLE] = "Треугольник",

    [UI_MENU_MENU_LANG] = "Язык меню", [UI_MENU_MENU_LANG_DESC] = "Язык этого меню и индикатора.",
    [UI_MENU_MENU_LANG_AUTO] = "Как в системе",
    [UI_MENU_THEME] = "Тема меню", [UI_MENU_THEME_DK] = "Donkey Kong", [UI_MENU_THEME_CLASSIC] = "Классическая",
    [UI_MENU_THEME_DESC] = "Вид этого меню: Donkey Kong (кора и банановый жёлтый) или классический тёмно-зелёный с золотом.",

    [UI_MENU_ADV_WARNING] = "Эти настройки нужны для тестов и поиска неполадок. Меняйте их, только если знаете зачем.",
    [UI_MENU_GROUP_SCHED] = "Планирование", [UI_MENU_GROUP_SHADERS] = "Шейдеры и конвейеры",
    [UI_MENU_GROUP_TEXMEM] = "Текстуры и память", [UI_MENU_GROUP_FIXES] = "Исправления графики",
    [UI_MENU_GROUP_VULKAN] = "Vulkan", [UI_MENU_GROUP_DIAG] = "Диагностика",
    [UI_MENU_ASYNC_PREEMPT] = "Вытеснение на виртуальных ядрах",
    [UI_MENU_ASYNC_PREEMPT_DESC] = "Позволяет потоку игры прервать другой поток на том же виртуальном ядре.",
    [UI_MENU_PREEMPT_OFF] = "Выкл.", [UI_MENU_PREEMPT_ON] = "Вкл.", [UI_MENU_PREEMPT_INHERIT] = "Только наследование",
    [UI_MENU_REPLAY_ASYNC] = "Подготовка кадра для GPU в отдельном потоке",
    [UI_MENU_REPLAY_ASYNC_DESC] = "Кадр для GPU готовится в отдельном потоке, а не в потоке игры. Добавляет до одного кадра задержки.",
    [UI_MENU_REPLAY_THREADS] = "Потоки записи больших проходов",
    [UI_MENU_REPLAY_THREADS_DESC] = "Отрисовки больших проходов записываются сразу в столько потоков. «Автоматически» — 6 потоков при 12 и более логических процессорах и 4 при 8–11 (кроме AMD); «Выкл.» — в одном потоке. Быстрее в насыщенных местах на процессорах с большим числом ядер.",
    [UI_MENU_TIMER_1MS] = "Таймер Windows 1 мс",
    [UI_MENU_TIMER_1MS_DESC] = "Повышает точность системного таймера до 1 мс для ровного темпа кадров.",
    [UI_MENU_PIPE_WAIT] = "Обработка ожидающих отрисовок",
    [UI_MENU_PIPE_WAIT_DESC] = "«По умолчанию» — текущее планирование с отдельным потоком для ожидающих отрисовок; «Старый способ» — прежнее планирование.",
    [UI_MENU_PIPE_WAIT_DEFAULT] = "По умолчанию", [UI_MENU_PIPE_WAIT_DRAW] = "Старый способ",
    [UI_MENU_PIPE_SKIP] = "Пропуск отрисовок при сборке конвейера",
    [UI_MENU_PIPE_SKIP_DESC] = "Какие отрисовки отбрасываются, а не ждут свой конвейер.",
    [UI_MENU_PIPE_SKIP_STRICT] = "Строго", [UI_MENU_PIPE_SKIP_ANY] = "Любые",
    [UI_MENU_PIPE_SKIP_FULLSCREEN] = "Полноэкранные проходы",
    [UI_MENU_LIB_PREBUILD] = "Предсборка библиотек стадий",
    [UI_MENU_LIB_PREBUILD_DESC] = "Заранее собирает библиотеки стадий шейдеров в фоне.",
    [UI_MENU_LIB_PREBUILD_THREADS] = "Потоки предсборки библиотек",
    [UI_MENU_LIB_PREBUILD_THREADS_DESC] = "Сколько потоков занято предсборкой; «Автоматически» — по числу ядер процессора.",
    [UI_MENU_PIPE_PREBUILD] = "Предсборка конвейеров",
    [UI_MENU_PIPE_PREBUILD_DESC] = "Заранее собирает известные конвейеры.",
    [UI_MENU_PREWARM_XLATE] = "Прогрев транслятора",
    [UI_MENU_PREWARM_XLATE_DESC] = "Потоки, которые при запуске переводят известные шейдеры; «Автоматически» — половина потоков процессора, не больше 16.",
    [UI_MENU_GPL] = "Библиотека графических конвейеров",
    [UI_MENU_GPL_DESC] = "Использует расширение Vulkan для библиотек конвейеров; выключите, если драйвер с ним сбоит.",
    [UI_MENU_ASTC] = "Распаковка текстур ASTC", [UI_MENU_ASTC_DESC] = "Где распаковываются сжатые текстуры ASTC.",
    [UI_MENU_ASTC_AUTO] = "Автоматически", [UI_MENU_ASTC_GPU] = "Шейдер GPU", [UI_MENU_ASTC_NATIVE] = "Аппаратно",
    [UI_MENU_ASTC_CPU] = "Процессор",
    [UI_MENU_MEMORY] = "Режим памяти GPU", [UI_MENU_MEMORY_DESC] = "Как память игры передаётся видеокарте.",
    [UI_MENU_MEMORY_AUTO] = "Автоматически", [UI_MENU_MEMORY_IMPORT] = "Импорт", [UI_MENU_MEMORY_MIRROR] = "Зеркало",
    [UI_MENU_MEMORY_VERIFY] = "Проверка",
    [UI_MENU_TEX_ALIAS] = "Наследование содержимого текстур", [UI_MENU_TEX_ALIAS_DESC] = "Новая текстура поверх памяти живой текстуры получает её содержимое, а не пустую.",
    [UI_MENU_TEX_VRAM_PRESSURE] = "Предел памяти текстур", [UI_MENU_TEX_VRAM_PRESSURE_DESC] = "Процент видеопамяти, выше которого освобождённые игрой текстуры удаляются (при повторном использовании загружаются снова). 0 — не удалять.",
    [UI_MENU_ASTC_BC] = "Сжатие ASTC в BC3", [UI_MENU_ASTC_BC_DESC] = "С декодером на GPU: декодированные текстуры ASTC пересжимаются в BC3 и занимают вчетверо меньше видеопамяти. Для видеокарт с 4 ГБ и меньше; текстуры чуть более блочные.",
    [UI_MENU_LOD_GUARD] = "Защита LOD", [UI_MENU_LOD_GUARD_DESC] = "Не даёт объектам мелькать не на своём месте, а дальнему плану — на кадр становиться плоским: ждёт, пока игра допишет данные уровней детализации, и только потом их читает GPU.",
    [UI_MENU_FIX_C0] = "Исправление константного буфера",
    [UI_MENU_FIX_C0_DESC] = "Заполняет зарезервированный драйвером банк констант c0 (размеры хранилищ, размер запуска, размер точки, базовый экземпляр) вместо чтения нулей.",
    [UI_MENU_FIX_ZERO] = "Обнулять новую память GPU",
    [UI_MENU_FIX_ZERO_DESC] = "Новые пулы памяти GPU заполняются нулями, как память Switch.",
    [UI_MENU_LIST_RESTART] = "Перезапуск примитивов в списках",
    [UI_MENU_LIST_RESTART_DESC] = "Применяет перезапуск примитивов игры и к спискам треугольников, как на Switch. Без него индексы перезапуска рисуются как вершины: длинные вытянутые треугольники, например на карте. Нужна видеокарта с primitiveTopologyListRestart.",
    [UI_MENU_BIND_SKIP] = "Пропуск повторных привязок",
    [UI_MENU_BIND_SKIP_DESC] = "Оптимизация: пропускает неизменившиеся привязки.",
    [UI_MENU_SHADER_MEMO] = "Память шейдеров", [UI_MENU_SHADER_MEMO_DESC] = "Оптимизация: запоминает поиск шейдеров.",
    [UI_MENU_PIPE_MEMO] = "Память конвейера", [UI_MENU_PIPE_MEMO_DESC] = "Оптимизация: повторяет конвейер прошлой отрисовки, если её состояние не изменилось.",
    [UI_MENU_TEX_MEMO] = "Память привязок текстур", [UI_MENU_TEX_MEMO_DESC] = "Оптимизация: запоминает дескрипторы текстур между отрисовками.",
    [UI_MENU_DESC_TEMPLATE] = "Шаблоны дескрипторов",
    [UI_MENU_DESC_TEMPLATE_DESC] = "Оптимизация: обновляет дескрипторы пачками.",
    [UI_MENU_PUSH_SKIP] = "Пропуск повторных push-констант",
    [UI_MENU_PUSH_SKIP_DESC] = "Оптимизация: пропускает неизменившиеся push-константы.",
    [UI_MENU_SPLIT_DRAWS] = "Ранняя отправка работы GPU",
    [UI_MENU_SPLIT_DRAWS_DESC] = "Отправляет работу GPU частями через каждые N отрисовок между проходами, чтобы GPU начинал раньше; 0 — выключено.",
    [UI_MENU_VK_VALIDATION] = "Слой проверки",
    [UI_MENU_VK_VALIDATION_DESC] = "Проверяет каждый вызов Vulkan и сильно замедляет игру.",
    [UI_MENU_VK_VALIDATION_MISSING] = "Слой проверки не установлен.",
    [UI_MENU_VK_DYN_VERTEX] = "Динамическое состояние вершин",
    [UI_MENU_VK_DYN_VERTEX_DESC] = "Принудительно включает или выключает динамический ввод вершин; по умолчанию зависит от GPU.",
    [UI_MENU_VK_DYN_A2C] = "Динамический alpha to coverage",
    [UI_MENU_VK_DYN_A2C_DESC] = "Принудительно включает или выключает динамический alpha to coverage. По умолчанию "
                                "выключен на драйвере AMD для RDNA 2 и старше, где с ним персонажи чёрные, и включён "
                                "на остальных.",
    [UI_MENU_VK_SUBGROUP_PIN] = "Фиксировать размер подгруппы",
    [UI_MENU_VK_SUBGROUP_PIN_DESC] = "Фиксирует размер подгруппы шейдера.",
    [UI_MENU_TRACE] = "Журнал производительности", [UI_MENU_TRACE_DESC] = "Пишет время кадров в файл.",
    [UI_MENU_FLIGHT] = "Запись последних кадров (для отчёта о фризе)",
    [UI_MENU_FLIGHT_DESC] = "Хранит короткую историю и сохраняет её, когда кадр подвисает.",
    [UI_MENU_FLIGHT_SPIKE_MS] = "Порог подвисания (мс)",
    [UI_MENU_FLIGHT_SPIKE_MS_DESC] = "Время кадра, которое считается подвисанием.",
    [UI_MENU_INPUT_RECORD] = "Запись ввода",
    [UI_MENU_INPUT_RECORD_DESC] = "Пишет ваш ввод в файл для отчёта об ошибке.",
    [UI_MENU_OPEN_LOG] = "Открыть папку журналов", [UI_MENU_OPEN_LOG_DESC] = "Показывает папку с файлами журнала.",
    [UI_MENU_COPY_INFO] = "Копировать сведения о системе",
    [UI_MENU_COPY_INFO_DESC] = "Копирует в буфер обмена видеокарту, драйвер, процессор, память и изменённые настройки.",
    [UI_MENU_COPY_INFO_DONE] = "Скопировано", [UI_MENU_ACTION_FAILED] = "не получилось",
    [UI_MENU_RESET] = "Сбросить все настройки", [UI_MENU_RESET_DESC] = "Действует на все вкладки.",
    [UI_MENU_RESET_CONFIRM] = "Сбросить все настройки к значениям по умолчанию?", [UI_MENU_RESET_YES] = "Сбросить",
    [UI_MENU_RESET_NO] = "Отмена",

    [UI_MENU_OV_FPS] = "%.0f FPS", [UI_MENU_OV_FRAME_MS] = "%.1f мс", [UI_MENU_OV_GAME_PCT] = "игра %.0f%%",
    [UI_MENU_OV_MAX_MS] = "макс. %.1f мс",
    [UI_MENU_OV_GPU] = "GPU", [UI_MENU_OV_CPU] = "ЦП", [UI_MENU_OV_RAM] = "ОЗУ", [UI_MENU_OV_VRAM] = "Видеопамять",
    [UI_MENU_OV_TEMP] = "Температура", [UI_MENU_OV_POWER] = "Мощность", [UI_MENU_OV_CLOCK] = "Частота",
    [UI_MENU_OV_NONE] = "-",
    [UI_MENU_OV_REPLAY] = "Подготовка кадра для GPU", [UI_MENU_OV_GPU_TIME] = "Занятость GPU за кадр",
    [UI_MENU_OV_PIPE_WAIT] = "Ожидания конвейеров",
    [UI_MENU_OV_SHADER_WAIT] = "Ожидания шейдеров", [UI_MENU_OV_TEX_UPLOADS] = "Загрузки текстур",
    [UI_MENU_OV_FMT_PCT] = "%.0f%%", [UI_MENU_OV_FMT_TEMP] = "%.0f °C", [UI_MENU_OV_FMT_POWER] = "%.0f Вт",
    [UI_MENU_OV_FMT_MHZ] = "%.0f МГц", [UI_MENU_OV_FMT_GB] = "%.1f ГБ", [UI_MENU_OV_FMT_GB_OF] = "/ %.1f ГБ",
    [UI_MENU_OV_GAME_GB] = "игра %.1f ГБ", [UI_MENU_OV_FMT_MS] = "%.2f мс", [UI_MENU_OV_PER_FRAME] = "%.1f за кадр",
    [UI_MENU_OV_WINDOW] = "Окно", [UI_MENU_OV_OUTPUT] = "Изображение игры", [UI_MENU_OV_SCENE] = "3D-сцена",
    [UI_MENU_OV_SCALING] = "Масштабирование", [UI_MENU_OV_LIMIT] = "Лимит кадров", [UI_MENU_OV_PRESENT] = "Вывод кадров",
    [UI_MENU_OV_FMT_SIZE] = "%ux%u", [UI_MENU_OV_FMT_SCENE] = "%ux%u, %.0f%%",
    [UI_MENU_OV_FMT_UPSCALE] = "%ux%u -> %ux%u",
    [UI_MENU_OV_NO_UPSCALER] = "без апскейлера",
    [UI_MENU_OV_FMT_FSR] = "AMD FSR 1, резкость %d%%", [UI_MENU_OV_SCALE_NONE] = "нет (1:1)",
    [UI_MENU_OV_SCALE_BILINEAR] = "билинейное",
};

#define SWKBD_ONLY(name, ...) static const char *const name[UI_TEXT_COUNT] = {SWKBD(__VA_ARGS__)}
SWKBD_ONLY(text_ja, "文字入力", "名前を入力してください", "ウマ", "OK", "キャンセル", "コントローラー: A = OK、B = キャンセル");
SWKBD_ONLY(text_fr, "Saisie de texte", "Entrez un nom", "Cheval", "OK", "Annuler", "Manette : A = OK, B = Annuler");
SWKBD_ONLY(text_de, "Texteingabe", "Gib einen Namen ein", "Pferd", "OK", "Abbrechen", "Controller: A = OK, B = Abbrechen");
SWKBD_ONLY(text_it, "Inserimento testo", "Inserisci un nome", "Cavallo", "OK", "Annulla", "Controller: A = OK, B = Annulla");
SWKBD_ONLY(text_es, "Introducir texto", "Introduce un nombre", "Caballo", "Aceptar", "Cancelar",
           "Mando: A = Aceptar, B = Cancelar");
SWKBD_ONLY(text_nl, "Tekstinvoer", "Voer een naam in", "Paard", "OK", "Annuleren", "Controller: A = OK, B = Annuleren");
SWKBD_ONLY(text_pt, "Inserir texto", "Introduz um nome", "Cavalo", "OK", "Cancelar", "Comando: A = OK, B = Cancelar");
// Simplified Chinese carries the whole menu; Traditional Chinese keeps the software keyboard only (a row with no menu
// text falls back to English).
static const char *const text_zh_hans[UI_TEXT_COUNT] = {
    [UI_MENU_PAD_DEVICE] = "手柄设备",
    [UI_MENU_PAD_DEVICE_DESC] = "选择使用哪个手柄。改动立即生效。\n\n"
        "新接入的手柄会自动接管。当前手柄断开时，另一只可用的手柄会接管。\n\n"
        "手动选择立即生效，并在下次启动时沿用。“自动”从第一个可用的手柄开始，之后接入的手柄仍可接管。\n\n"
        "键盘和鼠标始终可用。",
    [UI_MENU_PAD_FULLSCREEN_HOLD] = "长按全屏快捷键",
    [UI_MENU_PAD_FULLSCREEN_HOLD_DESC] = "关闭：Plus + Minus 打开或关闭设置。\n\n开启：改为长按 Plus + Minus 0.5 秒切换全屏。\n\n两种模式下 Guide/Home 键和键盘 F1 都可用。",
    [UI_MENU_SHORTCUT_SETTINGS_COMBO_DESC] = "同时按住屏幕上显示的 Plus 和 Minus 绑定来打开或关闭设置。\n\n此快捷键跟随当前的按键绑定。\n\n键盘等效：F1。",
    [UI_MENU_SHORTCUTS] = "快捷键",
    [UI_MENU_SHORTCUT_READONLY] = "只读快捷键",
    [UI_MENU_SHORTCUT_SETTINGS] = "打开设置菜单",
    [UI_MENU_SHORTCUT_SETTINGS_DESC] = "按 Guide 或 Home 键打开或关闭设置。\n\n键盘等效：F1。\n\n客户端或操作系统必须把这个按键传给游戏。",
    [UI_MENU_SHORTCUT_FULLSCREEN] = "切换全屏",
    [UI_MENU_SHORTCUT_FULLSCREEN_DESC] = "同时按住屏幕上显示的两个按键 0.5 秒来切换全屏。\n\n再次使用前请先松开这两个按键。\n\n键盘等效：F11。",
    [UI_MENU_SHORTCUT_PERFORMANCE] = "切换性能数据",
    [UI_MENU_SHORTCUT_PERFORMANCE_DESC] = "同时按下两个摇杆按压绑定来循环切换性能数据：关、FPS、完整。\n\n此快捷键跟随当前的摇杆按压绑定。\n\n键盘等效：F10。",
    [UI_MENU_PAD_CONNECTED] = "已连接",
    [UI_MENU_PAD_WAITING] = "连接手柄以继续",
    [UI_MENU_PAD_NONE] = "未连接手柄",
    [UI_MENU_PAD_SAVED] = "已保存的手柄",
    SWKBD("文字输入", "请输入名字", "马儿", "确定", "取消", "手柄：A 确定，B 取消"),

    [UI_MENU_TITLE] = "设置",
    [UI_MENU_TAB_DISPLAY] = "显示", [UI_MENU_TAB_GRAPHICS] = "图形",
    [UI_MENU_TAB_GAME] = "游戏", [UI_MENU_TAB_SOUND] = "声音",
    [UI_MENU_TAB_CONTROLS] = "操作", [UI_MENU_TAB_MENU] = "菜单", [UI_MENU_TAB_ADVANCED] = "高级",
    [UI_MENU_CONTINUE] = "继续", [UI_MENU_QUIT] = "退出游戏",
    [UI_MENU_ON] = "开", [UI_MENU_OFF] = "关", [UI_MENU_AUTO] = "自动", [UI_MENU_IS_DEFAULT] = "默认",
    [UI_MENU_AFTER_RESTART] = "需要重启", [UI_MENU_SET_BY_ENV] = "被环境变量锁定",
    [UI_MENU_SAVE_FAILED] = "无法保存设置",
    [UI_MENU_VERSION] = "DKB Recomp %s，游戏 %s",

    [UI_MENU_FULLSCREEN] = "全屏", [UI_MENU_FULLSCREEN_DESC] = "让游戏铺满屏幕；按 F11 切换。",
    [UI_MENU_WINDOW_MODE] = "窗口模式",
    [UI_MENU_WINDOW_MODE_DESC] = "按 F11 在窗口模式和你上次使用的全屏模式之间切换。",
    [UI_MENU_WINDOW_MODE_WINDOWED] = "窗口", [UI_MENU_WINDOW_MODE_BORDERLESS] = "无边框",
    [UI_MENU_WINDOW_MODE_FULLSCREEN] = "全屏",
    [UI_MENU_WINDOW_SIZE] = "窗口大小", [UI_MENU_WINDOW_SIZE_DESC] = "设置窗口模式下桌面窗口的大小。",
    [UI_MENU_WINDOW_SIZE_CUSTOM] = "自定义",
    [UI_MENU_WINDOWED_ONLY] = "仅在窗口模式下可用",
    [UI_MENU_PRESENT] = "帧同步", [UI_MENU_PRESENT_DESC] = "VSync 等待显示器刷新。\n"
                             "Mailbox 在支持时每次刷新显示最新的完整帧。\n"
                             "Immediate 一准备好就输出帧，可能出现画面撕裂。",
    [UI_MENU_PRESENT_MAILBOX] = "Mailbox", [UI_MENU_PRESENT_VSYNC] = "VSync", [UI_MENU_PRESENT_IMMEDIATE] = "Immediate",
    [UI_MENU_PRESENT_IN_USE] = "正在使用：%s",
    [UI_MENU_GPU] = "显卡", [UI_MENU_GPU_DESC] = "选择运行游戏的显卡。除非需要指定某块卡，否则用“自动”。",
    [UI_MENU_GPU_AUTO] = "自动",
    [UI_MENU_PERF_OVERLAY] = "性能悬浮窗",
    [UI_MENU_PERF_OVERLAY_DESC] = "在右上角显示帧率。“完整”还会显示帧时间曲线和硬件统计。按 F10 循环切换模式。",
    [UI_MENU_PERF_OFF] = "关", [UI_MENU_PERF_FPS] = "FPS", [UI_MENU_PERF_FULL] = "完整",
    // printf format of the overlay line in FPS mode: the frame rate
    [UI_PERF_LINE_FPS] = "%.0f fps",
    [UI_MENU_RESOLUTION_GROUP] = "分辨率",
    [UI_MENU_DISPLAY_OUTPUT] = "显示输出",
    [UI_MENU_RENDER_OPTIONS] = "渲染选项",
    [UI_MENU_OUTPUT_RES] = "渲染分辨率",
    [UI_MENU_ULTRAWIDE] = "超宽屏",
    [UI_MENU_ENABLE_ULTRAWIDE] = "启用超宽屏",
    [UI_MENU_ULTRAWIDE_DESC] = "为 21:9 和 32:9 显示器加宽游戏画面。",
    [UI_MENU_WIDE_RES] = "渲染分辨率",
    [UI_MENU_WIDE_RES_DESC] = "设置游戏画面在缩放到显示器之前的尺寸。更高的分辨率更清晰，也更耗 GPU 和显存。\n\n"
                             "要使用 21:9 和 32:9 选项，请先启用超宽屏。",
    [UI_MENU_HUD_POSITION] = "HUD 位置",
    [UI_MENU_HUD_POSITION_DESC] = "把角落的显示放在屏幕边缘，或保持在居中的 16:9 区域内。菜单和准星仍居中。",
    [UI_MENU_HUD_EDGES] = "屏幕边缘", [UI_MENU_HUD_CENTERED] = "居中（16:9）",
    [UI_MENU_WIDE_DISABLED] = "请先启用超宽屏",
    [UI_MENU_OUTPUT_RES_DESC] = "游戏绘制画面和界面所用的尺寸。高于 1920 x 1080 时需要更多显存和 GPU 时间。更宽"
                                "的尺寸（21:9）会扩大相机视野，并把 HUD 移到屏幕四角；更高的尺寸（16:10、Steam "
                                "Deck）会把它移到上下边缘。",
    [UI_MENU_RENDER_SCALE] = "3D 渲染比例",
    [UI_MENU_RENDER_SCALE_DESC] = "3D 场景相对于游戏分辨率的尺寸；游戏的 FSR 会把它缩放到该尺寸。高于 100 % "
                                  "可以平滑锯齿（超采样），但很耗 GPU。",
    [UI_MENU_RENDER_SCALE_GAME] = "游戏原生（83 %）",
    [UI_MENU_SCENE_RES] = "3D 场景分辨率",
    [UI_MENU_SCENE_RES_DESC] = "“自动”让游戏按负载缩放 3D 场景，通常每边缩到渲染分辨率的三分之二，然后再放大。"
                               "“完整”以整个渲染分辨率绘制 3D 场景：画面更锐利，但 GPU 开销更高，且不使用放大方式和 DLSS。"
                               "下次启动时生效。",
    [UI_MENU_SCENE_RES_AUTO] = "自动（游戏）", [UI_MENU_SCENE_RES_FULL] = "完整",
    [UI_MENU_UPSCALER] = "放大方式",
    [UI_MENU_UPSCALER_DESC] = "当游戏画面小于它在屏幕上或窗口中占据的区域时生效。AMD FSR 1 的边缘比双线性更锐利。",
    [UI_MENU_UPSCALER_FSR] = "AMD FSR 1", [UI_MENU_UPSCALER_BILINEAR] = "双线性", [UI_MENU_UPSCALER_DLSS] = "NVIDIA DLSS",
    [UI_MENU_UPSCALER_FSR3] = "AMD FSR 3.1",
    [UI_MENU_ASPECT] = "显示缩放",
    [UI_MENU_ASPECT_DESC] = "默认保持游戏的画面比例，需要时加黑边。\n"
                            "“拉伸填满”使用整个窗口或屏幕；只有比例不同时画面才会被拉伸。",
    [UI_MENU_ASPECT_FIT] = "默认", [UI_MENU_ASPECT_STRETCH] = "拉伸填满",
    [UI_MENU_FOV] = "视野",
    [UI_MENU_FOV_DESC] = "按原始角度的百分比调整相机视野。100% 使用游戏的默认视野。",
    [UI_MENU_SHARPNESS] = "FSR 锐度",
    [UI_MENU_SHARPNESS_DESC] = "FSR 缩放后的锐化；0 关闭。",
    [UI_MENU_NO_UPSCALING] = "需要较小的游戏画面",
    [UI_MENU_SHARPNESS_NEEDS_FSR] = "需要 AMD FSR 1",
    [UI_MENU_SMAA] = "抗锯齿（SMAA）",
    [UI_MENU_SMAA_DESC] = "平滑游戏画面和 HUD 中的锯齿。设置越高越耗 GPU 时间。",
    [UI_MENU_SMAA_LOW] = "低", [UI_MENU_SMAA_MEDIUM] = "中", [UI_MENU_SMAA_HIGH] = "高",
    [UI_MENU_COLOR] = "颜色",
    [UI_MENU_COLOR_PRESET] = "色彩配置",
    [UI_MENU_COLOR_PRESET_DESC] = "游戏默认使用原始色彩。\n鲜艳（OLED）提升对比度和色彩。\n"
                                  "清晰让画面稍亮、更鲜明。\n\n"
                                  "调整任意滑块都会把配置切换为“自定义”。",
    [UI_MENU_COLOR_DEFAULT] = "游戏默认", [UI_MENU_COLOR_VIVID] = "鲜艳（OLED）",
    [UI_MENU_COLOR_CLARITY] = "清晰",
    [UI_MENU_COLOR_CUSTOM] = "自定义",
    [UI_MENU_COLOR_BRIGHTNESS] = "亮度", [UI_MENU_COLOR_BRIGHTNESS_DESC] = "让画面更亮或更暗。默认 0%。",
    [UI_MENU_COLOR_CONTRAST] = "对比度",
    [UI_MENU_COLOR_CONTRAST_DESC] = "数值越高，暗部越暗、亮部越亮。默认 100%。",
    [UI_MENU_COLOR_SATURATION] = "饱和度",
    [UI_MENU_COLOR_SATURATION_DESC] = "数值越高色彩越鲜艳。默认 100%。",
    [UI_MENU_COLOR_GAMMA] = "伽马",
    [UI_MENU_COLOR_GAMMA_DESC] = "低于 1.00 提亮暗部和中调，高于 1.00 压暗它们。",
    [UI_MENU_HDR_GROUP] = "HDR（实验性）",
    [UI_MENU_HDR] = "HDR 输出",
    [UI_MENU_HDR_DESC] = "在 HDR 显示器上提亮游戏 SDR 画面的高光。\n\n"
                         "请先在系统显示设置里启用 HDR。",
    [UI_MENU_HDR_NATIVE] = "游戏原生 HDR",
    [UI_MENU_HDR_NATIVE_DESC] = "在 3.2.0 更新上使用游戏自带的 HDR。高光跟随峰值亮度，最高 1400 尼特。"
                                "此模式会禁用 FSR 1、SMAA 和颜色调整。",
    [UI_MENU_HDR_PEAK] = "峰值亮度",
    [UI_MENU_HDR_PEAK_DESC] = "把最亮的高光设成显示器的峰值亮度（尼特）。",
    [UI_MENU_HDR_GAME] = "游戏亮度",
    [UI_MENU_HDR_GAME_DESC] = "设置游戏和 HUD 里普通白色的亮度。画面太暗时调高它。",
    [UI_MENU_HDR_UI] = "设置菜单亮度",
    [UI_MENU_HDR_UI_DESC] = "设置此设置菜单里白色的亮度。游戏 HUD 跟随游戏亮度。",

    [UI_MENU_FRAME_RATE_GROUP] = "帧率",
    [UI_MENU_FPS_LIMIT] = "帧率上限",
    [UI_MENU_DKB_FPS_LIMIT_DESC] = "限制游戏的帧率。“显示器刷新率”使用显示器当前的刷新率。",
    [UI_MENU_FPS_LIMIT_DESC] = "游戏每秒绘制的最大帧数。高于 60 属实验性，并且需要“游戏速度跟随时钟”。"
                               "“不限”也是实验性的。",
    [UI_MENU_FPS_30] = "30 FPS", [UI_MENU_FPS_40] = "40 FPS", [UI_MENU_FPS_45] = "45 FPS",
    [UI_MENU_FPS_60] = "60 FPS", [UI_MENU_FPS_90] = "90 FPS", [UI_MENU_FPS_120] = "120 FPS",
    [UI_MENU_FPS_144] = "144 FPS",
    // the monitor's refresh rate; printf format with the rate in Hz when it is known
    [UI_MENU_FPS_DISPLAY] = "显示器刷新率", [UI_MENU_FPS_DISPLAY_HZ] = "显示器刷新率（%.0f Hz）",
    [UI_MENU_FPS_UNLIMITED] = "不限",
    [UI_MENU_FPS_NEEDS_REALTIME] = "请启用“允许更高帧率”并重启",
    [UI_MENU_REALTIME] = "游戏速度跟随时钟",
    [UI_MENU_REALTIME_DESC] = "按真实时间推进游戏，这样慢帧不会让游戏变慢。",
    [UI_MENU_DKB_REALTIME] = "允许更高帧率",
    [UI_MENU_DKB_REALTIME_DESC] = "解锁帧率上限设置，包括高于 60 FPS 的帧率。游戏计时随帧率变化调整。"
                                 "此模式为实验性。",
    [UI_MENU_CLOTH_REALTIME] = "布料跟随时钟",
    [UI_MENU_CLOTH_REALTIME_DESC] = "布料和头发也按真实时间推进。",
    [UI_MENU_HIGHFPS_FIX] = "高帧率修正",
    [UI_MENU_HIGHFPS_FIX_DESC] = "修正高于每秒 30 帧时出问题的物理，以及高于 60 帧时仍保持半透明或不可见的物体。",
    [UI_MENU_GAME_LANG] = "游戏语言", [UI_MENU_GAME_LANG_DESC] = "游戏文本和语音的语言。",
    [UI_MENU_GAME_LANG_AUTO] = "自动",
    [UI_MENU_OPERATION_MODE] = "主机模式",
    [UI_MENU_OPERATION_MODE_DESC] = "选择游戏使用的 Switch 模式。座上模式用电视配置，掌机模式用便携配置。",
    [UI_MENU_MODE_DOCKED] = "座上模式", [UI_MENU_MODE_HANDHELD] = "掌机模式",
    [UI_MENU_ASSUME_FOCUS] = "后台继续运行",
    [UI_MENU_ASSUME_FOCUS_DESC] = "在你用其他窗口时让游戏继续运行。关闭后一切换出去就会暂停。\n\n"
                                  "在加载过程中暂停可能导致加载无法完成。",

    [UI_MENU_VOLUME] = "音量", [UI_MENU_VOLUME_DESC] = "调整游戏整体音量。设为 0% 即静音。",
    [UI_MENU_AUDIO_CHANNELS] = "音频输出",
    [UI_MENU_AUDIO_CHANNELS_DESC] = "选择立体声或 5.1 环绕声。“自动”在默认音频设备支持至少六声道时使用 5.1。",
    [UI_MENU_CHANNELS_AUTO] = "自动", [UI_MENU_CHANNELS_STEREO] = "立体声",
    [UI_MENU_CHANNELS_51] = "5.1 环绕声",

    [UI_MENU_CONTROLLER] = "手柄", [UI_MENU_MOUSE_KEYBOARD] = "鼠标和键盘",
    [UI_MENU_MOTION] = "体感操作", [UI_MENU_MOTION_DESC] = "用手柄的陀螺仪瞄准。",
    [UI_MENU_RUMBLE_STRENGTH] = "震动强度",
    [UI_MENU_RUMBLE_STRENGTH_DESC] = "手柄震动的强度。",
    [UI_MENU_RUMBLE_CURVE] = "震动曲线",
    [UI_MENU_RUMBLE_CURVE_DESC] = "数值越低，细微的震动越容易感觉到。",
    [UI_MENU_RUMBLE_TRIGGERS] = "扳机震动（Xbox）",
    [UI_MENU_RUMBLE_TRIGGERS_DESC] = "在支持的 Xbox 手柄上启用扳机震动。",
    [UI_MENU_PAD_LAYOUT] = "按键布局",
    [UI_MENU_PAD_LAYOUT_DESC] = "“自动”对任天堂风格手柄选 Nintendo，其他选 Xbox。\n"
                                "Nintendo 用右侧按键作为 A。\nXbox 用下方按键作为 A。\n"
                                "“Xbox 提示”用 Nintendo 的位置配 Xbox 标签。\n\n"
                                "你重新映射过的按键，游戏仍显示原始提示。",
    [UI_MENU_PAD_LAYOUT_AUTO] = "自动", [UI_MENU_PAD_LAYOUT_NINTENDO] = "Nintendo（A 在右）",
    [UI_MENU_PAD_LAYOUT_XBOX] = "Xbox（A 在下）", [UI_MENU_PAD_LAYOUT_XBOX_PROMPTS] = "Xbox 提示（A 在右）",
    [UI_MENU_PAD_LAYOUT_IN_USE] = "正在使用：%s",
    [UI_MENU_PAD_LAYOUT_PROMPTS_MISSING] = "缺少提示文件：请重新运行安装程序",
    [UI_MENU_PAD_LAYOUT_PROMPTS_RESTART] = "提示会在重启后改变",
    [UI_MENU_STICK_DEADZONE] = "摇杆死区",
    [UI_MENU_STICK_DEADZONE_DESC] = "设置摇杆要移动多远才被识别。角色或相机漂移时调高它。",
    [UI_MENU_MOUSE_CAMERA] = "鼠标直接控制相机",
    [UI_MENU_MOUSE_CAMERA_DESC] = "鼠标移动直接转动跟随相机。其他相机模式把鼠标移动当作摇杆输入。",
    [UI_MENU_MOUSE_SENS] = "鼠标灵敏度",
    [UI_MENU_MOUSE_SENS_DESC] = "设置移动鼠标时相机转动的幅度。",

    [UI_BINDS_HELP_KEYS] = "点击一个绑定或按回车，然后按一个键或鼠标按钮。Delete 或 Backspace 清除它。"
                           "已在使用中的键会移到新动作上。",
    [UI_BINDS_HELP_PAD] = "在一个绑定上按 A，然后按一个手柄按钮。X 清除它。已在使用中的按钮会互换。"
                          "左摇杆移动，右摇杆控制相机。",
    [UI_BINDS_COL_KEY1] = "主键", [UI_BINDS_COL_KEY2] = "副键",
    [UI_BINDS_GROUP_BUTTONS] = "按键", [UI_BINDS_GROUP_MOVE] = "移动", [UI_BINDS_GROUP_CAMERA] = "相机",
    [UI_BINDS_PRESS_KEY] = "请为 %s 按一个键或鼠标按钮",
    [UI_BINDS_PRESS_PAD] = "请为 %s 按一个手柄按钮",
    [UI_BINDS_CANCEL_HINT] = "按 Esc 取消，或等待 %d 秒",
    [UI_BINDS_TAKEN] = "%s 已从 %s 移走", [UI_BINDS_SWAPPED] = "%s 与 %s 互换",
    [UI_BINDS_RESET] = "重置所有绑定",
    [UI_BINDS_RESET_DESC] = "回到默认的键盘按键和手柄按钮。上面的按键布局会重新生效：重新映射过的手柄按钮"
                            "会保留在原处，直到这次重置。",
    [UI_BIND_A] = "A", [UI_BIND_B] = "B", [UI_BIND_X] = "X", [UI_BIND_Y] = "Y",
    [UI_BIND_LCLICK] = "按下左摇杆", [UI_BIND_RCLICK] = "按下右摇杆", [UI_BIND_L] = "L",
    [UI_BIND_R] = "R", [UI_BIND_ZL] = "ZL", [UI_BIND_ZR] = "ZR", [UI_BIND_PLUS] = "＋", [UI_BIND_MINUS] = "－",
    [UI_BIND_LEFT] = "十字键左", [UI_BIND_UP] = "十字键上", [UI_BIND_RIGHT] = "十字键右",
    [UI_BIND_DOWN] = "十字键下",
    [UI_BIND_LSTICK_LEFT] = "左移", [UI_BIND_LSTICK_UP] = "前移", [UI_BIND_LSTICK_RIGHT] = "右移",
    [UI_BIND_LSTICK_DOWN] = "后移",
    [UI_BIND_RSTICK_LEFT] = "相机左转", [UI_BIND_RSTICK_UP] = "相机上看", [UI_BIND_RSTICK_RIGHT] = "相机右转",
    [UI_BIND_RSTICK_DOWN] = "相机下看",
    [UI_BIND_DESC_A] = "互动、拾取、对话。", [UI_BIND_DESC_B] = "奔跑、取消、返回。",
    [UI_BIND_DESC_X] = "跳跃。", [UI_BIND_DESC_Y] = "攻击。", [UI_BIND_DESC_LCLICK] = "蹲下。",
    [UI_BIND_DESC_RCLICK] = "第一人称视角。", [UI_BIND_DESC_L] = "能力。",
    [UI_BIND_DESC_R] = "投掷武器。", [UI_BIND_DESC_ZL] = "盾牌、锁定。", [UI_BIND_DESC_ZR] = "弓箭。",
    [UI_BIND_DESC_PLUS] = "菜单、物品栏。", [UI_BIND_DESC_MINUS] = "地图。",
    [UI_BIND_DESC_DPAD] = "快速选择。",
    [UI_BIND_DESC_MOVE] = "像左摇杆一样移动林克。手柄左摇杆始终可用。",
    [UI_BIND_DESC_CAMERA] = "像右摇杆一样转动相机。鼠标和手柄右摇杆始终可用。",
    [UI_DKB_BIND_A] = "跳跃。在菜单中确认选择。",
    [UI_DKB_BIND_B] = "向下出拳。在菜单中返回。",
    [UI_DKB_BIND_X] = "向上出拳。",
    [UI_DKB_BIND_Y] = "向前出拳，或挥动你抱着的石块。",
    [UI_DKB_BIND_LCLICK] = "执行《Bananza》操作设置里分配给摇杆按压的动作。",
    [UI_DKB_BIND_RCLICK] = "执行《Bananza》操作设置里分配给右摇杆按压的动作。",
    [UI_DKB_BIND_L] = "按住吹口哨，显示前往目标的路线。",
    [UI_DKB_BIND_R] = "用“拍击”收集附近物品并找出隐藏物体。",
    [UI_DKB_BIND_ZL] = "向前翻滚；抱着石块时使出“冲刺滑行”。",
    [UI_DKB_BIND_ZR] = "抓起一块地形，或扔出你抱着的石块。",
    [UI_DKB_BIND_PLUS] = "打开暂停菜单。",
    [UI_DKB_BIND_MINUS] = "打开地图。",
    [UI_DKB_BIND_LEFT] = "切换到上一个已解锁的 Bananza 形态。",
    [UI_DKB_BIND_UP] = "打开技能菜单。",
    [UI_DKB_BIND_RIGHT] = "切换到下一个已解锁的 Bananza 形态。",
    [UI_DKB_BIND_DOWN] = "可用时打开拍照模式。",
    [UI_DKB_BIND_MOVE] = "向该方向移动大金刚。",
    [UI_DKB_BIND_CAMERA] = "向该方向转动相机。",
    [UI_KEY_SPACE] = "空格", [UI_KEY_TAB] = "Tab", [UI_KEY_LSHIFT] = "左 Shift", [UI_KEY_LCTRL] = "左 Ctrl",
    [UI_KEY_UP] = "上方向键", [UI_KEY_DOWN] = "下方向键", [UI_KEY_LEFT] = "左方向键",
    [UI_KEY_RIGHT] = "右方向键",
    [UI_KEY_MOUSE_L] = "鼠标左键", [UI_KEY_MOUSE_R] = "鼠标右键", [UI_KEY_MOUSE_M] = "鼠标中键",
    [UI_KEY_MOUSE_X1] = "鼠标 4", [UI_KEY_MOUSE_X2] = "鼠标 5",
    [UI_KEY_ENTER] = "回车", [UI_KEY_BACKSPACE] = "退格", [UI_KEY_RSHIFT] = "右 Shift",
    [UI_KEY_RCTRL] = "右 Ctrl", [UI_KEY_LALT] = "Alt", [UI_KEY_CAPSLOCK] = "大写锁定",
    [UI_KEY_INSERT] = "Insert", [UI_KEY_DELETE] = "Delete", [UI_KEY_HOME] = "Home", [UI_KEY_END] = "End",
    [UI_KEY_PAGEUP] = "Page Up", [UI_KEY_PAGEDOWN] = "Page Down", [UI_KEY_NUM_FMT] = "数字键 %s",
    [UI_PAD_SOUTH] = "下方按钮", [UI_PAD_EAST] = "右侧按钮", [UI_PAD_WEST] = "左侧按钮",
    [UI_PAD_NORTH] = "上方按钮", [UI_PAD_LSTICK] = "左摇杆", [UI_PAD_RSTICK] = "右摇杆",
    [UI_PAD_L] = "左肩键", [UI_PAD_R] = "右肩键", [UI_PAD_START] = "Start", [UI_PAD_BACK] = "Back",
    [UI_PAD_DPAD_LEFT] = "十字键左", [UI_PAD_DPAD_UP] = "十字键上", [UI_PAD_DPAD_RIGHT] = "十字键右",
    [UI_PAD_DPAD_DOWN] = "十字键下", [UI_PAD_ZL] = "左扳机", [UI_PAD_ZR] = "右扳机",
    [UI_PAD_LPADDLE1] = "左背键", [UI_PAD_RPADDLE1] = "右背键",
    [UI_PAD_LPADDLE2] = "左背键 2", [UI_PAD_RPADDLE2] = "右背键 2", [UI_PAD_TOUCHPAD] = "触摸板",
    [UI_PAD_CROSS] = "Cross", [UI_PAD_CIRCLE] = "Circle", [UI_PAD_SQUARE] = "Square", [UI_PAD_TRIANGLE] = "Triangle",

    [UI_MENU_MENU_LANG] = "设置语言", [UI_MENU_MENU_LANG_DESC] = "设置设置菜单和性能悬浮窗的语言。"
                             "“跟随系统”在系统语言为中文时用中文，俄语时用俄语，其他情况用英文。",
    [UI_MENU_MENU_LANG_AUTO] = "跟随系统",
    [UI_MENU_THEME] = "菜单主题", [UI_MENU_THEME_DK] = "大金刚", [UI_MENU_THEME_CLASSIC] = "经典",
    [UI_MENU_THEME_DESC] = "选择大金刚主题，或经典的绿色与金色主题。",

    [UI_MENU_ADV_WARNING] = "这些设置用于测试和排查问题。只有在明白原因时才改动它们。",
    [UI_MENU_GROUP_SCHED] = "调度", [UI_MENU_GROUP_SHADERS] = "着色器与管线",
    [UI_MENU_GROUP_TEXMEM] = "纹理与内存", [UI_MENU_GROUP_FIXES] = "渲染修正",
    [UI_MENU_GROUP_VULKAN] = "Vulkan", [UI_MENU_GROUP_DIAG] = "诊断",
    [UI_MENU_ASYNC_PREEMPT] = "虚拟核心抢占",
    [UI_MENU_ASYNC_PREEMPT_DESC] = "允许同一虚拟核心上的一个游戏线程打断另一个线程。",
    [UI_MENU_PREEMPT_OFF] = "关", [UI_MENU_PREEMPT_ON] = "开", [UI_MENU_PREEMPT_INHERIT] = "仅继承",
    [UI_MENU_REPLAY_ASYNC] = "GPU 帧准备放到独立线程",
    [UI_MENU_REPLAY_ASYNC_DESC] = "在独立线程上为 GPU 准备帧，而不是在游戏线程上。最多增加一帧延迟。",
    [UI_MENU_REPLAY_THREADS] = "录制大型渲染通道的线程数",
    [UI_MENU_REPLAY_THREADS_DESC] = "同时用这么多线程录制大型渲染通道的绘制。在 8 个或更多逻辑处理器上"
                                    "“自动”使用 4 个线程（AMD 上不用）；“关”用一个线程录制。核心多的 CPU "
                                    "在繁忙区域会更快。",
    [UI_MENU_TIMER_1MS] = "1 毫秒 Windows 计时器",
    [UI_MENU_TIMER_1MS_DESC] = "把系统计时器精度提高到 1 毫秒，让帧节奏更稳定。",
    [UI_MENU_PIPE_WAIT] = "等待型绘制的处理",
    [UI_MENU_PIPE_WAIT_DESC] = "“默认”保留当前调度，为等待型绘制预留一个工作线程；“旧（绘制）”使用之前的调度。",
    [UI_MENU_PIPE_WAIT_DEFAULT] = "默认", [UI_MENU_PIPE_WAIT_DRAW] = "旧（绘制）",
    [UI_MENU_PIPE_SKIP] = "管线构建时跳过的绘制",
    [UI_MENU_PIPE_SKIP_DESC] = "哪些绘制被丢弃，而不是等它的管线建好。",
    [UI_MENU_PIPE_SKIP_STRICT] = "严格", [UI_MENU_PIPE_SKIP_ANY] = "任意",
    [UI_MENU_PIPE_SKIP_FULLSCREEN] = "全屏通道",
    [UI_MENU_LIB_PREBUILD] = "阶段库预构建",
    [UI_MENU_LIB_PREBUILD_DESC] = "在后台提前构建着色器阶段库。",
    [UI_MENU_LIB_PREBUILD_THREADS] = "阶段库预构建线程数",
    [UI_MENU_LIB_PREBUILD_THREADS_DESC] = "预构建使用的线程数；“自动”根据你的 CPU 选择。",
    [UI_MENU_PIPE_PREBUILD] = "管线预构建",
    [UI_MENU_PIPE_PREBUILD_DESC] = "提前构建已知的管线。",
    [UI_MENU_PREWARM_XLATE] = "翻译器预热",
    [UI_MENU_PREWARM_XLATE_DESC] = "启动时翻译已知着色器的线程数；“自动”取 CPU 线程数的一半，最多 16。",
    [UI_MENU_GPL] = "图形管线库",
    [UI_MENU_GPL_DESC] = "使用 Vulkan 管线库扩展；驱动因此出问题时就关掉。",
    [UI_MENU_ASTC] = "ASTC 纹理解码", [UI_MENU_ASTC_DESC] = "压缩的 ASTC 纹理在哪里解码。",
    [UI_MENU_ASTC_AUTO] = "自动", [UI_MENU_ASTC_GPU] = "GPU 着色器", [UI_MENU_ASTC_NATIVE] = "原生",
    [UI_MENU_ASTC_CPU] = "CPU",
    [UI_MENU_MEMORY] = "GPU 内存模式", [UI_MENU_MEMORY_DESC] = "游戏内存如何与 GPU 共享。",
    [UI_MENU_MEMORY_AUTO] = "自动", [UI_MENU_MEMORY_IMPORT] = "导入", [UI_MENU_MEMORY_MIRROR] = "镜像",
    [UI_MENU_MEMORY_VERIFY] = "校验",
    [UI_MENU_TEX_ALIAS] = "纹理别名", [UI_MENU_TEX_ALIAS_DESC] = "在已有纹理的内存上创建新纹理时，以该内容"
                            "开始，而不是空白。",
    [UI_MENU_TEX_VRAM_PRESSURE] = "纹理内存上限", [UI_MENU_TEX_VRAM_PRESSURE_DESC] = "超过此百分比显存后，游戏"
                                    "已释放的纹理会丢弃（再次使用时从存储里取回）。0 表示保留。",
    [UI_MENU_ASTC_BC] = "把 ASTC 压缩为 BC3", [UI_MENU_ASTC_BC_DESC] = "配合 GPU 解码器：把解码后的 ASTC 纹理"
                          "重新编码为 BC3，只占四分之一显存。适用于 4 GB 及以下的显卡；纹理会略显方块化。",
    [UI_MENU_LOD_GUARD] = "LOD 保护", [UI_MENU_LOD_GUARD_DESC] = "在 GPU 读取之前等待游戏改完细节层级数据，避免"
                            "物体在一帧内闪到错误位置，或远景变成一片平坦。",
    [UI_MENU_FIX_C0] = "常量缓冲修正",
    [UI_MENU_FIX_C0_DESC] = "填充驱动保留的常量 bank c0（存储大小、dispatch 大小、点大小、基础实例），"
                            "而不是读取零值。",
    [UI_MENU_FIX_ZERO] = "清零新的 GPU 内存",
    [UI_MENU_FIX_ZERO_DESC] = "新的 GPU 内存池以零填充开始，与 Switch 内存一致。",
    [UI_MENU_LIST_RESTART] = "列表图元重启",
    [UI_MENU_LIST_RESTART_DESC] = "按 Switch 的做法，对三角形列表绘制应用游戏的图元重启。没有它，重启索引"
                                  "会被当成顶点绘制，出现长条拉伸三角形（例如地图上）。需要支持 "
                                  "primitiveTopologyListRestart 的 GPU。",
    [UI_MENU_BIND_SKIP] = "跳过重复绑定",
    [UI_MENU_BIND_SKIP_DESC] = "优化：跳过没有变化的绑定。",
    [UI_MENU_SHADER_MEMO] = "着色器备忘", [UI_MENU_SHADER_MEMO_DESC] = "优化：记住着色器查找结果。",
    [UI_MENU_PIPE_MEMO] = "管线备忘", [UI_MENU_PIPE_MEMO_DESC] = "优化：状态重复时复用上一次绘制的管线。",
    [UI_MENU_TEX_MEMO] = "纹理绑定备忘", [UI_MENU_TEX_MEMO_DESC] = "优化：在绘制之间记住纹理描述符。",
    [UI_MENU_DESC_TEMPLATE] = "描述符模板",
    [UI_MENU_DESC_TEMPLATE_DESC] = "优化：批量更新描述符。",
    [UI_MENU_PUSH_SKIP] = "跳过重复推送常量",
    [UI_MENU_PUSH_SKIP_DESC] = "优化：跳过没有变化的推送常量。",
    [UI_MENU_SPLIT_DRAWS] = "提前提交 GPU",
    [UI_MENU_SPLIT_DRAWS_DESC] = "把 GPU 工作按每 N 个绘制分批、在通道之间提前提交，让 GPU 更早开始；0 关闭。",
    [UI_MENU_VK_VALIDATION] = "校验层",
    [UI_MENU_VK_VALIDATION_DESC] = "检查每一个 Vulkan 调用，会让游戏慢很多。",
    [UI_MENU_VK_VALIDATION_MISSING] = "没有安装校验层。",
    [UI_MENU_VK_DYN_VERTEX] = "动态顶点状态",
    [UI_MENU_VK_DYN_VERTEX_DESC] = "强制开关动态顶点输入；默认值取决于显卡。",
    [UI_MENU_VK_DYN_A2C] = "动态 Alpha to Coverage",
    [UI_MENU_VK_DYN_A2C_DESC] = "强制开关动态 alpha to coverage。在 AMD 驱动上，RDNA 2 及更早的卡默认关闭"
                                "（开启会把角色画成黑色），其他情况默认开启。",
    [UI_MENU_VK_SUBGROUP_PIN] = "固定子组大小", [UI_MENU_VK_SUBGROUP_PIN_DESC] = "固定着色器子组大小。",
    [UI_MENU_TRACE] = "性能日志", [UI_MENU_TRACE_DESC] = "把帧时间写入文件。",
    [UI_MENU_FLIGHT] = "飞行记录器",
    [UI_MENU_FLIGHT_DESC] = "保留一段短历史，在某一帧卡死时保存下来。",
    [UI_MENU_FLIGHT_SPIKE_MS] = "卡死阈值（毫秒）",
    [UI_MENU_FLIGHT_SPIKE_MS_DESC] = "算作卡死的帧时间。",
    [UI_MENU_INPUT_RECORD] = "输入录制",
    [UI_MENU_INPUT_RECORD_DESC] = "把你的输入写入文件，用于报告问题。",
    [UI_MENU_OPEN_LOG] = "打开日志文件夹", [UI_MENU_OPEN_LOG_DESC] = "显示存放日志文件的文件夹。",
    [UI_MENU_COPY_INFO] = "复制系统信息",
    [UI_MENU_COPY_INFO_DESC] = "把 GPU、驱动、CPU、内存和改动过的设置复制到剪贴板。",
    [UI_MENU_COPY_INFO_DONE] = "已复制", [UI_MENU_ACTION_FAILED] = "没有成功",
    [UI_MENU_RESET] = "把所有设置重置为默认", [UI_MENU_RESET_DESC] = "对所有标签页生效。",
    [UI_MENU_RESET_CONFIRM] = "要把每个设置都重置为默认吗？", [UI_MENU_RESET_YES] = "重置",
    [UI_MENU_RESET_NO] = "取消",

    [UI_MENU_OV_FPS] = "%.0f FPS", [UI_MENU_OV_FRAME_MS] = "%.1f 毫秒", [UI_MENU_OV_GAME_PCT] = "游戏 %.0f%%",
    [UI_MENU_OV_MAX_MS] = "最大 %.1f 毫秒",
    [UI_MENU_OV_GPU] = "GPU", [UI_MENU_OV_CPU] = "CPU", [UI_MENU_OV_RAM] = "内存", [UI_MENU_OV_VRAM] = "显存",
    [UI_MENU_OV_TEMP] = "温度", [UI_MENU_OV_POWER] = "功耗", [UI_MENU_OV_CLOCK] = "频率",
    [UI_MENU_OV_NONE] = "-",
    [UI_MENU_OV_REPLAY] = "GPU 帧准备", [UI_MENU_OV_GPU_TIME] = "每帧 GPU 占用",
    [UI_MENU_OV_PIPE_WAIT] = "管线等待",
    [UI_MENU_OV_SHADER_WAIT] = "着色器等待", [UI_MENU_OV_TEX_UPLOADS] = "纹理上传",
    [UI_MENU_OV_FMT_PCT] = "%.0f%%", [UI_MENU_OV_FMT_TEMP] = "%.0f °C", [UI_MENU_OV_FMT_POWER] = "%.0f W",
    [UI_MENU_OV_FMT_MHZ] = "%.0f MHz", [UI_MENU_OV_FMT_GB] = "%.1f GB", [UI_MENU_OV_FMT_GB_OF] = "/ %.1f GB",
    [UI_MENU_OV_GAME_GB] = "游戏 %.1f GB", [UI_MENU_OV_FMT_MS] = "%.2f 毫秒",
    [UI_MENU_OV_PER_FRAME] = "每帧 %.1f",
    [UI_MENU_OV_WINDOW] = "窗口", [UI_MENU_OV_OUTPUT] = "游戏画面", [UI_MENU_OV_SCENE] = "3D 场景",
    [UI_MENU_OV_SCALING] = "缩放", [UI_MENU_OV_LIMIT] = "帧率上限", [UI_MENU_OV_PRESENT] = "输出方式",
    [UI_MENU_OV_FMT_SIZE] = "%ux%u", [UI_MENU_OV_FMT_SCENE] = "%ux%u，%.0f%%",
    [UI_MENU_OV_FMT_UPSCALE] = "%ux%u -> %ux%u",
    [UI_MENU_OV_NO_UPSCALER] = "无放大",
    [UI_MENU_OV_FMT_FSR] = "AMD FSR 1，锐度 %d%%", [UI_MENU_OV_SCALE_NONE] = "无（1:1）",
    [UI_MENU_OV_SCALE_BILINEAR] = "双线性",
};
SWKBD_ONLY(text_zh_hant, "文字輸入", "請輸入名字", "馬兒", "確定", "取消", "手把：A 確定，B 取消");
SWKBD_ONLY(text_ko, "문자 입력", "이름을 입력하세요", "말", "확인", "취소", "컨트롤러: A 확인, B 취소");

static const Row rows[] = {
    {"en", text_en}, {"ja", text_ja}, {"fr", text_fr}, {"de", text_de}, {"it", text_it}, {"es", text_es},
    {"nl", text_nl}, {"pt", text_pt}, {"ru", text_ru}, {"zh-Hans", text_zh_hans}, {"zh-Hant", text_zh_hant},
    {"ko", text_ko},
};

const char *ui_text_lang(const char *code) {
    static const struct { const char *code, *lang; } map[] = {
        {"en-US", "en"}, {"en-GB", "en"}, {"fr-CA", "fr"}, {"es-419", "es"}, {"pt-BR", "pt"},
        {"zh-CN", "zh-Hans"}, {"zh-TW", "zh-Hant"},
    };
    for (size_t i = 0; i < sizeof map / sizeof *map; i++)
        if (!strcmp(code, map[i].code)) return map[i].lang;
    return code;
}

static const Row *find_row(const char *lang) {
    for (size_t i = 0; i < sizeof rows / sizeof *rows; i++)
        if (!strcmp(rows[i].lang, lang)) return &rows[i];
    return NULL;
}

int ui_text_defined(const char *lang, UiTextId id) {
    const Row *r = find_row(lang);
    return (unsigned)id < UI_TEXT_COUNT && r && r->text[id] && *r->text[id];
}

const char *ui_text_in(const char *lang, UiTextId id) {
    if ((unsigned)id >= UI_TEXT_COUNT) return "";
    return ui_text_defined(lang, id) ? find_row(lang)->text[id] : rows[0].text[id];
}

const char *ui_text(UiTextId id) { return ui_text_in(ui_text_lang(ov_language()), id); }

// The menu language, re-read when cfg_version changes. TOTK_MENU_LANG "ru", "en" or "zh-Hans"; anything else (unset
// included) follows the system UI language: Russian for "ru", "ru-RU", ..., the menu's Simplified Chinese for "zh",
// "zh-CN", ..., else English.
static const char *resolve_menu_lang(void) {
    const char *v = cfg_value("TOTK_MENU_LANG");
    if (v && !strcmp(v, "ru")) return "ru";
    if (v && !strcmp(v, "en")) return "en";
    if (v && !strcmp(v, "zh-Hans")) return "zh-Hans";
    const char *sys = plat_ui_language();
    if (sys && (!strcmp(sys, "ru") || !strncmp(sys, "ru-", 3))) return "ru";
    if (sys && (!strcmp(sys, "zh") || !strncmp(sys, "zh-", 3))) return "zh-Hans";   // zh-TW and zh-HK read the same row
    return "en";
}

const char *menu_text(UiTextId id) {
    static _Atomic(const char *) lang = "en";
    static _Atomic unsigned seen = ~0u;
    unsigned v = cfg_version();
    if (v != atomic_load_explicit(&seen, memory_order_relaxed)) {
        atomic_store_explicit(&lang, resolve_menu_lang(), memory_order_relaxed);
        atomic_store_explicit(&seen, v, memory_order_relaxed);
    }
    return ui_text_in(atomic_load_explicit(&lang, memory_order_relaxed), id);
}
