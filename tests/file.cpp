#include "tldr-tests.h"
#include "tldr.h"

TEST_TLDR(Save, File, 0, "some.key", "something",
	  {
	      ASSERT(tldrSaveBinary(m_data.context, "test1.tldr") == TLDR_SUCCESS);
	  });

#ifndef WIN32
TEST_TLDR(SaveFail, File, 0, "", "",
	  {
	      ASSERT(tldrSaveBinary(m_data.context, "/testfail.tldr") == TLDR_WRITE_PROTECT);
	  });
#endif

TEST_TLDR(Load, File, 0, "", "",
	  {
	      ASSERT(tldrLoadBinary(m_data.context, "test1.tldr", true) == TLDR_SUCCESS);
	  });

TEST_TLDR(LoadFail, File, 0, "", "",
	  {
	      ASSERT(tldrLoadBinary(m_data.context, "testfail.tldr", true) == TLDR_NO_FILE);
	  });
