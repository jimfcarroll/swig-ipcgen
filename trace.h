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

#pragma once

#define ENABLE_TRACE_API

/**
 * This file contains the public definitions for the Addon api. It's meant to be used
 * by those writing language bindings.
 */
namespace ipcgenutils
{
#ifdef ENABLE_TRACE_API
  class TraceGuard
  {
    // this is NOT thread safe.

    const char* function;
  public:
    TraceGuard* parent;
    int depth;

    TraceGuard(const char* _function);
    ~TraceGuard();
  };
#endif
}

#ifdef ENABLE_TRACE_API
#define TRACE ipcgenutils::TraceGuard _tg(__PRETTY_FUNCTION__)
#else
#define TRACE
#endif


