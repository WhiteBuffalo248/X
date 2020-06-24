#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "Imports.h"
#include <map>
using namespace std;
using namespace Gdiplus;

#define Config() (Configuration::instance())
#define ConfigColor(x) (Config()->getColor(x))
#define ConfigBool(x,y) (Config()->getBool(x,y))
#define ConfigUint(x,y) (Config()->getUint(x,y))
#define ConfigFont(x) (Config()->getFont(x))

class Configuration
{
public:
	//Structures
	/*
	struct Shortcut
	{
		CString Name;
		CKeyFrame Hotkey;
		bool GlobalShortcut;

		Shortcut(CString name, CString hotkey, bool global = false)
			: Name(name), Hotkey(hotkey), GlobalShortcut(global) { }

		Shortcut(std::initializer_list<CString> names, CString hotkey = QString(), bool global = false)
			: Shortcut(CStringList(names).join(" -> "), hotkey, global) { }
	};
	*/
	static Configuration* mPtr;
	//Functions
	Configuration();
	~Configuration();
	static Configuration* instance();

	void save();
	void load();
	void readUints();
	void writeUints();
	void readColors();
	void writeFonts();
	void readFonts();

	

	const Color getColor(const string id) const;
	const bool getBool(const string category, const string id) const;
	void setUint(const string category, const string id, const duint i);
	const duint getUint(const string category, const string id) const;
	const LOGFONT getFont(const string id) const;
	LOGFONT fontFromConfig(const string id);
	bool fontToConfig(const string id, const LOGFONT& font);
	
	mutable bool noMoreMsgbox = false;

	//default setting maps
	map<string, Color> defaultColors;
	map<string, map<string, bool>> defaultBools;
	map<string, map<string, duint>> defaultUints;	
	map<string, LOGFONT> defaultFonts;
	//map<CString, Shortcut> defaultShortcuts;

	//public variables
	map<string, Color> Colors;
	map<string, map<string, bool>> Bools;
	map<string, map<string, duint>> Uints;
	map<string, LOGFONT> Fonts;

private:
	

	bool uintToConfig(const string category, const string id, duint i);
	duint uintFromConfig(const string category, const string id);
	Color colorFromConfig(const string id);
	bool colorToConfig(const string id, const Color color);

};

#endif // CONFIGURATION_H