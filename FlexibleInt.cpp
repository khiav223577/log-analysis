
#ifndef ___FlexibleInt_cpp__
#define ___FlexibleInt_cpp__
//#include <iostream>
#include "lib/bigint-2010.04.30/_BigIntegerLibrary.cc"
#define GETVALUE(x) ((x).isBigInt() ? *(x).getValuePtr() : (x).getValue())
class FlexibleInt{
private:
     union DATA{
        int value;
        BigInteger *valuePtr;
     } inner_data;
     bool bigInt_flag;
public:
    inline FlexibleInt() : bigInt_flag(false){}
    inline FlexibleInt(int _value) : bigInt_flag(false){
        setValue(_value);
    }
    inline FlexibleInt(BigInteger *_valuePtr) : bigInt_flag(true){
        setValuePtr(_valuePtr);
    }
    ~FlexibleInt(){
        if (isBigInt()){
            //std::cout << *(getValuePtr()) << std::endl;
            delete inner_data.valuePtr;
            inner_data.valuePtr = NULL;
        }
        /* testing
        FlexibleInt a(new BigInteger(1)), b(new BigInteger(3));
        a = b + b + FlexibleInt(new BigInteger(12));
        Sleep(500);
        printf("!");

        FlexibleInt c(new BigInteger(100)), d(new BigInteger(300));
        FlexibleInt e = c + d;
        Sleep(500);
        printf("!");

        FlexibleInt f = FlexibleInt(new BigInteger(2000));
        Sleep(500);
        printf("!");
        */
    }
//----------------------------------------------
//  ACCESS
//----------------------------------------------
    inline void   setBigInt()               { bigInt_flag = true;         }
    inline void setValuePtr(BigInteger *ptr){ inner_data.valuePtr = ptr;  }
    inline void    setValue(int _value)     { inner_data.value = _value;  }

    inline bool            isBigInt() const { return bigInt_flag;         }
    inline BigInteger*  getValuePtr() const { return inner_data.valuePtr; }
    inline int             getValue() const { return inner_data.value;    }

    inline void output() const{
        std::cout << GETVALUE(*this) << " ";
    }
//----------------------------------------------
//  Operator
//----------------------------------------------
    inline FlexibleInt operator +(const FlexibleInt &x) const {
        FlexibleInt ans;
        if (isBigInt() || x.isBigInt()){
            ans.setBigInt();
            ans.setValuePtr(new BigInteger(GETVALUE(*this) + GETVALUE(x)));
        }else{
            ans.setValue(getValue() + x.getValue());
        }
        return ans;
    }
    inline FlexibleInt operator -(const FlexibleInt &x) const {
        FlexibleInt ans;
        if (isBigInt() || x.isBigInt()){
            ans.setBigInt();
            ans.setValuePtr(new BigInteger(GETVALUE(*this) + GETVALUE(x)));
        }else{
            ans.setValue(getValue() - x.getValue());
        }
        return ans;
    }
    inline FlexibleInt& operator =(const FlexibleInt &x) {
        if (x.isBigInt()) setBigInt();
        if (isBigInt()){
            *getValuePtr() = GETVALUE(x);
        }else{
            setValue(x.getValue());
        }
        return *this;
    }
    inline bool operator ==(const FlexibleInt &x) const { return GETVALUE(*this) == GETVALUE(x); }
	inline bool operator !=(const FlexibleInt &x) const { return GETVALUE(*this) != GETVALUE(x); }
	inline bool operator < (const FlexibleInt &x) const { return GETVALUE(*this) <  GETVALUE(x); }
	inline bool operator <=(const FlexibleInt &x) const { return GETVALUE(*this) <= GETVALUE(x); }
	inline bool operator >=(const FlexibleInt &x) const { return GETVALUE(*this) >  GETVALUE(x); }
	inline bool operator > (const FlexibleInt &x) const { return GETVALUE(*this) >= GETVALUE(x); }

};
#undef GETVALUE
#endif
