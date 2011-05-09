/*
 *      Copyright (C) 2005-2010 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "trace.h"

#include <stdio.h>

namespace ipcgenutils
{
#ifdef ENABLE_TRACE_API
  static TraceGuard* s_parent = NULL;

  static char** getSpacesArray(int size)
  {
    char** ret = new char*[size];
    for (int i = 0; i < size; i++)
    {
      ret[i] = new char[i + 1];

      int j;
      for (j = 0; j < i; j++)
        ret[i][j] = ' ';
      ret[i][j] = 0;
    }
    return ret;
  }

  static char** spaces = getSpacesArray(256);

  TraceGuard::TraceGuard(const char* _function) :function(_function) 
  {
    parent = s_parent;
    depth = parent == NULL ? 0 : parent->depth + 1;
    s_parent = this;

    printf("%s TRACE Entering %s\n", spaces[depth], function);
  }

  TraceGuard::~TraceGuard() 
  {
    printf("%s TRACE Leaving %s\n", spaces[depth], function);

    s_parent = parent;
  }
#endif
}
