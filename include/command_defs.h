#ifndef NAXX_COMMAND_DEFS_H
#define NAXX_COMMAND_DEFS_H

struct CommandResult;
struct Command;

enum CommandType
{
    COMMAND_TYPE_UNKNOWN, // Unknown command, prompt and ask

    // Gameplay commands
    COMMAND_TYPE_MOVE,
    COMMAND_TYPE_PASS_TURN,
    COMMAND_TYPE_PICK_UP,
    COMMAND_TYPE_DISPLAY_CHARACTER_SCREEN,
    COMMAND_TYPE_DISPLAY_INVENTORY,

    // Info commands -- Display in message box area
    COMMAND_TYPE_DISPLAY_POSITION,
    COMMAND_TYPE_LOOK_AT,

    // Game state commands
    COMMAND_TYPE_SAVE_AND_QUIT,
    COMMAND_TYPE_NO_SAVE_AND_QUIT

#ifdef DEBUG
    ,

    // Debug helper commands
    COMMAND_TYPE_DEBUG_DISPLAY_LONG_MESSAGE,
    COMMAND_TYPE_DEBUG_DISPLAY_LONG_MESSAGE_NO_SPACES
#endif
};

typedef void (*HandlerFunc)(struct Command*, struct CommandResult*);

#endif
