/* Callback function adaptors
   Copyright (C) 2004 Joseph H. Allen

This file is part of Notif++

Notif is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

Notif is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
Notif; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

// in FnX_Y
//   X is number of args which are passed by installer
//   Y is number of args which are added by caller

// Declare container...
//
//   Fn_2<return_type,arg1_type,arg2_type> *foo;
//
// Install regular callback function...
//
//   foo = new Fn1_2<return_type,pass_thru_arg_type,arg1_type,arg2_type>(cancel_func,main_func,pass_thru_arg);
//
// Install member callback function...
//
// foo = new Mfn1_2<return_type,class_type,pass_thru_arg_type,arg1_type,arg2_type>(cancel_func,main_func,ptr_to_class,pass_thru_arg);
//
// Execute callback function...
//
//   if (foo) foo->exec(arg1,arg2);
//
// Execute callback function and clear 'foo':
// (foo is cleared before the call to main_func).
//
//   if (foo) foo->cont(foo,arg1,arg2);
//
// Main_func gets called as follows:
//   main_func(pass_thru_arg,arg1,arg2);
//
// FAQ:
//   Why is foo a pointer?  Can't we pass by value?
//   No- because virtual function dispatch only works with '->' not '.'
//
// Todo:
//   Default cancel func: call class destructor on cancel?
//   Call cancel when Fn_0 is destroyed?  Maybe we don't need cancel.

#ifndef _Ifn
#define _Ifn 1

template <class R>
struct Fn_0
  {
  virtual R exec() = 0;
  virtual void cancel() = 0;
  R cont(Fn_0 *&fn) { Fn_0 *tmp = fn; fn = 0; return exec(); }
  };

template <class R>
struct Fn0_0 : public Fn_0<R>
  {
  R (*func)();
  void (*cancel_func)();
  
  virtual R exec() { return func(); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)()) { cancel_func=cf; func=f; }
  Fn0_0(void (*cf)(),R (*f)()) { cancel_func=cf; func=f; }
  Fn0_0(R (*f)()) { cancel_func=0; func=f; }
  Fn0_0() { cancel_func=0; func=0; }
  };

template <class R,class T>
struct Mfn0_0 : public Fn_0<R>
  {
  R (T::*func)();
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec() { return (argT->*func)(); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_0(void (T::*cf)(),R (T::*f)(),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_0(R (T::*f)(),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_0() { cancel_func=0; func=0; }
  };

template <class R,class A>
struct Fn1_0 : public Fn_0<R>
  {
  R (*func)(A);
  void (*cancel_func)(A);
  A argA;
  virtual R exec() { return func(argA); }
  virtual void cancel() { if(cancel_func) cancel_func(argA); }
  void set(void (*cf)(A),R (*f)(A),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_0(void (*cf)(A),R (*f)(A),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_0(R (*f)(A),A a) { cancel_func=0; func=f; argA=a; }
  Fn1_0() { cancel_func=0; func=0; }
  };

template <class R,class T,class A>
struct Mfn1_0 : public Fn_0<R>
  {
  R (T::*func)(A);
  void (T::*cancel_func)(A);
  T *argT;
  A argA;
  virtual R exec() { return (argT->*func)(argA); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA); }
  void set(void (T::*cf)(A),R (T::*f)(A),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_0(void (T::*cf)(A),R (T::*f)(A),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_0(R (T::*f)(A),T *t,A a) { cancel_func=0; func=f; argT=t; argA=a; }
  Mfn1_0() { cancel_func=0; func=0; }
  };

template <class R,class A,class B>
struct Fn2_0 : public Fn_0<R>
  {
  R (*func)(A,B);
  void (*cancel_func)(A,B);
  A argA; B argB;
  virtual R exec() { return func(argA,argB); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB); }
  void set(void (*cf)(A,B),R (*f)(A,B),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_0(void (*cf)(A,B),R (*f)(A,B),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_0(R (*f)(A,B),A a,B b) { cancel_func=0; func=f; argA=a; argB=b; }
  Fn2_0() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B>
struct Mfn2_0 : public Fn_0<R>
  {
  R (T::*func)(A,B);
  void (T::*cancel_func)(A,B);
  T *argT;
  A argA; B argB;
  virtual R exec() { return (argT->*func)(argA,argB); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB); }
  void set(void (T::*cf)(A,B),R (T::*f)(A,B),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_0(void (T::*cf)(A,B),R (T::*f)(A,B),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_0(R (T::*f)(A,B),T *t,A a,B b) { cancel_func=0; func=f; argT=t; argA=a; argB=b; }
  Mfn2_0() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C>
struct Fn3_0 : public Fn_0<R>
  {
  R (*func)(A,B,C);
  void (*cancel_func)(A,B,C);
  A argA; B argB; C argC;
  virtual R exec() { return func(argA,argB,argC); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC); }
  void set(void (*cf)(A,B,C),R (*f)(A,B,C),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_0(void (*cf)(A,B,C),R (*f)(A,B,C),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_0(R (*f)(A,B,C),A a,B b,C c) { cancel_func=0; func=f; argA=a; argB=b; argC=c; }
  Fn3_0() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C>
struct Mfn3_0 : public Fn_0<R>
  {
  R (T::*func)(A,B,C);
  void (T::*cancel_func)(A,B,C);
  T *argT;
  A argA; B argB; C argC;
  virtual R exec() { return (argT->*func)(argA,argB,argC); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC); }
  void set(void (T::*cf)(A,B,C),R (T::*f)(A,B,C),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_0(void (T::*cf)(A,B,C),R (T::*f)(A,B,C),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_0(R (T::*f)(A,B,C),T *t,A a,B b,C c) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_0() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D>
struct Fn4_0 : public Fn_0<R>
  {
  R (*func)(A,B,C,D);
  void (*cancel_func)(A,B,C,D);
  A argA; B argB; C argC; D argD;
  virtual R exec() { return func(argA,argB,argC,argD); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD); }
  void set(void (*cf)(A,B,C,D),R (*f)(A,B,C,D),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_0(void (*cf)(A,B,C,D),R (*f)(A,B,C,D),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_0(R (*f)(A,B,C,D),A a,B b,C c,D d) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_0() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D>
struct Mfn4_0 : public Fn_0<R>
  {
  R (T::*func)(A,B,C,D);
  void (T::*cancel_func)(A,B,C,D);
  T *argT;
  A argA; B argB; C argC; D argD;
  virtual R exec() { return (argT->*func)(argA,argB,argC,argD); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD); }
  void set(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_0(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_0(R (T::*f)(A,B,C,D),T *t,A a,B b,C c,D d) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_0() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class E>
struct Fn5_0 : public Fn_0<R>
  {
  R (*func)(A,B,C,D,E);
  void (*cancel_func)(A,B,C,D,E);
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec() { return func(argA,argB,argC,argD,argE); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD,argE); }
  void set(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_0(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_0(R (*f)(A,B,C,D,E),A a,B b,C c,D d,E e) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_0() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class E>
struct Mfn5_0 : public Fn_0<R>
  {
  R (T::*func)(A,B,C,D,E);
  void (T::*cancel_func)(A,B,C,D,E);
  T *argT;
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec() { return (argT->*func)(argA,argB,argC,argD,argE); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD,argE); }
  void set(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_0(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_0(R (T::*f)(A,B,C,D,E),T *t,A a,B b,C c,D d,E e) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_0() { cancel_func=0; func=0; }
  };

template <class R,class V>
struct Fn_1
  {
  virtual R exec(V argV) = 0;
  virtual void cancel() = 0;
  R cont(Fn_1 *&fn, V argV) { Fn_1 *tmp = fn; fn = 0; return exec(argV); }
  };

template <class R,class V>
struct Fn0_1 : public Fn_1<R,V>
  {
  R (*func)(V);
  void (*cancel_func)();
  
  virtual R exec(V argV) { return func(argV); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)(V)) { cancel_func=cf; func=f; }
  Fn0_1(void (*cf)(),R (*f)(V)) { cancel_func=cf; func=f; }
  Fn0_1(R (*f)(V)) { cancel_func=0; func=f; }
  Fn0_1() { cancel_func=0; func=0; }
  };

template <class R,class T,class V>
struct Mfn0_1 : public Fn_1<R,V>
  {
  R (T::*func)(V);
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec(V argV) { return (argT->*func)(argV); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(V),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_1(void (T::*cf)(),R (T::*f)(V),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_1(R (T::*f)(V),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_1() { cancel_func=0; func=0; }
  };

template <class R,class A,class V>
struct Fn1_1 : public Fn_1<R,V>
  {
  R (*func)(A,V);
  void (*cancel_func)(A);
  A argA;
  virtual R exec(V argV) { return func(argA,argV); }
  virtual void cancel() { if (cancel_func) cancel_func(argA); }
  void set(void (*cf)(A),R (*f)(A,V),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_1(void (*cf)(A),R (*f)(A,V),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_1(R (*f)(A,V),A a) { cancel_func=0; func=f; argA=a; }
  Fn1_1() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class V>
struct Mfn1_1 : public Fn_1<R,V>
  {
  R (T::*func)(A,V);
  void (T::*cancel_func)(A);
  T *argT;
  A argA;
  virtual R exec(V argV) { return (argT->*func)(argA,argV); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA); }
  void set(void (T::*cf)(A),R (T::*f)(A,V),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_1(void (T::*cf)(A),R (T::*f)(A,V),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_1(R (T::*f)(A,V),T *t,A a) { cancel_func=0; func=f; argT=t; argA=a; }
  Mfn1_1() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class V>
struct Fn2_1 : public Fn_1<R,V>
  {
  R (*func)(A,B,V);
  void (*cancel_func)(A,B);
  A argA; B argB;
  virtual R exec(V argV) { return func(argA,argB,argV); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB); }
  void set(void (*cf)(A,B),R (*f)(A,B,V),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_1(void (*cf)(A,B),R (*f)(A,B,V),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_1(R (*f)(A,B,V),A a,B b) { cancel_func=0; func=f; argA=a; argB=b; }
  Fn2_1() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class V>
struct Mfn2_1 : public Fn_1<R,V>
  {
  R (T::*func)(A,B,V);
  void (T::*cancel_func)(A,B);
  T *argT;
  A argA; B argB;
  virtual R exec(V argV) { return (argT->*func)(argA,argB,argV); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB); }
  void set(void (T::*cf)(A,B),R (T::*f)(A,B,V),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_1(void (T::*cf)(A,B),R (T::*f)(A,B,V),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_1(R (T::*f)(A,B,V),T *t,A a,B b) { cancel_func=0; func=f; argT=t; argA=a; argB=b; }
  Mfn2_1() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class V>
struct Fn3_1 : public Fn_1<R,V>
  {
  R (*func)(A,B,C,V);
  void (*cancel_func)(A,B,C);
  A argA; B argB; C argC;
  virtual R exec(V argV) { return func(argA,argB,argC,argV); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC); }
  void set(void (*cf)(A,B,C),R (*f)(A,B,C,V),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_1(void (*cf)(A,B,C),R (*f)(A,B,C,V),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_1(R (*f)(A,B,C,V),A a,B b,C c) { cancel_func=0; func=f; argA=a; argB=b; argC=c; }
  Fn3_1() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class V>
struct Mfn3_1 : public Fn_1<R,V>
  {
  R (T::*func)(A,B,C,V);
  void (T::*cancel_func)(A,B,C);
  T *argT;
  A argA; B argB; C argC;
  virtual R exec(V argV) { return (argT->*func)(argA,argB,argC,argV); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC); }
  void set(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_1(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_1(R (T::*f)(A,B,C,V),T *t,A a,B b,C c) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_1() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class V>
struct Fn4_1 : public Fn_1<R,V>
  {
  R (*func)(A,B,C,D,V);
  void (*cancel_func)(A,B,C,D);
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV) { return func(argA,argB,argC,argD,argV); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD); }
  void set(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_1(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_1(R (*f)(A,B,C,D,V),A a,B b,C c,D d) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_1() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class V>
struct Mfn4_1 : public Fn_1<R,V>
  {
  R (T::*func)(A,B,C,D,V);
  void (T::*cancel_func)(A,B,C,D);
  T *argT;
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV) { return (argT->*func)(argA,argB,argC,argD,argV); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD); }
  void set(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_1(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_1(R (T::*f)(A,B,C,D,V),T *t,A a,B b,C c,D d) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_1() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class E,class V>
struct Fn5_1 : public Fn_1<R,V>
  {
  R (*func)(A,B,C,D,E,V);
  void (*cancel_func)(A,B,C,D,E);
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV) { return func(argA,argB,argC,argD,argE,argV); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD,argE); }
  void set(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_1(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_1(R (*f)(A,B,C,D,E,V),A a,B b,C c,D d,E e) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_1() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class E,class V>
struct Mfn5_1 : public Fn_1<R,V>
  {
  R (T::*func)(A,B,C,D,E,V);
  void (T::*cancel_func)(A,B,C,D,E);
  T *argT;
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV) { return (argT->*func)(argA,argB,argC,argD,argE,argV); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD,argE); }
  void set(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_1(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_1(R (T::*f)(A,B,C,D,E,V),T *t,A a,B b,C c,D d,E e) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_1() { cancel_func=0; func=0; }
  };

template <class R,class V,class W>
struct Fn_2
  {
  virtual R exec(V argV,W argW) = 0;
  virtual void cancel() = 0;
  R cont(Fn_2 *&fn, V argV, W argW) { Fn_2 *tmp = fn; fn = 0; return exec(argV,argW); }
  };

template <class R,class V,class W>
struct Fn0_2 : public Fn_2<R,V,W>
  {
  R (*func)(V,W);
  void (*cancel_func)();
  
  virtual R exec(V argV,W argW) { return func(argV,argW); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)(V,W)) { cancel_func=cf; func=f; }
  Fn0_2(void (*cf)(),R (*f)(V,W)) { cancel_func=cf; func=f; }
  Fn0_2(R (*f)(V,W)) { cancel_func=0; func=f; }
  Fn0_2() { cancel_func=0; func=0; }
  };

template <class R,class T,class V,class W>
struct Mfn0_2 : public Fn_2<R,V,W>
  {
  R (T::*func)(V,W);
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec(V argV,W argW) { return (argT->*func)(argV,argW); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(V,W),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_2(void (T::*cf)(),R (T::*f)(V,W),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_2(R (T::*f)(V,W),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_2() { cancel_func=0; func=0; }
  };

template <class R,class A,class V,class W>
struct Fn1_2 : public Fn_2<R,V,W>
  {
  R (*func)(A,V,W);
  void (*cancel_func)(A);
  A argA;
  virtual R exec(V argV,W argW) { return func(argA,argV,argW); }
  virtual void cancel() { if(cancel_func) cancel_func(argA); }
  void set(void (*cf)(A),R (*f)(A,V,W),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_2(void (*cf)(A),R (*f)(A,V,W),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_2(R (*f)(A,V,W),A a) { cancel_func=0; func=f; argA=a; }
  Fn1_2() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class V,class W>
struct Mfn1_2 : public Fn_2<R,V,W>
  {
  R (T::*func)(A,V,W);
  void (T::*cancel_func)(A);
  T *argT;
  A argA;
  virtual R exec(V argV,W argW) { return (argT->*func)(argA,argV,argW); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA); }
  void set(void (T::*cf)(A),R (T::*f)(A,V,W),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_2(void (T::*cf)(A),R (T::*f)(A,V,W),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_2(R (T::*f)(A,V,W),T *t,A a) { cancel_func=0; func=f; argT=t; argA=a; }
  Mfn1_2() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class V,class W>
struct Fn2_2 : public Fn_2<R,V,W>
  {
  R (*func)(A,B,V,W);
  void (*cancel_func)(A,B);
  A argA; B argB;
  virtual R exec(V argV,W argW) { return func(argA,argB,argV,argW); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB); }
  void set(void (*cf)(A,B),R (*f)(A,B,V,W),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_2(void (*cf)(A,B),R (*f)(A,B,V,W),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_2(R (*f)(A,B,V,W),A a,B b) { cancel_func=0; func=f; argA=a; argB=b; }
  Fn2_2() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class V,class W>
struct Mfn2_2 : public Fn_2<R,V,W>
  {
  R (T::*func)(A,B,V,W);
  void (T::*cancel_func)(A,B);
  T *argT;
  A argA; B argB;
  virtual R exec(V argV,W argW) { return (argT->*func)(argA,argB,argV,argW); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB); }
  void set(void (T::*cf)(A,B),R (T::*f)(A,B,V,W),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_2(void (T::*cf)(A,B),R (T::*f)(A,B,V,W),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_2(R (T::*f)(A,B,V,W),T *t,A a,B b) { cancel_func=0; func=f; argT=t; argA=a; argB=b; }
  Mfn2_2() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class V,class W>
struct Fn3_2 : public Fn_2<R,V,W>
  {
  R (*func)(A,B,C,V,W);
  void (*cancel_func)(A,B,C);
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW) { return func(argA,argB,argC,argV,argW); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC); }
  void set(void (*cf)(A,B,C),R (*f)(A,B,C,V,W),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_2(void (*cf)(A,B,C),R (*f)(A,B,C,V,W),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_2(R (*f)(A,B,C,V,W),A a,B b,C c) { cancel_func=0; func=f; argA=a; argB=b; argC=c; }
  Fn3_2() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class V,class W>
struct Mfn3_2 : public Fn_2<R,V,W>
  {
  R (T::*func)(A,B,C,V,W);
  void (T::*cancel_func)(A,B,C);
  T *argT;
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW) { return (argT->*func)(argA,argB,argC,argV,argW); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC); }
  void set(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_2(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_2(R (T::*f)(A,B,C,V,W),T *t,A a,B b,C c) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_2() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class V,class W>
struct Fn4_2 : public Fn_2<R,V,W>
  {
  R (*func)(A,B,C,D,V,W);
  void (*cancel_func)(A,B,C,D);
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW) { return func(argA,argB,argC,argD,argV,argW); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD); }
  void set(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_2(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_2(R (*f)(A,B,C,D,V,W),A a,B b,C c,D d) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_2() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class V,class W>
struct Mfn4_2 : public Fn_2<R,V,W>
  {
  R (T::*func)(A,B,C,D,V,W);
  void (T::*cancel_func)(A,B,C,D);
  T *argT;
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW) { return (argT->*func)(argA,argB,argC,argD,argV,argW); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD); }
  void set(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_2(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_2(R (T::*f)(A,B,C,D,V,W),T *t,A a,B b,C c,D d) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_2() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class E,class V,class W>
struct Fn5_2 : public Fn_2<R,V,W>
  {
  R (*func)(A,B,C,D,E,V,W);
  void (*cancel_func)(A,B,C,D,E);
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW) { return func(argA,argB,argC,argD,argE,argV,argW); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD,argE); }
  void set(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_2(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_2(R (*f)(A,B,C,D,E,V,W),A a,B b,C c,D d,E e) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_2() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class E,class V,class W>
struct Mfn5_2 : public Fn_2<R,V,W>
  {
  R (T::*func)(A,B,C,D,E,V,W);
  void (T::*cancel_func)(A,B,C,D,E);
  T *argT;
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW) { return (argT->*func)(argA,argB,argC,argD,argE,argV,argW); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD,argE); }
  void set(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_2(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_2(R (T::*f)(A,B,C,D,E,V,W),T *t,A a,B b,C c,D d,E e) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_2() { cancel_func=0; func=0; }
  };

template <class R,class V,class W,class X>
struct Fn_3
  {
  virtual R exec(V argV,W argW,X argX) = 0;
  virtual void cancel() = 0;
  R cont(Fn_3 *&fn, V argV, W argW, X argX) { Fn_3 *tmp = fn; fn = 0; return exec(argV,argW,argX); }
  };

template <class R,class V,class W,class X>
struct Fn0_3 : public Fn_3<R,V,W,X>
  {
  R (*func)(V,W,X);
  void (*cancel_func)();
  
  virtual R exec(V argV,W argW,X argX) { return func(argV,argW,argX); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)(V,W,X)) { cancel_func=cf; func=f; }
  Fn0_3(void (*cf)(),R (*f)(V,W,X)) { cancel_func=cf; func=f; }
  Fn0_3(R (*f)(V,W,X)) { cancel_func=0; func=f; }
  Fn0_3() { cancel_func=0; func=0; }
  };

template <class R,class T,class V,class W,class X>
struct Mfn0_3 : public Fn_3<R,V,W,X>
  {
  R (T::*func)(V,W,X);
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec(V argV,W argW,X argX) { return (argT->*func)(argV,argW,argX); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(V,W,X),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_3(void (T::*cf)(),R (T::*f)(V,W,X),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_3(R (T::*f)(V,W,X),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_3() { cancel_func=0; func=0; }
  };

template <class R,class A,class V,class W,class X>
struct Fn1_3 : public Fn_3<R,V,W,X>
  {
  R (*func)(A,V,W,X);
  void (*cancel_func)(A);
  A argA;
  virtual R exec(V argV,W argW,X argX) { return func(argA,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) cancel_func(argA); }
  void set(void (*cf)(A),R (*f)(A,V,W,X),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_3(void (*cf)(A),R (*f)(A,V,W,X),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_3(R (*f)(A,V,W,X),A a) { cancel_func=0; func=f; argA=a; }
  Fn1_3() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class V,class W,class X>
struct Mfn1_3 : public Fn_3<R,V,W,X>
  {
  R (T::*func)(A,V,W,X);
  void (T::*cancel_func)(A);
  T *argT;
  A argA;
  virtual R exec(V argV,W argW,X argX) { return (argT->*func)(argA,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA); }
  void set(void (T::*cf)(A),R (T::*f)(A,V,W,X),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_3(void (T::*cf)(A),R (T::*f)(A,V,W,X),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_3(R (T::*f)(A,V,W,X),T *t,A a) { cancel_func=0; func=f; argT=t; argA=a; }
  Mfn1_3() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class V,class W,class X>
struct Fn2_3 : public Fn_3<R,V,W,X>
  {
  R (*func)(A,B,V,W,X);
  void (*cancel_func)(A,B);
  A argA; B argB;
  virtual R exec(V argV,W argW,X argX) { return func(argA,argB,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB); }
  void set(void (*cf)(A,B),R (*f)(A,B,V,W,X),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_3(void (*cf)(A,B),R (*f)(A,B,V,W,X),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_3(R (*f)(A,B,V,W,X),A a,B b) { cancel_func=0; func=f; argA=a; argB=b; }
  Fn2_3() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class V,class W,class X>
struct Mfn2_3 : public Fn_3<R,V,W,X>
  {
  R (T::*func)(A,B,V,W,X);
  void (T::*cancel_func)(A,B);
  T *argT;
  A argA; B argB;
  virtual R exec(V argV,W argW,X argX) { return (argT->*func)(argA,argB,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB); }
  void set(void (T::*cf)(A,B),R (T::*f)(A,B,V,W,X),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_3(void (T::*cf)(A,B),R (T::*f)(A,B,V,W,X),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_3(R (T::*f)(A,B,V,W,X),T *t,A a,B b) { cancel_func=0; func=f; argT=t; argA=a; argB=b; }
  Mfn2_3() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class V,class W,class X>
struct Fn3_3 : public Fn_3<R,V,W,X>
  {
  R (*func)(A,B,C,V,W,X);
  void (*cancel_func)(A,B,C);
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW,X argX) { return func(argA,argB,argC,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC); }
  void set(void (*cf)(A,B,C),R (*f)(A,B,C,V,W,X),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_3(void (*cf)(A,B,C),R (*f)(A,B,C,V,W,X),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_3(R (*f)(A,B,C,V,W,X),A a,B b,C c) { cancel_func=0; func=f; argA=a; argB=b; argC=c; }
  Fn3_3() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class V,class W,class X>
struct Mfn3_3 : public Fn_3<R,V,W,X>
  {
  R (T::*func)(A,B,C,V,W,X);
  void (T::*cancel_func)(A,B,C);
  T *argT;
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW,X argX) { return (argT->*func)(argA,argB,argC,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC); }
  void set(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W,X),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_3(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W,X),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_3(R (T::*f)(A,B,C,V,W,X),T *t,A a,B b,C c) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_3() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class V,class W,class X>
struct Fn4_3 : public Fn_3<R,V,W,X>
  {
  R (*func)(A,B,C,D,V,W,X);
  void (*cancel_func)(A,B,C,D);
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW,X argX) { return func(argA,argB,argC,argD,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD); }
  void set(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W,X),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_3(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W,X),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_3(R (*f)(A,B,C,D,V,W,X),A a,B b,C c,D d) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_3() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class V,class W,class X>
struct Mfn4_3 : public Fn_3<R,V,W,X>
  {
  R (T::*func)(A,B,C,D,V,W,X);
  void (T::*cancel_func)(A,B,C,D);
  T *argT;
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW,X argX) { return (argT->*func)(argA,argB,argC,argD,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD); }
  void set(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W,X),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_3(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W,X),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_3(R (T::*f)(A,B,C,D,V,W,X),T *t,A a,B b,C c,D d) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_3() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class E,class V,class W,class X>
struct Fn5_3 : public Fn_3<R,V,W,X>
  {
  R (*func)(A,B,C,D,E,V,W,X);
  void (*cancel_func)(A,B,C,D,E);
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW,X argX) { return func(argA,argB,argC,argD,argE,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD,argE); }
  void set(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W,X),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_3(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W,X),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_3(R (*f)(A,B,C,D,E,V,W,X),A a,B b,C c,D d,E e) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_3() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class E,class V,class W,class X>
struct Mfn5_3 : public Fn_3<R,V,W,X>
  {
  R (T::*func)(A,B,C,D,E,V,W,X);
  void (T::*cancel_func)(A,B,C,D,E);
  T *argT;
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW,X argX) { return (argT->*func)(argA,argB,argC,argD,argE,argV,argW,argX); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD,argE); }
  void set(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W,X),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_3(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W,X),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_3(R (T::*f)(A,B,C,D,E,V,W,X),T *t,A a,B b,C c,D d,E e) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_3() { cancel_func=0; func=0; }
  };

template <class R,class V,class W,class X,class Y>
struct Fn_4
  {
  virtual R exec(V argV,W argW,X argX,Y argY) = 0;
  virtual void cancel() = 0;
  R cont(Fn_4 *&fn, V argV, W argW, X argX, Y argY) { Fn_4 *tmp = fn; fn = 0; return exec(argV,argW,argX,argY); }
  };
 
template <class R,class V,class W,class X,class Y>
struct Fn0_4 : public Fn_4<R,V,W,X,Y>
  {
  R (*func)(V,W,X,Y);
  void (*cancel_func)();
  
  virtual R exec(V argV,W argW,X argX,Y argY) { return func(argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)(V,W,X,Y)) { cancel_func=cf; func=f; }
  Fn0_4(void (*cf)(),R (*f)(V,W,X,Y)) { cancel_func=cf; func=f; }
  Fn0_4(R (*f)(V,W,X,Y)) { cancel_func=0; func=f; }
  Fn0_4() { cancel_func=0; func=0; }
  };

template <class R,class T,class V,class W,class X,class Y>
struct Mfn0_4 : public Fn_4<R,V,W,X,Y>
  {
  R (T::*func)(V,W,X,Y);
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec(V argV,W argW,X argX,Y argY) { return (argT->*func)(argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(V,W,X,Y),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_4(void (T::*cf)(),R (T::*f)(V,W,X,Y),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_4(R (T::*f)(V,W,X,Y),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_4() { cancel_func=0; func=0; }
  };

template <class R,class A,class V,class W,class X,class Y>
struct Fn1_4 : public Fn_4<R,V,W,X,Y>
  {
  R (*func)(A,V,W,X,Y);
  void (*cancel_func)(A);
  A argA;
  virtual R exec(V argV,W argW,X argX,Y argY) { return func(argA,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) cancel_func(argA); }
  void set(void (*cf)(A),R (*f)(A,V,W,X,Y),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_4(void (*cf)(A),R (*f)(A,V,W,X,Y),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_4(R (*f)(A,V,W,X,Y),A a) { cancel_func=0; func=f; argA=a; }
  Fn1_4() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class V,class W,class X,class Y>
struct Mfn1_4 : public Fn_4<R,V,W,X,Y>
  {
  R (T::*func)(A,V,W,X,Y);
  void (T::*cancel_func)(A);
  T *argT;
  A argA;
  virtual R exec(V argV,W argW,X argX,Y argY) { return (argT->*func)(argA,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA); }
  void set(void (T::*cf)(A),R (T::*f)(A,V,W,X,Y),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_4(void (T::*cf)(A),R (T::*f)(A,V,W,X,Y),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_4(R (T::*f)(A,V,W,X,Y),T *t,A a) { cancel_func=0; func=f; argT=t; argA=a; }
  Mfn1_4() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class V,class W,class X,class Y>
struct Fn2_4 : public Fn_4<R,V,W,X,Y>
  {
  R (*func)(A,B,V,W,X,Y);
  void (*cancel_func)(A,B);
  A argA; B argB;
  virtual R exec(V argV,W argW,X argX,Y argY) { return func(argA,argB,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB); }
  void set(void (*cf)(A,B),R (*f)(A,B,V,W,X,Y),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_4(void (*cf)(A,B),R (*f)(A,B,V,W,X,Y),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_4(R (*f)(A,B,V,W,X,Y),A a,B b) { cancel_func=0; func=f; argA=a; argB=b; }
  Fn2_4() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class V,class W,class X,class Y>
struct Mfn2_4 : public Fn_4<R,V,W,X,Y>
  {
  R (T::*func)(A,B,V,W,X,Y);
  void (T::*cancel_func)(A,B);
  T *argT;
  A argA; B argB;
  virtual R exec(V argV,W argW,X argX,Y argY) { return (argT->*func)(argA,argB,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB); }
  void set(void (T::*cf)(A,B),R (T::*f)(A,B,V,W,X,Y),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_4(void (T::*cf)(A,B),R (T::*f)(A,B,V,W,X,Y),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_4(R (T::*f)(A,B,V,W,X,Y),T *t,A a,B b) { cancel_func=0; func=f; argT=t; argA=a; argB=b; }
  Mfn2_4() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class V,class W,class X,class Y>
struct Fn3_4 : public Fn_4<R,V,W,X,Y>
  {
  R (*func)(A,B,C,V,W,X,Y);
  void (*cancel_func)(A,B,C);
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW,X argX,Y argY) { return func(argA,argB,argC,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC); }
  void set(void (*cf)(A,B,C),R (*f)(A,B,C,V,W,X,Y),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_4(void (*cf)(A,B,C),R (*f)(A,B,C,V,W,X,Y),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_4(R (*f)(A,B,C,V,W,X,Y),A a,B b,C c) { cancel_func=0; func=f; argA=a; argB=b; argC=c; }
  Fn3_4() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class V,class W,class X,class Y>
struct Mfn3_4 : public Fn_4<R,V,W,X,Y>
  {
  R (T::*func)(A,B,C,V,W,X,Y);
  void (T::*cancel_func)(A,B,C);
  T *argT;
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW,X argX,Y argY) { return (argT->*func)(argA,argB,argC,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC); }
  void set(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W,X,Y),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_4(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W,X,Y),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_4(R (T::*f)(A,B,C,V,W,X,Y),T *t,A a,B b,C c) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_4() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class V,class W,class X,class Y>
struct Fn4_4 : public Fn_4<R,V,W,X,Y>
  {
  R (*func)(A,B,C,D,V,W,X,Y);
  void (*cancel_func)(A,B,C,D);
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW,X argX,Y argY) { return func(argA,argB,argC,argD,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD); }
  void set(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W,X,Y),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_4(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W,X,Y),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_4(R (*f)(A,B,C,D,V,W,X,Y),A a,B b,C c,D d) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_4() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class V,class W,class X,class Y>
struct Mfn4_4 : public Fn_4<R,V,W,X,Y>
  {
  R (T::*func)(A,B,C,D,V,W,X,Y);
  void (T::*cancel_func)(A,B,C,D);
  T *argT;
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW,X argX,Y argY) { return (argT->*func)(argA,argB,argC,argD,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD); }
  void set(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W,X,Y),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_4(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W,X,Y),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_4(R (T::*f)(A,B,C,D,V,W,X,Y),T *t,A a,B b,C c,D d) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_4() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class E,class V,class W,class X,class Y>
struct Fn5_4 : public Fn_4<R,V,W,X,Y>
  {
  R (*func)(A,B,C,D,E,V,W,X,Y);
  void (*cancel_func)(A,B,C,D,E);
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW,X argX,Y argY) { return func(argA,argB,argC,argD,argE,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD,argE); }
  void set(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W,X,Y),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_4(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W,X,Y),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_4(R (*f)(A,B,C,D,E,V,W,X,Y),A a,B b,C c,D d,E e) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_4() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class E,class V,class W,class X,class Y>
struct Mfn5_4 : public Fn_4<R,V,W,X,Y>
  {
  R (T::*func)(A,B,C,D,E,V,W,X,Y);
  void (T::*cancel_func)(A,B,C,D,E);
  T *argT;
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW,X argX,Y argY) { return (argT->*func)(argA,argB,argC,argD,argE,argV,argW,argX,argY); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD,argE); }
  void set(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W,X,Y),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_4(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W,X,Y),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_4(R (T::*f)(A,B,C,D,E,V,W,X,Y),T *t,A a,B b,C c,D d,E e) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_4() { cancel_func=0; func=0; }
  };

template <class R,class V,class W,class X,class Y,class Z>
struct Fn_5
  {
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) = 0;
  virtual void cancel() = 0;
  R cont(Fn_5 *&fn, V argV, W argW, X argX, Y argY, Z argZ) { Fn_5 *tmp = fn; fn = 0; return exec(argV,argW,argX,argY,argZ); }
  };

template <class R,class V,class W,class X,class Y,class Z>
struct Fn0_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (*func)(V,W,X,Y,Z);
  void (*cancel_func)();
  
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return func(argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)(V,W,X,Y,Z)) { cancel_func=cf; func=f; }
  Fn0_5(void (*cf)(),R (*f)(V,W,X,Y,Z)) { cancel_func=cf; func=f; }
  Fn0_5(R (*f)(V,W,X,Y,Z)) { cancel_func=0; func=f; }
  Fn0_5() { cancel_func=0; func=0; }
  };

template <class R,class T,class V,class W,class X,class Y,class Z>
struct Mfn0_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (T::*func)(V,W,X,Y,Z);
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return (argT->*func)(argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(V,W,X,Y,Z),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_5(void (T::*cf)(),R (T::*f)(V,W,X,Y,Z),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_5(R (T::*f)(V,W,X,Y,Z),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_5() { cancel_func=0; func=0; }
  };

template <class R,class A,class V,class W,class X,class Y,class Z>
struct Fn1_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (*func)(A,V,W,X,Y,Z);
  void (*cancel_func)(A);
  A argA;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return func(argA,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) cancel_func(argA); }
  void set(void (*cf)(A),R (*f)(A,V,W,X,Y,Z),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_5(void (*cf)(A),R (*f)(A,V,W,X,Y,Z),A a) { cancel_func=cf; func=f; argA=a; }
  Fn1_5(R (*f)(A,V,W,X,Y,Z),A a) { cancel_func=0; func=f; argA=a; }
  Fn1_5() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class V,class W,class X,class Y,class Z>
struct Mfn1_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (T::*func)(A,V,W,X,Y,Z);
  void (T::*cancel_func)(A);
  T *argT;
  A argA;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return (argT->*func)(argA,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA); }
  void set(void (T::*cf)(A),R (T::*f)(A,V,W,X,Y,Z),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_5(void (T::*cf)(A),R (T::*f)(A,V,W,X,Y,Z),T *t,A a) { cancel_func=cf; func=f; argT=t; argA=a; }
  Mfn1_5(R (T::*f)(A,V,W,X,Y,Z),T *t,A a) { cancel_func=0; func=f; argT=t; argA=a; }
  Mfn1_5() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class V,class W,class X,class Y,class Z>
struct Fn2_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (*func)(A,B,V,W,X,Y,Z);
  void (*cancel_func)(A,B);
  A argA; B argB;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return func(argA,argB,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB); }
  void set(void (*cf)(A,B),R (*f)(A,B,V,W,X,Y,Z),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_5(void (*cf)(A,B),R (*f)(A,B,V,W,X,Y,Z),A a,B b) { cancel_func=cf; func=f; argA=a; argB=b; }
  Fn2_5(R (*f)(A,B,V,W,X,Y,Z),A a,B b) { cancel_func=0; func=f; argA=a; argB=b; }
  Fn2_5() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class V,class W,class X,class Y,class Z>
struct Mfn2_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (T::*func)(A,B,V,W,X,Y,Z);
  void (T::*cancel_func)(A,B);
  T *argT;
  A argA; B argB;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return (argT->*func)(argA,argB,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB); }
  void set(void (T::*cf)(A,B),R (T::*f)(A,B,V,W,X,Y,Z),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_5(void (T::*cf)(A,B),R (T::*f)(A,B,V,W,X,Y,Z),T *t,A a,B b) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; }
  Mfn2_5(R (T::*f)(A,B,V,W,X,Y,Z),T *t,A a,B b) { cancel_func=0; func=f; argT=t; argA=a; argB=b; }
  Mfn2_5() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class V,class W,class X,class Y,class Z>
struct Fn3_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (*func)(A,B,C,V,W,X,Y,Z);
  void (*cancel_func)(A,B,C);
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return func(argA,argB,argC,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC); }
  void set(void (*cf)(A,B,C),R (*f)(A,B,C,V,W,X,Y,Z),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_5(void (*cf)(A,B,C),R (*f)(A,B,C,V,W,X,Y,Z),A a,B b,C c) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; }
  Fn3_5(R (*f)(A,B,C,V,W,X,Y,Z),A a,B b,C c) { cancel_func=0; func=f; argA=a; argB=b; argC=c; }
  Fn3_5() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class V,class W,class X,class Y,class Z>
struct Mfn3_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (T::*func)(A,B,C,V,W,X,Y,Z);
  void (T::*cancel_func)(A,B,C);
  T *argT;
  A argA; B argB; C argC;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return (argT->*func)(argA,argB,argC,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC); }
  void set(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W,X,Y,Z),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_5(void (T::*cf)(A,B,C),R (T::*f)(A,B,C,V,W,X,Y,Z),T *t,A a,B b,C c) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_5(R (T::*f)(A,B,C,V,W,X,Y,Z),T *t,A a,B b,C c) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; }
  Mfn3_5() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class V,class W,class X,class Y,class Z>
struct Fn4_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (*func)(A,B,C,D,V,W,X,Y,Z);
  void (*cancel_func)(A,B,C,D);
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return func(argA,argB,argC,argD,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD); }
  void set(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W,X,Y,Z),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_5(void (*cf)(A,B,C,D),R (*f)(A,B,C,D,V,W,X,Y,Z),A a,B b,C c,D d) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_5(R (*f)(A,B,C,D,V,W,X,Y,Z),A a,B b,C c,D d) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; }
  Fn4_5() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class V,class W,class X,class Y,class Z>
struct Mfn4_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (T::*func)(A,B,C,D,V,W,X,Y,Z);
  void (T::*cancel_func)(A,B,C,D);
  T *argT;
  A argA; B argB; C argC; D argD;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return (argT->*func)(argA,argB,argC,argD,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD); }
  void set(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W,X,Y,Z),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_5(void (T::*cf)(A,B,C,D),R (T::*f)(A,B,C,D,V,W,X,Y,Z),T *t,A a,B b,C c,D d) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_5(R (T::*f)(A,B,C,D,V,W,X,Y,Z),T *t,A a,B b,C c,D d) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; }
  Mfn4_5() { cancel_func=0; func=0; }
  };

template <class R,class A,class B,class C,class D,class E,class V,class W,class X,class Y,class Z>
struct Fn5_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (*func)(A,B,C,D,E,V,W,X,Y,Z);
  void (*cancel_func)(A,B,C,D,E);
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return func(argA,argB,argC,argD,argE,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) cancel_func(argA,argB,argC,argD,argE); }
  void set(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W,X,Y,Z),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_5(void (*cf)(A,B,C,D,E),R (*f)(A,B,C,D,E,V,W,X,Y,Z),A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_5(R (*f)(A,B,C,D,E,V,W,X,Y,Z),A a,B b,C c,D d,E e) { cancel_func=0; func=f; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Fn5_5() { cancel_func=0; func=0; }
  };

template <class R,class T,class A,class B,class C,class D,class E,class V,class W,class X,class Y,class Z>
struct Mfn5_5 : public Fn_5<R,V,W,X,Y,Z>
  {
  R (T::*func)(A,B,C,D,E,V,W,X,Y,Z);
  void (T::*cancel_func)(A,B,C,D,E);
  T *argT;
  A argA; B argB; C argC; D argD; E argE;
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ) { return (argT->*func)(argA,argB,argC,argD,argE,argV,argW,argX,argY,argZ); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(argA,argB,argC,argD,argE); }
  void set(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W,X,Y,Z),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_5(void (T::*cf)(A,B,C,D,E),R (T::*f)(A,B,C,D,E,V,W,X,Y,Z),T *t,A a,B b,C c,D d,E e) { cancel_func=cf; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_5(R (T::*f)(A,B,C,D,E,V,W,X,Y,Z),T *t,A a,B b,C c,D d,E e) { cancel_func=0; func=f; argT=t; argA=a; argB=b; argC=c; argD=d; argE=e; }
  Mfn5_5() { cancel_func=0; func=0; }
  };

template <class R,class V,class W,class X,class Y,class Z,class AA>
struct Fn_6
  {
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ,AA argAA) = 0;
  virtual void cancel() = 0;
  R cont(Fn_6 *&fn, V argV, W argW, X argX, Y argY, Z argZ, AA argAA)
    { Fn_6 *tmp = fn; fn = 0; return exec(argV,argW,argX,argY,argZ,argAA); }
  };

template <class R,class V,class W,class X,class Y,class Z,class AA>
struct Fn0_6 : public Fn_6<R,V,W,X,Y,Z,AA>
  {
  R (*func)(V,W,X,Y,Z,AA);
  void (*cancel_func)();
  
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ,AA argAA) { return func(argV,argW,argX,argY,argZ,argAA); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)(V,W,X,Y,Z,AA)) { cancel_func=cf; func=f; }
  Fn0_6(void (*cf)(),R (*f)(V,W,X,Y,Z,AA)) { cancel_func=cf; func=f; }
  Fn0_6(R (*f)(V,W,X,Y,Z,AA)) { cancel_func=0; func=f; }
  Fn0_6() { cancel_func=0; func=0; }
  };

template <class R,class T,class V,class W,class X,class Y,class Z,class AA>
struct Mfn0_6 : public Fn_6<R,V,W,X,Y,Z,AA>
  {
  R (T::*func)(V,W,X,Y,Z,AA);
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ,AA argAA) { return (argT->*func)(argV,argW,argX,argY,argZ,argAA); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(V,W,X,Y,Z,AA),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_6(void (T::*cf)(),R (T::*f)(V,W,X,Y,Z,AA),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_6(R (T::*f)(V,W,X,Y,Z,AA),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_6() { cancel_func=0; func=0; }
  };

template <class R,class V,class W,class X,class Y,class Z,class AA,class BB>
struct Fn_7
  {
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ,AA argAA,BB argBB) = 0;
  virtual void cancel() = 0;
  R cont(Fn_7 *&fn, V argV, W argW, X argX, Y argY, Z argZ, AA argAA, BB argBB)
    { Fn_7 *tmp = fn; fn = 0; return exec(argV,argW,argX,argY,argZ,argAA,argBB); }
  };

template <class R,class V,class W,class X,class Y,class Z,class AA,class BB>
struct Fn0_7 : public Fn_7<R,V,W,X,Y,Z,AA,BB>
  {
  R (*func)(V,W,X,Y,Z,AA,BB);
  void (*cancel_func)();
  
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ,AA argAA,BB argBB) { return func(argV,argW,argX,argY,argZ,argAA,argBB); }
  virtual void cancel() { if(cancel_func) cancel_func(); }
  void set(void (*cf)(),R (*f)(V,W,X,Y,Z,AA,BB)) { cancel_func=cf; func=f; }
  Fn0_7(void (*cf)(),R (*f)(V,W,X,Y,Z,AA,BB)) { cancel_func=cf; func=f; }
  Fn0_7(R (*f)(V,W,X,Y,Z,AA,BB)) { cancel_func=0; func=f; }
  Fn0_7() { cancel_func=0; func=0; }
  };

template <class R,class T,class V,class W,class X,class Y,class Z,class AA,class BB>
struct Mfn0_7 : public Fn_7<R,V,W,X,Y,Z,AA,BB>
  {
  R (T::*func)(V,W,X,Y,Z,AA,BB);
  void (T::*cancel_func)();
  T *argT;
  
  virtual R exec(V argV,W argW,X argX,Y argY,Z argZ,AA argAA,BB argBB) { return (argT->*func)(argV,argW,argX,argY,argZ,argAA,argBB); }
  virtual void cancel() { if(cancel_func) (argT->*cancel_func)(); }
  void set(void (T::*cf)(),R (T::*f)(V,W,X,Y,Z,AA,BB),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_7(void (T::*cf)(),R (T::*f)(V,W,X,Y,Z,AA,BB),T *t) { cancel_func=cf; func=f; argT=t; }
  Mfn0_7(R (T::*f)(V,W,X,Y,Z,AA,BB),T *t) { cancel_func=0; func=f; argT=t; }
  Mfn0_7() { cancel_func=0; func=0; }
  };

#endif
