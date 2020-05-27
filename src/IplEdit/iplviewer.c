/*
 * Copyright 2014-2018 Vectaport Consulting
 * Copyright 2005-2008 Theseus Research Inc.
 * 
 * This file may be used under the terms of the GNU General Public
 * License version 2.0 as published by the Free Software Foundation                                
 * and appearing in the file LICENSE.GPL included in the packaging of
 * this file. For commercial licensing contact info@vectaport.com
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND
 *
 */

#include <IplServ/iplcomps.h>
#include <IplEdit/ipleditor.h>
#include <IplEdit/iplviewer.h>

#include <OverlayUnidraw/ovdamage.h>
#include <OverlayUnidraw/ovselection.h>

#include <InterViews/cursor.h>
#include <InterViews/window.h>

#include <IV-2_6/InterViews/painter.h>

/*****************************************************************************/

IplViewer::IplViewer (
    Editor* ed, GraphicView* gv, UPage* page, Grid* grid, 
    Coord w, Coord h, Orientation orientation,
    Alignment align, Zooming zoom
) : FrameViewer(ed, gv, page, grid, w, h, orientation, align, zoom)
{
  _lomagstop = 1./512.;
  _himagstop = 512.;
}

float IplViewer::LimitMagnification (float desired) {
    const float lo = _lomagstop; 
    const float hi = _himagstop; 

    return (desired < lo) ? lo : (desired > hi) ? hi : desired;
}

void IplViewer::SetMagLimits(float lo, float hi) {
  if (lo<hi) {
    _lomagstop = lo;
    _himagstop = hi;
  }
}

void IplViewer::GetMagLimits(float& lo, float& hi) {
  lo = _lomagstop;
  hi = _himagstop;
}

void IplViewer::Draw() 
{
    OverlaySelection* s = (OverlaySelection*)GetSelection();

    _editor->GetWindow()->cursor(hourglass);
    StartBuffering();
    s->ShowHighlights(this);
    GraphicBlock::Draw();
    FinishBuffering(true);

    s->Init(this);
    s->ShowHandles(this);
    _editor->GetWindow()->cursor(arrow);

    _damage->Reset();
}

void IplViewer::Redraw(Coord x0, Coord y0, Coord x1, Coord y1) 
{
    OverlaySelection* s = (OverlaySelection*)GetSelection();

    _editor->GetWindow()->cursor(hourglass);
    StartBuffering();
    s->ShowHighlights(this);
    GraphicBlock::Redraw(x0, y0, x1, y1);
    FinishBuffering(true);

    xorPainter->Clip(canvas, x0, y0, x1, y1);
    s->ShowHandles(this);
    xorPainter->NoClip();
    _editor->GetWindow()->cursor(arrow);
}

