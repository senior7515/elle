#ifndef ELLE_FORMAT_JSON_DETAIL_HH
# define ELLE_FORMAT_JSON_DETAIL_HH

namespace elle { namespace format { namespace json { namespace detail {

    template<bool Cond, typename Then, typename Else> struct StaticIf;
    template<typename Then, typename Else> struct StaticIf<true, Then, Else>
      {
        typedef Then type;
      };
    template<typename Then, typename Else> struct StaticIf<false, Then, Else>
      {
        typedef Else type;
      };


    template<typename T> struct FastConst
      {
        typedef typename StaticIf<
            std::is_arithmetic<T>::value ||
            (std::is_trivial<T>::value && (sizeof(T) <= 4 * sizeof(size_t))) /// XXX different value should be tested
          , T
          , T const&
        >::type type;
      };

    template<typename T>
      class BasicObject
        : public elle::format::json::Object
      {
      private:
        typedef typename FastConst<T>::type FastConstType;
      public:
        typedef T Type;
        typedef FastConstType CastType;

      private:
        Type _value;

      public:
        BasicObject(FastConstType value = T{}) : _value(value) {}
        BasicObject(BasicObject const& other) : _value(other._value) {}
        BasicObject(BasicObject&& other) : _value(other._value) {}
        BasicObject& operator =(BasicObject const& other)
        {
          _value = other._value;
          return *this;
        }
        BasicObject& operator =(BasicObject&& other)
        {
          _value = other._value;
          return *this;
        }
        BasicObject& operator =(FastConstType value)
        {
          _value = value;
          return *this;
        }
        operator CastType() const { return _value; }

        bool operator ==(FastConstType value) const
        {
          return _value == value;
        }

        virtual bool operator ==(Object const& other) const
          { return other == *this; }

        virtual bool operator ==(BasicObject const& other) const
          {
            return _value == other._value;
          }

        std::unique_ptr<Object> Clone() const
          { return std::unique_ptr<Object>(new BasicObject(_value)); }

      protected:
        void Save(elle::serialize::OutputJSONArchive& ar) const;
      };

    template<typename T>
      struct IsString {
          static bool const value = (
                std::is_same<T, std::string>::value
            ||  std::is_convertible<T, char const* const>::value
          );
      };

    template<typename T>
      struct HasIterator
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template<typename K>
            static Yes f(K*, typename K::iterator* = nullptr);
          static bool const value =
            sizeof(f(static_cast<T*>(nullptr))) == sizeof(Yes);
        };

    template<typename T>
      struct IsMap
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template<typename K>
            static Yes f(K*, typename K::mapped_type* = nullptr, typename K::key_type* = nullptr);
          static bool const value = (
                HasIterator<T>::value
            &&  sizeof(f(static_cast<T*>(nullptr))) == sizeof(Yes)
          );
        };

    template<typename T, typename KeyType = void>
      struct IsMappedWith
        {
          typedef char Yes;
          typedef struct { Yes _[2]; } No;
          static No f(...);
          template<typename K> static Yes f(typename K::key_type*);
          static bool const value = (
                IsMap<T>::value
            &&  sizeof(f(static_cast<KeyType*>(nullptr))) == sizeof(Yes)
          );
        };

    template<typename Container>
      struct IsStringMap {
          static bool const value = IsMappedWith<Container, std::string>::value;
      };

    template<typename T>
      struct IsArray
        {
          static bool const value = (
                std::is_array<T>::value
            ||  (
                    HasIterator<T>::value
                &&  !IsString<T>::value
                &&  !IsMap<T>::value
            )
          );
        };

}}}} // !namespace elle::format::json::detail

#endif /* ! _DETAIL_HH */


