

CPPHelper 是一个C++基础辅助类库，目的在于更加高效、安全的进行开发。目前该类库有以下几个功能：



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