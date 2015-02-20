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

#ifndef delimfunc_h
#define delimfunc_h

#include <ComTerp/comfunc.h>
#include <ComTerp/comterp.h>

class ComTerp;

class ParensFunc : public ComFunc {
public:
    ParensFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "() -- run for matched parens"; }

protected:
};

class BracketsFunc : public ComFunc {
public:
    BracketsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "() -- run for matched brackets"; }

protected:
};

class BracesFunc : public ComFunc {
public:
    BracesFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "[] -- run for matched braces"; }

protected:
};

class AngleBracketsFunc : public ComFunc {
public:
    AngleBracketsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "<> -- run for matched angle-brackets"; }

protected:
};

class DoubleAngleBracketsFunc : public ComFunc {
public:
    DoubleAngleBracketsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "<<>> -- run for matched double angle-brackets"; }

protected:
};

#endif

