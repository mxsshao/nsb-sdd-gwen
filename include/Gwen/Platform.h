/*
 *  GWEN
 *  Copyright (c) 2010 Facepunch Studios
 *  See license in Gwen.h
 */

#pragma once
#ifndef GWEN_PLATFORM_H
#define GWEN_PLATFORM_H

#include "Gwen/Structures.h"
#include "Gwen/Events.h"
#include "allegro5/allegro.h"

namespace Gwen
{
    class Platform
    {
	public:
		static ALLEGRO_DISPLAY* g_display;
        //
        // Do nothing for this many milliseconds
        //
        static void Sleep(unsigned int iMS);

        //
        // Set the system cursor to iCursor
        // Cursors are defined in Structures.h
        //
        static void SetCursor(unsigned char iCursor);

        //
        //
        //
        static void GetCursorPos(Gwen::Point& p);
        static void GetDesktopSize(int& w, int& h);

        //
        // Used by copy/paste
        //
        static UnicodeString GetClipboardText();
        static bool          SetClipboardText(const UnicodeString& str);

        //
        // Needed for things like double click
        //
        static float GetTimeInSeconds();

        //
        // System Dialogs ( Can return false if unhandled )
        //
        static bool FileOpen(const String& Name, const String& StartPath,
                                  const String& Extension, Gwen::Event::Handler* pHandler,
                                  Event::Handler::FunctionWithInformation fnCallback);
        static bool FileSave(const String& Name, const String& StartPath,
                                  const String& Extension, Gwen::Event::Handler* pHandler,
                                  Event::Handler::FunctionWithInformation fnCallback);
        static bool FolderOpen(const String& Name, const String& StartPath,
                                    Gwen::Event::Handler* pHandler,
                                    Event::Handler::FunctionWithInformation fnCallback);


        //
        // Window Creation
        //
        static void* CreatePlatformWindow(int x, int y, int w, int h,
                                               const Gwen::String& strWindowTitle);
        static void DestroyPlatformWindow(void* pPtr);
        static void SetBoundsPlatformWindow(void* pPtr, int x, int y, int w, int h);
        static void MessagePump(void* pWindow, Gwen::Controls::Canvas* ptarget);
        static bool HasFocusPlatformWindow(void* pPtr);
        static void SetWindowMaximized(void* pPtr, bool bMaximized, Gwen::Point& pNewPos,
                                            Gwen::Point& pNewSize);
        static void SetWindowMinimized(void* pPtr, bool bMinimized);
    };

}
#endif // ifndef GWEN_PLATFORM_H
