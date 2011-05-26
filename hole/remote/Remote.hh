//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Remote.hh
//
// created       julien quintard   [fri may 20 19:31:08 2011]
// updated       julien quintard   [thu may 26 13:08:47 2011]
//

#ifndef HOLE_LOCAL_REMOTE_HH
#define HOLE_LOCAL_REMOTE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/Holeable.hh>

#include <hole/remote/Node.hh>

namespace hole
{
  namespace remote
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the remote hole implementation stores data on a remote host's
    /// storage.
    ///
    class Remote:
      public Holeable
    {
    public:
      //
      // constructors & destructors
      //
      Remote(const nucleus::Network&);
      ~Remote();

      //
      // methods
      //
      elle::Status	Host(const elle::Address&);

      //
      // interfaces
      //

      // holeable
      elle::Status	Join();
      elle::Status	Leave();

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
      // attributes
      //
      elle::Address	host;

      Node*		node;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/remote/Client.hh>
#include <hole/remote/Manifest.hh>
#include <hole/remote/Server.hh>

#endif
