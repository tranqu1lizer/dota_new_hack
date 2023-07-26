#pragma once

#include "ISteamClient.hpp"

const uint32_t k_uAppIdInvalid = 0x0;

class CGameID
{
public:

	CGameID( )
	{
		m_gameID.m_nType = k_EGameIDTypeApp;
		m_gameID.m_nAppID = k_uAppIdInvalid;
		m_gameID.m_nModID = 0;
	}

	explicit CGameID( uint64_t ulGameID )
	{
		m_ulGameID = ulGameID;
	}

	explicit CGameID( int32_t nAppID )
	{
		m_ulGameID = 0;
		m_gameID.m_nAppID = nAppID;
	}

	explicit CGameID( uint32_t nAppID )
	{
		m_ulGameID = 0;
		m_gameID.m_nAppID = nAppID;
	}

	CGameID( uint32_t nAppID, uint32_t nModID )
	{
		m_ulGameID = 0;
		m_gameID.m_nAppID = nAppID;
		m_gameID.m_nModID = nModID;
		m_gameID.m_nType = k_EGameIDTypeGameMod;
	}

	// Hidden functions used only by Steam
	explicit CGameID( const char* pchGameID );
	const char* Render( ) const;					// render this Game ID to string
	static const char* Render( uint64_t ulGameID );		// static method to render a uint64 representation of a Game ID to a string

	uint64_t ToUint64( ) const
	{
		return m_ulGameID;
	}

	uint64_t* GetUint64Ptr( )
	{
		return &m_ulGameID;
	}

	bool IsMod( ) const
	{
		return ( m_gameID.m_nType == k_EGameIDTypeGameMod );
	}

	bool IsShortcut( ) const
	{
		return ( m_gameID.m_nType == k_EGameIDTypeShortcut );
	}

	bool IsP2PFile( ) const
	{
		return ( m_gameID.m_nType == k_EGameIDTypeP2P );
	}

	bool IsSteamApp( ) const
	{
		return ( m_gameID.m_nType == k_EGameIDTypeApp );
	}

	uint32_t ModID( ) const
	{
		return m_gameID.m_nModID;
	}

	uint32_t AppID( ) const
	{
		return m_gameID.m_nAppID;
	}

	bool operator == ( const CGameID& rhs ) const
	{
		return m_ulGameID == rhs.m_ulGameID;
	}

	bool operator != ( const CGameID& rhs ) const
	{
		return !( *this == rhs );
	}

	bool operator < ( const CGameID& rhs ) const
	{
		return ( m_ulGameID < rhs.m_ulGameID );
	}

	bool IsValid( ) const
	{
		// each type has it's own invalid fixed point:
		switch ( m_gameID.m_nType )
		{
		case k_EGameIDTypeApp:
			return m_gameID.m_nAppID != k_uAppIdInvalid;
			break;
		case k_EGameIDTypeGameMod:
			return m_gameID.m_nAppID != k_uAppIdInvalid && m_gameID.m_nModID & 0x80000000;
			break;
		case k_EGameIDTypeShortcut:
			return ( m_gameID.m_nModID & 0x80000000 ) != 0;
			break;
		case k_EGameIDTypeP2P:
			return m_gameID.m_nAppID == k_uAppIdInvalid && m_gameID.m_nModID & 0x80000000;
			break;
		default:
#if defined(Assert)
			Assert( false );
#endif
			return false;
		}

	}

	void Reset( )
	{
		m_ulGameID = 0;
	}



private:

	enum EGameIDType
	{
		k_EGameIDTypeApp = 0,
		k_EGameIDTypeGameMod = 1,
		k_EGameIDTypeShortcut = 2,
		k_EGameIDTypeP2P = 3,
	};

	struct GameID_t
	{
#ifdef VALVE_BIG_ENDIAN
		unsigned int m_nModID : 32;
		unsigned int m_nType : 8;
		unsigned int m_nAppID : 24;
#else
		unsigned int m_nAppID : 24;
		unsigned int m_nType : 8;
		unsigned int m_nModID : 32;
#endif
	};

	union
	{
		uint64_t m_ulGameID;
		GameID_t m_gameID;
	};
};

enum EFriendRelationship
{
	k_EFriendRelationshipNone = 0,
	k_EFriendRelationshipBlocked = 1,			// this doesn't get stored; the user has just done an Ignore on an friendship invite
	k_EFriendRelationshipRequestRecipient = 2,
	k_EFriendRelationshipFriend = 3,
	k_EFriendRelationshipRequestInitiator = 4,
	k_EFriendRelationshipIgnored = 5,			// this is stored; the user has explicit blocked this other user from comments/chat/etc
	k_EFriendRelationshipIgnoredFriend = 6,
	k_EFriendRelationshipSuggested = 7,

	// keep this updated
	k_EFriendRelationshipMax = 8,
};

// maximum length of friend group name (not including terminating nul!)
const int k_cchMaxFriendsGroupName = 64;

// maximum number of groups a single user is allowed
const int k_cFriendsGroupLimit = 100;

// friends group identifier type
typedef int16_t FriendsGroupID_t;

// invalid friends group identifier constant
const FriendsGroupID_t k_FriendsGroupID_Invalid = -1;

const int k_cEnumerateFollowersMax = 50;


//-----------------------------------------------------------------------------
// Purpose: list of states a friend can be in
//-----------------------------------------------------------------------------
enum EPersonaState
{
	k_EPersonaStateOffline = 0,			// friend is not currently logged on
	k_EPersonaStateOnline = 1,			// friend is logged on
	k_EPersonaStateBusy = 2,			// user is on, but busy
	k_EPersonaStateAway = 3,			// auto-away feature
	k_EPersonaStateSnooze = 4,			// auto-away for a long time
	k_EPersonaStateLookingToTrade = 5,	// Online, trading
	k_EPersonaStateLookingToPlay = 6,	// Online, wanting to play
	k_EPersonaStateMax,
};


//-----------------------------------------------------------------------------
// Purpose: flags for enumerating friends list, or quickly checking a the relationship between users
//-----------------------------------------------------------------------------
enum EFriendFlags
{
	k_EFriendFlagNone = 0x00,
	k_EFriendFlagBlocked = 0x01,
	k_EFriendFlagFriendshipRequested = 0x02,
	k_EFriendFlagImmediate = 0x04,			// "regular" friend
	k_EFriendFlagClanMember = 0x08,
	k_EFriendFlagOnGameServer = 0x10,
	// k_EFriendFlagHasPlayedWith	= 0x20,	// not currently used
	// k_EFriendFlagFriendOfFriend	= 0x40, // not currently used
	k_EFriendFlagRequestingFriendship = 0x80,
	k_EFriendFlagRequestingInfo = 0x100,
	k_EFriendFlagIgnored = 0x200,
	k_EFriendFlagIgnoredFriend = 0x400,
	k_EFriendFlagSuggested = 0x800,
	k_EFriendFlagChatMember = 0x1000,
	k_EFriendFlagAll = 0xFFFF,
};

typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
//typedef uint64_t uint64;
typedef uint64_t CSteamID;


struct FriendGameInfo_t
{
	CGameID m_gameID;
	uint32 m_unGameIP;
	uint16 m_usGamePort;
	uint16 m_usQueryPort;
	CSteamID m_steamIDLobby;
};

// maximum number of characters in a user's name. Two flavors; one for UTF-8 and one for UTF-16.
// The UTF-8 version has to be very generous to accomodate characters that get large when encoded
// in UTF-8.
enum
{
	k_cchPersonaNameMax = 128,
	k_cwchPersonaNameMax = 32,
};

//-----------------------------------------------------------------------------
// Purpose: user restriction flags
//-----------------------------------------------------------------------------
enum EUserRestriction
{
	k_nUserRestrictionNone = 0,	// no known chat/content restriction
	k_nUserRestrictionUnknown = 1,	// we don't know yet (user offline)
	k_nUserRestrictionAnyChat = 2,	// user is not allowed to (or can't) send/recv any chat
	k_nUserRestrictionVoiceChat = 4,	// user is not allowed to (or can't) send/recv voice chat
	k_nUserRestrictionGroupChat = 8,	// user is not allowed to (or can't) send/recv group chat
	k_nUserRestrictionRating = 16,	// user is too young according to rating in current region
	k_nUserRestrictionGameInvites = 32,	// user cannot send or recv game invites (e.g. mobile)
	k_nUserRestrictionTrading = 64,	// user cannot participate in trading (console, mobile)
};

//-----------------------------------------------------------------------------
// Purpose: information about user sessions
//-----------------------------------------------------------------------------
struct FriendSessionStateInfo_t
{
	uint32 m_uiOnlineSessionInstances;
	uint8 m_uiPublishedToFriendsSessionInstance;
};



// size limit on chat room or member metadata
const uint32 k_cubChatMetadataMax = 8192;

// size limits on Rich Presence data
enum { k_cchMaxRichPresenceKeys = 20 };
enum { k_cchMaxRichPresenceKeyLength = 64 };
enum { k_cchMaxRichPresenceValueLength = 256 };

// These values are passed as parameters to the store
enum EOverlayToStoreFlag
{
	k_EOverlayToStoreFlag_None = 0,
	k_EOverlayToStoreFlag_AddToCart = 1,
	k_EOverlayToStoreFlag_AddToCartAndShow = 2,
};

//-----------------------------------------------------------------------------
// Purpose: interface to accessing information about individual users,
//			that can be a friend, in a group, on a game server or in a lobby with the local user
//-----------------------------------------------------------------------------
class ISteamFriends
{
public:
	// returns the local players name - guaranteed to not be NULL.
	// this is the same name as on the users community profile page
	// this is stored in UTF-8 format
	// like all the other interface functions that return a char *, it's important that this pointer is not saved
	// off; it will eventually be free'd or re-allocated
	virtual const char* GetPersonaName( ) = 0;

	// Sets the player name, stores it on the server and publishes the changes to all friends who are online.
	// Changes take place locally immediately, and a PersonaStateChange_t is posted, presuming success.
	//
	// The final results are available through the return value SteamAPICall_t, using SetPersonaNameResponse_t.
	//
	// If the name change fails to happen on the server, then an additional global PersonaStateChange_t will be posted
	// to change the name back, in addition to the SetPersonaNameResponse_t callback.
	virtual int SetPersonaName( const char* pchPersonaName ) = 0;

	// gets the status of the current user
	virtual EPersonaState GetPersonaState( ) = 0;

	// friend iteration
	// takes a set of k_EFriendFlags, and returns the number of users the client knows about who meet that criteria
	// then GetFriendByIndex() can then be used to return the id's of each of those users
	virtual int GetFriendCount( int iFriendFlags ) = 0;

	// returns the steamID of a user
	// iFriend is a index of range [0, GetFriendCount())
	// iFriendsFlags must be the same value as used in GetFriendCount()
	// the returned CSteamID can then be used by all the functions below to access details about the user
	virtual CSteamID GetFriendByIndex( int iFriend, int iFriendFlags ) = 0;

	// returns a relationship to a user
	virtual EFriendRelationship GetFriendRelationship( CSteamID steamIDFriend ) = 0;

	// returns the current status of the specified user
	// this will only be known by the local user if steamIDFriend is in their friends list; on the same game server; in a chat room or lobby; or in a small group with the local user
	virtual EPersonaState GetFriendPersonaState( CSteamID steamIDFriend ) = 0;

	// returns the name another user - guaranteed to not be NULL.
	// same rules as GetFriendPersonaState() apply as to whether or not the user knowns the name of the other user
	// note that on first joining a lobby, chat room or game server the local user will not known the name of the other users automatically; that information will arrive asyncronously
	// 
	virtual const char* GetFriendPersonaName( CSteamID steamIDFriend ) = 0;

	// returns true if the friend is actually in a game, and fills in pFriendGameInfo with an extra details 
	virtual bool GetFriendGamePlayed( void ) = 0;
	// accesses old friends names - returns an empty string when their are no more items in the history
	virtual const char* GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName ) = 0;
	// friends steam level
	virtual int GetFriendSteamLevel( CSteamID steamIDFriend ) = 0;

	// Returns nickname the current user has set for the specified player. Returns NULL if the no nickname has been set for that player.
	virtual const char* GetPlayerNickname( CSteamID steamIDPlayer ) = 0;

	// friend grouping (tag) apis
	// returns the number of friends groups
	virtual int GetFriendsGroupCount( ) = 0;
	// returns the friends group ID for the given index (invalid indices return k_FriendsGroupID_Invalid)
	virtual FriendsGroupID_t GetFriendsGroupIDByIndex( int iFG ) = 0;
	// returns the name for the given friends group (NULL in the case of invalid friends group IDs)
	virtual const char* GetFriendsGroupName( FriendsGroupID_t friendsGroupID ) = 0;
	// returns the number of members in a given friends group
	virtual int GetFriendsGroupMembersCount( FriendsGroupID_t friendsGroupID ) = 0;
	// gets up to nMembersCount members of the given friends group, if fewer exist than requested those positions' SteamIDs will be invalid
	virtual void GetFriendsGroupMembersList( void ) = 0;

	// returns true if the specified user meets any of the criteria specified in iFriendFlags
	// iFriendFlags can be the union (binary or, |) of one or more k_EFriendFlags values
	virtual bool HasFriend( CSteamID steamIDFriend, int iFriendFlags ) = 0; // 17

	// clan (group) iteration and access functions
	virtual int GetClanCount( ) = 0;
	virtual CSteamID GetClanByIndex( int iClan ) = 0;
	virtual const char* GetClanName( CSteamID steamIDClan ) = 0;
	virtual const char* GetClanTag( CSteamID steamIDClan ) = 0;
	// returns the most recent information we have about what's happening in a clan
	virtual bool GetClanActivityCounts( CSteamID steamIDClan, int* pnOnline, int* pnInGame, int* pnChatting ) = 0;
	// for clans a user is a member of, they will have reasonably up-to-date information, but for others you'll have to download the info to have the latest
	virtual int DownloadClanActivityCounts( void ) = 0;

	// iterators for getting users in a chat room, lobby, game server or clan
	// note that large clans that cannot be iterated by the local user
	// note that the current user must be in a lobby to retrieve CSteamIDs of other users in that lobby
	// steamIDSource can be the steamID of a group, game server, lobby or chat room
	virtual int GetFriendCountFromSource( CSteamID steamIDSource ) = 0;
	virtual CSteamID GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend ) = 0;

	// returns true if the local user can see that steamIDUser is a member or in steamIDSource
	virtual bool IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource ) = 0;

	// User is in a game pressing the talk button (will suppress the microphone for all voice comms from the Steam friends UI)
	virtual void SetInGameVoiceSpeaking( CSteamID steamIDUser, bool bSpeaking ) = 0;

	// activates the game overlay, with an optional dialog to open 
	// valid options are "Friends", "Community", "Players", "Settings", "OfficialGameGroup", "Stats", "Achievements"
	virtual void ActivateGameOverlay( const char* pchDialog ) = 0;

	// activates game overlay to a specific place
	// valid options are
	//		"steamid" - opens the overlay web browser to the specified user or groups profile
	//		"chat" - opens a chat window to the specified user, or joins the group chat 
	//		"jointrade" - opens a window to a Steam Trading session that was started with the ISteamEconomy/StartTrade Web API
	//		"stats" - opens the overlay web browser to the specified user's stats
	//		"achievements" - opens the overlay web browser to the specified user's achievements
	//		"friendadd" - opens the overlay in minimal mode prompting the user to add the target user as a friend
	//		"friendremove" - opens the overlay in minimal mode prompting the user to remove the target friend
	//		"friendrequestaccept" - opens the overlay in minimal mode prompting the user to accept an incoming friend invite
	//		"friendrequestignore" - opens the overlay in minimal mode prompting the user to ignore an incoming friend invite
	virtual void ActivateGameOverlayToUser( const char* pchDialog, CSteamID steamID ) = 0;

	// activates game overlay web browser directly to the specified URL
	// full address with protocol type is required, e.g. http://www.steamgames.com/
	virtual void ActivateGameOverlayToWebPage( const char* pchURL ) = 0;

	// activates game overlay to store page for app
	virtual void ActivateGameOverlayToStore( uint32 nAppID, EOverlayToStoreFlag eFlag ) = 0;

	// Mark a target user as 'played with'. This is a client-side only feature that requires that the calling user is 
	// in game 
	virtual void SetPlayedWith( CSteamID steamIDUserPlayedWith ) = 0;

	// activates game overlay to open the invite dialog. Invitations will be sent for the provided lobby.
	virtual void ActivateGameOverlayInviteDialog( CSteamID steamIDLobby ) = 0;

	// gets the small (32x32) avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	virtual int GetSmallFriendAvatar( CSteamID steamIDFriend ) = 0;

	// gets the medium (64x64) avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	virtual int GetMediumFriendAvatar( CSteamID steamIDFriend ) = 0;

	// gets the large (184x184) avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	// returns -1 if this image has yet to be loaded, in this case wait for a AvatarImageLoaded_t callback and then call this again
	virtual int GetLargeFriendAvatar( CSteamID steamIDFriend ) = 0;

	// requests information about a user - persona name & avatar
	// if bRequireNameOnly is set, then the avatar of a user isn't downloaded 
	// - it's a lot slower to download avatars and churns the local cache, so if you don't need avatars, don't request them
	// if returns true, it means that data is being requested, and a PersonaStateChanged_t callback will be posted when it's retrieved
	// if returns false, it means that we already have all the details about that user, and functions can be called immediately
	virtual bool RequestUserInformation( CSteamID steamIDUser, bool bRequireNameOnly ) = 0;
	virtual int RequestClanOfficerList( CSteamID steamIDClan ) = 0;

	// iteration of clan officers - can only be done when a RequestClanOfficerList() call has completed

	// returns the steamID of the clan owner
	virtual CSteamID GetClanOwner( CSteamID steamIDClan ) = 0;
	// returns the number of officers in a clan (including the owner)
	virtual int GetClanOfficerCount( CSteamID steamIDClan ) = 0;
	// returns the steamID of a clan officer, by index, of range [0,GetClanOfficerCount)
	virtual CSteamID GetClanOfficerByIndex( CSteamID steamIDClan, int iOfficer ) = 0;
	virtual uint32 GetUserRestrictions( ) = 0;
	virtual bool SetRichPresence( const char* pchKey, const char* pchValue ) = 0; // 43
	virtual void ClearRichPresence( ) = 0;
	virtual const char* GetFriendRichPresence( CSteamID steamIDFriend, const char* pchKey ) = 0;
	virtual int GetFriendRichPresenceKeyCount( CSteamID steamIDFriend ) = 0;
	virtual const char* GetFriendRichPresenceKeyByIndex( CSteamID steamIDFriend, int iKey ) = 0;
	virtual void RequestFriendRichPresence( CSteamID steamIDFriend ) = 0;
	virtual bool InviteUserToGame( CSteamID steamIDFriend, const char* pchConnectString ) = 0; // 49

	// recently-played-with friends iteration
	// this iterates the entire list of users recently played with, across games
	// GetFriendCoplayTime() returns as a unix time
	virtual int GetCoplayFriendCount( ) = 0;
	virtual CSteamID GetCoplayFriend( int iCoplayFriend ) = 0;
	virtual int GetFriendCoplayTime( CSteamID steamIDFriend ) = 0;
	virtual uint32 GetFriendCoplayGame( CSteamID steamIDFriend ) = 0;

	// chat interface for games
	// this allows in-game access to group (clan) chats from in the game
	// the behavior is somewhat sophisticated, because the user may or may not be already in the group chat from outside the game or in the overlay
	// use ActivateGameOverlayToUser( "chat", steamIDClan ) to open the in-game overlay version of the chat
	virtual int JoinClanChatRoom( CSteamID steamIDClan ) = 0;
	virtual bool LeaveClanChatRoom( CSteamID steamIDClan ) = 0;
	virtual int GetClanChatMemberCount( CSteamID steamIDClan ) = 0;
	virtual CSteamID GetChatMemberByIndex( CSteamID steamIDClan, int iUser ) = 0;
	virtual bool SendClanChatMessage( CSteamID steamIDClanChat, const char* pchText ) = 0;
	virtual int GetClanChatMessage( CSteamID steamIDClanChat, int iMessage, void* prgchText, int cchTextMax, void* peChatEntryType, CSteamID* psteamidChatter ) = 0;
	virtual bool IsClanChatAdmin( CSteamID steamIDClanChat, CSteamID steamIDUser ) = 0;

	// interact with the Steam (game overlay / desktop)
	virtual bool IsClanChatWindowOpenInSteam( CSteamID steamIDClanChat ) = 0;
	virtual bool OpenClanChatWindowInSteam( CSteamID steamIDClanChat ) = 0;
	virtual bool CloseClanChatWindowInSteam( CSteamID steamIDClanChat ) = 0;

	// peer-to-peer chat interception
	// this is so you can show P2P chats inline in the game
	virtual bool SetListenForFriendsMessages( bool bInterceptEnabled ) = 0;
	virtual bool ReplyToFriendMessage( CSteamID steamIDFriend, const char* pchMsgToSend ) = 0;
	virtual int GetFriendMessage( CSteamID steamIDFriend, int iMessageID, void* pvData, int cubData, void* peChatEntryType ) = 0;
};