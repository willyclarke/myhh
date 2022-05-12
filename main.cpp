/*
==============================================================================

    file                 : main.cpp
    created              : May 23 2015
    copyright            : (C) 2015 Joe Thompson
    email                : beaglejoe@users.sourceforge.net
    version              : $Id: main.cpp 2751 2021-01-24 05:38:48Z joe $

==============================================================================

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================
*/
#include "main.h"
//#include "input.h"

/* Boolean */
#define GL_TRUE                           1
#define GL_FALSE                          0

#define PLAYGROUND_OK                     0

// Global variables
SDL_Window* gWin = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_GLContext gContext = NULL;


int gWinWidth = 640;
int gWinHeight = 480;

int defMajorOGL = 0;
int defMinorOGL = 0;

//int msBuffers = 0;
//int msSamples = 0;


bool gGameOver = false;

// Forward declare functions
int do_work();
int update_state();
int redraw();

int handle_events();

void dumpSDLversions();
void dumpVideoDrivers();

int setupSDL2();
int shutdownSDL2();

//============================================================================
int main(int argc, char **argv)
{
   int return_value = 0;

   dumpSDLversions();

   if (PLAYGROUND_OK == setupSDL2())
   {
      do_work();

      shutdownSDL2();
   }
   else
   {
      printf("Couldn't initialize SDL video sub-system (%s)\n", SDL_GetError());
      return_value = -1;
   }

   return return_value;
}
//============================================================================
int do_work()
{
   int return_value = 0;

   while(false == gGameOver)
   {
      handle_events();
      update_state();
      redraw();
   }

   return return_value;
}
//============================================================================
void dumpVideoDrivers()
{
    int nDrivers = SDL_GetNumVideoDrivers();

    for(int i = 0;i < nDrivers;i++)
    {
        printf("Video Driver %d: name: %s\n",i, SDL_GetVideoDriver(i));
    }
     printf("\n");

}

//============================================================================
int setupSDL2()
{
   int return_value = 0;

   if (SDL_InitSubSystem(SDL_INIT_VIDEO) >= 0)
   {
      dumpVideoDrivers();

      SDL_GL_ResetAttributes();

      gWin = SDL_CreateWindow("SDL2 Window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,gWinWidth,gWinHeight,SDL_WINDOW_OPENGL);//SDL_WINDOW_SHOWN
      if(NULL != gWin)
      {
         gRenderer = SDL_CreateRenderer(gWin,-1,0);
         if(NULL != gRenderer)
         {
             int Major = -1;
             int Minor = -1;

             gContext = SDL_GL_CreateContext(gWin);
             SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &Major);
             SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &Minor);
             printf("OpenGL version: %d.%d\n", Major,Minor);

             return_value = 0;
         }
         else
         {
            printf("SDL_CreateRenderer() failed: (%s)\n", SDL_GetError());
            return_value = -1;
         }
      }
      else
      {
         printf("SDL_CreateWindow() failed: (%s)\n", SDL_GetError());
         return_value = -1;
      }
   }
   else
   {
      printf("Couldn't initialize SDL video sub-system (%s)\n", SDL_GetError());
      return_value = -1;
   }
   return return_value;
}
//============================================================================
int shutdownSDL2()
{
   int return_value = 0;

   // Shutdown the renderer
   if(NULL != gRenderer)
   {
      SDL_DestroyRenderer(gRenderer);
      gRenderer = NULL;
   }
   if(NULL != gContext)
   {
       SDL_GL_DeleteContext(gContext);
       gContext = NULL;
   }

   // Shutdown the window
   if(NULL != gWin)
   {
      SDL_DestroyWindow(gWin);
      gWin = NULL;
   }
   // Shutdown SDL video sub-system.
   SDL_QuitSubSystem(SDL_INIT_VIDEO);
   
   // Shudown SDL.
   SDL_Quit();

   return return_value;
}
//============================================================================
void dumpSDLversions()
{
   SDL_version compiled;
   SDL_version linked;

   SDL_VERSION(&compiled);
   SDL_GetVersion(&linked);
   printf("Compiled against SDL version %d.%d.%d \n",
      compiled.major, compiled.minor, compiled.patch);
   printf("Linking against SDL version %d.%d.%d.\n",
      linked.major, linked.minor, linked.patch);
}
//============================================================================
int redraw()
{
   int return_value = 0;

   SDL_SetRenderDrawColor(gRenderer,0,0,0,255);
   
   SDL_RenderClear(gRenderer);

   SDL_RenderPresent(gRenderer);

   SDL_Delay(200);

   return return_value;
}
//============================================================================
int update_state()
{
   int return_value = 0;
   return return_value;
}
//============================================================================
int handle_events()
{
   int return_value = 0;
   SDL_Event event;
   static int unicode = 0;
   static SDL_Keymod modifier = KMOD_NONE;

   while(SDL_PollEvent(&event))
   {
      // Process events we care about, and ignore the others.
         switch(event.type)
         {
            case SDL_TEXTINPUT:
               unicode = (int)(event.text.text[0]);
               modifier = SDL_GetModState();
               printf("SDL_TEXTINPUT: %c %X\r\n",(char)unicode,modifier);
               break;

            case SDL_KEYDOWN:
               printf("SDL_KEYDOWN: %c\r\n",(char)event.key.keysym.sym);
               switch(event.key.keysym.sym)
               {
                  case SDLK_ESCAPE:
                     gGameOver = true;
                     break;
               }
               break;

            case SDL_KEYUP:
               printf("SDL_KEYUP: %c unicode = %c\r\n",(char)event.key.keysym.sym,(char)unicode);
               break;

            case SDL_QUIT:
                printf("SDL_QUIT\r\n");
               gGameOver = true;
               break;

            default:
               break;
         }
   }

   return return_value;
}
