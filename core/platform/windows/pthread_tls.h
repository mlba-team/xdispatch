#ifndef DISPATCH_WIN_PTHREAD_TLS_
#define DISPATCH_WIN_PTHREAD_TLS_

static void _pthread_clean_tls(){

}


#define PTHREAD_KEYS_MAX (1<<20)

pthread_rwlock_t _pthread_key_lock;
long _pthread_key_max;
long _pthread_key_sch;
void (**_pthread_key_dest)(void *);

typedef unsigned pthread_key_t;

static int pthread_key_create(pthread_key_t *key, void (* dest)(void *))
{
	int i;
	long nmax;
	void (**d)(void *);

	if (!key) return EINVAL;
	
	pthread_rwlock_wrlock(&_pthread_key_lock);
	
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
			pthread_rwlock_unlock(&_pthread_key_lock);
			
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
			pthread_rwlock_unlock(&_pthread_key_lock);
			
			return 0;
		}
	}
	
	if (!_pthread_key_max) _pthread_key_max = 1;
	if (_pthread_key_max == PTHREAD_KEYS_MAX)
	{
		pthread_rwlock_unlock(&_pthread_key_lock);
		
		return ENOMEM;
	}
	
	nmax = _pthread_key_max * 2;
	if (nmax > PTHREAD_KEYS_MAX) nmax = PTHREAD_KEYS_MAX;
	
	/* No spare room anywhere */
	d = realloc(_pthread_key_dest, nmax * sizeof(*d));
	if (!d)
	{
		pthread_rwlock_unlock(&_pthread_key_lock);
		
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

	pthread_rwlock_unlock(&_pthread_key_lock);
	
	return 0;
}

static int pthread_key_delete(pthread_key_t key)
{
	if (key > _pthread_key_max) return EINVAL;
	if (!_pthread_key_dest) return EINVAL;
	
	pthread_rwlock_wrlock(&_pthread_key_lock);
	_pthread_key_dest[key] = NULL;
	
	/* Start next search from our location */
	if (_pthread_key_sch > key) _pthread_key_sch = key;
	
	pthread_rwlock_unlock(&_pthread_key_lock);
	
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
		void **kv = realloc(t->keyval, keymax * sizeof(void *));
		
		if (!kv) return ENOMEM;
		
		/* Clear new region */
		memset(&kv[t->keymax], 0, (keymax - t->keymax)*sizeof(void*));
		
		t->keyval = kv;
		t->keymax = keymax;
	}
	
	t->keyval[key] = (void *) value;
	
	return 0;
}


//typedef DWORD pthread_key_t;
//#define pthread_key_create(k,d) (!((*k=TlsAlloc())!=TLS_OUT_OF_INDEXES))
//#define pthread_setspecific(k,v) (!(TlsSetValue(k, v)))
//#define pthread_getspecific(k) (TlsGetValue(k))

#endif /* DISPATCH_WIN_PTHREAD_TLS_ */
