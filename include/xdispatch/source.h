
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


#ifndef XDISPATCH_SOURCE_H_
#define XDISPATCH_SOURCE_H_

#ifndef __XDISPATCH_INDIRECT__
#error "Please #include <xdispatch/dispatch.h> instead of this file directly."
#endif

#include <typeinfo>

__XDISPATCH_BEGIN_NAMESPACE

class source;

/**
Used for transferring data from a sourcetype
to source::data without loosing typesafety.

Adapted from boost::any 
(http://www.boost.org/doc/libs/1_46_1/doc/html/any.html)
(c) 2001 Kevlin Henney
Distributed under the Boost Software License, Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt) 
*/
struct any {

	any() : stored(0) {}
	template<typename Type> any(const Type& val) : stored(new holder<Type>(val)) {}
	any(const any& other) : stored(other.stored ? other.stored->clone() : 0 ) {}
	~any() { delete stored; }

	any & swap(any & v)
	{
		std::swap(stored, v.stored);
		return *this;
	}

	template<typename OtherType>
	any & operator=(const OtherType & v)
	{
		any(v).swap(*this);
		return *this;
	}

	any & operator=(any v)
	{
		v.swap(*this);
		return *this;
	}

	template<typename TargetType>
	TargetType cast() const {
		if(stored->type() == typeid(TargetType))
			return static_cast< holder<TargetType>* >(stored)->value;
		else
			throw std::bad_cast();
	}

private:

	struct place_holder {
		virtual place_holder * clone() const = 0;
		virtual const std::type_info & type() const = 0;
	};

	template <typename Type>
	struct holder : public place_holder {
		holder (const Type& val) : value(val) {}

		virtual place_holder * clone() const {
			return new holder(value);
		}

		virtual const std::type_info & type() const {
			return typeid(Type);
		}

		Type value;

	private:
		holder & operator=(const holder&);
	};

	place_holder* stored;
};

/**
Declares an abstract source type.

Subclass this type to implement custom source types.
Whenever your type becomes ready and the source should
dispatch the handler, simply call ready() with some data
you want to make available via source::data() to your handler
*/
class XDISPATCH_EXPORT sourcetype {

protected:
	sourcetype();

	/**
	This method needs to be called every time
	your sourcetype is read. The passed data can
	later be obtained in the handler by using
	source::data()
	*/
	void ready(const any& = any());
	/**
	Overload this method in case you are implementing
	a sourcetype based on an dispatch_source_t. This
	way users can access the native object by using
	source::native().

	By default this is returning NULL
	*/
	virtual dispatch_source_t native();

private:
	void set_cb(source*);
	friend class source;
	source* cb;
};

/**
Provides a source implementation.

A dispatch source will wait for a specific ressource or
operation (as defined by the given type) to complete and
dispatch a given handler on completion.
*/
class XDISPATCH_EXPORT source : public object {

public:
	/**
	Creates a new source waiting for the given source type
	*/
	source(sourcetype*);
	~source();

	void resume();
	void suspend();

	/**
	Sets the handler to dispatch each time the source
	becomes ready. You can use data() to obtain a pointer
	to data provided by the source and possibly being the
	reason for the handler to be dispatched.
	*/
	void handler(xdispatch::operation*);
#ifdef XDISPATCH_HAS_BLOCKS
	/**
	Sets the handler to dispatch each time the source
	becomes ready. You can use data() to obtain a pointer
	to data provided by the source and possibly being the
	reason for the handler to be dispatched.
	*/
	void handler(dispatch_block_t);
#endif
	/**
	Sets the queue the handler will be executed on
	*/
	void set_queue(const xdispatch::queue&);
	/**
	@returns the data that is associated with the currently dispatched
	and executed handler. The returned data is defined by the sourcetype
	used.

	Pass the type of the data to retrieve as template parameter. If the requested
	datatype cannot be provided, a bad_cast exception will be thrown.

	@remarks Calling this method from outside of a handler is undefined
	*/
	template <typename T> static T data(){
		return _data()->cast<T>();
	}

	/**
	@returns the native dispatch_object_t associated with this source.
	This depends on the sourcetype used and will normally return NULL
	*/
	virtual dispatch_object_t native() const;

private:
	source(const source&);
	source& operator=(const source&);
	class pdata;
	pdata* d;

	void notify(const any&);
	static const any* _data();
	friend class sourcetype;
};

__XDISPATCH_END_NAMESPACE

#endif /* XDISPATCH_SOURCE_H_ */
