#ifndef INFINIT_PROTOCOL_FWD_HH
# define INFINIT_PROTOCOL_FWD_HH

namespace infinit
{
  namespace protocol
  {
    class Channel;
    class ChanneledStream;
    class BaseRPC;
    template <typename ISerializer, typename OSerializer>
    class RPC;
    class Serializer;
    class Stream;
  }
}

#endif
