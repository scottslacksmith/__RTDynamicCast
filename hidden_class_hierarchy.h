#pragma once
#include <memory>

namespace hidden_polymorphic
{

  struct Base1;

  struct Base2;

  struct Derived;

  std::pair<std::shared_ptr<Base2>, Derived *> get_base();

};

namespace hidden_nonpolymorphic
{

  struct Base;

  struct Derived;

  std::shared_ptr<Derived> get_derived();

};
