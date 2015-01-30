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

#ifndef iplclasses_h
#define iplclasses_h

#include <GraphUnidraw/graphclasses.h>

/* 
 * Unique IplServ class identifiers.
 */

#define CONN_COMP       9900
#define PIPE_COMP       9901
#define ARBITER_COMP    9902
#define PIPES_COMP      9903
#define INVOPIPES_COMP  9904
#define INVO_COMP       9905
#define FORK_COMP       9906
#define ALU_COMP        9907
#define IAD_COMP        9908
#define OAD_COMP        9909
#define IPL_IDRAW_COMP  9910
#define DISTANT_COMP    9911
#define MEM_COMP        9912
#define SQUARE_CMD      9923
#define REORIENT_TEXT_CMD 9924

/* Composite ids associating subjects with their views */
#define PIPE_SCRIPT        Combine(PIPE_COMP, SCRIPT_VIEW)
#define CONN_SCRIPT        Combine(CONN_COMP, SCRIPT_VIEW)
#define ARBITER_SCRIPT     Combine(ARBITER_COMP, SCRIPT_VIEW)
#define INVO_SCRIPT        Combine(INVO_COMP, SCRIPT_VIEW)
#define FORK_SCRIPT        Combine(FORK_COMP, SCRIPT_VIEW)
#define ALU_SCRIPT         Combine(ALU_COMP, SCRIPT_VIEW)
#define IAD_SCRIPT         Combine(IAD_COMP, SCRIPT_VIEW)
#define OAD_SCRIPT         Combine(OAD_COMP, SCRIPT_VIEW)
#define IPL_IDRAW_SCRIPT   Combine(IPL_IDRAW_COMP, SCRIPT_VIEW)
#define DISTANT_SCRIPT     Combine(DISTANT_COMP, SCRIPT_VIEW)
#define MEM_SCRIPT         Combine(MEM_COMP, SCRIPT_VIEW)


/* none yet */
#endif
