#include "tldr-tests.h"
#include "tldr.h"
#include <string.h>

TEST(InitTerminate, Basic, 0.0f,
     // initialisation
     {
	 m_data.context = 0;
     },
     // cleanup
     {
	 tldrTerminateContext(&m_data.context);
     },
     // test
     {
	 ASSERT(m_data.context = tldrInitContext());
	 ASSERT(tldrTerminateContext(&m_data.context) == TLDR_SUCCESS);
     },
     // data
     {
	 tldrContext* context;
     }
    );

TEST_TLDR(Write, Basic, 0, "", "",
	  {
	      ASSERT(tldrWriteValue(m_data.context, "some.key", "something") == TLDR_SUCCESS);
	  });

TEST_TLDR(Read, Basic, 0, "some.key", "something",
	  {
	      char val[TLDR_STRING_MAX];
	      ASSERT(tldrReadValue(m_data.context, "some.key", val) == TLDR_SUCCESS);
	      ASSERT(strcmp(val, "something") == 0);
	  });

TEST_TLDR(KeyExists, Basic, 0, "some.key", "something",
	  {
	      ASSERT(tldrKeyExists(m_data.context, "some.key") == TLDR_SUCCESS);
	  });

TEST_TLDR(NoValue, Basic, 0, "some.key", "something",
	  {
	      ASSERT(tldrKeyExists(m_data.context, "some") == TLDR_NO_VALUE);
	  });

TEST_TLDR(NoKey, Basic, 0, "some.key", "something",
	  {
	      ASSERT(tldrKeyExists(m_data.context, "some.other.key") == TLDR_NO_KEY);
	  });
