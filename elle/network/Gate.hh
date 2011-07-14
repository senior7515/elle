//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Gate.hh
//
// created       julien quintard   [wed may 25 10:49:12 2011]
// updated       julien quintard   [thu jul 14 14:08:16 2011]
//

///
/// this very special include is required as Channel needs to know Door/Gate
/// while Door/Gate inherit Channel. including Channel.hh normally makes moc,
/// the QT meta object compiler, unable to detect the QObject classes.
///
/// therefore, Channel.hh is not included when moc processes a header file.
///
#ifndef Q_MOC_RUN
# include <elle/network/Channel.hh>
#endif

#ifndef ELLE_NETWORK_GATE_HH
#define ELLE_NETWORK_GATE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

#include <elle/concurrency/Event.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Port.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QTcpSocket>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

  namespace network
  {

    ///
    /// XXX
    ///
    class Gate:
      public ::QObject,

      public Channel
    {
      Q_OBJECT;

    public:
      //
      // constants
      //
      static const Natural32		Timeout;

      //
      // constructors & destructors
      //
      Gate();
      ~Gate();

      //
      // methods
      //
      Status		Create();
      Status		Create(::QTcpSocket*);

      Status		Connect(const Address&);
      Status		Disconnect();

      Status		Write(const Packet&);

      Status		Read();
      Status		Fetch();

      template <typename I>
      Status		Send(const I,
			     const Event& = Event::Null);
      template <typename O>
      Status		Receive(const Event&,
				O);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O);
      template <typename I>
      Status		Reply(const I);

      //
      // callbacks
      //
      Status		Error(const String&);
      Status		Dispatch();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QTcpSocket*	socket;
      Port		port;

    private slots:
      //
      // slots
      //
      void		_error(const QAbstractSocket::SocketError);
      void		_ready();
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Gate.hxx>

#endif
