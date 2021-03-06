#define HELP_DEFINITION
#include "UtilityClass.h"

#include <SDL.h>
#include <sstream>

static const char* GCChar(const SDL_GameControllerButton button)
{
	switch (button)
	{
	case SDL_CONTROLLER_BUTTON_A:
		return "A";
	case SDL_CONTROLLER_BUTTON_B:
		return "B";
	case SDL_CONTROLLER_BUTTON_X:
		return "X";
	case SDL_CONTROLLER_BUTTON_Y:
		return "Y";
	case SDL_CONTROLLER_BUTTON_BACK:
		return "BACK";
	case SDL_CONTROLLER_BUTTON_GUIDE:
		return "GUIDE";
	case SDL_CONTROLLER_BUTTON_START:
		return "START";
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		return "L3";
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
		return "R3";
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		return "LB";
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		return "RB";
	default:
		SDL_assert(0 && "Unhandled button!");
		return NULL;
	}
}

int ss_toi(const std::string& str)
{
	int retval = 0;
	bool negative = false;
	static const int radix = 10;

	for (size_t i = 0; i < str.size(); ++i)
	{
		const char chr = str[i];

		if (i == 0 && chr == '-')
		{
			negative = true;
			continue;
		}

		if (SDL_isdigit(chr))
		{
			retval *= radix;
			retval += chr - '0';
		}
		else
		{
			break;
		}
	}

	if (negative)
	{
		return -retval;
	}

	return retval;
}

std::vector<std::string> split( const std::string &s, char delim, std::vector<std::string> &elems )
{
	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split( const std::string &s, char delim )
{
	std::vector<std::string> elems;
	return split(s, delim, elems);
}

UtilityClass::UtilityClass() :
glow(0),
	glowdir(0)
{
	for (size_t i = 0; i < SDL_arraysize(splitseconds); i++)
	{
		splitseconds[i] = (i * 100) / 30;
	}

	slowsine = 0;
}

std::string UtilityClass::String( int _v )
{
	std::ostringstream os;
	os << _v;
	return(os.str());
}

int UtilityClass::Int(const char* str, int fallback /*= 0*/)
{
	if (!is_number(str))
	{
		return fallback;
	}

	return (int) SDL_strtol(str, NULL, 0);
}

std::string UtilityClass::GCString(const std::vector<SDL_GameControllerButton>& buttons)
{
	std::string retval = "";
	for (size_t i = 0; i < buttons.size(); i += 1)
	{
		retval += GCChar(buttons[i]);
		if ((i + 1) < buttons.size())
		{
			retval += ",";
		}
	}
	return retval;
}

std::string UtilityClass::twodigits( int t )
{
	if (t < 10)
	{
		return "0" + String(t);
	}
	if (t >= 100)
	{
		return "??";
	}
	return String(t);
}

std::string UtilityClass::timestring( int t )
{
	//given a time t in frames, return a time in seconds
	std::string tempstring = "";
	int temp = (t - (t % 30)) / 30;
	if (temp < 60)   //less than one minute
	{
		t = t % 30;
		tempstring = String(temp) + ":" + twodigits(splitseconds[t]);
	}
	else
	{
		int temp2 = (temp - (temp % 60)) / 60;
		temp = temp % 60;
		t = t % 30;
		tempstring = String(temp2) + ":" + twodigits(temp) + ":" + twodigits(splitseconds[t]);
	}
	return tempstring;
}

std::string UtilityClass::number( int _t )
{
	static const std::string ones_place[] = {"One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
	static const std::string tens_place[] = {"Ten", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
	static const std::string teens[] = {"Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};

	if (_t < 0)
	{
		return "???";
	}
	else if (_t > 100)
	{
		return "Lots";
	}
	else if (_t == 0)
	{
		return "Zero";
	}
	else if (_t == 100)
	{
		return "One Hundred";
	}
	else if (_t >= 1 && _t <= 9)
	{
		return ones_place[_t-1];
	}
	else if (_t >= 11 && _t <= 19)
	{
		return teens[_t-11];
	}
	else if (_t % 10 == 0)
	{
		return tens_place[(_t/10)-1];
	}
	else
	{
		return tens_place[(_t/10)-1] + " " + ones_place[(_t%10)-1];
	}
}

bool UtilityClass::intersects( SDL_Rect A, SDL_Rect B )
{
	return (SDL_HasIntersection(&A, &B) == SDL_TRUE);
}

void UtilityClass::updateglow()
{
	slowsine++;
	if (slowsine >= 64) slowsine = 0;

	if (glowdir == 0) {
		glow+=2;
		if (glow >= 62) glowdir = 1;
	}else {
		glow-=2;
		if (glow < 2) glowdir = 0;
	}
}

bool is_number(const char* str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (!SDL_isdigit(static_cast<unsigned char>(str[i])) && (i != 0 || str[0] != '-'))
		{
			return false;
		}
	}
	return true;
}

static bool VVV_isxdigit(const unsigned char digit)
{
	return (digit >= 'a' && digit <= 'z')
	|| (digit >= 'A' && digit <= 'Z')
	|| SDL_isdigit(digit);
}

bool is_positive_num(const std::string& str, bool hex)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (hex)
		{
			if (!VVV_isxdigit(static_cast<unsigned char>(str[i])))
			{
				return false;
			}
		}
		else
		{
			if (!SDL_isdigit(static_cast<unsigned char>(str[i])))
			{
				return false;
			}
		}
	}
	return true;
}

bool endsWith(const std::string& str, const std::string& suffix)
{
	if (str.size() < suffix.size())
	{
		return false;
	}
	return str.compare(
		str.size() - suffix.size(),
		suffix.size(),
		suffix
	) == 0;
}
