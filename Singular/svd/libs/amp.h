#ifndef _AMP_R_H
#define _AMP_R_H

#include <gmp.h>
#include <mpfr.h>
#include <stdexcept>
#include <math.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <vector>
#include <list>
#include <ap.h>

//#define _AMP_NO_TEMPLATE_CONSTRUCTORS

namespace amp
{
    class exception {};
    class incorrectPrecision    : public exception {};
    class overflow              : public exception {};
    class divisionByZero        : public exception {};
    class sqrtOfNegativeNumber  : public exception {};
    class invalidConversion     : public exception {};
    class invalidString         : public exception {};
    class internalError         : public exception {};
    class domainError           : public exception {};

    typedef unsigned long unsigned32;
    typedef signed long   signed32;

    struct mpfr_record
    {
        unsigned int refCount;
        unsigned int Precision;
        mpfr_t value;
        mpfr_record *next;
    };

    typedef mpfr_record* mpfr_record_ptr;

    //
    // storage for mpfr_t instances
    //
    class mpfr_storage
    {
    public:
        static mpfr_record* newMpfr(unsigned int Precision);
        static void deleteMpfr(mpfr_record* ref);
        /*static void clearStorage();*/
        static gmp_randstate_t* getRandState();
    private:
        static mpfr_record_ptr& getList(unsigned int Precision);
    };

    //
    // mpfr_t reference
    //
    class mpfr_reference
    {
    public:
        mpfr_reference();
        mpfr_reference(const mpfr_reference& r);
        mpfr_reference& operator= (const mpfr_reference &r);
        ~mpfr_reference();

        void initialize(int Precision);
        void free();

        mpfr_srcptr getReadPtr() const;
        mpfr_ptr getWritePtr();
    private:
        mpfr_record *ref;
    };

    //
    // ampf template
    //
    template<unsigned int Precision>
    class ampf
    {
    public:
        //
        // Destructor
        //
        ~ampf()
        {
            rval->refCount--;
            if( rval->refCount==0 )
                mpfr_storage::deleteMpfr(rval);
        }

        //
        // Initializing
        //
        ampf ()                 { InitializeAsZero(); }
        ampf(mpfr_record *v)    { rval = v; }

        ampf (long double v)    { InitializeAsDouble(v); }
        ampf (double v)         { InitializeAsDouble(v); }
        ampf (float v)          { InitializeAsDouble(v); }
        ampf (signed long v)    { InitializeAsSLong(v); }
        ampf (unsigned long v)  { InitializeAsULong(v); }
        ampf (signed int v)     { InitializeAsSLong(v); }
        ampf (unsigned int v)   { InitializeAsULong(v); }
        ampf (signed short v)   { InitializeAsSLong(v); }
        ampf (unsigned short v) { InitializeAsULong(v); }
        ampf (signed char v)    { InitializeAsSLong(v); }
        ampf (unsigned char v)  { InitializeAsULong(v); }

        //
        // initializing from string
        // string s must have format "X0.hhhhhhhh@eee" or "X-0.hhhhhhhh@eee"
        //
        ampf (const std::string &s) { InitializeAsString(s.c_str()); }
        ampf (const char *s)        { InitializeAsString(s); }

        //
        // copy constructors
        //
        ampf(const ampf& r)
        {
            rval = r.rval;
            rval->refCount++;
        }
#ifndef _AMP_NO_TEMPLATE_CONSTRUCTORS
        template<unsigned int Precision2>
        ampf(const ampf<Precision2>& r)
        {
            CheckPrecision();
            rval = mpfr_storage::newMpfr(Precision);
            mpfr_set(getWritePtr(), r.getReadPtr(), GMP_RNDN);
        }
#endif

        //
        // Assignment constructors
        //
        ampf& operator= (long double v)         { mpfr_set_ld(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (double v)              { mpfr_set_ld(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (float v)               { mpfr_set_ld(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (signed long v)         { mpfr_set_si(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (unsigned long v)       { mpfr_set_ui(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (signed int v)          { mpfr_set_si(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (unsigned int v)        { mpfr_set_ui(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (signed short v)        { mpfr_set_si(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (unsigned short v)      { mpfr_set_ui(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (signed char v)         { mpfr_set_si(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (unsigned char v)       { mpfr_set_ui(getWritePtr(), v, GMP_RNDN); return *this; }
        ampf& operator= (const char *s)         { mpfr_strtofr(getWritePtr(), s, NULL, 0, GMP_RNDN); return *this; }
        ampf& operator= (const std::string &s)  { mpfr_strtofr(getWritePtr(), s.c_str(), NULL, 0, GMP_RNDN); return *this; }
        ampf& operator= (const ampf& r)
        {
            // TODO: may be copy ref
            if( this==&r )
                return *this;
            if( rval==r.rval )
                return *this;
            rval->refCount--;
            if( rval->refCount==0 )
                mpfr_storage::deleteMpfr(rval);
            rval = r.rval;
            rval->refCount++;
            //mpfr_set(getWritePtr(), r.getReadPtr(), GMP_RNDN);
            return *this;
        }
#ifndef _AMP_NO_TEMPLATE_CONSTRUCTORS
        template<unsigned int Precision2>
        ampf& operator= (const ampf<Precision2>& r)
        {
            if( (void*)this==(void*)(&r) )
                return *this;
            mpfr_set(getWritePtr(), r.getReadPtr(), GMP_RNDN);
            return *this;
        }
#endif

        //
        // in-place operators
        // TODO: optimize
        //
        template<class T> ampf& operator+=(const T& v){ *this = *this + v; return *this; };
        template<class T> ampf& operator-=(const T& v){ *this = *this - v; return *this; };
        template<class T> ampf& operator*=(const T& v){ *this = *this * v; return *this; };
        template<class T> ampf& operator/=(const T& v){ *this = *this / v; return *this; };

        //
        // MPFR access
        //
        mpfr_srcptr getReadPtr() const;
        mpfr_ptr getWritePtr();

        //
        // properties and information
        //
        bool isFiniteNumber() const;
        bool isPositiveNumber() const;
        bool isZero() const;
        bool isNegativeNumber() const;
        const ampf getUlpOf();

        //
        // conversions
        //
        double toDouble() const;
        std::string toHex() const;
        std::string toDec() const;
        char * toString() const;


        //
        // static methods
        //
        static const ampf getUlpOf(const ampf &x);
        static const ampf getUlp();
        static const ampf getUlp256();
        static const ampf getUlp512();
        static const ampf getMaxNumber();
        static const ampf getMinNumber();
        static const ampf getAlgoPascalEpsilon();
        static const ampf getAlgoPascalMaxNumber();
        static const ampf getAlgoPascalMinNumber();
        static const ampf getRandom();
    private:
        void CheckPrecision();
        void InitializeAsZero();
        void InitializeAsSLong(signed long v);
        void InitializeAsULong(unsigned long v);
        void InitializeAsDouble(long double v);
        void InitializeAsString(const char *s);

        //mpfr_reference  ref;
        mpfr_record *rval;
    };

    /*void ampf<Precision>::CheckPrecision()
    {
        if( Precision<32 )
            throw incorrectPrecision();
    }***/

    template<unsigned int Precision>
    void ampf<Precision>::CheckPrecision()
    {
        if( Precision<32 )
            throw incorrectPrecision();
    }

    template<unsigned int Precision>
    void ampf<Precision>::InitializeAsZero()
    {
        CheckPrecision();
        rval = mpfr_storage::newMpfr(Precision);
        mpfr_set_ui(getWritePtr(), 0, GMP_RNDN);
    }

    template<unsigned int Precision>
    void ampf<Precision>::InitializeAsSLong(signed long sv)
    {
        CheckPrecision();
        rval = mpfr_storage::newMpfr(Precision);
        mpfr_set_si(getWritePtr(), sv, GMP_RNDN);
    }

    template<unsigned int Precision>
    void ampf<Precision>::InitializeAsULong(unsigned long v)
    {
        CheckPrecision();
        rval = mpfr_storage::newMpfr(Precision);
        mpfr_set_ui(getWritePtr(), v, GMP_RNDN);
    }

    template<unsigned int Precision>
    void ampf<Precision>::InitializeAsDouble(long double v)
    {
        CheckPrecision();
        rval = mpfr_storage::newMpfr(Precision);
        mpfr_set_ld(getWritePtr(), v, GMP_RNDN);
    }

    template<unsigned int Precision>
    void ampf<Precision>::InitializeAsString(const char *s)
    {
        CheckPrecision();
        rval = mpfr_storage::newMpfr(Precision);
        mpfr_strtofr(getWritePtr(), s, NULL, 0, GMP_RNDN);
    }

    template<unsigned int Precision>
    mpfr_srcptr ampf<Precision>::getReadPtr() const
    {
        // TODO: подумать, нужно ли сделать, чтобы и при getRead, и при
        //       getWrite создавалась новая instance mpfr_t.
        //       это может быть нужно для корректной обработки ситуаций вида
        //       mpfr_чего_то_там( a.getWritePtr(), a.getReadPtr())
        //       вроде бы нужно, а то если там завязано на side-effects...
        return rval->value;
    }

    template<unsigned int Precision>
    mpfr_ptr ampf<Precision>::getWritePtr()
    {
        if( rval->refCount==1 )
            return rval->value;
        mpfr_record *newrval = mpfr_storage::newMpfr(Precision);
        mpfr_set(newrval->value, rval->value, GMP_RNDN);
        rval->refCount--;
        rval = newrval;
        return rval->value;
    }

    template<unsigned int Precision>
    bool ampf<Precision>::isFiniteNumber() const
    {
        return mpfr_number_p(getReadPtr())!=0;
    }

    template<unsigned int Precision>
    bool ampf<Precision>::isPositiveNumber() const
    {
        if( !isFiniteNumber() )
            return false;
        return mpfr_sgn(getReadPtr())>0;
    }

    template<unsigned int Precision>
    bool ampf<Precision>::isZero() const
    {
        return mpfr_zero_p(getReadPtr())!=0;
    }

    template<unsigned int Precision>
    bool ampf<Precision>::isNegativeNumber() const
    {
        if( !isFiniteNumber() )
            return false;
        return mpfr_sgn(getReadPtr())<0;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getUlpOf()
    {
        return getUlpOf(*this);
    }

    template<unsigned int Precision>
    double ampf<Precision>::toDouble() const
    {
        return mpfr_get_d(getReadPtr(), GMP_RNDN);
    }

    template<unsigned int Precision>
    std::string ampf<Precision>::toHex() const
    {
        //
        // some special cases
        //
        if( !isFiniteNumber() )
        {
            std::string r;
            mp_exp_t _e;
            char *ptr;
            ptr = mpfr_get_str(NULL, &_e, 16, 0, getReadPtr(), GMP_RNDN);
            r = ptr;
            mpfr_free_str(ptr);
            return r;
        }

        //
        // general case
        //
        std::string r;
        char buf_e[128];
        signed long iexpval;
        mp_exp_t expval;
        char *ptr;
        char *ptr2;
        ptr = mpfr_get_str(NULL, &expval, 16, 0, getReadPtr(), GMP_RNDN);
        ptr2 = ptr;
        iexpval = expval;
        if( iexpval!=expval )
            throw internalError();
        sprintf(buf_e, "%ld", long(iexpval));
        if( *ptr=='-' )
        {
            r = "-";
            ptr++;
        }
        r += "0x0.";
        r += ptr;
        r += "@";
        r += buf_e;
        mpfr_free_str(ptr2);
        return r;
    }

    template<unsigned int Precision>
    std::string ampf<Precision>::toDec() const
    {
        // TODO: advanced output formatting (zero, integers)

        //
        // some special cases
        //
        if( !isFiniteNumber() )
        {
            std::string r;
            mp_exp_t _e;
            char *ptr;
            ptr = mpfr_get_str(NULL, &_e, 10, 0, getReadPtr(), GMP_RNDN);
            r = ptr;
            mpfr_free_str(ptr);
            return r;
        }

        //
        // general case
        //
        std::string r;
        char buf_e[128];
        signed long iexpval;
        mp_exp_t expval;
        char *ptr;
        char *ptr2;
        ptr = mpfr_get_str(NULL, &expval, 10, 0, getReadPtr(), GMP_RNDN);
        ptr2 = ptr;
        iexpval = expval;
        if( iexpval!=expval )
            throw internalError();
        sprintf(buf_e, "%ld", long(iexpval));
        if( *ptr=='-' )
        {
            r = "-";
            ptr++;
        }
        r += "0.";
        r += ptr;
        r += "E";
        r += buf_e;
        mpfr_free_str(ptr2);
        return r;
    }
    template<unsigned int Precision>
    char * ampf<Precision>::toString() const
    {
         char *toString_Block=(char *)omAlloc(256);
        //
        // some special cases
        //
        if( !isFiniteNumber() )
        {
            mp_exp_t _e;
            char *ptr;
            ptr = mpfr_get_str(NULL, &_e, 10, 0, getReadPtr(), GMP_RNDN);
            strcpy(toString_Block, ptr);
            mpfr_free_str(ptr);
            return toString_Block;
        }

        //
        // general case
        //

        char buf_e[128];
        signed long iexpval;
        mp_exp_t expval;
        char *ptr;
        char *ptr2;
        ptr = mpfr_get_str(NULL, &expval, 10, 0, getReadPtr(), GMP_RNDN);
        ptr2 = ptr;
        iexpval = expval;
        if( iexpval!=expval )
            throw internalError();
        sprintf(buf_e, "%ld", long(iexpval));
        if( *ptr=='-' )
        {
            ptr++;
           sprintf(toString_Block,"-0.%sE%s",ptr,buf_e);
        }
        else
          sprintf(toString_Block,"0.%sE%s",ptr,buf_e);
        mpfr_free_str(ptr2);
        return toString_Block;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getUlpOf(const ampf<Precision> &x)
    {
        if( !x.isFiniteNumber() )
            return x;
        if( x.isZero() )
            return x;
        ampf<Precision> r(1);
        mpfr_nextabove(r.getWritePtr());
        mpfr_sub_ui(r.getWritePtr(), r.getWritePtr(), 1, GMP_RNDN);
        mpfr_mul_2si(
            r.getWritePtr(),
            r.getWritePtr(),
            mpfr_get_exp(x.getReadPtr()),
            GMP_RNDN);
        mpfr_div_2si(
            r.getWritePtr(),
            r.getWritePtr(),
            1,
            GMP_RNDN);
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getUlp()
    {
        ampf<Precision> r(1);
        mpfr_nextabove(r.getWritePtr());
        mpfr_sub_ui(r.getWritePtr(), r.getWritePtr(), 1, GMP_RNDN);
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getUlp256()
    {
        ampf<Precision> r(1);
        mpfr_nextabove(r.getWritePtr());
        mpfr_sub_ui(r.getWritePtr(), r.getWritePtr(), 1, GMP_RNDN);
        mpfr_mul_2si(
            r.getWritePtr(),
            r.getWritePtr(),
            8,
            GMP_RNDN);
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getUlp512()
    {
        ampf<Precision> r(1);
        mpfr_nextabove(r.getWritePtr());
        mpfr_sub_ui(r.getWritePtr(), r.getWritePtr(), 1, GMP_RNDN);
        mpfr_mul_2si(
            r.getWritePtr(),
            r.getWritePtr(),
            9,
            GMP_RNDN);
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getMaxNumber()
    {
        ampf<Precision> r(1);
        mpfr_nextbelow(r.getWritePtr());
        mpfr_set_exp(r.getWritePtr(),mpfr_get_emax());
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getMinNumber()
    {
        ampf<Precision> r(1);
        mpfr_set_exp(r.getWritePtr(),mpfr_get_emin());
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getAlgoPascalEpsilon()
    {
        return getUlp256();
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getAlgoPascalMaxNumber()
    {
        ampf<Precision> r(1);
        mp_exp_t e1 = mpfr_get_emax();
        mp_exp_t e2 = -mpfr_get_emin();
        mp_exp_t e  = e1>e2 ? e1 : e2;
        mpfr_set_exp(r.getWritePtr(), e-5);
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getAlgoPascalMinNumber()
    {
        ampf<Precision> r(1);
        mp_exp_t e1 = mpfr_get_emax();
        mp_exp_t e2 = -mpfr_get_emin();
        mp_exp_t e  = e1>e2 ? e1 : e2;
        mpfr_set_exp(r.getWritePtr(), 2-(e-5));
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ampf<Precision>::getRandom()
    {
        ampf<Precision> r;
        while(mpfr_urandomb(r.getWritePtr(), *amp::mpfr_storage::getRandState()));
        return r;
    }

    //
    // comparison operators
    //
    template<unsigned int Precision>
    const bool operator==(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        return mpfr_cmp(op1.getReadPtr(), op2.getReadPtr())==0;
    }

    template<unsigned int Precision>
    const bool operator!=(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        return mpfr_cmp(op1.getReadPtr(), op2.getReadPtr())!=0;
    }

    template<unsigned int Precision>
    const bool operator<(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        return mpfr_cmp(op1.getReadPtr(), op2.getReadPtr())<0;
    }

    template<unsigned int Precision>
    const bool operator>(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        return mpfr_cmp(op1.getReadPtr(), op2.getReadPtr())>0;
    }

    template<unsigned int Precision>
    const bool operator<=(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        return mpfr_cmp(op1.getReadPtr(), op2.getReadPtr())<=0;
    }

    template<unsigned int Precision>
    const bool operator>=(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        return mpfr_cmp(op1.getReadPtr(), op2.getReadPtr())>=0;
    }

    //
    // arithmetic operators
    //
    template<unsigned int Precision>
    const ampf<Precision> operator+(const ampf<Precision>& op1)
    {
        return op1;
    }

    template<unsigned int Precision>
    const ampf<Precision> operator-(const ampf<Precision>& op1)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_neg(v->value, op1.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> operator+(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_add(v->value, op1.getReadPtr(), op2.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> operator-(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_sub(v->value, op1.getReadPtr(), op2.getReadPtr(), GMP_RNDN);
        return v;
    }


    template<unsigned int Precision>
    const ampf<Precision> operator*(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_mul(v->value, op1.getReadPtr(), op2.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> operator/(const ampf<Precision>& op1, const ampf<Precision>& op2)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_div(v->value, op1.getReadPtr(), op2.getReadPtr(), GMP_RNDN);
        return v;
    }

    //
    // basic functions
    //
    template<unsigned int Precision>
    const ampf<Precision> sqr(const ampf<Precision> &x)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> res;
        mpfr_sqr(res.getWritePtr(), x.getReadPtr(), GMP_RNDN);
        return res;
    }

    template<unsigned int Precision>
    const int sign(const ampf<Precision> &x)
    {
        int s = mpfr_sgn(x.getReadPtr());
        if( s>0 )
            return +1;
        if( s<0 )
            return -1;
        return 0;
    }

    template<unsigned int Precision>
    const ampf<Precision> abs(const ampf<Precision> &x)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> res;
        mpfr_abs(res.getWritePtr(), x.getReadPtr(), GMP_RNDN);
        return res;
    }

    template<unsigned int Precision>
    const ampf<Precision> maximum(const ampf<Precision> &x, const ampf<Precision> &y)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> res;
        mpfr_max(res.getWritePtr(), x.getReadPtr(), y.getReadPtr(), GMP_RNDN);
        return res;
    }

    template<unsigned int Precision>
    const ampf<Precision> minimum(const ampf<Precision> &x, const ampf<Precision> &y)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> res;
        mpfr_min(res.getWritePtr(), x.getReadPtr(), y.getReadPtr(), GMP_RNDN);
        return res;
    }

    template<unsigned int Precision>
    const ampf<Precision> sqrt(const ampf<Precision> &x)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> res;
        mpfr_sqrt(res.getWritePtr(), x.getReadPtr(), GMP_RNDN);
        return res;
    }

    template<unsigned int Precision>
    const signed long trunc(const ampf<Precision> &x)
    {
        ampf<Precision> tmp;
        signed long r;
        mpfr_trunc(tmp.getWritePtr(), x.getReadPtr());
        if( mpfr_integer_p(tmp.getReadPtr())==0 )
            throw invalidConversion();
        mpfr_clear_erangeflag();
        r = mpfr_get_si(tmp.getReadPtr(), GMP_RNDN);
        if( mpfr_erangeflag_p()!=0 )
            throw invalidConversion();
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> frac(const ampf<Precision> &x)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> r;
        mpfr_frac(r.getWritePtr(), x.getReadPtr(), GMP_RNDN);
        return r;
    }

    template<unsigned int Precision>
    const signed long floor(const ampf<Precision> &x)
    {
        ampf<Precision> tmp;
        signed long r;
        mpfr_floor(tmp.getWritePtr(), x.getReadPtr());
        if( mpfr_integer_p(tmp.getReadPtr())==0 )
            throw invalidConversion();
        mpfr_clear_erangeflag();
        r = mpfr_get_si(tmp.getReadPtr(), GMP_RNDN);
        if( mpfr_erangeflag_p()!=0 )
            throw invalidConversion();
        return r;
    }

    template<unsigned int Precision>
    const signed long ceil(const ampf<Precision> &x)
    {
        ampf<Precision> tmp;
        signed long r;
        mpfr_ceil(tmp.getWritePtr(), x.getReadPtr());
        if( mpfr_integer_p(tmp.getReadPtr())==0 )
            throw invalidConversion();
        mpfr_clear_erangeflag();
        r = mpfr_get_si(tmp.getReadPtr(), GMP_RNDN);
        if( mpfr_erangeflag_p()!=0 )
            throw invalidConversion();
        return r;
    }

    template<unsigned int Precision>
    const signed long round(const ampf<Precision> &x)
    {
        ampf<Precision> tmp;
        signed long r;
        mpfr_round(tmp.getWritePtr(), x.getReadPtr());
        if( mpfr_integer_p(tmp.getReadPtr())==0 )
            throw invalidConversion();
        mpfr_clear_erangeflag();
        r = mpfr_get_si(tmp.getReadPtr(), GMP_RNDN);
        if( mpfr_erangeflag_p()!=0 )
            throw invalidConversion();
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> frexp2(const ampf<Precision> &x, mp_exp_t *exponent)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> r;
        if( !x.isFiniteNumber() )
            throw invalidConversion();
        if( x.isZero() )
        {
            *exponent = 0;
            r = 0;
            return r;
        }
        r = x;
        *exponent = mpfr_get_exp(r.getReadPtr());
        mpfr_set_exp(r.getWritePtr(),0);
        return r;
    }

    template<unsigned int Precision>
    const ampf<Precision> ldexp2(const ampf<Precision> &x, mp_exp_t exponent)
    {
        // TODO: optimize temporary for return value
        ampf<Precision> r;
        mpfr_mul_2si(r.getWritePtr(), x.getReadPtr(), exponent, GMP_RNDN);
        return r;
    }

    //
    // different types of arguments
    //
    #define __AMP_BINARY_OPI(type) \
        template<unsigned int Precision> const ampf<Precision> operator+(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)+op2; }   \
        template<unsigned int Precision> const ampf<Precision> operator+(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)+op2; } \
        template<unsigned int Precision> const ampf<Precision> operator+(const ampf<Precision>& op1, const signed type& op2) { return op1+ampf<Precision>(op2); }   \
        template<unsigned int Precision> const ampf<Precision> operator+(const ampf<Precision>& op1, const unsigned type& op2) { return op1+ampf<Precision>(op2); } \
        template<unsigned int Precision> const ampf<Precision> operator-(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)-op2; }   \
        template<unsigned int Precision> const ampf<Precision> operator-(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)-op2; } \
        template<unsigned int Precision> const ampf<Precision> operator-(const ampf<Precision>& op1, const signed type& op2) { return op1-ampf<Precision>(op2); }   \
        template<unsigned int Precision> const ampf<Precision> operator-(const ampf<Precision>& op1, const unsigned type& op2) { return op1-ampf<Precision>(op2); } \
        template<unsigned int Precision> const ampf<Precision> operator*(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)*op2; }   \
        template<unsigned int Precision> const ampf<Precision> operator*(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)*op2; } \
        template<unsigned int Precision> const ampf<Precision> operator*(const ampf<Precision>& op1, const signed type& op2) { return op1*ampf<Precision>(op2); }   \
        template<unsigned int Precision> const ampf<Precision> operator*(const ampf<Precision>& op1, const unsigned type& op2) { return op1*ampf<Precision>(op2); } \
        template<unsigned int Precision> const ampf<Precision> operator/(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)/op2; }   \
        template<unsigned int Precision> const ampf<Precision> operator/(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)/op2; } \
        template<unsigned int Precision> const ampf<Precision> operator/(const ampf<Precision>& op1, const signed type& op2) { return op1/ampf<Precision>(op2); }   \
        template<unsigned int Precision> const ampf<Precision> operator/(const ampf<Precision>& op1, const unsigned type& op2) { return op1/ampf<Precision>(op2); } \
        template<unsigned int Precision> const bool       operator==(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)==op2; }   \
        template<unsigned int Precision> const bool       operator==(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)==op2; } \
        template<unsigned int Precision> const bool       operator==(const ampf<Precision>& op1, const signed type& op2) { return op1==ampf<Precision>(op2); }   \
        template<unsigned int Precision> const bool       operator==(const ampf<Precision>& op1, const unsigned type& op2) { return op1==ampf<Precision>(op2); } \
        template<unsigned int Precision> const bool       operator!=(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)!=op2; }   \
        template<unsigned int Precision> const bool       operator!=(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)!=op2; } \
        template<unsigned int Precision> const bool       operator!=(const ampf<Precision>& op1, const signed type& op2) { return op1!=ampf<Precision>(op2); }   \
        template<unsigned int Precision> const bool       operator!=(const ampf<Precision>& op1, const unsigned type& op2) { return op1!=ampf<Precision>(op2); } \
        template<unsigned int Precision> const bool       operator<=(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)<=op2; }   \
        template<unsigned int Precision> const bool       operator<=(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)<=op2; } \
        template<unsigned int Precision> const bool       operator<=(const ampf<Precision>& op1, const signed type& op2) { return op1<=ampf<Precision>(op2); }   \
        template<unsigned int Precision> const bool       operator<=(const ampf<Precision>& op1, const unsigned type& op2) { return op1<=ampf<Precision>(op2); } \
        template<unsigned int Precision> const bool       operator>=(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)>=op2; }   \
        template<unsigned int Precision> const bool       operator>=(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)>=op2; } \
        template<unsigned int Precision> const bool       operator>=(const ampf<Precision>& op1, const signed type& op2) { return op1>=ampf<Precision>(op2); }   \
        template<unsigned int Precision> const bool       operator>=(const ampf<Precision>& op1, const unsigned type& op2) { return op1>=ampf<Precision>(op2); } \
        template<unsigned int Precision> const bool       operator<(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)<op2; }   \
        template<unsigned int Precision> const bool       operator<(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)<op2; } \
        template<unsigned int Precision> const bool       operator<(const ampf<Precision>& op1, const signed type& op2) { return op1<ampf<Precision>(op2); }   \
        template<unsigned int Precision> const bool       operator<(const ampf<Precision>& op1, const unsigned type& op2) { return op1<ampf<Precision>(op2); } \
        template<unsigned int Precision> const bool       operator>(const signed type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)>op2; }   \
        template<unsigned int Precision> const bool       operator>(const unsigned type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)>op2; } \
        template<unsigned int Precision> const bool       operator>(const ampf<Precision>& op1, const signed type& op2) { return op1>ampf<Precision>(op2); }   \
        template<unsigned int Precision> const bool       operator>(const ampf<Precision>& op1, const unsigned type& op2) { return op1>ampf<Precision>(op2); }
    __AMP_BINARY_OPI(char)
    __AMP_BINARY_OPI(short)
    __AMP_BINARY_OPI(long)
    __AMP_BINARY_OPI(int)
    #undef __AMP_BINARY_OPI
    #define __AMP_BINARY_OPF(type) \
        template<unsigned int Precision> const ampf<Precision> operator+(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)+op2; } \
        template<unsigned int Precision> const ampf<Precision> operator+(const ampf<Precision>& op1, const type& op2) { return op1+ampf<Precision>(op2); } \
        template<unsigned int Precision> const ampf<Precision> operator-(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)-op2; } \
        template<unsigned int Precision> const ampf<Precision> operator-(const ampf<Precision>& op1, const type& op2) { return op1-ampf<Precision>(op2); } \
        template<unsigned int Precision> const ampf<Precision> operator*(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)*op2; } \
        template<unsigned int Precision> const ampf<Precision> operator*(const ampf<Precision>& op1, const type& op2) { return op1*ampf<Precision>(op2); } \
        template<unsigned int Precision> const ampf<Precision> operator/(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)/op2; } \
        template<unsigned int Precision> const ampf<Precision> operator/(const ampf<Precision>& op1, const type& op2) { return op1/ampf<Precision>(op2); } \
        template<unsigned int Precision> bool             operator==(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)==op2; } \
        template<unsigned int Precision> bool             operator==(const ampf<Precision>& op1, const type& op2) { return op1==ampf<Precision>(op2); } \
        template<unsigned int Precision> bool             operator!=(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)!=op2; } \
        template<unsigned int Precision> bool             operator!=(const ampf<Precision>& op1, const type& op2) { return op1!=ampf<Precision>(op2); } \
        template<unsigned int Precision> bool             operator<=(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)<=op2; } \
        template<unsigned int Precision> bool             operator<=(const ampf<Precision>& op1, const type& op2) { return op1<=ampf<Precision>(op2); } \
        template<unsigned int Precision> bool             operator>=(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)>=op2; } \
        template<unsigned int Precision> bool             operator>=(const ampf<Precision>& op1, const type& op2) { return op1>=ampf<Precision>(op2); } \
        template<unsigned int Precision> bool             operator<(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)<op2; } \
        template<unsigned int Precision> bool             operator<(const ampf<Precision>& op1, const type& op2) { return op1<ampf<Precision>(op2); } \
        template<unsigned int Precision> bool             operator>(const type& op1, const ampf<Precision>& op2) { return ampf<Precision>(op1)>op2; } \
        template<unsigned int Precision> bool             operator>(const ampf<Precision>& op1, const type& op2) { return op1>ampf<Precision>(op2); }
    __AMP_BINARY_OPF(float)
    __AMP_BINARY_OPF(double)
    __AMP_BINARY_OPF(long double)
    #undef __AMP_BINARY_OPF

    //
    // transcendent functions
    //
    template<unsigned int Precision>
    const ampf<Precision> pi()
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_const_pi(v->value, GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> halfpi()
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_const_pi(v->value, GMP_RNDN);
        mpfr_mul_2si(v->value, v->value, -1, GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> twopi()
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_const_pi(v->value, GMP_RNDN);
        mpfr_mul_2si(v->value, v->value, +1, GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> sin(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_sin(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> cos(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_cos(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> tan(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_tan(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> asin(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_asin(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> acos(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_acos(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> atan(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_atan(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> atan2(const ampf<Precision> &y, const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_atan2(v->value, y.getReadPtr(), x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> log(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_log(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> log2(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_log2(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> log10(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_log10(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> exp(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_exp(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> sinh(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_sinh(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> cosh(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_cosh(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> tanh(const ampf<Precision> &x)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_tanh(v->value, x.getReadPtr(), GMP_RNDN);
        return v;
    }

    template<unsigned int Precision>
    const ampf<Precision> pow(const ampf<Precision> &x, const ampf<Precision> &y)
    {
        mpfr_record *v = mpfr_storage::newMpfr(Precision);
        mpfr_pow(v->value, x.getReadPtr(), y.getReadPtr(), GMP_RNDN);
        return v;
    }

    //
    // complex ampf
    //
    template<unsigned int Precision>
    class campf
    {
    public:
        campf():x(0),y(0){};
        campf(long double v)    { x=v; y=0; }
        campf(double v)         { x=v; y=0; }
        campf(float v)          { x=v; y=0; }
        campf(signed long v)    { x=v; y=0; }
        campf(unsigned long v)  { x=v; y=0; }
        campf(signed int v)     { x=v; y=0; }
        campf(unsigned int v)   { x=v; y=0; }
        campf(signed short v)   { x=v; y=0; }
        campf(unsigned short v) { x=v; y=0; }
        campf(signed char v)    { x=v; y=0; }
        campf(unsigned char v)  { x=v; y=0; }
        campf(const ampf<Precision> &_x):x(_x),y(0){};
        campf(const ampf<Precision> &_x, const ampf<Precision> &_y):x(_x),y(_y){};
        campf(const campf &z):x(z.x),y(z.y){};
#ifndef _AMP_NO_TEMPLATE_CONSTRUCTORS
        template<unsigned int Prec2>
        campf(const campf<Prec2> &z):x(z.x),y(z.y){};
#endif

        campf& operator= (long double v)         { x=v; y=0; return *this; }
        campf& operator= (double v)              { x=v; y=0; return *this; }
        campf& operator= (float v)               { x=v; y=0; return *this; }
        campf& operator= (signed long v)         { x=v; y=0; return *this; }
        campf& operator= (unsigned long v)       { x=v; y=0; return *this; }
        campf& operator= (signed int v)          { x=v; y=0; return *this; }
        campf& operator= (unsigned int v)        { x=v; y=0; return *this; }
        campf& operator= (signed short v)        { x=v; y=0; return *this; }
        campf& operator= (unsigned short v)      { x=v; y=0; return *this; }
        campf& operator= (signed char v)         { x=v; y=0; return *this; }
        campf& operator= (unsigned char v)       { x=v; y=0; return *this; }
        campf& operator= (const char *s)         { x=s; y=0; return *this; }
        campf& operator= (const std::string &s)  { x=s; y=0; return *this; }
        campf& operator= (const campf& r)
        {
            x = r.x;
            y = r.y;
            return *this;
        }
#ifndef _AMP_NO_TEMPLATE_CONSTRUCTORS
        template<unsigned int Precision2>
        campf& operator= (const campf<Precision2>& r)
        {
            x = r.x;
            y = r.y;
            return *this;
        }
#endif

        ampf<Precision> x, y;
    };

    //
    // complex operations
    //
    template<unsigned int Precision>
    const bool operator==(const campf<Precision>& lhs, const campf<Precision>& rhs)
    { return lhs.x==rhs.x && lhs.y==rhs.y; }

    template<unsigned int Precision>
    const bool operator!=(const campf<Precision>& lhs, const campf<Precision>& rhs)
    { return lhs.x!=rhs.x || lhs.y!=rhs.y; }

    template<unsigned int Precision>
    const campf<Precision> operator+(const campf<Precision>& lhs)
    { return lhs; }

    template<unsigned int Precision>
    campf<Precision>& operator+=(campf<Precision>& lhs, const campf<Precision>& rhs)
    { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }

    template<unsigned int Precision>
    const campf<Precision> operator+(const campf<Precision>& lhs, const campf<Precision>& rhs)
    { campf<Precision> r = lhs; r += rhs; return r; }

    template<unsigned int Precision>
    const campf<Precision> operator-(const campf<Precision>& lhs)
    { return campf<Precision>(-lhs.x, -lhs.y); }

    template<unsigned int Precision>
    campf<Precision>& operator-=(campf<Precision>& lhs, const campf<Precision>& rhs)
    { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }

    template<unsigned int Precision>
    const campf<Precision> operator-(const campf<Precision>& lhs, const campf<Precision>& rhs)
    { campf<Precision> r = lhs; r -= rhs; return r; }

    template<unsigned int Precision>
    campf<Precision>& operator*=(campf<Precision>& lhs, const campf<Precision>& rhs)
    {
        ampf<Precision> xx(lhs.x*rhs.x), yy(lhs.y*rhs.y), mm((lhs.x+lhs.y)*(rhs.x+rhs.y));
        lhs.x = xx-yy;
        lhs.y = mm-xx-yy;
        return lhs;
    }

    template<unsigned int Precision>
    const campf<Precision> operator*(const campf<Precision>& lhs, const campf<Precision>& rhs)
    { campf<Precision> r = lhs; r *= rhs; return r; }

    template<unsigned int Precision>
    const campf<Precision> operator/(const campf<Precision>& lhs, const campf<Precision>& rhs)
    {
        campf<Precision> result;
        ampf<Precision> e;
        ampf<Precision> f;
        if( abs(rhs.y)<abs(rhs.x) )
        {
            e = rhs.y/rhs.x;
            f = rhs.x+rhs.y*e;
            result.x = (lhs.x+lhs.y*e)/f;
            result.y = (lhs.y-lhs.x*e)/f;
        }
        else
        {
            e = rhs.x/rhs.y;
            f = rhs.y+rhs.x*e;
            result.x = (lhs.y+lhs.x*e)/f;
            result.y = (-lhs.x+lhs.y*e)/f;
        }
        return result;
    }

    template<unsigned int Precision>
    campf<Precision>& operator/=(campf<Precision>& lhs, const campf<Precision>& rhs)
    {
        lhs = lhs/rhs;
        return lhs;
    }

    template<unsigned int Precision>
    const ampf<Precision> abscomplex(const campf<Precision> &z)
    {
        ampf<Precision> w, xabs, yabs, v;

        xabs = abs(z.x);
        yabs = abs(z.y);
        w = xabs>yabs ? xabs : yabs;
        v = xabs<yabs ? xabs : yabs;
        if( v==0 )
            return w;
        else
        {
            ampf<Precision> t = v/w;
            return w*sqrt(1+sqr(t));
        }
    }

    template<unsigned int Precision>
    const campf<Precision> conj(const campf<Precision> &z)
    {
        return campf<Precision>(z.x, -z.y);
    }

    template<unsigned int Precision>
    const campf<Precision> csqr(const campf<Precision> &z)
    {
        ampf<Precision> t = z.x*z.y;
        return campf<Precision>(sqr(z.x)-sqr(z.y), t+t);
    }

    //
    // different types of arguments
    //
    #define __AMP_BINARY_OPI(type) \
        template<unsigned int Precision> const campf<Precision> operator+ (const signed type& op1,      const campf<Precision>& op2) { return campf<Precision>(op1+op2.x, op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator+ (const unsigned type& op1,    const campf<Precision>& op2) { return campf<Precision>(op1+op2.x, op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator+ (const campf<Precision>& op1, const signed type& op2)      { return campf<Precision>(op1.x+op2, op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator+ (const campf<Precision>& op1, const unsigned type& op2)    { return campf<Precision>(op1.x+op2, op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator- (const signed type& op1,      const campf<Precision>& op2) { return campf<Precision>(op1-op2.x, -op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator- (const unsigned type& op1,    const campf<Precision>& op2) { return campf<Precision>(op1-op2.x, -op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator- (const campf<Precision>& op1, const signed type& op2)      { return campf<Precision>(op1.x-op2, op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator- (const campf<Precision>& op1, const unsigned type& op2)    { return campf<Precision>(op1.x-op2, op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator* (const signed type& op1,      const campf<Precision>& op2) { return campf<Precision>(op1*op2.x, op1*op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator* (const unsigned type& op1,    const campf<Precision>& op2) { return campf<Precision>(op1*op2.x, op1*op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator* (const campf<Precision>& op1, const signed type& op2)      { return campf<Precision>(op2*op1.x, op2*op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator* (const campf<Precision>& op1, const unsigned type& op2)    { return campf<Precision>(op2*op1.x, op2*op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator/ (const signed type& op1,      const campf<Precision>& op2) { return campf<Precision>(ampf<Precision>(op1),ampf<Precision>(0))/op2; }   \
        template<unsigned int Precision> const campf<Precision> operator/ (const unsigned type& op1,    const campf<Precision>& op2) { return campf<Precision>(ampf<Precision>(op1),ampf<Precision>(0))/op2; }   \
        template<unsigned int Precision> const campf<Precision> operator/ (const campf<Precision>& op1, const signed type& op2)      { return campf<Precision>(op1.x/op2, op1.y/op2); }   \
        template<unsigned int Precision> const campf<Precision> operator/ (const campf<Precision>& op1, const unsigned type& op2)    { return campf<Precision>(op1.x/op2, op1.y/op2); }   \
        template<unsigned int Precision>                   bool operator==(const signed type& op1,      const campf<Precision>& op2) { return op1==op2.x && op2.y==0; }   \
        template<unsigned int Precision>                   bool operator==(const unsigned type& op1,    const campf<Precision>& op2) { return op1==op2.x && op2.y==0; }   \
        template<unsigned int Precision>                   bool operator==(const campf<Precision>& op1, const signed type& op2)      { return op1.x==op2 && op1.y==0; }   \
        template<unsigned int Precision>                   bool operator==(const campf<Precision>& op1, const unsigned type& op2)    { return op1.x==op2 && op1.y==0; }   \
        template<unsigned int Precision>                   bool operator!=(const campf<Precision>& op1, const signed type& op2)      { return op1.x!=op2 || op1.y!=0; }   \
        template<unsigned int Precision>                   bool operator!=(const campf<Precision>& op1, const unsigned type& op2)    { return op1.x!=op2 || op1.y!=0; }   \
        template<unsigned int Precision>                   bool operator!=(const signed type& op1,      const campf<Precision>& op2) { return op1!=op2.x || op2.y!=0; }   \
        template<unsigned int Precision>                   bool operator!=(const unsigned type& op1,    const campf<Precision>& op2) { return op1!=op2.x || op2.y!=0; }
    __AMP_BINARY_OPI(char)
    __AMP_BINARY_OPI(short)
    __AMP_BINARY_OPI(long)
    __AMP_BINARY_OPI(int)
    #undef __AMP_BINARY_OPI
    #define __AMP_BINARY_OPF(type) \
        template<unsigned int Precision> const campf<Precision> operator+ (const type& op1,             const campf<Precision>& op2) { return campf<Precision>(op1+op2.x, op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator+ (const campf<Precision>& op1, const type& op2)             { return campf<Precision>(op1.x+op2, op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator- (const type& op1,             const campf<Precision>& op2) { return campf<Precision>(op1-op2.x, -op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator- (const campf<Precision>& op1, const type& op2)             { return campf<Precision>(op1.x-op2, op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator* (const type& op1,             const campf<Precision>& op2) { return campf<Precision>(op1*op2.x, op1*op2.y); }   \
        template<unsigned int Precision> const campf<Precision> operator* (const campf<Precision>& op1, const type& op2)             { return campf<Precision>(op2*op1.x, op2*op1.y); }   \
        template<unsigned int Precision> const campf<Precision> operator/ (const type& op1,             const campf<Precision>& op2) { return campf<Precision>(ampf<Precision>(op1),ampf<Precision>(0))/op2; }   \
        template<unsigned int Precision> const campf<Precision> operator/ (const campf<Precision>& op1, const type& op2)             { return campf<Precision>(op1.x/op2, op1.y/op2); }   \
        template<unsigned int Precision>                   bool operator==(const type& op1,             const campf<Precision>& op2) { return op1==op2.x && op2.y==0; }   \
        template<unsigned int Precision>                   bool operator==(const campf<Precision>& op1, const type& op2)             { return op1.x==op2 && op1.y==0; }   \
        template<unsigned int Precision>                   bool operator!=(const type& op1,             const campf<Precision>& op2) { return op1!=op2.x || op2.y!=0; }   \
        template<unsigned int Precision>                   bool operator!=(const campf<Precision>& op1, const type& op2)             { return op1.x!=op2 || op1.y!=0; }
    __AMP_BINARY_OPF(float)
    __AMP_BINARY_OPF(double)
    __AMP_BINARY_OPF(long double)
    __AMP_BINARY_OPF(ampf<Precision>)
    #undef __AMP_BINARY_OPF

    //
    // Real linear algebra
    //
    template<unsigned int Precision>
    ampf<Precision> vDotProduct(ap::const_raw_vector< ampf<Precision> > v1, ap::const_raw_vector< ampf<Precision> > v2)
    {
        ap::ap_error::make_assertion(v1.GetLength()==v2.GetLength());
        int i, cnt = v1.GetLength();
        const ampf<Precision> *p1 = v1.GetData();
        const ampf<Precision> *p2 = v2.GetData();
        mpfr_record *r = NULL;
        mpfr_record *t = NULL;
        try
        {
            r = mpfr_storage::newMpfr(Precision);
            t = mpfr_storage::newMpfr(Precision);
            mpfr_set_ui(r->value, 0, GMP_RNDN);
            for(i=0; i<cnt; i++)
            {
                mpfr_mul(t->value, p1->getReadPtr(), p2->getReadPtr(), GMP_RNDN);
                mpfr_add(r->value, r->value, t->value, GMP_RNDN);
                p1 += v1.GetStep();
                p2 += v2.GetStep();
            }
            mpfr_storage::deleteMpfr(t);
            return r;
        }
        catch(...)
        {
            if( r!=NULL )
                mpfr_storage::deleteMpfr(r);
            if( t!=NULL )
                mpfr_storage::deleteMpfr(t);
            throw;
        }
    }

    template<unsigned int Precision>
    void vMove(ap::raw_vector< ampf<Precision> > vDst, ap::const_raw_vector< ampf<Precision> > vSrc)
    {
        ap::ap_error::make_assertion(vDst.GetLength()==vSrc.GetLength());
        int i, cnt = vDst.GetLength();
        ampf<Precision> *pDst = vDst.GetData();
        const ampf<Precision> *pSrc = vSrc.GetData();
        if( pDst==pSrc )
            return;
        for(i=0; i<cnt; i++)
        {
            *pDst = *pSrc;
            pDst += vDst.GetStep();
            pSrc += vSrc.GetStep();
        }
    }

    template<unsigned int Precision>
    void vMoveNeg(ap::raw_vector< ampf<Precision> > vDst, ap::const_raw_vector< ampf<Precision> > vSrc)
    {
        ap::ap_error::make_assertion(vDst.GetLength()==vSrc.GetLength());
        int i, cnt = vDst.GetLength();
        ampf<Precision> *pDst = vDst.GetData();
        const ampf<Precision> *pSrc = vSrc.GetData();
        for(i=0; i<cnt; i++)
        {
            *pDst = *pSrc;
            mpfr_ptr v = pDst->getWritePtr();
            mpfr_neg(v, v, GMP_RNDN);
            pDst += vDst.GetStep();
            pSrc += vSrc.GetStep();
        }
    }

    template<unsigned int Precision, class T2>
    void vMove(ap::raw_vector< ampf<Precision> > vDst, ap::const_raw_vector< ampf<Precision> > vSrc, T2 alpha)
    {
        ap::ap_error::make_assertion(vDst.GetLength()==vSrc.GetLength());
        int i, cnt = vDst.GetLength();
        ampf<Precision>       *pDst = vDst.GetData();
        const ampf<Precision> *pSrc = vSrc.GetData();
        ampf<Precision>       a(alpha);
        for(i=0; i<cnt; i++)
        {
            *pDst = *pSrc;
            mpfr_ptr v = pDst->getWritePtr();
            mpfr_mul(v, v, a.getReadPtr(), GMP_RNDN);
            pDst += vDst.GetStep();
            pSrc += vSrc.GetStep();
        }
    }

    template<unsigned int Precision>
    void vAdd(ap::raw_vector< ampf<Precision> > vDst, ap::const_raw_vector< ampf<Precision> > vSrc)
    {
        ap::ap_error::make_assertion(vDst.GetLength()==vSrc.GetLength());
        int i, cnt = vDst.GetLength();
        ampf<Precision>       *pDst = vDst.GetData();
        const ampf<Precision> *pSrc = vSrc.GetData();
        for(i=0; i<cnt; i++)
        {
            mpfr_ptr    v  = pDst->getWritePtr();
            mpfr_srcptr vs = pSrc->getReadPtr();
            mpfr_add(v, v, vs, GMP_RNDN);
            pDst += vDst.GetStep();
            pSrc += vSrc.GetStep();
        }
    }

    template<unsigned int Precision, class T2>
    void vAdd(ap::raw_vector< ampf<Precision> > vDst, ap::const_raw_vector< ampf<Precision> > vSrc, T2 alpha)
    {
        ap::ap_error::make_assertion(vDst.GetLength()==vSrc.GetLength());
        int i, cnt = vDst.GetLength();
        ampf<Precision>       *pDst = vDst.GetData();
        const ampf<Precision> *pSrc = vSrc.GetData();
        ampf<Precision>       a(alpha), tmp;
        for(i=0; i<cnt; i++)
        {
            mpfr_ptr    v  = pDst->getWritePtr();
            mpfr_srcptr vs = pSrc->getReadPtr();
            mpfr_mul(tmp.getWritePtr(), a.getReadPtr(), vs, GMP_RNDN);
            mpfr_add(v, v, tmp.getWritePtr(), GMP_RNDN);
            pDst += vDst.GetStep();
            pSrc += vSrc.GetStep();
        }
    }

    template<unsigned int Precision>
    void vSub(ap::raw_vector< ampf<Precision> > vDst, ap::const_raw_vector< ampf<Precision> > vSrc)
    {
        ap::ap_error::make_assertion(vDst.GetLength()==vSrc.GetLength());
        int i, cnt = vDst.GetLength();
        ampf<Precision>       *pDst = vDst.GetData();
        const ampf<Precision> *pSrc = vSrc.GetData();
        for(i=0; i<cnt; i++)
        {
            mpfr_ptr    v  = pDst->getWritePtr();
            mpfr_srcptr vs = pSrc->getReadPtr();
            mpfr_sub(v, v, vs, GMP_RNDN);
            pDst += vDst.GetStep();
            pSrc += vSrc.GetStep();
        }
    }

    template<unsigned int Precision, class T2>
    void vSub(ap::raw_vector< ampf<Precision> > vDst, ap::const_raw_vector< ampf<Precision> > vSrc, T2 alpha)
    {
        vAdd(vDst, vSrc, -alpha);
    }

    template<unsigned int Precision, class T2>
    void vMul(ap::raw_vector< ampf<Precision> > vDst, T2 alpha)
    {
        int i, cnt = vDst.GetLength();
        ampf<Precision>       *pDst = vDst.GetData();
        ampf<Precision>       a(alpha);
        for(i=0; i<cnt; i++)
        {
            mpfr_ptr    v  = pDst->getWritePtr();
            mpfr_mul(v, a.getReadPtr(), v, GMP_RNDN);
            pDst += vDst.GetStep();
        }
    }
}

#endif
