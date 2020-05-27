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

/*
 * IplViewer - a OverlayViewer with support for ipl operations
 */

#ifndef iplviewer_h
#define iplviewer_h

#include <FrameUnidraw/frameviewer.h>

#include <IV-2_6/_enter.h>

class OverlayComp;
class OverlayPage;

class IplViewer : public FrameViewer {
public:
    IplViewer(
        Editor*, GraphicView*, UPage*, Grid* = nil, 
        Coord = 0, Coord = 0, Orientation = Normal,
	Alignment = Center, Zooming = Continuous
    );
    virtual void Draw();
    virtual void Redraw(Coord, Coord, Coord, Coord);

    void SetMagLimits(float lo, float hi);
    void GetMagLimits(float& lo, float& hi);
protected:
    virtual float LimitMagnification(float);

    float _himagstop;
    float _lomagstop;
};

#include <IV-2_6/_leave.h>

#endif
