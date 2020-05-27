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
 * Implementation of SpriteQueue class.
 */

#include <IplServ/iplclasses.h>
#include <IplServ/iplcomps.h>

#include <Attribute/alist.h>
#include <Attribute/attrlist.h>
#include <Attribute/aliterator.h>
#include <IplServ/spritequeue.h>

#include <Unidraw/globals.h>
#include <Unidraw/iterator.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/Graphic/picture.h>
#include <Unidraw/Graphic/ellipses.h>
#include <Unidraw/Graphic/polygons.h>

#include <iostream.h>
#include <string.h>
#include <unistd.h>

#include <IV-2_6/_enter.h>

/*****************************************************************************/

Sprite::Sprite(PipeComp * pipe, float origx, float origy, float deltax, float deltay, int acknowledgeflag, InvoComp* invo) { 
  _pipe = pipe; 
  _invo = invo;
   _origx = origx;
  _origy = origy;
  _deltax = deltax;
  _deltay = deltay;
  _acknowledgeflag = acknowledgeflag;
}

Sprite::Sprite(Sprite* sprite) { 
  _pipe = sprite->_pipe; 
  _invo = sprite->_invo; 
  _origx = sprite->_origx;
  _origy = sprite->_origy;
  _deltax = sprite->_deltax;
  _deltay = sprite->_deltay;
}

void Sprite::animate(float percent) {
  float curx, cury;
  if (!acknowledgeflag()) {
    if (!_invo && (strlen(_pipe->GetText2()->GetOriginal())==0 || !_pipe->nsrc())) return;
    float adjusted_percent;
    if (PipeComp::animateflag()==1)
      adjusted_percent = min(1.0, percent*1.3333);
    else
      adjusted_percent = percent;
    curx = _origx + _deltax * adjusted_percent;
    cury = _origy + _deltay * adjusted_percent;
    if (!_invo)
      _pipe->MoveText2To(curx, cury);
    else {
      int srcflag;
      int index = _invo-> find_subpipe_index(_pipe, srcflag);
      if(index!=-1) {
	Picture* spritepict = _invo->GetSpritePict();
	Iterator it;
	spritepict->First(it);
	for(int i=0; i<index; i++) spritepict->Next(it);
	TextGraphic* textgr = (TextGraphic*) spritepict->GetGraphic(it);
	float cx, cy;
	textgr->GetCenter(cx, cy);
	textgr->Translate(curx-cx, cury-cy);
	textgr->SetColors(PipeComp::psred, pswhite);
	_invo->NotifyLater();
      }
    }
  } else {

    if (PipeComp::animateflag()==1) {
      const char* ackstr = (percent<.23 || percent >.98) ? "" : "@";
      curx = _origx + _deltax * (percent-.23)/.75;
      cury = _origy + _deltay * (percent-.23)/.75;
      if (!invo()) {
	_pipe->ChangeText2(ackstr);
	((Picture*)_pipe->GetGraphic())->SetColors(nil, nil);
	_pipe->GetEllipse()->SetColors(PipeComp::eventqueueflag() ? PipeComp::psred : PipeComp::psblue, pswhite);
	_pipe->GetText()->SetColors(PipeComp::eventqueueflag() ? PipeComp::psred : PipeComp::psblue, pswhite);
	_pipe->GetText2()->SetColors(PipeComp::psgreen, pswhite);
	_pipe->MoveText2To(curx, cury);
      } else {
	int srcflag;
	int index = invo()-> find_subpipe_index(_pipe, srcflag);
	if(index!=-1) {
	  Picture* spritepict = invo()->GetSpritePict();
	  Iterator it;
	  spritepict->First(it);
	  for(int i=0; i<index; i++) spritepict->Next(it);
	  TextGraphic* textgr = (TextGraphic*) spritepict->GetGraphic(it);
	  textgr->SetOriginal(ackstr);
	  float cx, cy;
	  textgr->GetCenter(cx, cy);
	  textgr->Translate(curx-cx, cury-cy);

	  ((Picture*)invo()->GetGraphic())->SetColors(nil, nil);
	  invo()->GetRect()->SetColors(PipeComp::psred, pswhite);
	  invo()->GetFuncText()->SetColors(PipeComp::psred, pswhite);
	  invo()->GetDstText()->SetColors(PipeComp::psred, pswhite);
	  invo()->GetSrcPict()->SetColors(PipeComp::psred, pswhite);
	  invo()->GetDstPict()->SetColors(PipeComp::psred, pswhite);
	  invo()->GetSpritePict()->SetColors(nil, nil);
	  textgr->SetColors(PipeComp::psgreen, pswhite);

	  invo()->NotifyLater();
	}
      }
    }
  }
  
}

void Sprite::wrap_up() {

  /* wrap up data flow */
  if (!acknowledgeflag()) {
    if(!invo()) {
      pipe()->GetGraphic()->SetColors(PipeComp::psred, pswhite);
      pipe()->color_outconn(PipeComp::psred);
      pipe()->NotifyLater();
      int order;
      PipeComp* upstream = pipe()->remote_srccomp(0, order, false);
      if(upstream->buff()->Number()==0) {
	if(!upstream->IsA(INVO_COMP)) upstream->ChangeText2("");
	upstream->GetGraphic()->SetColors(PipeComp::psblue, pswhite);
	upstream->NotifyLater();
	upstream->color_outconn(PipeComp::psblue);
      }
    }
    else {
      if(invo()->invopipes()->srccomp()->complete()) {
	invo()->GetGraphic()->SetColors(PipeComp::psred, pswhite);
	invo()->NotifyLater();
	invo()->color_outconn(PipeComp::psred);
      }
      int order;
      int srcflag;
      int index = invo()->find_subpipe_index(pipe(), srcflag);
      PipeComp* upstream = invo()->remote_srccomp(index, order, false);
      if(!upstream->IsA(INVO_COMP)) {
	if(upstream->buff()->Number()==0) {
	  upstream->ChangeText2("");
	  upstream->GetGraphic()->SetColors(PipeComp::psblue, pswhite);
	  upstream->NotifyLater();
	  upstream->color_outconn(PipeComp::psblue);
	}
      } else {
	if(!((InvoComp*)upstream)->invopipes()->dstcomp()->complete()) {
	  upstream->GetGraphic()->SetColors(PipeComp::psblue, pswhite);
	  upstream->NotifyLater();
	  upstream->color_outconn(PipeComp::psblue);
	}
      }
    }
    
  } 

  /* wrap up acknowledge flow */
  else {
    if(PipeComp::animateflag()==1) {
      if(!invo() && pipe()->buff()->Number()==0) {
	pipe()->ChangeText2("");
	pipe()->GetGraphic()->SetColors(PipeComp::psblue, pswhite);
	pipe()->color_outconn(PipeComp::psblue);
	pipe()->NotifyLater();
      }
    }
  }
}

/*****************************************************************************/

SpriteQueue* SpriteQueue::_instance = nil;

SpriteQueue::SpriteQueue (SpriteQueue* s) {
    _alist = new AList;
    _count = 0;
    if (s != nil) {
        ALIterator i;

        for (s->First(i); !s->Done(i); s->Next(i)) {
	    Append(new Sprite(s->GetSprite(i)));
	}
    }
}

SpriteQueue::~SpriteQueue () { 
    if (_alist) {
        ALIterator i;
	for (First(i); !Done(i); Next(i)) {
	    delete GetSprite(i);
	}
	delete _alist; 
    }
}

Sprite* SpriteQueue::GetSprite (AList* r) {
    return (Sprite*) (*r)();
}

AList* SpriteQueue::Elem (ALIterator i) { return (AList*) i.GetValue(); }

void SpriteQueue::Append (Sprite* v) {
    _alist->Append(new AList(v));
    ++_count;
}

void SpriteQueue::Prepend (Sprite* v) {
    _alist->Prepend(new AList(v));
    ++_count;
}

void SpriteQueue::InsertAfter (ALIterator i, Sprite* v) {
    Elem(i)->Prepend(new AList(v));
    ++_count;
}

void SpriteQueue::InsertBefore (ALIterator i, Sprite* v) {
    Elem(i)->Append(new AList(v));
    ++_count;
}

void SpriteQueue::Remove (ALIterator& i) {
    AList* doomed = Elem(i);

    Next(i);
    _alist->Remove(doomed);
    delete doomed;
    --_count;
}	
    
void SpriteQueue::Remove (Sprite* p) {
    AList* temp;

    if ((temp = _alist->Find(p)) != nil) {
	_alist->Remove(temp);
        delete temp;
	--_count;
    }
}

Sprite* SpriteQueue::GetSprite (ALIterator i) { return GetSprite
(Elem(i)); }

void SpriteQueue::SetSprite (Sprite* gv, ALIterator& i) {
    i.SetValue(_alist->Find(gv));
}

void SpriteQueue::First (ALIterator& i) { i.SetValue(_alist->First()); }
void SpriteQueue::Last (ALIterator& i) { i.SetValue(_alist->Last()); }
void SpriteQueue::Next (ALIterator& i) { i.SetValue(Elem(i)->Next()); }
void SpriteQueue::Prev (ALIterator& i) { i.SetValue(Elem(i)->Prev()); }
boolean SpriteQueue::Done (ALIterator i) { return Elem(i) == _alist->End(); }
int SpriteQueue::Number () { return _count; }

boolean SpriteQueue::Includes (Sprite* e) {
    return _alist->Find(e) != nil;
}

boolean SpriteQueue::IsEmpty () { return _alist->IsEmpty(); }

void SpriteQueue::clear() {
  ALIterator it;
  for( First(it); !Done(it); ) {
    Sprite* sprite = GetSprite(it);
    Remove(it);
    sprite->pipe()->sprite(nil);
    delete sprite;
  }
}

Sprite* SpriteQueue::Get(unsigned int index) {
  if (Number()<=index) return nil;
  Iterator it;
  First(it);
  for (int i=0; i<index; i++) Next(it);
  return GetSprite(it);
}

Sprite* SpriteQueue::Replace (ALIterator& i, Sprite* av) {
    AList* doomed = Elem(i);
    Sprite* removed = GetSprite(i);
    Next(i);
    _alist->Remove(doomed);
    delete doomed;
    Elem(i)->Append(new AList(av));
    return removed;
}	

void SpriteQueue::run(int numsteps) {
  
  Iterator it;
  First(it);
  if (!Done(it)) {
    for(int i=0; i<numsteps; i++) {
      First(it);
      float percent = (float) (i+1) / numsteps;
      while(!Done(it)) {
	Sprite* sprite = GetSprite(it);
	sprite->animate(percent);
	Next(it);
      }
      unidraw->Update(true);
      usleep(10000);
    }
    
    /* wrap up */
    First(it);
    while(!Done(it)) {
      Sprite* sprite = GetSprite(it);
      sprite->wrap_up();
      Next(it);
    }
    unidraw->Update(true);
  }
}
