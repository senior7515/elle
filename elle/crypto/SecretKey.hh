//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/SecretKey.hh
//
// created       julien quintard   [thu nov  1 11:38:37 2007]
// updated       julien quintard   [wed mar 11 16:00:15 2009]
//

#ifndef ELLE_CRYPTO_SECRETKEY_HH
#define ELLE_CRYPTO_SECRETKEY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/misc/Misc.hh>

#include <elle/archive/Archive.hh>

#include <elle/crypto/Plain.hh>
#include <elle/crypto/Cipher.hh>
#include <elle/crypto/Clear.hh>
#include <elle/crypto/Digest.hh>
#include <elle/crypto/OneWay.hh>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <fcntl.h>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a secret key for symmetric encryption.
    ///
    class SecretKey:
      public Object,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const String		Class;
      static const Character		Magic[];

      struct Default
      {
	static const Natural32		Length;
      };

      struct Algorithms
      {
	static const ::EVP_CIPHER*	Cipher;
	static const ::EVP_MD*		Digest;
      };

      //
      // constructors & destructors
      //
      SecretKey()
      {
	SecretKey::New(*this);
      }

      ~SecretKey()
      {
	SecretKey::Delete(*this);
      }

      //
      // methods
      //
      Status		Create(const String&);

      Status		Generate();
      Status		Generate(const Natural32);

      Status		Encrypt(const Archivable&,
				Cipher&) const;
      Status		Encrypt(const Plain&,
				Cipher&) const;

      Status		Decrypt(const Cipher&,
				Archivable&) const;
      Status		Decrypt(const Cipher&,
				Clear&) const;

      //
      // interfaces
      //

      // object
      static Status	New(SecretKey&);
      static Status	Delete(SecretKey&);

      SecretKey&	operator=(const SecretKey&);
      Boolean		operator==(const SecretKey&);
      Boolean		operator!=(const SecretKey&);

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Region		key;
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const elle::crypto::SecretKey&);
}

#endif
