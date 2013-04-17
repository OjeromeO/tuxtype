/*
   main.c:
   
   main() function for Tux Typing

   Copyright 2000, 2003, 2007, 2008, 2009, 2010.
   Authors: Sam Hart, Jesse Andrews, David Bruce.
   
   Project email: <tux4kids-tuxtype-dev@lists.alioth.debian.org>
   Project website: http://tux4kids.alioth.debian.org

   main.c is part of Tux Typing, a.k.a "tuxtype".

Tux Typing is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Tux Typing is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "globals.h"
#include "funcs.h"
#include "SDL_extras.h"

SDL_Surface* screen;
SDL_Event  event;




/********************
  main : init stuff
*********************/
int main(int argc, char *argv[])
{
  Uint32 lib_flags = 0;

  srand(time(NULL));

  //FIXME we should inspect return values and bail if these functions fail:
  Opts_Initialize(); // First, initialize settings with hard-coded defaults 
  // This sets settings.default_data_path to the default theme file path:
  if(!SetupPaths(NULL))
  {  
    fprintf(stderr, "Error - SetupPaths() failed, exiting program.\n");
    return 0;
  }  
  if (!LoadSettings())    // Second, read saved any saved settings
  {
    fprintf(stderr, "Warning - LoadSettings() failed, continuing with default settings.\n");
  }

  // Third, check command line args as these should override saved settings
  handle_command_args(argc, argv);

  DEBUGCODE(debug_all)
  {
    fprintf(stderr, "\n%s, version %s BEGIN\n", PACKAGE, VERSION);
  }

  //Now initialize locale/gettext system.
  //This is done after reading settings just so we can print
  //if the "-d" debug flag is set.
  {
    char *s1, *s2, *s3, *s4;

    s1 = setlocale(LC_ALL, "");
    s2 = bindtextdomain(PACKAGE, TUXLOCALE);
    s3 = bind_textdomain_codeset(PACKAGE, "UTF-8");
    s4 = textdomain(PACKAGE);

    DEBUGCODE(debug_all)
    {
      fprintf(stderr, "PACKAGE = %s\n", PACKAGE);
      fprintf(stderr, "TUXLOCALE = %s\n", TUXLOCALE);
      fprintf(stderr, "setlocale(LC_ALL, \"\") returned: %s\n", s1);
      fprintf(stderr, "bindtextdomain(PACKAGE, TUXLOCALE) returned: %s\n", s2);
      fprintf(stderr, "bind_textdomain_codeset(PACKAGE, \"UTF-8\") returned: %s\n", s3);
      fprintf(stderr, "textdomain(PACKAGE) returned: %s\n", s4);
      fprintf(stderr, "gettext(\"Fish\"): %s\n\n", gettext("Fish"));
      fprintf(stderr, "After gettext() call\n");
    }
  }


  lib_flags = SDL_INIT_VIDEO;

  lib_flags |= SDL_INIT_AUDIO;

  LibInit(lib_flags); /* calls SDL_Init(), TTF_Init(), some other settings */
  GraphicsInit(); /* calls SDL_SetVideoMode(), a few others     */

  if (settings.sys_sound)
  {
    Mix_VolumeMusic(settings.mus_volume);
    Mix_Volume(-1, settings.sfx_volume);
  }

  /* FIXME: we should check config files/environment variables like LANG! */
  /* NOTE what should we do if LANG is something without a theme - should */
  /* we then default to English?                                          */

  LoadLang();
  LoadKeyboard();

  /* Now actually play the game: */
  TitleScreen();

  SaveSettings();

  /* Release heap: */
  Cleanup();

  LOG( "---GAME DONE, EXIT---- Thank you.\n" );

  return EXIT_SUCCESS;
}
