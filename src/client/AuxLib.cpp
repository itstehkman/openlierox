/////////////////////////////////////////
//
//   OpenLieroX
//
//   Auxiliary Software class library
//
//   based on the work of JasonB
//   enhanced by Dark Charlie and Albert Zeyer
//
//   code under LGPL
//
/////////////////////////////////////////


// Auxiliary library
// Created 12/11/01
// By Jason Boettcher

#include "defs.h"
#include <SDL/SDL_syswm.h>
#include "LieroX.h"
#include "CServer.h"
#include "Menu.h"
#include "GfxPrimitives.h"
#include "Cache.h"
#include "FindFile.h"

// Game info
tString	GameName;
int         nFocus = true;
bool		bActivated = false;

// Config file
tString	ConfigFile;

// Keyboard, Mouse, & Event
keyboard_t	Keyboard;
mouse_t		Mouse;
SDL_Event	Event;

// Screen

SDL_Surface *bmpIcon=NULL;


///////////////////
// Initialize the standard Auxiliary Library
int InitializeAuxLib(const tString& gname, const tString& config, int bpp, int vidflags)
{
	// Set the game info
	GameName=gname;

	ConfigFile=config;

	// Solves problem with FPS in fullscreen
#ifdef WIN32
	SDL_putenv("SDL_VIDEODRIVER=directx");
#endif

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) == -1) {
		SystemError("Failed to initialize the SDL system!\nErrorMsg: %s",SDL_GetError());
#ifdef WIN32
		// retry it with any available video driver	
		SDL_putenv("SDL_VIDEODRIVER=");
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) != -1)
			printf("... but we still have success with the any driver\n");
		else
#endif		
		return false;
	}

	bmpIcon = SDL_LoadBMP("data/icon.bmp");
	if(bmpIcon)
		SDL_WM_SetIcon(bmpIcon,NULL);


	if(!SetVideoMode())
		return false;

    // Enable the system events
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);


    if( !nDisableSound ) {
	    // Initialize sound
		//if(!InitSoundSystem(22050, 1, 512)) {
		if(!InitSoundSystem(44100, 1, 512)) {
		    printf("Warning: Failed the initialize the sound system\n");
		}
    }

	if( tLXOptions->iSoundOn ) {
		StartSoundSystem();
		SetSoundVolume( tLXOptions->iSoundVolume );
	}
	else
		StopSoundSystem();


	// Give a seed to the random number generator
	srand((unsigned int)time(NULL));


	// Initialize the cache
	if(!InitializeCache())
		return false;



	// Initialize the keyboard & mouse
	ProcessEvents();
	for(int k = 0;k<SDLK_LAST;k++)
		Keyboard.KeyUp[k] = false;

	Mouse.Button = 0;
	Mouse.Up = 0;

	return true;
}


///////////////////
// Set the video mode
int SetVideoMode(void)
{
	// TODO: Use DOUBLEBUF and hardware surfaces
	int HardwareBuf = false;
	int DoubleBuf = false;
	int vidflags = 0;

	// Check that the bpp is valid
	switch (tLXOptions->iColourDepth) {
	case 0:
	case 16:
	case 24:
	case 32:
		{} break;
	default: tLXOptions->iColourDepth = 16;
	}

	// BlueBeret's addition (2007): OpenGL support
	int opengl = tLXOptions->iOpenGL;

	// Initialize the video
	if(tLXOptions->iFullscreen)  {
		vidflags |= SDL_FULLSCREEN;
	}

	if (opengl) {
		printf("HINT: using OpenGL\n");
		vidflags |= SDL_OPENGLBLIT;
		SDL_GL_SetAttribute (SDL_GL_RED_SIZE,   5);
		SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE,  5);
		SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, DoubleBuf);
	}

	if(HardwareBuf)  {
		vidflags |= SDL_HWSURFACE;
		if (tLXOptions->iFullscreen)
			iSurfaceFormat = SDL_HWSURFACE;
	}
	else  {
		vidflags |= SDL_SWSURFACE;
		iSurfaceFormat = SDL_SWSURFACE;
	}

	if(DoubleBuf)
		vidflags |= SDL_DOUBLEBUF;


	if( SDL_SetVideoMode(640,480, tLXOptions->iColourDepth,vidflags) == NULL) {
		SystemError("Failed to set the video mode %dx%dx%d\nErrorMsg: %s", 640, 480, tLXOptions->iColourDepth,SDL_GetError());
		return false;
	}


	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(200,20);
	SDL_WM_SetCaption(GameName.c_str(),NULL);
	SDL_ShowCursor(SDL_DISABLE);

#ifdef WIN32
	// Hint: Reset the mouse state - this should avoid the mouse staying pressed
	Mouse.Button = 0;
	Mouse.Down = 0;
	Mouse.FirstDown = 0;
	Mouse.Up = 0;

	if (!tLXOptions->iFullscreen)  {
		SubclassWindow();
	}
	else {
		UnSubclassWindow();
	}
#endif

	return true;
}

#ifdef WIN32
//////////////////////
// Get the window handle
HWND GetWindowHandle(void)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if(!SDL_GetWMInfo(&info))
		return 0;

	return info.window;
}
#endif

///////////////////
// Process the events
void ProcessEvents(void)
{
    // Clear the queue
    Keyboard.queueLength = 0;

	// Reset mouse wheel
	Mouse.WheelScrollUp = 0;
	Mouse.WheelScrollDown = 0;

	bActivated = false;

	while(SDL_PollEvent(&Event)) {

        // Quit event
		if(Event.type == SDL_QUIT) {
			// Quit
			tLX->iQuitGame = true;
			tLX->iQuitEngine = true;
			tMenu->iMenuRunning = false;
		}

		// Mouse wheel scroll
		if(Event.type == SDL_MOUSEBUTTONDOWN)  {
			switch(Event.button.button){
				case SDL_BUTTON_WHEELUP:
					Mouse.WheelScrollUp = true;
					break;
				case SDL_BUTTON_WHEELDOWN:
					Mouse.WheelScrollDown  = true;
					break;
			}  // switch
		 }  // if


#ifdef WIN32
        // System events
        if(Event.type == SDL_SYSWMEVENT) {
            SDL_SysWMmsg *msg = Event.syswm.msg;

            switch(msg->msg) {

                // Lose focus event
                case WM_KILLFOCUS:
			        nFocus = false;
                    break;

                // Gain focus event
                case WM_SETFOCUS:  {
						// Hint: Reset the mouse state - this should avoid the mouse staying pressed
						Mouse.Button = 0;
						Mouse.Down = 0;
						Mouse.FirstDown = 0;
						Mouse.Up = 0;

						nFocus = true;
						bActivated = true;
					}
                    break;
            }
        }
#else
	// TODO: ignore it?
#endif

        // Keyboard events
		if(Event.type == SDL_KEYUP || Event.type == SDL_KEYDOWN) {

			// Check the characters
			if(Event.key.state == SDL_PRESSED || Event.key.state == SDL_RELEASED) {

				int input = (Event.key.keysym.unicode);
				if ((char)input == 0)
					switch (Event.key.keysym.sym) {
					case SDLK_HOME:
						input = 2;
						break;
					case SDLK_END:
						input = 3;
						break;
					case SDLK_KP0:
					case SDLK_KP1:
					case SDLK_KP2:
					case SDLK_KP3:
					case SDLK_KP4:
					case SDLK_KP5:
					case SDLK_KP6:
					case SDLK_KP7:
					case SDLK_KP8:
					case SDLK_KP9:
					case SDLK_KP_MULTIPLY:
					case SDLK_KP_MINUS:
					case SDLK_KP_PLUS:
					case SDLK_KP_EQUALS:
						input = (char) (Event.key.keysym.sym - 208);
						break;
					case SDLK_KP_PERIOD:
					case SDLK_KP_DIVIDE:
						input = (char) (Event.key.keysym.sym - 220);
						break;
					case SDLK_KP_ENTER:
						input = '\r';
						break;
                    default: // these are a lot; comment out and activate warnings to list them
                        // nothing
                        // TODO: is that correct?
                        break;
				}  // switch

                // If we're going to over the queue length, shift the list down and remove the oldest key
                if(Keyboard.queueLength+1 >= MAX_KEYQUEUE) {
                    for(int i=0; i<Keyboard.queueLength-1; i++)
                        Keyboard.keyQueue[i] = Keyboard.keyQueue[i+1];
                    Keyboard.queueLength--;
                }

                // Key down
                if(Event.type == SDL_KEYDOWN) {
                    Keyboard.keyQueue[Keyboard.queueLength++] = input;
                }

				// Key up
				if(Event.type == SDL_KEYUP || Event.key.state == SDL_RELEASED)
                    Keyboard.keyQueue[Keyboard.queueLength++] = -input;


            }
        }
	}


    // If we don't have focus, don't update as often
    if(!nFocus)
        SDL_Delay(14);


	// Mouse
	Mouse.Button = SDL_GetMouseState(&Mouse.X,&Mouse.Y);
	Mouse.Up = 0;
    Mouse.FirstDown = 0;

	// Left Mouse Button Up event
	if(!(Mouse.Button & SDL_BUTTON(SDL_BUTTON_LEFT)) && Mouse.Down & SDL_BUTTON(SDL_BUTTON_LEFT))
		Mouse.Up |= SDL_BUTTON(SDL_BUTTON_LEFT);

	// Right Mouse Button Up event
	if(!(Mouse.Button & SDL_BUTTON(SDL_BUTTON_RIGHT)) && Mouse.Down & SDL_BUTTON(SDL_BUTTON_RIGHT))
		Mouse.Up |= SDL_BUTTON(SDL_BUTTON_RIGHT);

	// First down
    for( int i=0; i<3; i++ ) {
        if( !(Mouse.Down & SDL_BUTTON(i)) && (Mouse.Button & SDL_BUTTON(i)) )
            Mouse.FirstDown |= SDL_BUTTON(i);
    }

	Mouse.Down = Mouse.Button;

    // SAFETY HACK: If we get any mouse presses, we must have focus
    if(Mouse.Down)  {
		if (!nFocus)
			bActivated = true;
        nFocus = true;
	}



	// Keyboard
	Keyboard.keys = SDL_GetKeyState(NULL);

	// Update the key up's
	for(int k=0;k<SDLK_LAST;k++) {
		Keyboard.KeyUp[k] = false;

		if(!Keyboard.keys[k] && Keyboard.KeyDown[k])
			Keyboard.KeyUp[k] = true;
		Keyboard.KeyDown[k] = Keyboard.keys[k];
	}
}


///////////////////
// Flip the screen
void FlipScreen(SDL_Surface *psScreen)
{
    // Take a screenshot?
    // We do this here, because there are so many graphics loops, but this function is common
    // to all of them
    if( cTakeScreenshot.isDownOnce() || cServer->getTakeScreenshot() )
        TakeScreenshot(tGameInfo.bTournament);

	/*if (tLXOptions->iOpenGL) SDL_GL_SwapBuffers();
    else*/ SDL_Flip( psScreen );
}


///////////////////
// Shutdown the standard Auxiliary Library
void ShutdownAuxLib(void)
{
	if(bmpIcon)
		SDL_FreeSurface(bmpIcon);

	QuitSoundSystem();

	// Shutdown the error system
	EndError();

#ifdef WIN32
	UnSubclassWindow();
#endif

	// Shutdown the cache
	ShutdownCache();

	// Shutdown the SDL system
	// this is a workaround to prevent the default segfault-routine
	try { SDL_Quit(); }
	catch(...) {
		printf("WARNING: ERROR while shutting down SDL\n");
	}
}



///////////////////
// Return the game name
tString GetGameName(void)
{
	return GameName;
}

///////////////////
// Return the keyboard structure
keyboard_t *GetKeyboard(void)
{
	return &Keyboard;
}

///////////////////
// Return the mouse structure
mouse_t *GetMouse(void)
{
	return &Mouse;
}

///////////////////
// Return the config filename
tString GetConfigFile(void)
{
	return ConfigFile;
}


///////////////////
// Return the event
SDL_Event *GetEvent(void)
{
	return &Event;
}


///////////////////
// Get text from the clipboard
// Returns the length of the text (0 for no text)
tString GetClipboardText() {
#ifdef WIN32
	tString szText;
    
    // Get the window handle
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if(!SDL_GetWMInfo(&info))
		return "";

	HWND    WindowHandle = info.window;
    HANDLE  CBDataHandle; // handle to the clipboard data
    LPSTR   CBDataPtr;    // pointer to data to send

    // Windows version
    if( IsClipboardFormatAvailable(CF_TEXT) ) {

        if( OpenClipboard(WindowHandle) ) {
            CBDataHandle = GetClipboardData(CF_TEXT);

            if(CBDataHandle) {
                CBDataPtr = (LPSTR)GlobalLock(CBDataHandle);
                szText = CBDataPtr;

                GlobalUnlock(CBDataHandle);
                CloseClipboard();

                return szText;
            }
        }
    }
#else
	// TODO: how to do on linux?
#endif

    return "";
}

///////////////////
// Set text to the clipboard
// Returns the length of the text (0 for no text)
int SetClipboardText(const tString& szText)
{
#ifdef WIN32
    // Get the window handle
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if(!SDL_GetWMInfo(&info))
		return 0;

	HWND    WindowHandle = info.window;

	// Open clipboard
	if(!OpenClipboard(WindowHandle))
		return 0;

	// Empty clipboard
	EmptyClipboard();


	// Allocate memory
	LPTSTR  lptstrCopy;
	int cch = szText.size();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));
    if (hglbCopy == NULL)
    {
        CloseClipboard();
        return 0;
    }

	// Copy the text to the memory
    lptstrCopy = (char *) GlobalLock(hglbCopy);
    memcpy(lptstrCopy, szText.c_str(), cch * sizeof(TCHAR));
    lptstrCopy[cch] = (TCHAR) 0;    // null character
    GlobalUnlock(hglbCopy);

	// Put to clipboard
	SetClipboardData(CF_TEXT, hglbCopy);

	// Close the clipboard
	CloseClipboard();

    return szText.size();
#else
	// TODO: what is with linux here?
	return 0;
#endif
}


///////////////////
// Take a screenshot
void TakeScreenshot(bool Tournament)
{
	static tString	picname;
	static tString	fullname;
	static tString	extension;
	int			i;

	// Set the extension
	switch (tLXOptions->iScreenshotFormat)  {
	case FMT_BMP: extension = ".bmp"; break;
	case FMT_PNG: extension = ".png"; break;
	case FMT_JPG: extension = ".jpg"; break;
	case FMT_GIF: extension = ".gif"; break;
	default: extension = ".png";
	}

	// Create the file name
    for(i=0; 1; i++) {
		picname = "lierox"+itoa(i)+extension;

		if (Tournament)
			fullname = "tourny_scrshots/"+picname;
		else
			fullname = "scrshots/" + picname;

		if (!IsFileAvailable(fullname,false))
			break;	// file doesn't exist
	}

	// Save the surface
	SaveSurface(SDL_GetVideoSurface(),fullname,tLXOptions->iScreenshotFormat,Tournament && cServer->getTakeScreenshot());
}

#ifdef WIN32
WNDPROC wpOriginal;
bool Subclassed = false;

////////////////////
// Subclass the window (control the incoming Windows messages)
void SubclassWindow(void)
{
	if (Subclassed)
		return;
	wpOriginal = (WNDPROC)SetWindowLong(GetWindowHandle(),GWL_WNDPROC,(LONG)&WindowProc);
	Subclassed = true;
}

////////////////////
// Remove the subclassing
void UnSubclassWindow(void)
{
	if (!Subclassed)
		return;
	SetWindowLong(GetWindowHandle(),GWL_WNDPROC, (LONG)wpOriginal);
	Subclassed = false;
}

/////////////////////
// Subclass callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Ignore the unwanted messages
	switch (uMsg)  {
	case WM_ENTERMENULOOP:
		return 0;
	case WM_INITMENU:
		return 0;
	case WM_MENUSELECT:
		return 0;
	case WM_SYSKEYUP:
		return 0;
	}

	return CallWindowProc(wpOriginal,hwnd,uMsg,wParam,lParam);
}
#endif
