#include "svd_si.h"
#ifdef HAVE_SVD

/************************************************************************
Storage of mpfr lists
************************************************************************/
std::vector< amp::mpfr_record* > _mpfr_storage_v;
VAR gmp_randstate_t _mpfr_storage_rs;
VAR bool _mpfr_storage_init = false;

amp::mpfr_record* amp::mpfr_storage::newMpfr(unsigned int Precision)
{
    amp::mpfr_record_ptr &lst = getList(Precision);
    if( lst==NULL )
    {
        amp::mpfr_record *rec = new amp::mpfr_record;
        rec->Precision = Precision;
        rec->refCount = 0;
        mpfr_init2(rec->value, Precision);
        rec->next = lst;
        lst = rec;
    }

    amp::mpfr_record *p = lst;
    p->refCount = 1;
    lst = lst->next;
    return p;
}

void amp::mpfr_storage::deleteMpfr(amp::mpfr_record* ref)
{
    amp::mpfr_record_ptr &lst = getList(ref->Precision);
    ref->next = lst;
    lst = ref;
}

gmp_randstate_t* amp::mpfr_storage::getRandState()
{
    if( !_mpfr_storage_init )
    {
        time_t _timer;
        gmp_randinit_default(_mpfr_storage_rs);
        gmp_randseed_ui(_mpfr_storage_rs, (unsigned long int)(time(&_timer)));
        _mpfr_storage_init = true;
    }
    return &_mpfr_storage_rs;
}

/*amp::mpfr_storage::clearStorage()
{
    unsigned int i;
    amp::mpfr_record *p;
    for(i=0; i<v.size(); i++)
        while( v[i]!=NULL )
        {
            p = _mpfr_storage_v[i]->next;
            mpfr_clear(_mpfr_storage_v[i]->value);
            delete _mpfr_storage_v[i];
            _mpfr_storage_v[i] = p;
        }
}*/

amp::mpfr_record_ptr& amp::mpfr_storage::getList(unsigned int Precision)
{
    static amp::mpfr_record_ptr tmp       = NULL;
    STATIC_VAR int lastPrec                   = -1;
    static amp::mpfr_record_ptr &lastList = tmp;
    if( lastPrec!=Precision )
    {
        while( _mpfr_storage_v.size()<Precision+1 )
            _mpfr_storage_v.push_back(NULL);
        lastPrec = (int)Precision;
        lastList = _mpfr_storage_v[Precision];
    }
    return lastList;
}


/************************************************************************
Storage of mpfr lists
************************************************************************/
amp::mpfr_reference::mpfr_reference()
{
    ref = NULL;
}

amp::mpfr_reference::mpfr_reference(const amp::mpfr_reference& r)
{
    ref = r.ref;
    if( ref!=NULL )
        ref->refCount++;
}

amp::mpfr_reference& amp::mpfr_reference::operator= (
    const amp::mpfr_reference &r)
{
    if ( &r==this )
        return *this;
    if ( ref==r.ref )
        return *this;
    if( ref!=NULL )
        free();
    ref = r.ref;
    if( ref!=NULL )
        ref->refCount++;
    return *this;
}

amp::mpfr_reference::~mpfr_reference()
{
    if( ref!=NULL )
        free();
}

void amp::mpfr_reference::initialize(int Precision)
{
    if( ref!=NULL )
        free();
    ref = amp::mpfr_storage::newMpfr(Precision);
    ref->refCount = 1;
}

void amp::mpfr_reference::free()
{
    if( ref==NULL )
        //throw amp::internalError();
	{WerrorS("internalError");return;}
    ref->refCount--;
    if( ref->refCount==0 )
        amp::mpfr_storage::deleteMpfr(ref);
    ref = NULL;
}

mpfr_srcptr amp::mpfr_reference::getReadPtr() const
{
    if( ref==NULL )
        //throw amp::internalError();
	{WerrorS("internalError");return NULL;}
    return ref->value;
}

mpfr_ptr amp::mpfr_reference::getWritePtr()
{
    if( ref==NULL )
        //throw amp::internalError();
	{WerrorS("internalError");return NULL;}
    if( ref->refCount==1 )
        return ref->value;

    amp::mpfr_record *newref = amp::mpfr_storage::newMpfr(ref->Precision);
    mpfr_set(newref->value, ref->value, GMP_RNDN);

    free();
    ref = newref;
    return ref->value;
}
#endif
