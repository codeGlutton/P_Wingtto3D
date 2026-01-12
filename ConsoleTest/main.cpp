#include "pch.h"
#include "Core/Object.h"

#include <chrono>

class IInterfaceTestBase abstract
{
public:
	IInterfaceTestBase() {}
	virtual ~IInterfaceTestBase() {}
};

class IInterfaceTestBase2 abstract
{
public:
	IInterfaceTestBase2() {}
	virtual ~IInterfaceTestBase2() {}
};

class IInterfaceTest abstract : public IInterfaceTestBase
{
};

class TestObjectBase
{
};

class TestObject : public TestObjectBase, public IInterfaceTestBase2, public IInterfaceTest
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

class IInterfaceReflectionTestBase2 abstract
{
	GEN_INTERFACE_REFLECTION(IInterfaceReflectionTestBase2)

public:
	IInterfaceReflectionTestBase2() {}
	virtual ~IInterfaceReflectionTestBase2() {}
};

class IInterfaceReflectionTest abstract : public InterfaceReflector<IInterfaceReflectionTestBase>
{
	GEN_INTERFACE_REFLECTION(IInterfaceReflectionTest)
};

class ReflectionTestObjectBase : public Object
{
	GEN_REFLECTION(ReflectionTestObjectBase)
};

class ReflectionTestObject : public ReflectionTestObjectBase, public InterfaceReflector<IInterfaceReflectionTestBase2, IInterfaceReflectionTest>
{
	GEN_REFLECTION(ReflectionTestObject)

public:
	METHOD(Do)
	void Do(int32 param) const
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
	BOOT_SYSTEM->Boot();

	/*const int32 trial = 100;

	std::chrono::microseconds dynamicResult = std::chrono::microseconds{ 0 };
	std::chrono::microseconds reflectionResult = std::chrono::microseconds{ 0 };
	for (int32 i = 0; i < trial; ++i)
	{
		{
			IInterfaceTestBase* base = new TestObject();

			Tester tester(TestDynamicCast, base);
			dynamicResult += tester();

			delete base;
		}
		{
			IInterfaceReflectionTestBase* base = new ReflectionTestObject();

			Tester tester(TestReflectionCast, base);
			reflectionResult += tester();

			delete base;
		}
	}
	std::cout << "Dynamic : " << dynamicResult / trial << std::endl;
	std::cout << "Reflection : " << reflectionResult / trial << std::endl;*/

	ReflectionTestObject test;
	auto cdo = test.GetDefaultObject();
	cdo->Do(8);

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