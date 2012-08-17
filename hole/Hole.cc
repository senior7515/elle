#include <hole/Hole.hh>
#include <hole/Holeable.hh>
#include <hole/implementations/local/Implementation.hh>
#include <hole/implementations/remote/Implementation.hh>
#include <hole/implementations/slug/Implementation.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/proton/ImmutableBlock.hh>
#include <nucleus/proton/MutableBlock.hh>

#include <Infinit.hh>

#include <elle/idiom/Close.hh>
# include <boost/format.hpp>
#include <elle/idiom/Open.hh>

namespace hole
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this value defines the component's name.
  ///
  const elle::Character         Component[] = "hole";

  ///
  /// this variable contains the network descriptor.
  ///
  lune::Descriptor              Hole::Descriptor;

  ///
  /// this variable contains the network set of initial nodes.
  ///
  lune::Set                     Hole::Set;

  ///
  /// this variable contains the device passport.
  ///
  lune::Passport                Hole::Passport;

  ///
  /// this variable holds the hole implementation.
  ///
  Holeable*                     Hole::Implementation = nullptr;

  ///
  /// XXX
  ///
  Hole::State                   Hole::state = Hole::StateOffline;

  ///
  /// XXX[to replace by a new signal]
  ///
  elle::concurrency::Signal<
    elle::radix::Parameters<>
    >                           Hole::ready;

//
// ---------- static methods --------------------------------------------------
//

  void
  Hole::Initialize()
  {
    nucleus::proton::Network network;

    // disable the meta logging.
    if (elle::radix::Meta::Disable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to disable the meta logging");

    //
    // retrieve the descriptor.
    //
    {
      // does the network exist.
      if (Hole::Descriptor.Exist(Infinit::Network) == elle::Status::False)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "this network does not seem to exist");

      // load the descriptor.
      if (Hole::Descriptor.Load(Infinit::Network) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to load the descriptor");

      // validate the descriptor.
      if (Hole::Descriptor.Validate(Infinit::Authority) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to validate the descriptor");
    }

    //
    // retrieve the set, if present.
    //
    if (Hole::Set.Exist(Infinit::Network) == elle::Status::True)
      {
        // load the set.
        if (Hole::Set.Load(Infinit::Network) == elle::Status::Error)
          throw std::runtime_error("unable to load the set");
      }

    //
    // retrieve the passport.
    //
    {
      // does the network exist.
      if (Hole::Passport.Exist() == elle::Status::False)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "the device passport does not seem to exist");

      // load the passport.
      if (Hole::Passport.Load() == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to load the passport");

      // validate the passport.
      if (Hole::Passport.Validate(Infinit::Authority) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                        "unable to validate the passport");
    }

    // enable the meta logging.
    if (elle::radix::Meta::Enable() == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to enable the meta logging");

    // create the network instance.
    if (network.Create(Infinit::Network) == elle::Status::Error)
      throw reactor::Exception(elle::concurrency::scheduler(),
                      "unable to create the network instance");

    // create the holeable depending on the model.
    switch (Hole::Descriptor.model.type)
      {
        case Model::TypeLocal:
        {
          // allocate the instance.
          Hole::Implementation =
            new implementations::local::Implementation(network);

          break;
        }
        case Model::TypeRemote:
        {
          // allocate the instance.
          Hole::Implementation =
            new implementations::remote::Implementation(network);

          break;
        }
        case Model::TypeSlug:
        {
          // allocate the instance.
          Hole::Implementation =
            new implementations::slug::Implementation(network);

          break;
        }
        case Model::TypeCirkle:
        {
          /* XXX
          // allocate the instance.
          Hole::Implementation =
          new implementations::cirkle::Implementation(network);
          */

          break;
        }
        default:
        {
          static boost::format fmt("unknown or not-yet-supported model '%u'");
          throw reactor::Exception(elle::concurrency::scheduler(),
                          str(fmt % Hole::Descriptor.model.type));
        }
      }
  }

  void
  Hole::join()
  {
    // join the network
    Hole::Implementation->Join();
  }

  void
  Hole::leave()
  {
    // XXX
  }

  ///
  /// this method cleans the hole.
  ///
  /// the components are recycled just to make sure the memory is
  /// released before the Meta allocator terminates.
  ///
  elle::Status          Hole::Clean()
  {
    // leave the network
    if (Hole::Implementation->Leave() == elle::Status::Error)
      escape("unable to leave the network");

    // delete the implementation.
    delete Hole::Implementation;

    return elle::Status::Ok;
  }

  ///
  /// this method can be called to signal the other components that the
  /// network layer is ready to receive requests.
  ///
  elle::Status          Hole::Ready()
  {
    if (Hole::state == Hole::StateOnline)
      return elle::Status::Ok;

    if (Hole::ready.Emit() == elle::Status::Error)
      escape("unable to emit the signal");

    Hole::state = Hole::StateOnline;

    return elle::Status::Ok;
  }

  ///
  /// this method returns the address of the root block i.e the origin.
  ///
  elle::Status          Hole::Origin(nucleus::proton::Address& address)
  {
    // return the address.
    address = Hole::Descriptor.root;

    return elle::Status::Ok;
  }

  ///
  /// this method stores the given block.
  ///
  elle::Status          Hole::Push(const nucleus::proton::Address& address,
                                   const nucleus::proton::Block& block)
  {
    // XXX check the block's footprint which should not exceed Extent

    // forward the request depending on the nature of the block which
    // the address indicates.
    switch (address.family)
      {
      case nucleus::proton::FamilyContentHashBlock:
        {
          const nucleus::proton::ImmutableBlock*        ib;

          // cast to an immutable block.
          ib = static_cast<const nucleus::proton::ImmutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::ImmutableBlock*>(
                   &block) != nullptr);

          // store the immutable block.
          if (Hole::Implementation->Put(address, *ib) == elle::Status::Error)
            escape("unable to put the block");

          break;
        }
      case nucleus::proton::FamilyPublicKeyBlock:
      case nucleus::proton::FamilyOwnerKeyBlock:
      case nucleus::proton::FamilyImprintBlock:
        {
          const nucleus::proton::MutableBlock*          mb;

          // cast to a mutable block.
          mb = static_cast<const nucleus::proton::MutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::MutableBlock*>(
                   &block) != nullptr);

          // store the mutable block.
          if (Hole::Implementation->Put(address, *mb) == elle::Status::Error)
            escape("unable to put the block");

          break;
        }
      default:
        {
          escape("unknown block family '%u'",
                 address.family);
        }
      }

    return elle::Status::Ok;
  }

  ///
  /// this method returns the block associated with the given address.
  ///
  elle::Status          Hole::Pull(const nucleus::proton::Address&      address,
                                   const nucleus::proton::Version&      version,
                                   nucleus::proton::Block&              block)
  {
    // forward the request depending on the nature of the block which
    // the addres indicates.
    switch (address.family)
      {
      case nucleus::proton::FamilyContentHashBlock:
        {
          nucleus::proton::ImmutableBlock*      ib;

          // cast to an immutable block.
          ib = static_cast<nucleus::proton::ImmutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::ImmutableBlock*>(
                   &block) != nullptr);

          // retrieve the immutable block.
          if (Hole::Implementation->Get(address, *ib) == elle::Status::Error)
            escape("unable to get the block");

          break;
        }
      case nucleus::proton::FamilyPublicKeyBlock:
      case nucleus::proton::FamilyOwnerKeyBlock:
      case nucleus::proton::FamilyImprintBlock:
        {
          nucleus::proton::MutableBlock*        mb;

          // cast to a mutable block.
          mb = static_cast<nucleus::proton::MutableBlock*>(&block);
          assert(dynamic_cast<const nucleus::proton::MutableBlock*>(
                   &block) != nullptr);

          // retrieve the mutable block.
          if (Hole::Implementation->Get(address, version,
                                        *mb) == elle::Status::Error)
            escape("unable to get the block");

          break;
        }
      default:
        {
          escape("unknown block family '%u'",
                 address.family);
        }
      }

    return elle::Status::Ok;
  }

  ///
  /// this method removes the block associated with the given address.
  ///
  elle::Status          Hole::Wipe(const nucleus::proton::Address&      address)
  {
    // forward the kill request to the implementation.
    if (Hole::Implementation->Kill(address) == elle::Status::Error)
      escape("unable to erase the block");

    return elle::Status::Ok;
  }

}
