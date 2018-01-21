/*
* qblockrunnable.h
*
* Copyright (c) 2011-2018 MLBA-Team
* All rights reserved.
*
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/


#ifndef QBLOCKRUNNABLE_H_
#define QBLOCKRUNNABLE_H_

#include "qdispatchglobal.h"

#include <QRunnable>

#if XDISPATCH_CPP11_TYPE_TRAITS
 # include <type_traits>
#endif

#if XDISPATCH_CPP11_FUNCTIONAL
 # include <functional>
#endif

/**
 * @addtogroup qtdispatch
 * @{
 */

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE QT_MODULE(
    Dispatch
)

/**
  A simple class for wrapping a callable
  as a QRunnable
  */
template< typename _Func >
class QDispatchRunnable
    : public QRunnable
{
public:
    QDispatchRunnable (
        const _Func &b
    )
        : QRunnable(),
          _function( b ) { }


    QDispatchRunnable (
        const QDispatchRunnable< _Func > &other
    )
        : QRunnable( other ),
          _function( other._function ) { }


    ~QDispatchRunnable () { }


    void run()
    {
        _function();
    }

private:
    _Func _function;
};


#if XDISPATCH_HAS_BLOCKS
/**
  Provides a QRunnable Implementation for use with
  blocks on clang or Apple gcc 4.2

  Please see the documentation for QRunnable for the
  functionality of the autoDelete flags as well.
  */
template< >
class Q_DISPATCH_EXPORT QDispatchRunnable< dispatch_block_t >
    : public QRunnable
{
public:
    /**
      Constructs a new QBlockRunnable using the given block, e.g.

      @code
      QBlockRunnable task(^{cout << "Hello World\n";});
      @endcode
      */
    QDispatchRunnable (
        dispatch_block_t b
    )
        : QRunnable(),
          _block( Block_copy( b ) ) { }


    QDispatchRunnable (
        const QDispatchRunnable< dispatch_block_t > &other
    )
        : QRunnable( other ),
          _block( Block_copy( other._block ) ) { }


    ~QDispatchRunnable ()
    {
        Block_release( _block );
    }

    void run()
    {
        _block();
    }

private:
    dispatch_block_t _block;
};


typedef QDispatchRunnable< dispatch_block_t > QBlockRunnable;

#endif // if XDISPATCH_HAS_BLOCKS


QT_END_NAMESPACE
QT_END_HEADER

/** @} */

__XDISPATCH_BEGIN_NAMESPACE

Q_DISPATCH_EXPORT xdispatch::operation *
make_operation(
    QRunnable *
);

__XDISPATCH_END_NAMESPACE

#if XDISPATCH_CPP11_TYPE_TRAITS

template< typename _Func >
inline typename std::enable_if<
    !std::is_pointer< _Func >::value,
    QRunnable
>::type * QDispatchMakeRunnable(
    const _Func &f
)
{
    return new QDispatchRunnable< _Func > ( f );
}

template< typename _Func >
inline typename std::enable_if<
    std::is_convertible< _Func, QRunnable * >::value,
    QRunnable
>::type * QDispatchMakeRunnable(
    const _Func &f
)
{
    return static_cast< QRunnable * > ( f );
}


__XDISPATCH_BEGIN_NAMESPACE

template< typename _Func >
inline typename std::enable_if<
    std::is_convertible< _Func, QRunnable * >::value,
    xdispatch::operation
>::type * make_operation(
    const _Func &f
)
{
    return make_operation( static_cast< QRunnable * > ( f ) );
}

__XDISPATCH_END_NAMESPACE

#else // if XDISPATCH_CPP11_TYPE_TRAITS


inline QRunnable * QDispatchMakeRunnable(
    QRunnable *r
)
{
    return r;
}


 # if XDISPATCH_HAS_FUNCTION
  #  if XDISPATCH_TR1_FUNCTIONAL

inline QRunnable * QDispatchMakeRunnable(
    const ::std::tr1::function< void(void) > &f
)
{
    return new QDispatchRunnable< ::std::tr1::function< void(void) > > ( f );
}


  #  elif XDISPATCH_CPP11_FUNCTIONAL

inline QRunnable * QDispatchMakeRunnable(
    const ::std::function< void(void) > &f
)
{
    return new QDispatchRunnable< ::std::function< void(void) > > ( f );
}


  #  endif // if XDISPATCH_TR1_FUNCTIONAL
 # endif // if XDISPATCH_HAS_FUNCTION


#endif // if XDISPATCH_CPP11_TYPE_TRAITS

#if XDISPATCH_HAS_BLOCKS

inline QRunnable * QDispatchMakeRunnable(
    dispatch_block_t b
)
{
    return new QBlockRunnable( b );
}


#endif // if XDISPATCH_HAS_BLOCKS


#endif /* QBLOCKRUNNABLE_H_ */