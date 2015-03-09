/*
 * Copyright 2005-2008 Theseus Research Inc., All Rights Reserved.
 * 
 * This file may be used under the terms of the GNU General Public
 * License version 2.0 as published by the Free Software Foundation                                
 * and appearing in the file LICENSE.GPL included in the packaging of
 * this file. For commercial licensing contact info@vectaport.com
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND
 *
 */

#include <OverlayUnidraw/ovclasses.h>

#include <IplEdit/iplabout.h>
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcmds.h>
#include <IplEdit/iplcreator.h>
#include <IplEdit/iplviews.h>
#include <IplServ/iplscripts.h>

#include <OverlayUnidraw/ovarrow.h>
#include <OverlayUnidraw/ovpspict.h>
#include <OverlayUnidraw/ovpsview.h>

#include <Unidraw/catalog.h>

/*****************************************************************************/

IplEditCreator::IplEditCreator () { }

void* IplEditCreator::Create (
    ClassId id, istream& in, ObjectMap* objmap, int objid
) {
    switch (id) {
        case IPLABOUT_CMD:         CREATE(IplAboutCmd, in,objmap,objid);

        default:                   return FrameCreator::Create(id, in,objmap,objid);
    }
}

void* IplEditCreator::Create (ClassId id) {
    if (id == IPL_IDRAW_VIEW)    return new IplIdrawView;
    if (id == IPL_IDRAW_PS)      return new OverlayIdrawPS;

    if (id == PIPE_VIEW)    return new PipeView;
    if (id == PIPE_PS)      return new PicturePS;

    if (id == CONN_VIEW)    return new ConnView;
    if (id == CONN_PS)      return new ArrowLinePS;

    if (id == ARBITER_VIEW)   return new ArbiterView;
    if (id == ARBITER_PS)     return new PicturePS;

    if (id == FORK_VIEW)    return new ForkView;
    if (id == FORK_PS)      return new PicturePS;

    if (id == INVO_VIEW)    return new InvoView;
    if (id == INVO_PS)      return new PicturePS;

#ifdef HAVE_ACE
    if (id == DISTANT_VIEW)    return new DistantView;
    if (id == DISTANT_PS)      return new PicturePS;
#endif

    return IplServCreator::Create(id);
}
