#pragma once

#include <crtdbg.h>

#ifdef _DEBUG

#pragma warning(disable:4074)  // initializers put in compiler reserved initialization area
#pragma init_seg(compiler)  // global objects in this file get constructed very early on

struct CrtBreakAllocSetter {
	CrtBreakAllocSetter() {
		_CrtSetBreakAlloc(154);  // 内存申请断点
	}
};

CrtBreakAllocSetter g_crtBreakAllocSetter;

#endif  //_DEBUG
/*-------------------- -
作者：qiyao_2000
来源：CSDN
原文：https ://blog.csdn.net/qiyao_2000/article/details/46639261 
版权声明：本文为博主原创文章，转载请附上博文链接！#pragma once*/
