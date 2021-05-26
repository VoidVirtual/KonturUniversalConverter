/** Created by D.Kabzhan, 30.04.2021 */
#pragma once
template<class T>
class IBinaryOperator{
public:
    virtual T operator()(T const& left, T const& right)const = 0;
};
template<class T>
class IAlgebraicOperator: public IBinaryOperator<T>{ // Binary operator of algebraic group
public:
    /**
     * creates a neutral element of the group, respecting the operator
     * e.g: operator (a+b)->c, neutral element - 0,
     *      operator (a*b)->c, neutral element - 1
    */
    virtual T getNeutralElement()const = 0; // Creates neutral element e.g: operator + , element
};
