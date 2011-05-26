//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Client.hh
//
// created       julien quintard   [thu may 26 10:21:46 2011]
// updated       julien quintard   [thu may 26 14:22:04 2011]
//
 
#ifndef HOLE_REMOTE_CLIENT_HH
#define HOLE_REMOTE_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/remote/Node.hh>

namespace hole
{
  namespace remote
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Client:
      public Node
    {
    public:
      //
      // constructors & destructors
      //
      Client(const nucleus::Network&,
	     const elle::Address&);

      //
      // interfaces
      //

      // node
      elle::Status	Initialize();
      elle::Status	Clean();

      elle::Status	Put(const nucleus::Address&,
			    const nucleus::ImmutableBlock&);
      elle::Status	Put(const nucleus::Address&,
			    const nucleus::MutableBlock&);
      elle::Status	Get(const nucleus::Address&,
			    nucleus::ImmutableBlock&);
      elle::Status	Get(const nucleus::Address&,
			    const nucleus::Version&,
			    nucleus::MutableBlock&);
      elle::Status	Kill(const nucleus::Address&);

      //
      // callbacks
      //
      elle::Status	Error(const elle::String&);

      //
      // attributes
      //
      elle::Gate	gate;
    };

  }
}

#endif
