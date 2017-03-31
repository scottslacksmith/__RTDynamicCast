#pragma once
#include <memory>

namespace hidden
{

  struct Base1;

  struct Base2;

  struct Derived;

  std::pair<std::shared_ptr<Base2>, Derived *> get_base();

};

