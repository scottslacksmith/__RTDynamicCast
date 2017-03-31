#include "hidden_class_hierarchy.h"

namespace hidden_polymorphic
{

  struct Base1 { virtual ~Base1() = default; };
  struct Base2 { virtual ~Base2() = default; };

  struct Derived : public Base1, public Base2 {};

  std::pair<std::shared_ptr<Base2>, Derived *> get_base()
  {
    auto p = std::make_shared<Derived>();
    return { p , p.get() };
  }

}

namespace hidden_nonpolymorphic
{

  struct Base {};

  struct Derived : public Base {};

  std::shared_ptr<Derived> get_derived()
  {
    return std::make_shared<Derived>();
  }

}
