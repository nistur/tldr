#ifndef __TLDR_TESTS_H__
#define __TLDR_TESTS_H__

enum Response
{
    SUCCESS,
    FAILURE,
    TIMEOUT,
};

class _Test
{
public:
    virtual void Init()                = 0;
    virtual void Cleanup()             = 0;
    virtual Response Test(float& time) = 0;

    static void RunTests(const char* group = 0, int runs = 0);
    
protected:
    void Add(const char* name, const char* group, float time);

private:
    struct _test
    {
	char*  name;
	_Test* test;
	float  time;
    };
    struct _group
    {
	char* name;
	_test* tests;
    };
    struct _result
    {
	float time;
	int   success;
	int   fail;
    _result() : time(0.0f), success(0), fail(0) {}
    };

    static void RunGroup(_group* group, _result* res, int runs);
    static void RunTest(_test* test, _result* res, int runs);
    static _group* s_Tests;
};

#ifdef WIN32
#include <Windows.h>
extern SYSTEMTIME tv_start;
inline float _time()
{
	SYSTEMTIME time;
	GetSystemTime(&time);
	float t = (float)(time.wSecond - tv_start.wSecond);
	t += (float)time.wMilliseconds;// / 1000.f;
	return t;
}

#else
#include <sys/time.h>
// this is naaaaaasty. Need to fix.
extern struct timeval tv_start;
inline float _time()
{
    struct timeval time;
    gettimeofday(&time, 0);
    float t = (float)(time.tv_sec - tv_start.tv_sec);// * 1000.f;
    t += (float)time.tv_usec / 1000000.f;
    return t;
}
#endif

#define TEST(x, grp, time, init, cleanup, test, data)	\
    class x : public _Test				\
    {							\
    private:						\
	struct _data data;				\
	_data m_data;					\
    public:						\
	x() { Add(#x, #grp, time); }			\
	void Init() {init}				\
	void Cleanup() {cleanup}			\
	Response Test(float& t)				\
	{						\
	    float start = _time();			\
	    {test}					\
	    float end = _time();			\
	    float dt = end - start;			\
	    if(t != 0.0f && t < dt)			\
	    {						\
		t = dt;					\
		return TIMEOUT;				\
	    }						\
	    t = dt;					\
	    return SUCCESS;				\
	}						\
    };          					\
    x _##x;

#define TEST_TLDR(x, grp, time, key, val,  test)			\
    TEST(x, grp, time,							\
    { m_data.context = tldrInitContext(); tldrWriteValue(		\
    m_data.context, key, val); },					\
	 { tldrTerminateContext(&m_data.context); },			\
	    test,							\
	 { tldrContext* context; } );


#define ASSERT(x)				\
    if(!(x))					\
    {						\
	t = _time() - start;			\
	return FAILURE;				\
    }

#define SAFE_DELETE(x)				\
    if(x)					\
    { delete x; x = 0; }

#endif/*__TLDR_TESTS_H__*/
