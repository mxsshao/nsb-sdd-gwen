/*
 *  GWEN
 *  Copyright (c) 2010 Facepunch Studios
 *  See license in Gwen.h
 */

#pragma once
#ifndef GWEN_TEXTURE_H
#define GWEN_TEXTURE_H

#include <string>

#include "Gwen/BaseRender.h"
#include "allegro5/allegro.h"

namespace Gwen
{
    //
    // Texture
    //
    struct Texture
    {
        typedef std::list<Texture*> List;

        TextObject name;
        void*   data;
        bool failed;
        int width;
        int height;

        Texture()
        :   data(NULL)
        ,   width(4)
        ,   height(4)
        ,   failed(false)
        {
        }

        ~Texture()
        {
        }

        void Load(const TextObject& str, Gwen::Renderer::Base* render)
        {
            name = str;
            Gwen::Debug::AssertCheck(render != NULL, "No renderer!");
            render->LoadTexture(this);
        }

		void Load(ALLEGRO_BITMAP* bitmap, Gwen::Renderer::Base* render)
        {
            name = "bitmap";
            Gwen::Debug::AssertCheck(render != NULL, "No renderer!");
            render->LoadTexture(this, bitmap);
        }

        void Release(Gwen::Renderer::Base* render)
        {
            render->FreeTexture(this);
        }

        bool FailedToLoad() const
        {
            return failed;
        }

    };


}
#endif // ifndef GWEN_TEXTURE_H
