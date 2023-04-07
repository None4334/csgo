
#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2

class INetChannelInfo
{
public:
	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		PAINTMAP,		// paintmap data
		ENCRYPTED,		// encrypted data
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName() const = 0;			
	virtual const char* GetAddress() const = 0;			
	virtual float		GetTime() const = 0;			
	virtual float		GetTimeConnected() const = 0;	
	virtual int			GetBufferSize() const = 0;		
	virtual int			GetDataRate() const = 0;		

	virtual bool		IsLoopback() const = 0;			// true if loopback channel
	virtual bool		IsTimingOut() const = 0;		// true if timing out
	virtual bool		IsPlayback() const = 0;			// true if demo playback
	virtual float		GetLatency(int iFlow) const = 0; // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int iFlow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int iFlow) const = 0; // avg packet loss[0..1]
	virtual float		GetAvgChoke(int iFlow) const = 0; // avg packet choke[0..1]
	virtual float		GetAvgData(int iFlow) const = 0; // data flow in bytes/sec
	virtual float		GetAvgPackets(int iFlow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int iFlow) const = 0; // total flow in/out in bytes
	virtual int			GetTotalPackets(int iFlow) const = 0;
	virtual int			GetSequenceNr(int iFlow) const = 0; // last send seq number
	virtual bool		IsValidPacket(int iFlow, int nFrame) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int iFlow, int nFrame) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int iFlow, int nFrame, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int iFlow, int* piReceived, int* piTotal) const = 0; // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived() const = 0; // get time since last received packet in seconds
	virtual	float		GetCommandInterpolationAmount(int iFlow, int nFrame) const = 0;
	virtual void		GetPacketResponseLatency(int iFlow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation, float* pflFrameStartTimeStdDeviation) const = 0;
	virtual float		GetTimeoutSeconds() const = 0;
};

class INetChannel
{
public:
	std::byte	pad0[0x14];				
	bool		bProcessingMessages;	
	bool		bShouldDelete;			
	bool		bStopProcessing;		
	std::byte	pad1[0x1];				
	int			iOutSequenceNr;			
	int			iInSequenceNr;			
	int			iOutSequenceNrAck;		
	int			iOutReliableState;		
	int			iInReliableState;		
	int			iChokedPackets;			
	std::byte	pad2[0x414];			

	int	SendDatagram(bf_write* pDatagram)
	{
		return TUtil::CallVFunc<int>(this, 46, pDatagram);
	}

	bool Transmit(bool bOnlyReliable = false)
	{
		return TUtil::CallVFunc<bool>(this, 49, bOnlyReliable);
	}
}; 

class INetMessage
{
public:
	virtual					~INetMessage() { }
	virtual void			SetNetChannel(void* pNetChannel) = 0;
	virtual void			SetReliable(bool bState) = 0;
	virtual bool			Process() = 0;
	virtual	bool			ReadFromBuffer(bf_read& buffer) = 0;
	virtual	bool			WriteToBuffer(bf_write& buffer) = 0;
	virtual bool			IsReliable() const = 0;
	virtual int				GetType() const = 0;
	virtual int				GetGroup() const = 0;
	virtual const char* GetName() const = 0;
	virtual INetChannel* GetNetChannel() const = 0;
	virtual const char* ToString() const = 0;
	virtual std::size_t		GetSize() const = 0;
};

class CCLCMsg_Move
{
private:
	std::byte pad0[0x8];
public:
	int nBackupCommands;
	int nNewCommands;
};

template <typename T>
class CNetMessagePB : public INetMessage, public T { };
using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move>;

class INetChannelHandler
{
public:
	virtual			~INetChannelHandler() { }
	virtual void	ConnectionStart(INetChannel* pChannel) = 0; 
	virtual void	ConnectionStop() = 0; 
	virtual void	ConnectionClosing(const char* szReason) = 0; 
	virtual void	ConnectionCrashed(const char* szReason) = 0;
	virtual void	PacketStart(int iIncomingSequence, int iOutgoingAcknowledged) = 0;	
	virtual void	PacketEnd() = 0; 
	virtual void	FileRequested(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; 
	virtual void	FileReceived(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; 
	virtual void	FileDenied(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; 
	virtual void	FileSent(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; 
	virtual bool	ChangeSplitscreenUser(int nSplitScreenUserSlot) = 0; 
};
