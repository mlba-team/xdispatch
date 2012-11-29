#ifndef DISPATCH_WIN_PTHREAD_TLS_
#define DISPATCH_WIN_PTHREAD_TLS_

/*
 * Posix Threads library for Microsoft Windows
 *
 * Use at own risk, there is no implied warranty to this code.
 * It uses undocumented features of Microsoft Windows that can change
 * at any time in the future.
 *
 * (C) 2010 Lockless Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of Lockless Inc. nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AN
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

typedef unsigned int pthread_key_t;

static void _pthread_cleanup_dest(pthread_t t)
{
	unsigned int i, j;
	
	for (j = 0; j < PTHREAD_DESTRUCTOR_ITERATIONS; j++)
	{
		int flag = 0;
	
		for (i = 0; i < t->keymax; i++)
		{
			void *val = t->keyval[i];
			
			if (val)
			{
				pthread_mutex_lock(&_pthread_key_lock);
				if ((uintptr_t) _pthread_key_dest[i] > 1)
				{
					/* Call destructor */
					t->keyval[i] = NULL;
					_pthread_key_dest[i](val);
					flag = 1;
				}
				pthread_mutex_unlock(&_pthread_key_lock);
			}
		}
	
		/* Nothing to do? */
		if (!flag) return;
	}
}

static int pthread_key_create(pthread_key_t *key, void (* dest)(void *))
{
	typedef void (**d_func)(void*);
	unsigned int i;
	long nmax;
	d_func d;

	if (!key) return EINVAL;
	
	pthread_mutex_lock(&_pthread_key_lock);
	
	for (i = _pthread_key_sch; i < _pthread_key_max; i++)
	{
		if (!_pthread_key_dest[i])
		{
			*key = i;
			if (dest)
			{
				_pthread_key_dest[i] = dest;
			}
			else
			{
				_pthread_key_dest[i] = (void(*)(void *))1;
			}
			pthread_mutex_unlock(&_pthread_key_lock);
			
			return 0;
		}
	}
	
	for (i = 0; i < _pthread_key_sch; i++)
	{
		if (!_pthread_key_dest[i])
		{
			*key = i;
			if (dest)
			{
				_pthread_key_dest[i] = dest;
			}
			else
			{
				_pthread_key_dest[i] = (void(*)(void *))1;
			}
			pthread_mutex_unlock(&_pthread_key_lock);
			
			return 0;
		}
	}
	
	if (!_pthread_key_max) _pthread_key_max = 1;
	if (_pthread_key_max == PTHREAD_KEYS_MAX)
	{
		pthread_mutex_unlock(&_pthread_key_lock);
		
		return ENOMEM;
	}
	
	nmax = _pthread_key_max * 2;
	if (nmax > PTHREAD_KEYS_MAX) nmax = PTHREAD_KEYS_MAX;
	
	/* No spare room anywhere */
	d = (d_func)realloc(_pthread_key_dest, nmax * sizeof(*d));
	if (!d)
	{
		pthread_mutex_unlock(&_pthread_key_lock);
		
		return ENOMEM;
	}
	
	/* Clear new region */
	memset((void *) &d[_pthread_key_max], 0, (nmax-_pthread_key_max)*sizeof(void *));
	
	/* Use new region */
	_pthread_key_dest = d;
	_pthread_key_sch = _pthread_key_max + 1;
	*key = _pthread_key_max;
	_pthread_key_max = nmax;
	
	if (dest)
	{
		_pthread_key_dest[*key] = dest;
	}
	else
	{
		_pthread_key_dest[*key] = (void(*)(void *))1;
	}

	pthread_mutex_unlock(&_pthread_key_lock);
	
	return 0;
}

static int pthread_key_delete(pthread_key_t key)
{
	if (key > _pthread_key_max) return EINVAL;
	if (!_pthread_key_dest) return EINVAL;
	
	pthread_mutex_lock(&_pthread_key_lock);
	_pthread_key_dest[key] = NULL;
	
	/* Start next search from our location */
	if (_pthread_key_sch > key) _pthread_key_sch = key;
	
	pthread_mutex_unlock(&_pthread_key_lock);
	
	return 0;
}

static void *pthread_getspecific(pthread_key_t key)
{
	pthread_t t = pthread_self();
	
	if (key >= t->keymax) return NULL;
	
	return t->keyval[key];

}

static int pthread_setspecific(pthread_key_t key, const void *value)
{
	pthread_t t = pthread_self();

	if (key > t->keymax)
	{
		int keymax = (key + 1) * 2;
		void **kv = (void**)realloc(t->keyval, keymax * sizeof(void *));
		
		if (!kv) return ENOMEM;
		
		/* Clear new region */
		memset(&kv[t->keymax], 0, (keymax - t->keymax)*sizeof(void*));
		
		t->keyval = kv;
		t->keymax = keymax;
	}
	
	t->keyval[key] = (void *) value;
	
	return 0;
}


#endif /* DISPATCH_WIN_PTHREAD_TLS_ */
