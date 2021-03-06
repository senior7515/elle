#pragma once

#include <utility>

#include <elle/compiler.hh>

namespace elle
{
  template <typename T>
  class With
  {
  /*------.
  | Types |
  `------*/
  public:
    using Self = With<T>;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    template <typename ... Args>
    With(Args&&... args);
    With(With<T>&& model);
    ~With();
  private:
    ELLE_COMPILER_ALIGN(alignof(T))
    char _data[sizeof(T)];
    bool _used;
    T* _value;

  /*--------.
  | Running |
  `--------*/
  public:
    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action(std::declval<T&>()));

    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action());

  private:
    static
    ELLE_COMPILER_ATTRIBUTE_NO_INLINE
    void
    destroy_it(T* ptr);
    template <typename F>
    auto
    _run(F const& action) -> decltype(action(std::declval<T&>()));
  };
}

#include <elle/With.hxx>

