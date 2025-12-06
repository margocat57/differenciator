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
        "Очевидно, что:\n",
        "Несложно заметить:\n",
        "Понимание этого преобразование предоставляется читателю в качестве несложного упражнения:\n",
        "Должно быть известно со школы:\n",
        "По теореме (какой там номер?) из параграфа ??:\n",
        "Как было показано в детском саду:\n",
        "Аналогично доказывается:\n",
        "Давайте представим это хозяйство, как:\n",
        "Плюс константа:\n",
        "Хорошая, годная задача?\n",
        "Если вы не понимаете это очевидное преобразование, вам нужно пойти на программу, где не изучают математический анализ:\n",
    };

    int max_idx = sizeof(messages) / sizeof(const char *);
    int idx = rand() % max_idx;

    return messages[idx];
}