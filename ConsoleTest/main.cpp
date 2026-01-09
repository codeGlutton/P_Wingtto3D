#include "pch.h"
#include "Core/Object.h"

#include <chrono>

class IInterfaceTestBase abstract
{
public:
	IInterfaceTestBase() {}
	virtual ~IInterfaceTestBase() {}
};

class IInterfaceTest abstract : public IInterfaceTestBase
{
};

class TestObjectBase
{
};

class TestObject : public TestObjectBase, public IInterfaceTest
{
};

void TestDynamicCast(IInterfaceTestBase* base)
{
	for (int32 i = 0; i < 50000; ++i)
	{
		TestObject* test = dynamic_cast<TestObject*>(base);
		assert(test != nullptr);
	}
}

class IInterfaceReflectionTestBase abstract
{
	GEN_INTERFACE_REFLECTION(IInterfaceReflectionTestBase)

public:
	IInterfaceReflectionTestBase() {}
	virtual ~IInterfaceReflectionTestBase() {}
};

class IInterfaceReflectionTest abstract : public InterfaceReflector<IInterfaceReflectionTestBase>
{
	GEN_INTERFACE_REFLECTION(IInterfaceReflectionTest)
};

class ReflectionTestObjectBase
{
	GEN_REFLECTION(ReflectionTestObjectBase)
};

class ReflectionTestObject : public InterfaceReflector<IInterfaceReflectionTest>
{
	GEN_REFLECTION(ReflectionTestObject)

public:
	METHOD(Do)
	void Do(int32 param)
	{
		std::cout << "value : " << param << std::endl;
	}

public:
	PROPERTY(mA)
	int mA;
};

void TestReflectionCast(IInterfaceReflectionTestBase* base)
{
	for (int32 i = 0; i < 50000; ++i)
	{
		ReflectionTestObject* test = Cast<ReflectionTestObject>(base);
		assert(test != nullptr);
	}
}

int main()
{
	//{
	//	IInterfaceTestBase* base = new TestObject();
	//
	//	Tester tester(TestDynamicCast, base);
	//	auto result = tester();
	//	std::cout << "Dynamic : " << result << std::endl;
	//
	//	delete base;
	//}
	//{
	//	IInterfaceReflectionTestBase* base = new ReflectionTestObject();
	//
	//	Tester tester(TestReflectionCast, base);
	//	auto result = tester();
	//	std::cout << "Reflection : " << result << std::endl;
	//
	//	delete base;
	//}
	ReflectionTestObject test;
	const Method* func = test.GetTypeInfo().GetMethod("Do");
	if (func != nullptr)
	{
		int32 s = 3;
		func->Invoke(&test, s);
		func->Invoke(&test, 6);
	}
	const Property* property = test.GetTypeInfo().GetProperty("mA");
	if (property != nullptr)
	{
		int32 s = 3;
		property->Set(&test, s);
		property->Set(&test, 5);
	}

	return 0;
}