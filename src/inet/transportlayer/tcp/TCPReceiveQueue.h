//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2009 Thomas Reschka
// Copyright (C) 2010 Zoltan Bojthe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_TCPRECEIVEQUEUE_H
#define __INET_TCPRECEIVEQUEUE_H

#include <list>
#include <string>

#include "inet/common/INETDefs.h"

#include "inet/common/packet/SequenceChunk.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/common/packet/ReorderBuffer.h"
#include "inet/transportlayer/tcp/TCPConnection.h"
#include "inet/transportlayer/tcp_common/TCPSegment.h"

namespace inet {

class TCPCommand;

namespace tcp {

class TcpHeader;


/**
 * Receive queue that manages Chunks.
 *
 * @see TCPSendQueue
 */
class INET_API TCPReceiveQueue : public cObject
{
  protected:
    TCPConnection *conn = nullptr;    // the connection that owns this queue
    uint32 rcv_nxt = 0;
    ReorderBuffer reorderBuffer;
    ChunkQueue kludgeQueue;

    uint32_t offsetToSeq(int64_t offs) const { return (uint32_t)offs; }

    int64_t seqToOffset(uint32_t seq) const
    {
        int64_t expOffs = reorderBuffer.getExpectedOffset();
        uint32_t expSeq = offsetToSeq(expOffs);
        return (seqGE(seq, expSeq)) ? expOffs + (seq - expSeq) : expOffs - (expSeq - seq);
    }

  public:
    /**
     * Ctor.
     */
    TCPReceiveQueue();

    /**
     * Virtual dtor.
     */
    virtual ~TCPReceiveQueue();

    /**
     * Set the connection that owns this queue.
     */
    virtual void setConnection(TCPConnection *_conn) { conn = _conn; }

    /**
     * Set initial receive sequence number.
     */
    virtual void init(uint32 startSeq);

    virtual std::string str() const override;

    /**
     * Called when a TCP segment arrives, it should extract the payload
     * from the segment and store it in the receive queue. The segment
     * object should *not* be deleted.
     *
     * The method should return the sequence number to be ACKed.
     */
    virtual uint32 insertBytesFromSegment(Packet *packet, TcpHeader *tcpseg);

    /**
     * Should create a packet to be passed up to the app, up to (but NOT
     * including) the given sequence no (usually rcv_nxt).
     * It should return nullptr if there's no more data to be passed up --
     * this method is called several times until it returns nullptr.
     */
    virtual cPacket *extractBytesUpTo(uint32 seq);

    /**
     * Returns the number of bytes (out-of-order-segments) currently buffered in queue.
     */
    virtual uint32 getAmountOfBufferedBytes();

    /**
     * Returns the number of bytes currently free (=available) in queue. freeRcvBuffer = maxRcvBuffer - usedRcvBuffer
     */
    virtual uint32 getAmountOfFreeBytes(uint32 maxRcvBuffer);

    /**
     * Returns the number of blocks currently buffered in queue.
     */
    virtual uint32 getQueueLength();

    /**
     * Shows current queue status.
     */
    virtual void getQueueStatus();

    /**
     * Returns left edge of enqueued region.
     */
    virtual uint32 getLE(uint32 fromSeqNum);

    /**
     * Returns right edge of enqueued region.
     */
    virtual uint32 getRE(uint32 toSeqNum);

    /** Returns the minimum of first byte seq.no. in queue and rcv_nxt */
    virtual uint32 getFirstSeqNo();
};

} // namespace tcp

} // namespace inet

#endif // ifndef __INET_TCPRECEIVEQUEUE_H

