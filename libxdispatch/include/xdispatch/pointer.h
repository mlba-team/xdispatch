/*
* pointer.h
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


#ifndef XDISPATCH_SCOPED_PTER_H_
#define XDISPATCH_SCOPED_PTER_H_

#include <memory>

/**
 * @addtogroup xdispatch
 * @{
 */

#ifndef __XDISPATCH_INDIRECT__
    # error "Please #include <xdispatch/dispatch.h> instead of this file directly."
    #include "platform.h"
#endif

// Note: This header provides some memory management classes
// by importing means provided by the STL into the xdispatch
// namespace. Although it is perfectly clear that the new unique_ptr
// differs from the old auto_ptr, it is ok to mix them regarding the
// way they are used within xdispatch

// MSVC 2010
#if XDISPATCH_COMPILER_MSVC2010

# include <memory>

__XDISPATCH_BEGIN_NAMESPACE

template< typename _Type >
struct pointer
{
    typedef ::std::auto_ptr< _Type > unique;
    typedef ::std::shared_ptr< _Type > shared;
    typedef ::std::weak_ptr< _Type > weak;
};


__XDISPATCH_END_NAMESPACE


#elif XDISPATCH_COMPILER_MSVC2008SP1

# include <memory>

__XDISPATCH_BEGIN_NAMESPACE

template< typename _Type >
struct pointer
{
    typedef ::std::auto_ptr< _Type > unique;
    typedef ::std::tr1::shared_ptr< _Type > shared;
    typedef ::std::tr1::weak_ptr< _Type > weak;
};


__XDISPATCH_END_NAMESPACE


#elif XDISPATCH_CPP11_MEMORY

# include <memory>

__XDISPATCH_BEGIN_NAMESPACE

template< typename _Type >
struct pointer
{
    typedef ::std::unique_ptr< _Type > unique;
    typedef ::std::shared_ptr< _Type > shared;
    typedef ::std::weak_ptr< _Type > weak;
};


__XDISPATCH_END_NAMESPACE

#else // all others

# include <memory>
# include <tr1/memory> // NOLINT(build/c++tr1)

__XDISPATCH_BEGIN_NAMESPACE

template< typename _Type >
struct pointer
{
    typedef ::std::auto_ptr< _Type > unique;
    typedef ::std::tr1::shared_ptr< _Type > shared;
    typedef ::std::tr1::weak_ptr< _Type > weak;
};


__XDISPATCH_END_NAMESPACE

#endif // if XDISPATCH_COMPILER_MSVC2010

/** @} */

#endif /* XDISPATCH_SCOPED_PTER_H_ */