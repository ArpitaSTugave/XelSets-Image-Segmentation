#ifndef __POINT2D_H__
#define __POINT2D_H__

#include <stdio.h>
#include <typeinfo>

//Abstracts 2D point operations for any numeric data-type

//declaration
template<typename T>  class TXY
{
public:
	T x, y;
	
	//constructors
	TXY();
	TXY(T x, T y);
	
	//functions for setting x & y values
	void set(T x, T y);					
	void set(T v);
	
	//returns the length of position vector : sqrt(x^2 + y^2)
	float length();
	
	//prints the 2D point coordinates on terminal
	void print();
	
	//operator overloading
	TXY<T> operator+(TXY<T> ob1);
	TXY<T> operator-(TXY<T> ob1);
	TXY<T> operator*(T val);
	TXY<T> operator/(T val);
	void operator=(TXY<T> ob1);
	bool operator==(TXY<T> ob1);
};

// class definitions
template<typename T> TXY<T>::TXY()
{
	this->x = this->y = 0;
}

template<typename T> TXY<T>::TXY(T x, T y)
{
	this->x = x; 
	this->y = y;
}

template<typename T> void TXY<T>::set(T x, T y)
{
	this->x = x; 
	this->y = y;
}

template<typename T> void TXY<T>::set(T v)
{
	this->x = v; 
	this->y = v;
}

template<typename T> TXY<T> TXY<T>::operator+(TXY<T> ob1)
{
	TXY<T> temp;
	temp.x = this->x + ob1.x;
	temp.y = this->y + ob1.y;
	return temp;
}

template<typename T> TXY<T> TXY<T>::operator-(TXY<T> ob1)
{
	TXY<T> temp;
	temp.x = this->x - ob1.x;
	temp.y = this->y - ob1.y;
	return temp;
}

template<typename T> TXY<T> TXY<T>::operator*(T val)
{
	TXY<T> temp;
	temp.x = this->x * val;
	temp.y = this->y * val;
	return temp;
}

template<typename T> TXY<T> TXY<T>::operator/(T val)
{
	TXY<T> temp;
	temp.x = this->x / val;
	temp.y = this->y / val;
	return temp;
}

template<typename T> void TXY<T>::operator=(TXY<T> ob1)
{
	this->x = ob1.x;
	this->y = ob1.y;
}

template<typename T> bool TXY<T>::operator==(TXY<T> ob1)
{
	return (this->x == ob1.x && this->y == ob1.y);
}

template<typename T> float TXY<T>::length()
{
	return sqrt(x*x + y*y);
}

template<typename T> void TXY<T>::print()
{
	float _x = x;
	float _y = y;
	printf("%f\t%f\n", _x, _y);
}

#endif
