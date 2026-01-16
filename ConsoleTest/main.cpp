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
public:
	virtual ~TestObjectBase() {}
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

public:
	METHOD(DoInterface)
	virtual void DoInterface(int32 param) const
	{
		std::cout << "interface value : " << param << std::endl;
	}
};

enum TestEnum
{
	None,
	One,
	Two
};

struct ReflectionTestStruct
{
	GEN_STRUCT_REFLECTION(ReflectionTestStruct)

public:
	PROPERTY(mE)
	TestEnum mE = TestEnum::Two;
};

class ReflectionTestObjectBase : public Object
{
	GEN_REFLECTION(ReflectionTestObjectBase)

public:
	PROPERTY(mStruct)
	ReflectionTestStruct mStruct{};

	PROPERTY(mSP)
	std::shared_ptr<Object> mSP;
};

class ReflectionTestObject : public InterfaceReflector<ReflectionTestObjectBase, IInterfaceReflectionTestBase2, IInterfaceReflectionTest>
{
	GEN_REFLECTION(ReflectionTestObject)

public:
	METHOD(Do)
	void Do(int32 param) const
	{
		std::cout << "value : " << param << std::endl;
	}

	virtual void DoInterface(int32 param) const override
	{
		std::cout << "override value : " << param << std::endl;
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

DECLARE_DELEGATE_1_PARAM(OnTest, int);
DECLARE_MULTICAST_DELEGATE_1_PARAM(OnTestMulti, IInterfaceTestBase*);

int main()
{
	BOOT_SYSTEM->Boot();

	/*std::shared_ptr<const ReflectionTestObject> t = std::make_shared<ReflectionTestObject>();
	OnTest testDelegate;
	auto handle = testDelegate.BindNativeMethod<IInterfaceReflectionTest>(t, &IInterfaceReflectionTest::DoInterface);
	OnTestMulti testMultiDelegate;
	auto handleMulti = testMultiDelegate.BindStatic(&TestDynamicCast);

	const int32 trial = 100;

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
	std::cout << "Reflection : " << reflectionResult / trial << std::endl;

	testDelegate.ExecuteIfBound(5);
	testDelegate.Unbind(handle);
	testDelegate.ExecuteIfBound(5);

	std::shared_ptr<IInterfaceTestBase> multiDelegateTarget = std::make_shared<TestObject>();
	testMultiDelegate.Multicast(multiDelegateTarget.get());
	testMultiDelegate.Unbind(handleMulti);
	testMultiDelegate.Multicast(multiDelegateTarget.get());*/

	/*std::shared_ptr<ReflectionTestObject> test = std::make_shared<ReflectionTestObject>();
	auto cdo = test->GetDefaultObject();
	cdo->Do(8);

	auto typeInfo = BOOT_SYSTEM->GetObjectTypeInfo("ReflectionTestObject");
	{
		const Method* func = typeInfo->GetMethod("Do");
		if (func != nullptr)
		{
			int32 s = 3;
			func->Invoke(test.get(), s);
			func->Invoke(test.get(), 6);
		}
	}
	{
		const Method* func = typeInfo->GetMethod("DoInterface");
		if (func != nullptr)
		{
			func->Invoke(test.get(), 6);
			IInterfaceReflectionTest* testInterface = static_cast<IInterfaceReflectionTest*>(test.get());
			func->Invoke(testInterface, 10);
		}
	}
	{
		const Property* property = typeInfo->GetProperty("mA");
		if (property != nullptr)
		{
			int32 s = 3;
			property->Set(test.get(), s);
			property->Set(test.get(), 5);
		}
	}
	{
		const Property* property = typeInfo->GetProperty("mSP");
		if (property != nullptr)
		{
			property->GetRawPtr(test.get());
			property->SetRawPtr(test.get(), &test);

			property->Set<std::shared_ptr<Object>>(test.get(), test);
		}
	}
	{
		const Property* property = typeInfo->GetProperty("mStruct");
		if (property != nullptr)
		{
			const ReflectionTestStruct& resultStruct = property->Get<ReflectionTestStruct>(&test);

			const StructTypeInfo* structTypeInfo = reinterpret_cast<const StructTypeInfo*>(&property->GetTypeInfo());
			const Property* structEnumProperty = structTypeInfo->GetProperty("mE");
			if (structEnumProperty != nullptr)
			{
				structEnumProperty->Set(&test->mStruct, TestEnum::None);
				structEnumProperty->Set(&test->mStruct, TestEnum::One);
			}
		}
	}*/

	return 0;
}