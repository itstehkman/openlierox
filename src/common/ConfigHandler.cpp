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


// Config file handler
// Created 30/9/01
// By Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "FindFile.h"


int			NumKeywords = 0;
keyword_t	Keywords[MAX_KEYWORDS];


///////////////////
// Add a keyword to the list
int AddKeyword(const tString& key, int value)
{
	// Check for enough spaces
	if(NumKeywords >= MAX_KEYWORDS-1)
		return false;

	Keywords[NumKeywords].key = key;
	Keywords[NumKeywords++].Value = value;

	return true;
}



///////////////////
// Read a keyword from a file
int ReadKeyword(const tString& filename, const tString& section, const tString& key, int *value, int defaultv)
{
	int n;
	static tString string;

	*value = defaultv;

	if(!GetString(filename,section,key,string))
		return false;

	// Try and find a keyword with matching keys
	for(n=0;n<NumKeywords;n++) {
		if(stringcasecmp(string,Keywords[n].key) == 0) {
			*value = Keywords[n].Value;
			return true;
		}
	}

	return false;
}

///////////////////
// Read a keyword from a file (bool version)
bool ReadKeyword(const tString& filename, const tString& section, const tString& key, bool *value, bool defaultv)
{
	int n;
	static tString string;

	*value = defaultv;

	if(!GetString(filename,section,key,string))
		return false;

	// Try and find a keyword with matching keys
	for(n=0;n<NumKeywords;n++) {
		if(stringcasecmp(string,Keywords[n].key) == 0) {
			*value = Keywords[n].Value != 0;
			return true;
		}
	}

	return false;
}


///////////////////
// Read an interger from a file
int ReadInteger(const tString& filename, const tString& section, const tString& key, int *value, int defaultv)
{
	static tString string;

	*value = defaultv;

	if(!GetString(filename,section,key,string))
		return false;

	*value = from_string<int>(string);

	return true;
}


///////////////////
// Read a string from a file
int ReadString(const tString& filename, const tString& section, const tString& key, tString& value, const tString& defaultv)
{
	value = defaultv;

	return GetString(filename,section,key,value);

	/*int result = GetString(filename,section,key,value);

	if (strlen(value) <= 0)
		strcpy(value,defaultv);

	return result;*/
}


///////////////////
// Read a float from a file
int ReadFloat(const tString& filename, const tString& section, const tString& key, float *value, float defaultv)
{
	static tString string;

	*value = defaultv;

	if(!GetString(filename,section,key,string))
		return false;

	*value = (float)atof(string);

	return true;
}


//////////////////
// Read a colour
int ReadColour(const tString& filename, const tString& section, const tString& key, Uint32 *value, Uint32 defaultv)
{
	static tString string;

	*value = defaultv;

	if(!GetString(filename,section,key,string))
		return false;

	*value = StrToCol(string);

	return true;

}

//////////////////
// Reads an array of integers
int ReadIntArray(const tString& filename, const tString& section, const tString& key, int *array, int num_items)
{
	tString string;

	if (!GetString(filename,section,key,string))
		return false;

	const std::vector<tString>& arr = explode(string,",");
	for (register unsigned int i=0; i<MIN(num_items,arr.size()); i++)
		array[i] = from_string<int>(arr[i]);

	return num_items == (int)arr.size();
}



///////////////////
// Read a string
int GetString(const tString& filename, const tString& section, const tString& key, tString& string)
{
	FILE	*config;
	static tString	Line;
	static tString	tmpLine;
	static tString	curSection;
	static tString	temp;
	static tString	curKey;
	size_t	chardest = 0;
	int		Position;
	int		found = false;

	if(filename == "")
		return false;

	config = OpenGameFile(filename,"rt");
	if(!config)
		return false;

	string="";
	curSection="";
	temp="";
	curKey="";


	while(!feof(config))
	{
		// Parse the lines
		Line = ReadUntil(config, '\n');
		TrimSpaces(Line);

		///////////////////
		// Comment, Ignore
		if(Line.size() == 0 || Line[0] == '#')
			continue;

		////////////
		// Sections
		if(Line[0] == '[' && Line[Line.size()-1] == ']')
		{
			curSection = Line.substr(1);
			curSection.erase(curSection.size()-1);
			continue;
		}

		////////
		// Keys
		chardest = Line.find('=');
		if(chardest != tString::npos)
		{
			// Key
			Position = chardest;
			tmpLine = Line;
			tmpLine.erase(Position);
			TrimSpaces(tmpLine);
			curKey = tmpLine;

			// Check if this is the key were looking for under the section were looking for
			if(stringcasecmp(curKey,key) == 0 && stringcasecmp(curSection,section) == 0)
			{
				// Get the value
				tmpLine = Line.substr(Position+1);
				TrimSpaces(tmpLine);
				string = tmpLine;
				found = true;
				break;
			}
			continue;
		}
	}

	fclose(config);

	return found;
}


