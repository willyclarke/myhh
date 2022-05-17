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

#ifndef HANDMADE
#define HANDMADE 0
#endif
namespace
{

struct window_data {
  int BitmapWidth{};
  int BitmapHeight{};
  int BytesPerPixel{4};

  bool BitmapUpdated{};

  SDL_Texture *ptrTexture{nullptr};

  void *ptrBitmapMemory{nullptr};

  window_data() {}
  ~window_data()
  {
    std::cerr << __PRETTY_FUNCTION__ << " -> Called. Pixels: " << ptrBitmapMemory << std::endl;
    if (ptrBitmapMemory) {
      std::cerr << __PRETTY_FUNCTION__ << " -> Will free Pixels ptr." << std::endl;
      munmap(ptrBitmapMemory,    //!<
             BitmapWidth *       //!<
                 BitmapHeight *  //!<
                 BytesPerPixel   //!<
      );
    }
  }
};

//------------------------------------------------------------------------------
internal void SDLResizeTexture(window_data *ptrWindowData,  //!<
                               SDL_Renderer *Renderer,      //!<
                               int Width,                   //!<
                               int Height)
{
  if (ptrWindowData->ptrTexture) {
    SDL_DestroyTexture(ptrWindowData->ptrTexture);
  }

  ptrWindowData->ptrTexture = SDL_CreateTexture(Renderer,                     //!<
                                                SDL_PIXELFORMAT_ARGB8888,     //!<
                                                SDL_TEXTUREACCESS_STREAMING,  //!<
                                                Width,                        //!<
                                                Height                        //!<
  );

  void *BitmapMemory = mmap(nullptr,                                        //!< Pointer to start
                            Width * Height * ptrWindowData->BytesPerPixel,  //!<
                            PROT_READ | PROT_WRITE,                         //!<  Protection
                            MAP_ANONYMOUS | MAP_PRIVATE,                    //!<  Flags
                            -1,  //!<  File descriptor, -1 when we dont want to map a file.
                            0    //!<  Offset
  );

  if (ptrWindowData->ptrBitmapMemory) {
    munmap(ptrWindowData->ptrBitmapMemory,    //!<
           ptrWindowData->BitmapWidth *       //!<
               ptrWindowData->BitmapHeight *  //!<
               ptrWindowData->BytesPerPixel   //!<
    );
  }

  ptrWindowData->BitmapWidth = Width;
  ptrWindowData->BitmapHeight = Height;
  ptrWindowData->ptrBitmapMemory = BitmapMemory;

  ptrWindowData->BitmapUpdated = ptrWindowData->ptrBitmapMemory != nullptr;
}

//------------------------------------------------------------------------------
internal void SDLUpdateWindow(window_data *ptrWindowData,  //!<
                              SDL_Renderer *ptrRenderer    //!<
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

  if (SDL_UpdateTexture(ptrWindowData->ptrTexture,                                  //!<
                        0,                                                          //!<
                        ptrWindowData->ptrBitmapMemory,                             //!<
                        ptrWindowData->BitmapWidth * ptrWindowData->BytesPerPixel)  //!<
  ) {
    char ErrStr[256]{0};
    printf("%s -> Error when updating texture. %s.\n", __FUNCTION__,
           SDL_GetErrorMsg(ErrStr, sizeof(ErrStr)));
    return;
  }

  if (SDL_RenderCopy(ptrRenderer,                //!<
                     ptrWindowData->ptrTexture,  //!<
                     nullptr,                    //!<
                     nullptr)                    //!<
  ) {
    char ErrStr[256]{0};
    printf("%s -> Error when doing RenderCopy. %s.\n", __FUNCTION__,
           SDL_GetErrorMsg(ErrStr, sizeof(ErrStr)));
    return;
  }

  SDL_RenderPresent(ptrRenderer);
}

/**
 * Event handler.
 */
internal bool HandleEvent(SDL_Event *Event, window_data *ptrWindowData)
{
#if HANDMADE
  static int Count{};
  ++Count;
  std::cout << __FUNCTION__ << " -> " << Event->type << ". Count: " << Count
            << ". Event type: " << Event->type << std::endl;
#endif

  bool ShouldQuit = false;
  switch (Event->type) {
    case SDL_QUIT: {
      std::printf("SDL_QUIT\n");
      ShouldQuit = true;
    } break;
    case SDL_WINDOWEVENT: {
      switch (Event->window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)\n",  //!<
                 Event->window.data1, Event->window.data2);

          SDL_Window *ptrWindow = SDL_GetWindowFromID(Event->window.windowID);

          if (!ptrWindow) {
            printf(
                "SDL_WINDOWEVENT_RESIZED (%d, %d) -> Failed to get pointer to "
                "Window.\n",
                Event->window.data1, Event->window.data2);
            ShouldQuit = true;
            return ShouldQuit;
          }

          SDL_Renderer *ptrRenderer = SDL_GetRenderer(ptrWindow);
          SDLResizeTexture(ptrWindowData, ptrRenderer, Event->window.data1, Event->window.data2);
        } break;
        case SDL_WINDOWEVENT_EXPOSED: {
          printf("SDL_WINDOWEVENT_EXPOSED (w:%d, h:%d)\n",  //!<
                 Event->window.data1, Event->window.data2);
        } break;
      }
    } break;
  }
  return (ShouldQuit);
}
};  // end of anonymous namespace

/**
Test code for SDLUpdateWindow(). The code in this function is kind of platform
independent. But since it uses the window_data struct which contain both
POD variables and SDL variables it is not strictly true.
*/
internal void RenderWeirdGradient(window_data *ptrWindowData,  //!<
                                  SDL_Renderer *ptrRenderer,   //!<
                                  int const BlueOffset = 0,    //!<
                                  int const GreenOffset = 0    //!<
)
{
  // int const Pitch = ptrWindowData->BitmapWidth * ptrWindowData->BytesPerPixel;

  uint8_t *ptrBitmapMemory = (uint8_t *)ptrWindowData->ptrBitmapMemory;
  uint32_t *Pixel = (uint32_t *)ptrBitmapMemory;

  for (size_t Row = 0;                     ///<!
       Row < ptrWindowData->BitmapHeight;  ///<!
       ++Row) {
    for (size_t Col = 0;                    ///<!
         Col < ptrWindowData->BitmapWidth;  ///<!
         ++Col) {
      uint8_t const Blue = Row + BlueOffset;
      uint8_t const Green = Col + GreenOffset;
      *Pixel = (Green << 8) | Blue;
      ++Pixel;
    }
  }
}

/**
 */
auto main() -> int
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "ERR: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Window *ptrWindow = SDL_CreateWindow("Handmade Hero", SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);

  // Create a "Renderer" for our window.
  SDL_Renderer *ptrRenderer = SDL_CreateRenderer(ptrWindow,  //!<
                                                 -1,         //!< Auto detect.
                                                 0           //!< Render flags (if any).
  );

  window_data WindowData{};

  int Width{}, Height{};
  SDL_GetWindowSize(ptrWindow, &Width, &Height);
  SDLResizeTexture(&WindowData, ptrRenderer, Width, Height);

  bool Running{true};
  int BlueOffset{};
  int GreenOffset{};
  int ScanCount{};

  while (Running) {
    SDL_Event Event{};
    while (SDL_PollEvent(&Event)) {
      if (HandleEvent(&Event, &WindowData)) {
        Running = false;
      }
    }
    RenderWeirdGradient(&WindowData, ptrRenderer, BlueOffset, GreenOffset);

    if (!(ScanCount % 10)) {
      ScanCount = 0;
      ++BlueOffset;
      ++GreenOffset;
    }
    ++ScanCount;

    SDLUpdateWindow(&WindowData, ptrRenderer);
  }

  SDL_Quit();

  return 0;
}
