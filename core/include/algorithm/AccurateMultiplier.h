/** Created by D.Kabzhan, 30.04.2021 */
#pragma once
#pragma warning(push,0)
#include <gmpxx.h>
#pragma warning(pop)
#include "BinaryOperator.h"
class AccurateMultiplier: public IAlgebraicOperator<mpf_class>
{
public:
    AccurateMultiplier(size_t significantDigits){
        size_t const exponentBitsNum = 32;
        size_t const mantiseBitsNum = significantDigits*4; // rude but fast
        bitScale_ = exponentBitsNum + mantiseBitsNum;
    }
    virtual mpf_class getNeutralElement()const{
        return mpf_class(1.0, bitScale_);
    }
    virtual mpf_class operator()(mpf_class const& left, mpf_class const& right)const override{
        return left*right;
    }
    mpf_class getOne()const{
        return getNeutralElement();
    }
private:
    size_t bitScale_;
};
