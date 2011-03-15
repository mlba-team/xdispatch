/*
* Copyright (c) 2011 MLBA. All rights reserved.
*
* @MLBA_OPEN_LICENSE_HEADER_START@
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*     http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* @MLBA_OPEN_LICENSE_HEADER_END@
*/



#ifndef JAVAUTILS_H_
#define JAVAUTILS_H_

#include <QObject>
#include <typeinfo>

/**
  Use this method similar to Java's instanceof

  @param A An object or pointer to it you want to test
  @param C The class the object should be an instance of
  @return true if A is an instance of C, false otherwise

  Example: if(instanceof(myWidget,QWidget))
  */
#define instanceof(A,C) _instanceof(A,#C)

inline bool _instanceof(const QObject* a, const char* clazz){
	return QString(typeid(*a).name()).contains(clazz);
}

inline bool _instanceof(const QObject& a, const char* clazz){
	return QString(typeid(a).name()).contains(clazz);
}

/**
  Use this method similar to Java's instanceof

  The difference to instanceof is that you can
  use objects not deriving from QObject here
  as well!

  @param A An object you want to test
  @param C The class the object should be an instance of
  @return true if A is an instance of C, false otherwise

  Example: if(instanceof(myWidget,QWidget))
	  */
#define instanceof_NoQObject(A,C) QString(typeid(A).name()).contains(#C)

#endif // JAVAUTILS_H_
