#pragma once
#include <string>

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// #####
// #####	4. The code you submit should be production ready, whatever that means to you.
// #####
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// $$$$ SOLUTION
// $$$$ 
// Ezt is implementalni kene ahhoz hogy meg jobban crash rezisztens legyen a program,
// de ez most nincs kimunkalva
class ExtraValidationLayer
{
public:
	static bool IsFileSizeValid(unsigned int filesize)
	{
		// 32-64 bit alatt maskepp kell kezelni a nagy fileokat ...
		return false;
	}

	static bool MemoryAllocator()
	{
		// a temporalis heap objektumokat egy memory mamangeren keresztul kene foglalni es felszabaditani es ha elfogy a memoria, akkor
		// szinten throw-olni valami hibauzenetet hogy a user vegyen tobb RAM-ot
		return false;
	}

	static bool IsFilenameValid(const std::string& filename)
	{
		// megnezzuk hogy egyaltalan ervenyes filenevet adtunk-e meg
		return true;
	}

};
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------