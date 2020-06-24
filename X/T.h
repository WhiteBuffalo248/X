#pragma once

#include <crtdbg.h>

#ifdef _DEBUG

#pragma warning(disable:4074)  // initializers put in compiler reserved initialization area
#pragma init_seg(compiler)  // global objects in this file get constructed very early on

struct CrtBreakAllocSetter {
	CrtBreakAllocSetter() {
		_CrtSetBreakAlloc(154);  // �ڴ�����ϵ�
	}
};

CrtBreakAllocSetter g_crtBreakAllocSetter;

#endif  //_DEBUG
/*-------------------- -
���ߣ�qiyao_2000
��Դ��CSDN
ԭ�ģ�https ://blog.csdn.net/qiyao_2000/article/details/46639261 
��Ȩ����������Ϊ����ԭ�����£�ת���븽�ϲ������ӣ�#pragma once*/
