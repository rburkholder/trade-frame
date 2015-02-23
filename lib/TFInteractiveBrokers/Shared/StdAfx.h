/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#ifdef _MSC_VER

#ifdef TWSAPIDLL
#    define TWSAPIDLLEXP __declspec(dllexport)
#endif

#define assert ASSERT
#define snprintf _snprintf

#endif

#ifndef TWSAPIDLLEXP
#define TWSAPIDLLEXP
#endif

