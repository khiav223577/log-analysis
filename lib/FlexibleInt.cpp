
#ifndef ___FlexibleInt_cpp__
#define ___FlexibleInt_cpp__
//#include <iostream>
#include "bigint-2010.04.30/_BigIntegerLibrary.cc"
#include "SafeScanf.cpp"
#include <string>
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
    inline FlexibleInt(const FlexibleInt &x) : bigInt_flag(false){
        this->set(x);
    }
    ~FlexibleInt(){
        clearBigInt();
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
    inline void clearBigInt(){
        if (bigInt_flag == false) return;
        bigInt_flag = false;
        //std::cout << *(getValuePtr()) << std::endl;
        delete inner_data.valuePtr;
        inner_data.valuePtr = NULL;
    }
    inline void   setBigInt()               { bigInt_flag = true;         }
    inline void setValuePtr(BigInteger *ptr){ inner_data.valuePtr = ptr;  }
    inline void    setValue(int _value)     { inner_data.value = _value;  }

    inline bool            isBigInt() const { return bigInt_flag;         }
    inline BigInteger*  getValuePtr() const { return inner_data.valuePtr; }
    inline int             getValue() const { return inner_data.value;    }
    inline std::string getValuePtrAsStr() const { return bigIntegerToString(*inner_data.valuePtr); }
    inline void output() const{
        std::cout << GETVALUE(*this) << " ";
    }
    inline bool try_to_cast_to_int(){
        if (!isBigInt()) return true;
        try{
            int result = getValuePtr()->toInt(); //may cause exception
            clearBigInt();
            setValue(result);
            return true;
        }catch (const char* message){
            return false; //fails
        }
    }
//----------------------------------------------
//  Operator Core
//----------------------------------------------
    inline void set(const FlexibleInt &x){
        if (x.isBigInt()){
            setBigInt();
            setValuePtr(new BigInteger(GETVALUE(x)));
        }else{
            clearBigInt();
            setValue(x.getValue());
        }
    }
    inline void set_by_add(const FlexibleInt &x, const FlexibleInt &y){
        if (x.isBigInt() || y.isBigInt()){
            setBigInt();
            setValuePtr(new BigInteger(GETVALUE(x) + GETVALUE(y)));
        }else{
            setValue(x.getValue() + y.getValue());
        }
    }
    inline void set_by_sub(const FlexibleInt &x, const FlexibleInt &y){
        if (x.isBigInt() || y.isBigInt()){
            setBigInt();
            setValuePtr(new BigInteger(GETVALUE(x) - GETVALUE(y)));
        }else{
            setValue(x.getValue() - y.getValue());
        }
    }
//----------------------------------------------
//  Operators
//----------------------------------------------
    inline FlexibleInt operator +(const FlexibleInt &x) const{
        FlexibleInt ans;
        ans.set_by_add(*this, x);
        return ans;
    }
    inline FlexibleInt operator -(const FlexibleInt &x) const{
        FlexibleInt ans;
        ans.set_by_sub(*this, x);
        return ans;
    }
    inline FlexibleInt& operator +=(const FlexibleInt &x){
        this->set_by_add(*this, x);
        return *this;
    }
    inline FlexibleInt& operator -=(const FlexibleInt &x){
        this->set_by_sub(*this, x);
        return *this;
    }
    inline FlexibleInt& operator =(const FlexibleInt &x){
        this->set(x);
        return *this;
    }
    inline bool operator ==(const FlexibleInt &x) const { return GETVALUE(*this) == GETVALUE(x); }
	inline bool operator !=(const FlexibleInt &x) const { return GETVALUE(*this) != GETVALUE(x); }
	inline bool operator < (const FlexibleInt &x) const { return GETVALUE(*this) <  GETVALUE(x); }
	inline bool operator <=(const FlexibleInt &x) const { return GETVALUE(*this) <= GETVALUE(x); }
	inline bool operator >=(const FlexibleInt &x) const { return GETVALUE(*this) >  GETVALUE(x); }
	inline bool operator > (const FlexibleInt &x) const { return GETVALUE(*this) >= GETVALUE(x); }
//----------------------------------------------
//  Save / Load
//----------------------------------------------
    inline void save(FILE *file){
        if (isBigInt()) fprintf(file, " B %s", getValuePtrAsStr().c_str());
        else fprintf(file, " I %d", getValue());
    }
    static inline FlexibleInt load(FILE *file){
        char tmp;
        fscanf(file, " %c", &tmp);
        PERROR(tmp != 'B' && tmp != 'I', printf("syntax error"););
        if (tmp == 'B'){
            std::string bigIntString = SafeScanf::readBigInt10(file);
            return FlexibleInt(new BigInteger(BigUnsignedInABase(bigIntString, 10)));
        }else{
            int value;
            fscanf(file, " %d", &value);
            return FlexibleInt(value);
        }
    }
};
#undef GETVALUE
#endif
