/*

Copyright 2016 Aleks Berg-Jones

This file is part of Shadows Spider.

Shadows Spider is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Shadows Spider is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Shadows Spider.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "StringManip.h"

string stringToUpper(string myString)
{
	auto out = myString;

	for (unsigned int i = 0; i < out.length(); ++i)
		out[i] = toupper(out[i]);

	return out;
}

vector<string> stringSplit(string data, string token)
{
	vector<string> output;
	auto pos = string::npos;

	do
	{
		pos = data.find(token);
		output.push_back(data.substr(0, pos));

		if (string::npos != pos)
			data = data.substr(pos + token.size());

	} while (string::npos != pos);

	return output;
}