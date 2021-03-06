// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.
#ifndef DEBUG_H_
#define DEBUG_H_

#include "Random.h"
#include "../libutil/src/Socket.h"
#include <list>

class DebugInfo : Socket
{
    public:
        DebugInfo();
        ~DebugInfo();

        bool Send(const void* buffer, int length);
        bool SendSigned(signed i);
        bool SendUnsigned(unsigned i);
        bool SendString(const char* str, unsigned len = 0);

#ifdef _MSC_VER
        bool SendStackTrace(LPCONTEXT ctx = NULL);
#else
        bool SendStackTrace();
#endif
        bool SendReplay();
        bool SendAsyncLog(std::list<RandomEntry>::iterator first_a, std::list<RandomEntry>::iterator first_b,
                          std::list<RandomEntry> &a, std::list<RandomEntry> &b, unsigned identical);
};

#endif

