//
// Copyright (C) 2004 Andras Varga
//               2005 Christian Dankbar, Irene Ruengeler, Michael Tuexen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_ICMPHEADERSERIALIZER_H
#define __INET_ICMPHEADERSERIALIZER_H

#include "inet/common/packet/Serializer.h"

namespace inet {

namespace serializer {

/**
 * Converts between ICMPHeader and binary (network byte order) ICMP header.
 */
class INET_API ICMPHeaderSerializer : public FieldsChunkSerializer
{
  protected:
    virtual void serialize(ByteOutputStream& stream, const std::shared_ptr<Chunk>& chunk) const override;
    virtual std::shared_ptr<Chunk> deserialize(ByteInputStream& stream) const override;

  public:
    ICMPHeaderSerializer() : FieldsChunkSerializer() {}
};

/**
 * Converts between PingPayload and binary (network byte order) ping payload.
 */
class INET_API PingPayloadSerializer : public FieldsChunkSerializer
{
  public:
    PingPayloadSerializer() : FieldsChunkSerializer() {}

    virtual void serialize(ByteOutputStream& stream, const std::shared_ptr<Chunk>& chunk) const;
    virtual std::shared_ptr<Chunk> deserialize(ByteInputStream& stream) const;
};

} // namespace serializer

} // namespace inet


#endif // ifndef __INET_ICMPHEADERSERIALIZER_H
