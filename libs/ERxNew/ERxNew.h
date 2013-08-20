#pragma once
#ifndef ERXNEW_H
#define ERXNEW_H


#include <stdlib.h> // for malloc and free
void* operator new(size_t size) ;
void operator delete(void* ptr) ;


#endif // ERXNEW_H