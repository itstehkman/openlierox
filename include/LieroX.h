/////////////////////////////////////////
//
//             OpenLieroX
//
// code under LGPL, based on JasonBs work,
// enhanced by Dark Charlie and Albert Zeyer
//
//
/////////////////////////////////////////


// Game header file
// Created 28/6/02
// Jason Boettcher


#ifndef __LIEROX_H__
#define __LIEROX_H__

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG
#endif // _MSC_VER

#if DEBUG == 1
#define		_AI_DEBUG
#endif

#define		LX_PORT			23400
#define		SPAWN_HOLESIZE	4
#ifndef		LX_VERSION
#	define		LX_VERSION		"0.57_beta3"
#endif
#define		LX_ENDWAIT		9.0f


// Game types
enum {
	GMT_DEATHMATCH,
	GMT_TEAMDEATH,
	GMT_TAG,
    GMT_DEMOLITION
};


const float	D2R(1.745329e-2f); // degrees to radians
const float	R2D(5.729578e+1f); // radians to degrees

#define DEG2RAD(a)  (a * D2R)
#define RAD2DEG(a)  (a * R2D)


// Game includes
#include "ProfileSystem.h"
#include "Networking.h"
#include "CChatBox.h"
#include "Frame.h"
#include "CViewport.h"
#include "CSimulation.h"
#include "Command.h"
#include "CWorm.h"
#include "CProjectile.h"
#include "CShootList.h"
#include "Entity.h"
#include "CWeather.h"
#include "Protocol.h"
#include "Options.h"


// LieroX structure
class lierox_t { public:
	float	fCurTime;
	float	fDeltaTime;
	CFont	cFont;
	CFont	cOutlineFont;
	CFont	cOutlineFontGrey;

	int		iQuitGame;
	int		iQuitEngine;

	int		debug_int;
	float	debug_float;
	CVec	debug_pos;

	// Default Colours
	Uint32			clNormalLabel;
	Uint32			clHeading;
	Uint32			clSubHeading;
	Uint32			clChatText;
	Uint32			clNetworkText;
	Uint32			clNormalText;
	Uint32			clNotice;
	Uint32			clDropDownText;
	Uint32			clDisabled;
	Uint32			clListView;
	Uint32			clTextBox;
	Uint32			clMouseOver;
	Uint32			clError;
	Uint32			clCredits1;
	Uint32			clCredits2;
	Uint32			clPopupMenu;
	Uint32			clWaiting;
	Uint32			clReady;
	Uint32			clPlayerName;
	Uint32			clBoxLight;
	Uint32			clBoxDark;
	Uint32			clWinBtnBody;
	Uint32			clWinBtnLight;
	Uint32			clWinBtnDark;
	Uint32			clMPlayerTime;
	Uint32			clMPlayerSong;

	Uint32			clPink;
	Uint32			clWhite;


	tString	debug_string;
};


// Game types
enum {
	GME_LOCAL=0,
	GME_HOST,
	GME_JOIN
};



// Game structure
class game_t { public:
	int			iGameType;		// Local, remote, etc
	int			iGameMode;		// DM, team DM, etc
	tString		sModName;
	tString		sMapname;
    tString        sPassword;
	tString		sModDir;
    maprandom_t sMapRandom;
	int			iLoadingTimes;
	tString		sServername;
	tString		sWelcomeMessage;
	bool		bRegServer;
	bool		bTournament;

	int			iLives;
	int			iKillLimit;
	int			iTimeLimit;
	int			iTagLimit;
	int			iBonusesOn;
	int			iShowBonusName;
	
	int			iNumPlayers;
	profile_t	*cPlayers[MAX_WORMS];
};


// TODO: move this somewhere else
// Game lobby structure
class game_lobby_t { public:
	int		nSet;
	int		nGameMode;
	int		nLives;
	int		nMaxWorms;
	int		nMaxKills;
	int		nLoadingTime;
	int		nBonuses;
	tString	szMapName;
	tString	szDecodedMapName;
	tString	szModName;
	tString	szModDir;
	bool	bHaveMap;
	bool	bHaveMod;
	bool	bTournament;
};



extern	lierox_t		*tLX;
extern	game_t			tGameInfo;
extern	CVec			vGravity;
extern  CInput			cTakeScreenshot;
extern  CInput			cSwitchMode;
extern	CInput			cToggleMediaPlayer;
extern  int				nDisableSound;
extern	bool			bActivated;

extern	tString		binary_dir;


// Main Routines
void    ParseArguments(int argc, char *argv[]);
int		InitializeLieroX(void);
void	StartGame(void);
void	ShutdownLieroX(void);
void	GameLoop(void);
void	QuittoMenu(void);
void	GotoLocalGameMenu(void);



// Miscellanous routines
float	GetFixedRandomNum(uchar index);
int		CheckCollision(CVec trg, CVec pos, uchar checkflags, CMap *map);
void	ConvertTime(float time, int *hours, int *minutes, int *seconds);
int 	CarveHole(CMap *cMap, CVec pos);
void	StripQuotes(char *dest, char *src);
void	StripQuotes(tString& str);
void    lx_strncpy(char *dest, char *src, int count);
bool    MouseInRect(int x, int y, int w, int h);
char    *StripLine(char *szLine);
char    *TrimSpaces(char *szLine);
void TrimSpaces(tString& szLine);
bool	replace(char *text, const char *what, const char *with, char *result);
bool	replace(const tString& text, const tString& what, const tString& with, tString& result);
bool	replace(tString& text, const tString& what, const tString& with);
tString replacemax(const tString& text, const tString& what, const tString& with, tString& result, int max);
tString replacemax(const tString& text, const tString& what, const tString& with, int max);
char	*strip(char *buf, int width);
tString strip(const tString& text, int width);
bool	stripdot(char *buf, int width);
bool stripdot(tString& text, int width);
char	*ucfirst(char *text);
void	ucfirst(tString& text);
void	stringtolower(tString& text);
tString	ReadUntil(const tString& text, char until_character = '\n');
tString	ReadUntil(FILE* fp, char until_character = '\n');
Uint32	StrToCol(const tString& str);
const char* sex(short wraplen = 0);
const std::vector<tString>& explode(const tString& str, const tString& delim);
tString freadstr(FILE *fp, size_t maxlen);
inline tString freadfixedcstr(FILE *fp, size_t maxlen) { return ReadUntil(freadstr(fp, maxlen), '\0'); }
size_t fwrite(const tString& txt, size_t len, FILE* fp);
size_t findLastPathSep(const tString& path);
void stringlwr(tString& txt);
bool strincludes(const tString& str, const tString& what);
short stringcasecmp(const tString& s1, const tString& s2);
tString GetFileExtension(const tString& filename);
void printf(const tString& txt);

template<typename T>
T from_string(const tString& s, std::ios_base& (*f)(std::ios_base&), bool& failed) {
#ifdef UNICODE
	std::wistringstream iss(s); 
#else
	std::istringstream iss(s); 
#endif
	T t;
	failed = (iss >> f >> t).fail();
	return t;
}

template<typename T>
T from_string(const tString& s, std::ios_base& (*f)(std::ios_base&)) {
#ifdef UNICODE
	std::wistringstream iss(s);
#else
	std::istringstream iss(s);
#endif
	T t;
	iss >> f >> t;
	return t;
}

template<typename T>
T from_string(const tString& s, bool& failed) {
#ifdef UNICODE
	std::wistringstream iss(s);
#else
	std::istringstream iss(s);
#endif
	T t;
	failed = (iss >> t).fail();
	return t;
}

template<typename T>
T from_string(const tString& s) {
#ifdef UNICODE
	std::wistringstream iss(s);
#else
	std::istringstream iss(s);
#endif
	T t;
	iss >> t;
	return t;
}

template<typename T>
tString to_string(T t) {
#ifdef UNICODE
	std::wostringstream iss(s);
#else
	std::ostringstream iss(s);
#endif
	iss << t << ends;
	return iss;
}


// tString itoa
inline tString itoa(int num,int base=10)  {
	// TODO: better!! (use ostringstream)
	static tChar buf[64];
	static tString ret;
#ifdef UNICODE
	ret = itow(num,buf,base);
#else
	ret = itoa(num,buf,base);
#endif
	fix_markend(buf);
	return ret;
}

inline int atoi(const tString& str)  { return from_string<int>(str);  }
inline float atof(const tString& str) { return from_string<float>(str);  }



class simple_reversestring_hasher { public:
	inline size_t operator() (const tString& str) const {
		tString::const_reverse_iterator pos = str.rbegin();
		unsigned short nibble = 0;
		size_t result = 0;
		for(; pos != str.rend() && nibble < sizeof(size_t)*2; pos++, nibble++)
			result += ((size_t)*pos % 16) << nibble*4;
		return result;
	}
};


// Useful XML functions
int		xmlGetInt(xmlNodePtr Node, const tString& Name);
float	xmlGetFloat(xmlNodePtr Node, const tString& Name);
Uint32	xmlGetColour(xmlNodePtr Node, const tString& Name);
void	xmlEntities(tString& text);

// Thread functions
#ifdef WIN32
void	nameThread(const DWORD threadId, const char *name);
#endif


#endif  //  __LIEROX_H__
