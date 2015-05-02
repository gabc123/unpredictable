
#include "up_concurrent_test.h"

int main(int argNum,char* argChar[])
{
    

    if(SDL_Init(SDL_INIT_EVERYTHING)!=0)
    {
        UP_ERROR_MSG_STR("SDL_INIT failed, we are all doomed!!\n",SDL_GetError());
    }

   up_unitTest_concurency_queue_spsc();
      
    
    SDL_Quit();
    return 0;
}