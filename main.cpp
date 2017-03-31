#include "stdafx.h"
#include "hidden_class_hierarchy.h"
#include <memory>
#include <cassert>
#include <iostream>
#include <type_traits>

#ifdef _MSC_VER
#include <windows.h>

// Copied from C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\crt\src\vcruntime\rtti.cpp

/////////////////////////////////////////////////////////////////////////////
//
// __RTDynamicCast - Runtime implementation of dynamic_cast<> operator
//
// Output: Pointer to the appropriate sub-object, if possible; nullptr otherwise
//
// Side-effects: Throws bad_cast() if cast fails & input of dynamic_cast<> is
// a reference
//
// Note: A normal runtime check can be generated for a down-cast (base to
// derived) and a cross-cast (a proper base of the complete object to some other
// proper base which is neither an accessible unambiguous base nor derived class
// of the first base).  But the compiler will also generate a runtime check for
// an up-cast (derived to base) that is disallowed because the target base class
// is not public or is ambiguous within the source derived class.  Such an
// invalid up-cast may be a valid cross-cast in the VI case, e.g.:
//
//      A
//     / \
//    B   C
//     \ /
//      D
//
//   class A { virtual ~A(); {} };
//   class B : virtual private A {};
//   class C : virtual public A {};
//   class D : public B, public C {};
//   ...
//   dynamic_cast<A*>((B*)new D);
//
// The up-cast B->A fails, since B inherits A privately.  But the cross-cast
// B->D->C->A succeeds with a runtime check.
//
extern "C" PVOID __CLRCALL_OR_CDECL __RTDynamicCast(
  PVOID inptr,            // Pointer to polymorphic object
  LONG VfDelta,           // Offset of vfptr in object
  PVOID SrcType,          // Static type of object pointed to by inptr
  PVOID TargetType,       // Desired result of cast
  BOOL isReference)       // TRUE if input is reference, FALSE if input is ptr
  throw(...);

#endif

template<typename CastToType, typename CastFromType>
CastToType msvc_dynamic_cast(CastFromType srcPtr)
{
  static_assert( std::is_pointer_v<CastFromType>, "Only pointers are supported by this implementation" );

  LONG virtualFuncOffet = 0;
  PVOID srcTypeInfo = & const_cast<std::type_info&>(typeid(std::remove_pointer_t<CastFromType>));
  PVOID targetTypeInfo = & const_cast<std::type_info&>(typeid(std::remove_pointer_t<CastToType>));
  BOOL isReference = (std::is_pointer_v<CastFromType> ? FALSE : TRUE);

  PVOID targetPtr = __RTDynamicCast(srcPtr, virtualFuncOffet, srcTypeInfo, targetTypeInfo, isReference);

  return static_cast<CastToType>(targetPtr);
}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

namespace example1
{

  struct Base { virtual ~Base() = default; };
  struct Derived : public Base {};

} // namespace example2

void test1()
{
  using namespace example1;

  std::unique_ptr<Derived> ptr = std::make_unique<Derived>();
  Derived * d = ptr.get();
  Base * b = d;

  Derived * d1 = dynamic_cast<Derived *>(b);
  Derived * d2 = msvc_dynamic_cast<Derived *>(b);
  std::cout << __FUNCTION__ << ((d1 == d2) ? " - OK" : " - Failed") << '\n';
}

void test2()
{
  using namespace example1;

  std::unique_ptr<Derived> ptr = std::make_unique<Derived>();
  Derived * d = ptr.get();
  Base * b = d;

  Derived * d1 = dynamic_cast<Derived *>(b);
  Derived const * d2 = msvc_dynamic_cast<Derived const *>(b);
  std::cout << __FUNCTION__ << ((d1 == d2) ? " - OK" : " - Failed") << '\n';
}

//---------------------------------------------------------------------------------------------------------------------

namespace example2
{

  struct Base1 { virtual ~Base1() = default; };
  struct Base2 { virtual ~Base2() = default; };

  struct Derived : public Base1, public Base2 {};

} // namespace example2

void test3()
{
  using namespace example2;
 
  std::unique_ptr<Derived> ptr = std::make_unique<Derived>();
  Derived * d = ptr.get();
  Base1 * b1 = d;
  
  Derived * d1 = dynamic_cast<Derived *>(b1);
  Derived * d2 = msvc_dynamic_cast<Derived *>(b1);
  std::cout << __FUNCTION__ << ((d1 == d2) ? " - OK" : " - Failed") << '\n';
}

void test4()
{
  using namespace example2;

  std::unique_ptr<Derived> ptr = std::make_unique<Derived>();
  Derived * d = ptr.get();
  Base2 * b2 = d;

  Derived * d1 = dynamic_cast<Derived *>(b2);
  Derived * d2 = msvc_dynamic_cast<Derived *>(b2);
  std::cout << __FUNCTION__ << ((d1 == d2) ? " - OK" : " - Failed") << '\n';
}

//---------------------------------------------------------------------------------------------------------------------

void test5()
{
  using namespace hidden;

  std::pair<std::shared_ptr<Base2>, Derived *> p = get_base();
  Base2 * b2 = p.first.get();
  Derived * d = p.second;

//  Derived * d11 = dynamic_cast<Derived *>(b2);
  Derived * d1 = d;
  Derived * d2 = msvc_dynamic_cast<Derived *>(b2);
  std::cout << __FUNCTION__ << ((d1 != d2) ? " - OK" : " - Failed") << '\n';
}

//---------------------------------------------------------------------------------------------------------------------

int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();

  return 0;
}

