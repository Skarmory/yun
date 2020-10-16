#include "message_utils.h"

#define VOWELS_ARRAY_LENGTH 5

static const char _vowels[VOWELS_ARRAY_LENGTH] =
{
    'a', 'e', 'i', 'o', 'u'
};

bool msg_starts_with_vowel(const char* msg)
{
    if(!msg)
    {
        return false;
    }

    for(int i = 0; i < VOWELS_ARRAY_LENGTH; ++i)
    {
        if(msg[0] == _vowels[i])
        {
            return true;
        }
    }

    return false;
}

const char* msg_a_an(const char* msg)
{
   if(msg_starts_with_vowel(msg))
   {
       return "an";
   }

   return "a";
}

