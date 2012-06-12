#include <elle/log.hh>

#include <elle/io/File.hh>
#include <elle/io/Piece.hh>

#include <elle/utility/Buffer.hh>

#include <elle/serialize/HexadecimalArchive.hh>

#include <nucleus/proton/ImmutableBlock.hh>

#include <lune/Lune.hh>
#include <Infinit.hh>

namespace nucleus
{
  namespace proton
  {

    ELLE_LOG_TRACE_COMPONENT("nucleus.proton.ImmutableBlock");

    ///
    /// default constructor.
    ///
    ImmutableBlock::ImmutableBlock():
      Block(FamilyUnknown, neutron::ComponentUnknown)
    {
    }

    ///
    /// specific constructor.
    ///
    ImmutableBlock::ImmutableBlock(const Family                 family,
                                   const neutron::Component     component):
      Block(family, component)
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(ImmutableBlock, _());

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method loads the block.
    ///
    elle::Status        ImmutableBlock::Load(const Network&     network,
                                             const Address&     address)
    {
      elle::Path        path;
      elle::String      unique;

      if (address.digest->Save(unique) == elle::Status::Error)
        escape("unable to convert the address in its hexadecimal form");

      ELLE_LOG_TRACE("Load immutable block from address %s", unique);

      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::ImmutableBlock::Load(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
          elle::Status::Error)
        escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                        elle::io::Piece("%ADDRESS%", unique)) == elle::Status::Error)
        escape("unable to complete the path");

      return this->Load(path);
    }

    ///
    /// this method stores the block in its file format.
    ///
    elle::Status        ImmutableBlock::Store(const Network&    network,
                                              const Address&    address) const
    {
      elle::Path        path;
      elle::String      unique;

      if (address.digest->Save(unique) == elle::Status::Error)
        escape("unable to convert the address in its hexadecimal form");

      ELLE_LOG_TRACE("Store immutable block from address %s", unique);

      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::ImmutableBlock::Store(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
          elle::Status::Error)
        escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                        elle::io::Piece("%ADDRESS%", unique)) == elle::Status::Error)
        escape("unable to complete the path");

      return this->Store(path);
    }

    ///
    /// this method erases a block.
    ///
    elle::Status        ImmutableBlock::Erase(const Network&    network,
                                              const Address&    address)
    {
      elle::Path        path;
      elle::String      unique;

      if (address.digest->Save(unique) == elle::Status::Error)
        escape("unable to convert the address in its hexadecimal form");

      ELLE_LOG_TRACE("Erase immutable block from address %s", unique);

      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::ImmutableBlock::Erase(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
          elle::Status::Error)
        escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                        elle::io::Piece("%ADDRESS%", unique)) == elle::Status::Error)
        escape("unable to complete the path");

      // is the file present...
      if (elle::io::File::Exist(path) == elle::Status::True)
        {
          // erase the file.
          if (elle::io::File::Erase(path) == elle::Status::Error)
            escape("unable to erase the file");
        }

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the block exists.
    ///
    elle::Status        ImmutableBlock::Exist(const Network&    network,
                                              const Address&    address) const
    {
      elle::Path        path;
      elle::String      unique;

      if (address.digest->Save(unique) == elle::Status::Error)
        escape("unable to convert the address in its hexadecimal form");

      ELLE_LOG_TRACE("Check existence immutable block from address %s", unique);

      // debug.
      if (Infinit::Configuration.nucleus.debug == true)
        printf("[nucleus] proton::ImmutableBlock::Exist(%s)\n",
               unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
          elle::Status::Error)
        flee("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::io::Piece("%NETWORK%", network.name),
                        elle::io::Piece("%ADDRESS%", unique)) == elle::Status::Error)
        flee("unable to complete the path");

      // test the file.
      if (elle::io::File::Exist(path) == elle::Status::True)
        return elle::Status::True;

      return elle::Status::False;
    }

  }
}
