#ifndef TestRODevIO_H
#define TestRODevIO_H

#include "Utils.h"

template<class wT, class T>
class TestRODevIOBase {
	public:
	TestRODevIOBase();
	T read();
	void setVal(wT v);
	private:
	wT test(ACS::Time& timestamp);
    std::unique_ptr<AMB::RODevIO<TestRODevIOBase<wT, T>, wT, T>> devio;
	wT val;
};

template<class wT, class T>
TestRODevIOBase<wT, T>::TestRODevIOBase()
    : devio(new AMB::RODevIO<TestRODevIOBase<wT, T>, wT, T>(
                this, &TestRODevIOBase<wT, T>::test))
{
}

template<class wT, class T>
T TestRODevIOBase<wT,T>::read()
{
	ACS::Time now(0L); 
	T l(devio->read(now));
	return l;
}

template<class wT, class T>
void TestRODevIOBase<wT,T>::setVal(wT v)
{
	val = v;
}

template<class wT, class T>
wT TestRODevIOBase<wT,T>::test(ACS::Time& timestamp)
{
	return val;
}


#endif
