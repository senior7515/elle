#ifndef ELLE_OPTION_HXX
# define ELLE_OPTION_HXX

# include <algorithm>
# include <type_traits>
# include <utility>

# include <elle/Error.hh>
# include <elle/TypeInfo.hh>
# include <elle/meta.hh>
# include <elle/printf.hh>
# include <elle/utils.hh>

namespace elle
{
  namespace _details
  {
    template <std::size_t Size,
              std::size_t Align,
              int Index,
              typename ... Types>
    class OptionHelper
    {
    public:
      using Self = OptionHelper;
      OptionHelper()
      {}

      OptionHelper(Self&&)
      {}

      OptionHelper(Self const&)
      {}

      void
      _emplace()
      {}

      template <typename T>
      void
      _assign(T&& source)
      {}

      template <typename T>
      void
      _assign(T const& source)
      {}

      template <typename Operation, typename ... Args>
      void
      _apply(Args&& ...)
      {
        ELLE_ABORT("unknown option type while applying %s",
                   elle::type_info<Operation>());
      }

      template <typename Operation, typename ... Args>
      void
      _apply(Args&& ... args) const
      {
        elle::unconst(this)->
          template _apply<Operation>(std::forward<Args>(args)...);
      }

      static constexpr std::size_t size = Size;
    protected:
      static_assert(Align > 0, "alignment should not be null");
      ELLE_COMPILER_ALIGN(Align)
      char _buffer[size];
      /// Initialized to empty state.
      int _index = -1;
    };

    template <std::size_t Size,
              std::size_t Align,
              int Index,
              typename Head, typename ... Tail>
    class OptionHelper<Size, Align, Index, Head, Tail ...>
      : public OptionHelper<(sizeof(Head) > Size ? sizeof(Head) : Size),
                            (alignof(Head) > Align ? alignof(Head) : Align),
                            Index + 1, Tail ...>
    {
    public:
      using Super =
        OptionHelper<(sizeof(Head) > Size ? sizeof(Head) : Size),
                     (alignof(Head) > Align ? alignof(Head) : Align),
                     Index + 1, Tail ...>;
      OptionHelper() = default;

      template <typename T, typename A>
      std::enable_if_t<
        std::is_same<std::remove_cv_reference_t<T>, Head>::value,
        void>
      _emplace(A&& value)
      {
        new (this->_buffer) Head(std::forward<A>(value));
        this->_index = Index;
      }

      template <typename T, typename A>
      std::enable_if_t<
        !std::is_same<std::remove_cv_reference_t<T>, Head>::value,
        void>
      _emplace(A&& value)
      {
        this->Super::template _emplace<T>(std::forward<A>(value));
      }

      template <typename T>
      void
      _assign(T&& source)
      {
        if (source._index == Index)
        {
          char* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head&&>(*buffer));
          this->_index = Index;
        }
        else
          this->Super::_assign(std::move(source));
      }

      template <typename T>
      void
      _assign(T const& source)
      {
        if (source._index == Index)
        {
          char const* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head const&>(*buffer));
          this->_index = Index;
        }
        else
          this->Super::_assign(source);
      }

      template <typename Operation, typename ... Args>
      void
      _apply(Args&& ... args)
      {
        if (this->_index == Index)
        {
          char* buffer = this->_buffer;
          Operation::apply(reinterpret_cast<Head&>(*buffer), this->_index,
                           std::forward<Args>(args)...);
        }
        else
          this->Super::template _apply<Operation>(std::forward<Args>(args)...);
      }

      template <typename Operation, typename ... Args>
      void
      _apply(Args&& ... args) const
      {
        if (this->_index == Index)
        {
          char const* buffer = this->_buffer;
          Operation::apply(reinterpret_cast<Head const&>(*buffer), this->_index,
                           std::forward<Args>(args)...);
        }
        else
          this->Super::template _apply<Operation>(std::forward<Args>(args)...);
      }

      ~OptionHelper()
      {
        if (this->_index == Index)
        {
          char* buffer = this->_buffer;
          reinterpret_cast<Head&>(*buffer).~Head();
        }
      }

      OptionHelper(
        OptionHelper<Size, Align, Index, Head, Tail...>&& source)
        : Super(static_cast<Super&&>(source))
      {
        if (source._index == Index)
        {
          char* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head&&>(*buffer));
          this->_index = Index;
        }
      }

      OptionHelper(
        OptionHelper<Size, Align, Index, Head, Tail...> const& source)
        : Super(static_cast<Super const&>(source))
      {
        if (source._index == Index)
        {
          char const* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head const&>(*buffer));
          this->_index = Index;
        }
      }
    };
  }

  template <typename ... Types>
  Option<Types ...>::Option(Option<Types...>&& source)
    : _details::OptionHelper<0u, 0u, 0, Types ...>(
      static_cast<_details::OptionHelper<0u, 0u, 0, Types ...>&&>(source))
  {
    ELLE_ASSERT_NEQ(this->_index, -1);
  };

  template <typename ... Types>
  Option<Types ...>::Option(Option<Types ...> const& source)
    : _details::OptionHelper<0u, 0u, 0, Types ...>(
      static_cast<_details::OptionHelper<0u, 0u, 0, Types ...> const&>(source))
  {
    ELLE_ASSERT_NEQ(this->_index, -1);
  };

  template <typename ... Types>
  Option<Types ...>::Option(Option<Types ...>& source)
    : _details::OptionHelper<0u, 0u, 0, Types ...>(
      static_cast<_details::OptionHelper<0u, 0u, 0, Types ...>&>(source))
  {
    ELLE_ASSERT_NEQ(this->_index, -1);
  };

  template <typename ... Types>
  template <typename T>
  Option<Types ...>::Option(T&& value)
  {
    this->template _emplace<std::remove_cv_reference_t<T>>(
      std::forward<T>(value));
  }

  namespace _details
  {
    struct OptionReset
    {
      template <typename V>
      static void
      apply(V& v, int& idx)
      {
        v.~V();
        idx = -1;
      }
    };
  }

  template <typename ... Types>
  Option<Types ...>&
  Option<Types ...>::operator =(Option<Types ...>&& option)
  {
    this->template _apply<_details::OptionReset>();
    this->_assign(std::move(option));
    return *this;
  }

  template <typename ... Types>
  Option<Types ...>&
  Option<Types ...>::operator =(Option<Types ...> const& option)
  {
    this->template _apply<_details::OptionReset>();
    this->_assign(option);
    return *this;
  }

  template <typename ... Types>
  template <typename T>
  void
  Option<Types ...>::emplace(T&& value)
  {
    using Plain = std::remove_cv_reference_t<T>;
    this->template _apply<_details::OptionReset>();
    this->template _emplace<Plain>(std::forward<T>(value));
  };

  template <typename ... Types>
  template <typename T>
  T&
  Option<Types ...>::get()
  {
    if (this->_index != meta::List<Types ...>::template index_of<T>::value)
      throw elle::Error(elle::sprintf("option is not a %s",
                                      elle::type_info<T>()));
    char* buffer = this->_buffer;
    return reinterpret_cast<T&>(*buffer);
  };

  template <typename ... Types>
  template <typename T>
  T const&
  Option<Types ...>::get() const
  {
    return elle::unconst(this)->template get<T>();
  };

  template <typename ... Types>
  template <typename T>
  bool
  Option<Types ...>::is() const
  {
    return this->_index == meta::List<Types ...>::template index_of<T>::value;
  };

  /*--------------.
  | Serialization |
  `--------------*/

  namespace _details
  {
    struct OptionDeserialize
    {
      template <typename V>
      static void
      apply(V& v, int, serialization::SerializerIn& s)
      {
        new (&v) V(s.deserialize<V>("value"));
      }
    };
  }

  template <typename ... Types>
  Option<Types ...>::Option(serialization::SerializerIn& s)
  {
    try
    {
      s.serialize("type", this->_index);
      this->template _apply<_details::OptionDeserialize>(s);
    }
    catch (...)
    {
      this->_index = -1;
      throw;
    }
  }

  namespace _details
  {
    struct OptionSerialize
    {
      template <typename V>
      static void
      apply(V& v, int, serialization::Serializer& s)
      {
        s.serialize("value", v);
      }
    };
  }

  template <typename ... Types>
  void
  Option<Types ...>::serialize(serialization::Serializer& s)
  {
    // FIXME: simplify with split serialization
    if (s.in())
      this->template _apply<_details::OptionReset>();
    s.serialize("type", this->_index);
    if (s.in())
      try
      {
        this->template _apply<_details::OptionDeserialize>(
          static_cast<serialization::SerializerIn&>(s));
      }
      catch (...)
      {
        this->_index = -1;
        throw;
      }
    else
      this->template _apply<_details::OptionSerialize>(s);
  }

  /*----------.
  | Printable |
  `----------*/

  namespace _details
  {
    struct OptionPrint
    {
      template <typename V>
      static void
      apply(V const& v, int, std::ostream& output)
      {
        elle::fprintf(output, "%s", v);
      }
    };
  }

  template <typename ... Args>
  std::ostream&
  operator << (std::ostream& output, Option<Args...> const& option)
  {
    option.template _apply<_details::OptionPrint>(output);
    return output;
  }
}

#endif
