/////////////////////////////////////////
//
//             OpenLieroX
//
// code under LGPL, based on JasonBs work,
// enhanced by Dark Charlie and Albert Zeyer
//
//
/////////////////////////////////////////


// Input box
// Created 31/7/02
// Jason Boettcher


#ifndef __CINPUTBOX_H__
#define __CINPUTBOX_H__


// Inputbox events
enum {
	INB_NONE=-1,
	INB_MOUSEUP
};


// inputbox messages
enum {
	INM_GETVALUE=0,
	INS_GETTEXT
};


class CInputbox : public CWidget {
public:
	// Constructor
	CInputbox(int val, const tString& _text, SDL_Surface *img, const tString& name) {
		iKeyvalue = val;
		sText = _text;
		sName = name;

		bmpImage = img;
		iType = wid_Inputbox;
		iMouseOver = false;
	}


private:
	// Attributes

	int			iKeyvalue;
	tString	sText;
	SDL_Surface	*bmpImage;
	int			iMouseOver;
	tString	sName;


public:
	// Methods

	void	Create(void) { }
	void	Destroy(void) { }

	//These events return an event id, otherwise they return -1
	inline int		MouseOver(mouse_t *tMouse)			{ iMouseOver = true; return INB_NONE; }
	inline int		MouseUp(mouse_t *tMouse, int nDown)		{ return INB_MOUSEUP; }
	inline int		MouseDown(mouse_t *tMouse, int nDown)	{ return INB_NONE; }
	inline int		MouseWheelDown(mouse_t *tMouse)		{ return INB_NONE; }
	inline int		MouseWheelUp(mouse_t *tMouse)		{ return INB_NONE; }
	inline int		KeyDown(int c)						{ return INB_NONE; }
	inline int		KeyUp(int c)						{ return INB_NONE; }	


	// Process a message sent
	inline DWORD SendMessage(int iMsg, DWORD Param1, DWORD Param2) {

				if(iMsg == INM_GETVALUE) {
						return iKeyvalue;
				}

				return 0;
			}

	DWORD SendMessage(int iMsg, const tString& sStr, DWORD Param) { return 0; }
	DWORD SendMessage(int iMsg, tString *sStr, DWORD Param)  {
		if (iMsg == INS_GETTEXT)  {
			*sStr = sText;
		}
		return 0;
	}

	// Draw the title button
	void	Draw(SDL_Surface *bmpDest);

	inline void	LoadStyle(void) {}


	inline int		getValue(void)						{ return iKeyvalue; }
	inline void	setValue(int _v)					{ iKeyvalue = _v; }
	inline tString	getText(void)				{ return sText; }
	inline void	setText(const tString& _t)		{ sText = _t; }
	inline tString	getName(void)				{ return sName; }

};



#endif  //  __CINPUTBOX_H__
