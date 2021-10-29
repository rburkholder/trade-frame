/* Copyright (C) 2020 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef TWS_API_CLIENT_ECLIENTEXCEPTION_H
#define TWS_API_CLIENT_ECLIENTEXCEPTION_H

#include <iostream>
#include <exception>
#include "TwsSocketClientErrors.h"

struct EClientException : public std::exception {

private:
    CodeMsgPair m_error;
    std::string m_text;

public:
    CodeMsgPair error() const { return m_error; }
    std::string text() const  { return m_text; }
    
    EClientException(CodeMsgPair error, std::string text) : m_error(error), m_text(text) { }
}; 

#endif