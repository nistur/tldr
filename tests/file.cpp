#include "tldr-tests.h"
#include "tldr.h"

TEST_TLDR(Save, File, 0, "some.key", "something",
	  {
	      ASSERT(tldrSaveBinary(m_data.context, "data/test1.tldr") == TLDR_SUCCESS);
	  });


TEST(SaveMultiple, File, 0.0f,
     // initialisation
     {
	 m_data.context = tldrInitContext();
	 tldrWriteValue(m_data.context, "some.key", "something");
	 tldrWriteValue(m_data.context, "some.other.key", "something else");
	 tldrWriteValue(m_data.context, "some.key.number", "12");
     },
     // cleanup
     {
	 tldrTerminateContext(&m_data.context);
     },
     // test
     {
	 ASSERT(tldrSaveBinary(m_data.context, "data/test2.tldr") == TLDR_SUCCESS);
     },
     // data
     {
	 tldrContext* context;
     }
    );


#ifndef WIN32
TEST_TLDR(SaveFail, File, 0, "", "",
	  {
	      ASSERT(tldrSaveBinary(m_data.context, "/testfail.tldr") == TLDR_WRITE_PROTECT);
	  });
#endif

TEST_TLDR(Load, File, 0, "", "",
	  {
	      ASSERT(tldrLoadBinary(m_data.context, "data/test1.tldr", true) == TLDR_SUCCESS);
	      ASSERT(tldrKeyExists(m_data.context, "some.key") == TLDR_SUCCESS);
	  });

TEST_TLDR(LoadLarge, File, 0, "", "",
	  {
	      ASSERT(tldrLoadBinary(m_data.context, "data/long.tldr", true) == TLDR_SUCCESS);
	  });

TEST_TLDR(LoadFail, File, 0, "", "",
	  {
	      ASSERT(tldrLoadBinary(m_data.context, "data/testfail.tldr", true) == TLDR_NO_FILE);
	  });
