

CPPHelper 是一个C++基础辅助类库，目的在于更加高效、安全的进行开发。


使用方法：
1：将CPPHelper复制到工程目录

比如D:\Visual Studio 2015\Projects\MySln\CPPHelper (MySln是我需要编译的工程)

2：在VC++目录  -- 包含路径 添加 $(SolutionDir)CPPHelper\
3：在 VC++目录 -- 库目录   添加 $(SolutionDir)CPPHelper\$(PlatformShortName)\

4：在stdafx.h 中添加 #include <BaseFunction.h>
5：在stdafx.cpp 中添加 #include <BaseFunction.cpp>

至此，基础类库已经可以正常使用，另外其他Thread等类，请自行在使用时 include




目前该类库有以下几个功能：

//////////////////////////////////////////////////rapidxml///////////////////////////////////////////////////////////
这是我修改后的一个rapidxml库，所有规范与原版的rapidxml一样，但是此版本完整支持UTF8以及UTF16。并且在执行print时提供了更佳性能。



/////////////////////////////////////////////////MsIsoReader////////////////////////////////////////////////////////
这是一个微软的一个ISO读取库，原本它是C#版，我将其移植到了C++上，一般用用过的去。Dism++的直接释放ISO功能也是此库支持


此外还有一些基础类库不在一一列举，以下是部分类的使用说明




//////////////////////////////////////////////////Thread类////////////////////////////////////////////////////////////

Thread：
异步类，用于执行异步任务。
头文件：Thread.h
使用方法：

Thread([]()
{
/*你需要异步执行的命令，Thread类自动会完成句柄释放。
这里是一个Lambda表达式，你可以参考Lambda表达式的使用。
*/
});


/////////////////////////////////////////////////hanlde智能句柄////////////////////////////////////////////////

hanlde:
智能句柄，用于自动关闭各种句柄。
头文件：handle.h
使用方法：

CHKEY - 注册表智能句柄

//自动初始化为NULL
CHKEY Test;

RegOpenKey(HKEY_LOCAL_MACHINE,L"SOFTWARE",&Test);

//你无需特意关闭Test，Test在析构时自动关闭HKEY


CHFile - 文件智能句柄

//自动初始化为INVALID_HANDLE_VALUE
CHFile Test;

Test=CreateFile(……);

//你无需特意关闭Test，Test在析构时自动关闭HANDLE

等等……



/////////////////////////////////////////////////资源守卫///////////////////////////////////////////////
头文件：Guard.h

int Test()
{
	auto pData=new int;

	//RunOnExit中的代码在退出时执行
	RunOnExit([pData]()
	{
		//Test函数结束时，将自动执行此代码
		delete pData;
	});



	//RunOnCrash中的会在程序崩溃时执行
	RunOnCrash([]()
	{
		MessageBox(NULL,L"我的程序崩溃啦",L"Sorry",0);
	});

	return 0;
}