#ifndef __TLDR_TESTS_H__
#define __TLDR_TESTS_H__

#include "testsuite/tests.h"

#define TEST_TLDR(x, grp, time, key, val,  test)			\
    TEST(x, grp, time,							\
    { m_data.context = tldrInitContext(); tldrWriteValue(		\
    m_data.context, key, val); },					\
	 { tldrTerminateContext(&m_data.context); },			\
	    test,							\
	 { tldrContext* context; } );

#endif/*__TLDR_TESTS_H__*/
