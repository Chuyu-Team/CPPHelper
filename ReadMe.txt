

CPPHelper 是一个C++基础辅助类库，目的在于更加高效、安全的进行开发。


使用方法：
1：将CPPHelper复制到工程目录

比如D:\Visual Studio 2015\Projects\MySln\CPPHelper (MySln是我需要编译的工程)

2：在 VC++目录 -- 包含路径 添加 $(SolutionDir)CPPHelper\
3：在 VC++目录 -- 库目录   添加 $(SolutionDir)CPPHelper\$(PlatformShortName)\

4：在stdafx.h 中添加 #include <BaseFunction.h>
5：在stdafx.cpp 中添加 #include <BaseFunction.cpp>

至此，基础类库已经可以正常使用，另外其他Thread等类，请自行在使用时 include


温馨提示：编译器必须支持C++ 0X,并且编码需要设置宽字符！！！！！！




目前该类库有以下几个功能：

///////////////////////////////////////////////rapidxml//////////////////////////////////////////////////////
这是我修改后的一个rapidxml库，所有规范与原版的rapidxml一样，但是此版本完整支持UTF8以及UTF16。并且在执行print时提供了更佳性能。

rapidxml::XMLDocument Document;

//从一个文件路径打开xml，自动进行编码判断
XMLCreateXMLDocumentByFile(L"D:\test.xml",&Document);


//从二进制数据打开XML，自动进行编码判断
XMLCreateXMLDocumentByData(pBinary,cbBinary,&Document);


//从字符串打开XML
XMLCreateXMLDocumentByString(L"<Root></Root>",&Document)


//打开Root/Files节点
auto pFiles= XMLOpenNote(&Document,L"Root/Files");

<Root>
	<Files/>
</Root>

……




///////////////////////////////////////////MsIsoReader////////////////////////////////////////////////////////
这是一个微软的一个ISO读取库，原本它是C#版，我将其移植到了C++上，一般用用过的去。Dism++的直接释放ISO功能也是此库支持









////////////////////////////////////////////Thread类////////////////////////////////////////////////////////

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



Task：
异步函数，用于执行异步任务，但是其线程创建于线程池，减少程序创建销毁线程的开销。
头文件：Thread.h
使用方法：

Task([]()
{
	/*你需要异步执行的命令。
	这里是一个Lambda表达式，你可以参考Lambda表达式的使用。
	*/
});



ThreadPool：
制定线程池，用于执行大批量任务，合适计算密集型并行计算。
头文件：Thread.h
使用方法：


ThreadPool WorkPool;

//最大十个线程并发，默认值500（微软规定的）
WorkPool.SetThreadMaximum(10);

int Datas[100];


WorkPool.For(Data,100,[](int& Data)
{
	//10线程并发输出Datas中的内容
	wprinf(L"%d\r\n",Data);
});






////////////////////////////////////////////hanlde智能句柄////////////////////////////////////////////////

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




//////////////////////////////////////////////其他/////////////////////////////////////////
1：ComHelper用于辅助编写com接口
2：CabinetHelper用于打开以及创建cab文件
3：DiskHelper用于修改磁盘MBR
4：FileHelper提供基础文件操作，比如文件删除、属性修改、大小计算
5：IniHelper，提供基础的ini读写
6：RegHelper，提供基础的注册表操作
7：StringHelper，提供基础的字符串操作
8：VirtDiskHelper，此库已经弃用，没啥功能
9：WinInetHelper，提供网络下载
10：WinnlsHelper，没什么功能，可以无视
11：StreamEx，没什么功能，通用层数据流交换使用，可以无视

