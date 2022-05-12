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

#include <cstdio> // for printf
#include <iostream>

/**
 * Event handler.
 */
bool HandleEvent(SDL_Event *Event) {
  bool ShouldQuit = false;
  switch (Event->type) {
  case SDL_QUIT: {
    std::printf("SDL_QUIT\n");
    ShouldQuit = true;
  } break;
  case SDL_WINDOWEVENT: {
    switch (Event->window.event) {
    case SDL_WINDOWEVENT_RESIZED: {
      printf("SDL_WINDOWEVENT_RESIZED (%d, %d)\n", Event->window.data1,
             Event->window.data2);
    } break;
    case SDL_WINDOWEVENT_EXPOSED: {
      SDL_Window *Window = SDL_GetWindowFromID(Event->window.windowID);
      SDL_Renderer *Renderer = SDL_GetRenderer(Window);
      static bool IsWhite = true;
      if (IsWhite == true) {
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
        IsWhite = false;
      } else {
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        IsWhite = true;
      }

      SDL_RenderClear(Renderer);
      SDL_RenderPresent(Renderer);

    } break;
    }
  } break;
  }
  return (ShouldQuit);
}

/**
 */
auto main() -> int {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "ERR: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Window *Window =
      SDL_CreateWindow("Handmade Hero", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);

  // Create a "Renderer" for our window.
  SDL_Renderer *Renderer = SDL_CreateRenderer(Window, //!<
                                              -1,     //!< Auto detect.
                                              0 //!< Render flags (if any).
  );

  for (;;) {
    SDL_Event Event;
    SDL_WaitEvent(&Event);
    if (HandleEvent(&Event)) {
      break;
    }
  }

  SDL_Quit();
  return 0;
}
