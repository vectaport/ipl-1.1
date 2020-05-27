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
 * Sprite and SpriteQueue
 */

#ifndef spritequeue_h
#define spritequeue_h

#include <OS/enter-scope.h>
#include <InterViews/resource.h>

#ifndef ALITERATOR
#define ALIterator _lib_iv(Iterator)
#define AList _lib_iv(UList)
#endif

class ALIterator;
class AList;
class PipeComp;
class InvoComp;
#include <iosfwd>

class Sprite {
 public:
  Sprite(PipeComp * pipe, 
	 float origx, float origy, float deltax, float deltay, 
	 int acknowledgeflag = 0, InvoComp* invo=nil);
  Sprite(Sprite* sprite);
  PipeComp* pipe() { return _pipe; }
  InvoComp* invo() { return _invo; }

  void animate(float percent);
  void wrap_up();

  int acknowledgeflag() { return _acknowledgeflag;}
  float origx() { return _origx;}
  float origy() { return _origy;}
  float deltax() { return _deltax;}
  float deltay() { return _deltay;}

 protected:
  PipeComp* _pipe;
  InvoComp* _invo;
  float _origx;
  float _origy;
  float _deltax;
  float _deltay;
  int _acknowledgeflag;
};
  

//: queue of Sprite objects.
// An SpriteQueue is derived from Resource, so it is a reference-counted
// object that can be freely shared between other objects.
//
// An SpriteQueue assumes responsibility for the memory of its member
// Sprite objects.
class SpriteQueue : public Resource {
public:
    SpriteQueue(SpriteQueue* = nil);
    // construct with optional SpriteQueue to copy.
    virtual ~SpriteQueue();
    // do not call directly.  Frees memory of associated Sprite objects.

public:
    void First(ALIterator&);
    // set iterator to point to first Sprite in list.
    void Last(ALIterator&);
    // set iterator to point to last Sprite in list.
    void Next(ALIterator&);
    // set iterator to point to next Sprite in list.
    void Prev(ALIterator&);
    // set iterator to point to previous Sprite in list.
    boolean Done(ALIterator);
    // return true if iterator is pointing off the end of the list.
    // works for forward and backward traversals.
    boolean IsEmpty();
    // true if no Sprite objects in list.
    int Number();
    // number of Sprite objects in list.

    void Append(Sprite*);
    // append Sprite to end of list.
    void Prepend(Sprite*);
    // append Sprite to front of list.
    void InsertAfter(ALIterator, Sprite*);
    // insert Sprite after position pointed to by iterator.
    void InsertBefore(ALIterator, Sprite*);
    // insert Sprite before position pointed to by iterator.
    void Remove(Sprite*);
    // remove Sprite from list, returning responsibility for freeing the
    // associated memory.
    void Remove(ALIterator&);
    // remove Sprite pointed to by iterator from the list, 
    // returning responsibility for freeing the associated memory.
    // This requires saving a pointer to the Sprite before calling this method.
    Sprite* Replace(ALIterator&, Sprite*);
    // remove Sprite pointed to by iterator from the list, 
    // returning responsibility for freeing the associated memory.
    // Then insert new Sprite in the same place.


    Sprite* GetSprite(ALIterator);
    // get Sprite pointed to by iterator.
    void SetSprite(Sprite*, ALIterator&);
    // set Sprite pointed to by iterator.
    boolean Includes(Sprite*);
    // check if list includes Sprite by pointer-comparison.

    Sprite* Get(unsigned int index);
    // retrieve value by index, return nil if not there
    AList* Elem(ALIterator); 
    // return AList (UList) pointed to by ALIterator (Iterator).
    Sprite* GetSprite(AList*);
    // return Sprite pointed to by AList (UList).

    void run(int numsteps);
    // run everything in queue

    void clear(); 
    // empty SpriteQueue, deleting all Sprite's.

    static SpriteQueue* instance() { if (!_instance) _instance = new SpriteQueue(); return _instance; }

protected:
    AList* _alist;
    unsigned int _count;

    static SpriteQueue* _instance;
};

#endif
