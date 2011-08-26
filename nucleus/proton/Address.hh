//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Address.hh
//
// created       julien quintard   [mon feb 16 21:13:00 2009]
// updated       julien quintard   [thu aug 25 10:44:19 2011]
//

#ifndef NUCLEUS_PROTON_ADDRESS_HH
#define NUCLEUS_PROTON_ADDRESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <nucleus/proton/Family.hh>

#include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Address:
      public elle::Object,
      public elle::Fileable<elle::FormatBase64>
    {
    public:
      //
      // constants
      //
      static const Address	Null;

      //
      // constructors & destructors
      //
      Address();
      Address(const Address&);
      ~Address();

      //
      // methods
      //
      template <typename T,
		typename... TT>
      elle::Status	Create(const Family&,
			       const neutron::Component&,
			       const T&,
			       const TT&...);

      //
      // interfaces
      //

      // object
      declare(Address);
      elle::Boolean	operator==(const Address&) const;
      elle::Boolean	operator<(const Address&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Family			family;
      neutron::Component	component;

      elle::Digest*		digest;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/proton/Address.hxx>

#endif
