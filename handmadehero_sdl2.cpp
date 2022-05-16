/******************************************************************************
 * * Filename : handmadehero_sdl2.cpp
 * * Date     : 2022 May 12
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : MIT
 * * Descripti: SDL2 version of the Handmade Hero platform layer. Loosely
 *              following David Gows tutorial. See
 * https://davidgow.net/handmadepenguin
 * ******************************************************************************/
#include <SDL.h>
#include <sys/mman.h>

#include <atomic>
#include <cassert>
#include <cstdio>   // for printf
#include <cstdlib>  // for malloc
#include <iostream>
#include <memory>

#define internal static
#define local_persist static
#define global_variable static

namespace
{

struct window_data {
  std::atomic<int> ScreenWidth{};
  std::atomic<int> ScreenHeight{};

  std::atomic<int> BitmapWidth{};
  std::atomic<int> BitmapHeight{};
  std::atomic<int> BytesPerPixel{4};
  SDL_Texture *Texture{nullptr};
  std::atomic<bool> BitmapUpdated{};

  void *BitmapMemory{nullptr};

  window_data() {}
  ~window_data()
  {
    std::cerr << __PRETTY_FUNCTION__ << " -> Called. Pixels: " << BitmapMemory << std::endl;
    if (BitmapMemory) {
      if (BitmapMemory) {
        std::cerr << __PRETTY_FUNCTION__ << " -> Will free Pixels ptr." << std::endl;
        munmap(BitmapMemory,       //!<
               BitmapWidth *       //!<
                   BitmapHeight *  //!<
                   BytesPerPixel  //!<
        );
      }
    }
  }
};

//------------------------------------------------------------------------------
internal void SDLResizeTexture(window_data *ptrWindowData,  //!<
                               SDL_Renderer *Renderer,      //!<
                               int Width,                   //!<
                               int Height)
{
  if (ptrWindowData->Texture) {
    SDL_DestroyTexture(ptrWindowData->Texture);
  }

  ptrWindowData->Texture = SDL_CreateTexture(Renderer,                     //!<
                                             SDL_PIXELFORMAT_ARGB8888,     //!<
                                             SDL_TEXTUREACCESS_STREAMING,  //!<
                                             Width,                        //!<
                                             Height                        //!<
  );

  void *BitmapMemory = mmap(nullptr,                                         //!< Pointer to start
                            Width * Height * ptrWindowData->BytesPerPixel,  //!<
                            PROT_READ | PROT_WRITE,                          //!<  Protection
                            MAP_ANONYMOUS | MAP_PRIVATE,                     //!<  Flags
                            -1,  //!<  File descriptor, -1 when we dont want to map a file.
                            0    //!<  Offset
  );

  if (ptrWindowData->BitmapMemory) {
    munmap(ptrWindowData->BitmapMemory,       //!<
           ptrWindowData->BitmapWidth *       //!<
               ptrWindowData->BitmapHeight *  //!<
               ptrWindowData->BytesPerPixel  //!<
    );
  }

  ptrWindowData->BitmapWidth = Width;
  ptrWindowData->BitmapHeight = Height;
  ptrWindowData->BitmapMemory = BitmapMemory;

  ptrWindowData->BitmapUpdated = ptrWindowData->BitmapMemory != nullptr;
}

//------------------------------------------------------------------------------
internal void SDLUpdateWindow(window_data *ptrWindowData,  //!<
                              SDL_Renderer *Renderer       //!<
                                                           //
)
{
  //------------------------------------------------------------------------------
  // NOTE: Return early when the texture has not been resized yet.
  //------------------------------------------------------------------------------
  if (!ptrWindowData->BitmapUpdated) return;

  assert(ptrWindowData->Texture);
  assert(ptrWindowData->Pixels);
  assert(ptrWindowData->TextureWidth);

  if (SDL_UpdateTexture(ptrWindowData->Texture,                                      //!<
                        0,                                                           //!<
                        ptrWindowData->BitmapMemory,                                 //!<
                        ptrWindowData->BitmapWidth * ptrWindowData->BytesPerPixel)  //!<
  ) {
    char ErrStr[256]{0};
    printf("%s -> Error when updating texture. %s.\n", __FUNCTION__,
           SDL_GetErrorMsg(ErrStr, sizeof(ErrStr)));
    return;
  }

  if (SDL_RenderCopy(Renderer,                //!<
                     ptrWindowData->Texture,  //!<
                     nullptr,                 //!<
                     nullptr)                 //!<
  ) {
    char ErrStr[256]{0};
    printf("%s -> Error when doing RenderCopy. %s.\n", __FUNCTION__,
           SDL_GetErrorMsg(ErrStr, sizeof(ErrStr)));
    return;
  }

  SDL_RenderPresent(Renderer);
}

/**
 * Event handler.
 */
bool HandleEvent(SDL_Event *Event, window_data *ptrWindowData)
{
  bool ShouldQuit = false;
  switch (Event->type) {
    case SDL_QUIT: {
      std::printf("SDL_QUIT\n");
      ShouldQuit = true;
    } break;
    case SDL_WINDOWEVENT: {
      switch (Event->window.event) {
          // case SDL_WINDOWEVENT_RESIZED: {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)\n",  //!<
                 Event->window.data1, Event->window.data2);

          SDL_Window *Window = SDL_GetWindowFromID(Event->window.windowID);

          if (!Window) {
            printf(
                "SDL_WINDOWEVENT_RESIZED (%d, %d) -> Failed to get pointer to "
                "Window.\n",
                Event->window.data1, Event->window.data2);
            ShouldQuit = true;
            return ShouldQuit;
          }

          SDL_Renderer *Renderer = SDL_GetRenderer(Window);

          int Width{}, Height{};
          SDL_GetWindowSize(Window, &Width, &Height);
          ptrWindowData->ScreenHeight = Height;
          ptrWindowData->ScreenWidth = Width;

          SDLResizeTexture(ptrWindowData, Renderer, Event->window.data1, Event->window.data2);

        } break;
        case SDL_WINDOWEVENT_EXPOSED: {
          printf("SDL_WINDOWEVENT_EXPOSED (%d, %d)\n",  //!<
                 Event->window.data1, Event->window.data2);

          SDL_Window *Window = SDL_GetWindowFromID(Event->window.windowID);
          SDL_Renderer *Renderer = SDL_GetRenderer(Window);

          // ---
          // FIXME: (Willy Clarke) Need to figure out why texture is not available
          //                       to the Render in the SDLUpdateWindow call below.
          // ---
          if (!ptrWindowData->BitmapUpdated) return (ShouldQuit);

          SDLUpdateWindow(ptrWindowData, Renderer);
        } break;
      }
    } break;
  }
  return (ShouldQuit);
}
};  // end of anonymous namespace

/**
 */
auto main() -> int
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "ERR: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Window *Window = SDL_CreateWindow("Handmade Hero", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);

  // Create a "Renderer" for our window.
  SDL_Renderer *Renderer = SDL_CreateRenderer(Window,  //!<
                                              -1,      //!< Auto detect.
                                              0        //!< Render flags (if any).
  );

  window_data WindowData{};

  for (;;) {
    SDL_Event Event{};
    SDL_WaitEvent(&Event);
    if (HandleEvent(&Event, &WindowData)) {
      break;
    }
  }

  SDL_Quit();

  return 0;
}
