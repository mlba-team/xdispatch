/*
* qiterationblockrunnable.h
* 
* Copyright (c) 2011-2016 MLBA-Team
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


#ifndef QITERATIONBLOCKRUNNABLE_H_
#define QITERATIONBLOCKRUNNABLE_H_

#include "qiterationrunnable.h"

#include "qdispatchglobal.h"

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
  as a QIterationRunnable
  */
template< typename _Func >
class QDispatchIterationRunnable
    : public QIterationRunnable
{
public:
    QDispatchIterationRunnable (
        const _Func &b
    )
        : QIterationRunnable(),
          _function( b ) { }


    QDispatchIterationRunnable (
        const QDispatchIterationRunnable< _Func > &other
    )
        : QIterationRunnable( other ),
          _function( other._function ) { }


    ~QDispatchIterationRunnable () { }


    void run(
        size_t index
    )
    {
        _function( index );
    }

private:
    _Func _function;
};


#if XDISPATCH_HAS_BLOCKS
/**
Provides a QIterationRunnable implementation for use with
blocks on clang or Apple's gcc 4.2

Please see the documentation for QRunnable for the
functionality of the autoDelete flags as well.
*/
template< >
class QDispatchIterationRunnable< dispatch_block_t >
    : public QIterationRunnable
{
public:
    /**
    Constructs a new QBlockRunnable using the given block, e.g.

    @code
    QIterationBlockRunnable task((size_t index){cout << "Hello World at" << index << "\n";}, 3);
    @endcode
    */
    QDispatchIterationRunnable (
        dispatch_iteration_block_t b
    )
        : QIterationRunnable(),
          _block( Block_copy( b ) ) { }


    QDispatchIterationRunnable (
        const QDispatchIterationRunnable< dispatch_block_t > &other
    )
        : QIterationRunnable( other ),
          _block( Block_copy( other._block ) ) { }


    virtual ~QDispatchIterationRunnable ()
    {
        Block_release( _block );
    }

    virtual inline void run(
        size_t index
    )
    {
        _block( index );
    }

private:
    dispatch_iteration_block_t _block;
};


typedef QDispatchIterationRunnable< dispatch_block_t > QIterationBlockRunnable;

#endif // if XDISPATCH_HAS_BLOCKS


QT_END_NAMESPACE
QT_END_HEADER

/** @} */

__XDISPATCH_BEGIN_NAMESPACE

Q_DISPATCH_EXPORT xdispatch::iteration_operation *
make_iteration_operation(
    QIterationRunnable *
);

__XDISPATCH_END_NAMESPACE

#if XDISPATCH_CPP11_TYPE_TRAITS

template< typename _Func >
inline typename std::enable_if<
    !std::is_pointer< _Func >::value,
    QIterationRunnable
>::type * QDispatchMakeIterationRunnable(
    const _Func &f
)
{
    return new QDispatchIterationRunnable< _Func > ( f );
}

__XDISPATCH_BEGIN_NAMESPACE

template< typename _Func >
inline typename std::enable_if<
    std::is_convertible< _Func, QIterationRunnable * >::value,
    xdispatch::iteration_operation
>::type * make_iteration_operation(
    const _Func &f
)
{
    return make_iteration_operation( static_cast< QIterationRunnable * > ( f ) );
}

__XDISPATCH_END_NAMESPACE

#else // if XDISPATCH_CPP11_TYPE_TRAITS

 # if XDISPATCH_HAS_FUNCTION
  #  if XDISPATCH_TR1_FUNCTIONAL

inline QIterationRunnable * QDispatchMakeIterationRunnable(
    const ::std::tr1::function< void(size_t) > &f
)
{
    return new QDispatchIterationRunnable< ::std::tr1::function< void(size_t) > > ( f );
}


  #  elif XDISPATCH_CPP11_FUNCTIONAL

inline QIterationRunnable * QDispatchMakeIterationRunnable(
    const ::std::function< void(size_t) > &f
)
{
    return new QDispatchIterationRunnable< ::std::function< void(size_t) > > ( f );
}


  #  endif // if XDISPATCH_TR1_FUNCTIONAL
 # endif // if XDISPATCH_HAS_FUNCTION

#endif // if XDISPATCH_CPP11_TYPE_TRAITS

#if XDISPATCH_HAS_BLOCKS

inline QIterationRunnable * QDispatchMakeIterationRunnable(
    dispatch_iteration_block_t b
)
{
    return new QIterationBlockRunnable( b );
}


#endif // if XDISPATCH_HAS_BLOCKS

#endif /* QITERATIONBLOCKRUNNABLE_H_ */