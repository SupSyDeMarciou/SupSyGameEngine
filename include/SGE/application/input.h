#ifndef __SGE_APP_INPUT_H__
#define __SGE_APP_INPUT_H__

#include "../constants.h"

/// @brief The possible states for a key
enum SGE_Key_State {
    SGE_KEY_STATE_UP = 1,       // The key is not pressed
    SGE_KEY_STATE_PRESSED = 2,  // The key has been pressed at the start of the current frame
    SGE_KEY_STATE_DOWN = 4,     // The key has been down for at least one frame
    SGE_KEY_STATE_RELEASED = 8, // The key has been released on the current frame
    SGE_KEY_STATE_REPEAT = 16   // The key has been pressed long enough for the OS to consider it repeating (useful for text input)
};

/// @brief The recognized inputs
enum SGE_Key {
    SGE_KEY_A = 'A',
    SGE_KEY_B = 'B',
    SGE_KEY_C = 'C',
    SGE_KEY_D = 'D',
    SGE_KEY_E = 'E',
    SGE_KEY_F = 'F',
    SGE_KEY_G = 'G',
    SGE_KEY_H = 'H',
    SGE_KEY_I = 'I',
    SGE_KEY_J = 'J',
    SGE_KEY_K = 'K',
    SGE_KEY_L = 'L',
    SGE_KEY_M = 'M',
    SGE_KEY_N = 'N',
    SGE_KEY_O = 'O',
    SGE_KEY_P = 'P',
    SGE_KEY_Q = 'Q',
    SGE_KEY_R = 'R',
    SGE_KEY_S = 'S',
    SGE_KEY_T = 'T',
    SGE_KEY_U = 'U',
    SGE_KEY_V = 'V',
    SGE_KEY_W = 'W',
    SGE_KEY_X = 'X',
    SGE_KEY_Y = 'Y',
    SGE_KEY_Z = 'Z',

    SGE_KEY_0 = '0',
    SGE_KEY_1 = '1',
    SGE_KEY_2 = '2',
    SGE_KEY_3 = '3',
    SGE_KEY_4 = '4',
    SGE_KEY_5 = '5',
    SGE_KEY_6 = '6',
    SGE_KEY_7 = '7',
    SGE_KEY_8 = '8',
    SGE_KEY_9 = '9',

    SGE_KEY_SPACE = ' ',
    SGE_KEY_PERIOD = '.',
    SGE_KEY_COMMA = ',',
    SGE_KEY_SEMICOLON = ';',
    SGE_KEY_COLON = ':',
    SGE_KEY_APOSTROPHE = '\'',
    SGE_KEY_MINUS = '-',
    SGE_KEY_PLUS = '+',
    SGE_KEY_EQUAL = '=',
    SGE_KEY_SLASH = '/',

    SGE_KEY_CTRL = GLFW_KEY_LEFT_CONTROL,
    SGE_KEY_CTRL_RIGHT = GLFW_KEY_RIGHT_CONTROL,
    SGE_KEY_SHIFT = GLFW_KEY_LEFT_SHIFT,
    SGE_KEY_SHIFT_RIGHT = GLFW_KEY_RIGHT_SHIFT,
    SGE_KEY_ENTER = GLFW_KEY_ENTER,
    SGE_KEY_RETURN = GLFW_KEY_BACKSPACE,
    SGE_KEY_TAB = GLFW_KEY_TAB,
    SGE_KEY_ESCAPE = GLFW_KEY_ESCAPE,

    SGE_KEY_RIGHT = GLFW_KEY_RIGHT,
    SGE_KEY_LEFT = GLFW_KEY_LEFT,
    SGE_KEY_DOWN = GLFW_KEY_DOWN,
    SGE_KEY_UP = GLFW_KEY_UP,
    
    SGE_KEY_ANY = 5,

    SGE_MOUSE_LEFT = GLFW_MOUSE_BUTTON_LEFT,
    SGE_MOUSE_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
    SGE_MOUSE_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,

    SGE_MOUSE_ANY = 3,
};

/// @brief Returns the current state of the key
/// @param key The key to inspect
enum SGE_Key_State inputGetKeyState(enum SGE_Key key);
/// @brief Check if a key has the wanted state
/// @param key The key to inspect
/// @param state The expected state (May be a "or" of multiple states)
/// @return Wether the key is in the wanted state
bool inputIsKey(enum SGE_Key key, enum SGE_Key_State state);

/// @brief Check if a key has just been pressed
/// @param key The key to inspect
/// @return Wether the key has just been pressed
bool inputIsKeyPressed(enum SGE_Key key);
/// @brief Check if a key has just been released
/// @param key The key to inspect
/// @return Wether the key has just been released
bool inputIsKeyReleased(enum SGE_Key key);
/// @brief Check if a key is currently held down
/// @param key The key to inspect
/// @return Wether the key is currently held down
bool inputIsKeyDown(enum SGE_Key key);
/// @brief Check if a key is currently not held down
/// @param key The key to inspect
/// @return Wether the key is currently not held down
bool inputIsKeyUp(enum SGE_Key key);
/// @brief Check if a key is currently considered "repeating"
/// @param key The key to inspect
/// @return Wether the key is currently considered "repeating"
bool inputIsKeyRepeat(enum SGE_Key key);

/// @brief Get the position of the mouse
/// @return The position of the mouse in pixel coordinates
vec2 inputGetMousePos();
/// @brief Get the position of the mouse
/// @return The position of the mouse in screen coordinates
vec2 inputGetMousePos_Screen();

/// @brief Get the text inputed during this frame
/// @return The array containing the inputed text (uint32 array)
/// @note This array is part of the input system, it should only be used as read-only
array(char32) inputGetTextInput();

#endif