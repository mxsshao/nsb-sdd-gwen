/*
 *  GWEN
 *  Copyright (c) 2010 Facepunch Studios
 *  See license in Gwen.h
 */

#pragma once
#ifndef GWEN_CONTROLS_RESIZABLECONTROL_H
#define GWEN_CONTROLS_RESIZABLECONTROL_H

#include "Gwen/Controls/Base.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/Button.h"
#include "Gwen/Controls/Dragger.h"
#include "Gwen/Controls/Label.h"
#include "Gwen/Controls/Resizer.h"
#include "Gwen/Gwen.h"
#include "Gwen/Skin.h"

namespace Gwen
{
    namespace Controls
    {
        class GWEN_EXPORT ResizableControl : public Base
        {
        public:

            GWEN_CONTROL(ResizableControl, Base);

            virtual void SetClampMovement(bool shouldClamp)
            {
                m_bClampMovement = shouldClamp;
            }

            virtual bool GetClampMovement()
            {
                return m_bClampMovement;
            }

            virtual void SetMinimumSize(const Gwen::Point& minSize)
            {
                m_MinimumSize = minSize;
            }

            virtual Gwen::Point GetMinimumSize()
            {
                return m_MinimumSize;
            }

            virtual void DisableResizing();

            virtual bool SetBounds(int x, int y, int w, int h);

            virtual void OnResized()
            {
            }

            Event::Caller onResize;
			Event::Caller onResizeUp;

            virtual ControlsInternal::Resizer* GetResizer(int iResizer)
            {
                return m_Resizer[iResizer];
            }

			bool m_bDepressed;

			virtual void OnMouseClickLeft(int x, int y, bool bDown)
			{
				if (!bDown && m_bDepressed)
				{
					onResizeUp.Call(this);
					m_bDepressed = false;
				}
			}
        protected:

            void OnResizedInternal(Controls::Base* pControl);

            Gwen::Point m_MinimumSize;
            bool m_bClampMovement;
            bool m_bResizable;

            ControlsInternal::Resizer* m_Resizer[10];


        };


    }
}
#endif // ifndef GWEN_CONTROLS_RESIZABLECONTROL_H
