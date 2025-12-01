#include <stdlib.h>
#include <time.h>

const char *GenerateRoflMsg(){
    // because need to init once
    static bool initialized = false;
    if(!initialized){
        srand((unsigned int)time(NULL));
        initialized = true;
    }

    const char *messages[] = {
        "It is obvious that:\n",
        "It is easy to see:\n",
        "Understanding this transformation is left to the reader as a simple exercise:\n",
        "Should be known from school:\n",
        "According to the theorem (which number?) from paragraph ??:\n",
        "It is common knowledge:\n",
        "As already shown earlier:\n",
        "A similar one can be proved:\n",
        "If this is not obvious to you, try attending a lecture for a change:",
        "Let's imagine this household as:\n",
        "Plus a constant:\n",
        "A good, solid task?\n",
        "If you don't understand this obvious transformation, then you need to go into a program where they don't study mathematical analys:\n",
    };

    int max_idx = sizeof(messages) / sizeof(const char *);
    int idx = rand() % max_idx;

    return messages[idx];
}