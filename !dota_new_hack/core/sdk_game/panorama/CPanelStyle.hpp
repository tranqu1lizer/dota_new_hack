class CStyleProperty;

class CStyleSymbol;
class CUILength;

class CStyleFactoryWrapper {
public:
	virtual bool BRegisteredProperty( CStyleSymbol symName ) = 0;
	virtual CStyleSymbol BRegisteredAlias( CStyleSymbol symName ) = 0;
	virtual CStyleSymbol GetPropertyNameForAlias( unsigned __int8* a2, unsigned __int8* a3 ) = 0;
	virtual CStyleProperty* CreateStyleProperty( CStyleSymbol symName ) = 0;
	virtual void FreeStyleProperty( CStyleProperty* pProperty ) = 0;
	// virtual const CUtlVector< CUtlString >& GetSortedPropertyAndAliasNames( ) = 0;
};

class CPanelStyle {
public:
	virtual void Clear( bool idk ) = 0; // 0x0
	virtual void* GetUIScaleFactor( void ) = 0; // 0x8
	virtual bool BHasAnyStyleDataForProperty( CStyleSymbol hSymbolProperty ) = 0;  // 0x10
	virtual void GetPosition( CUILength& x, CUILength& y, CUILength& z, bool bIncludeUIScaleFactor = true ) = 0;// 0x18
	virtual void GetInterpolatedPosition( CUILength& x, CUILength& y, CUILength& z, bool bFinal, bool bIncludeUIScaleFactor = true ) = 0; // 0x20
	virtual void SetPosition( CUILength x, CUILength y, CUILength z, bool bPreScaledByUIScaleFactor = false ) = 0; // 0x28
	virtual void SetPositionWithoutTransition( CUILength x, CUILength y, CUILength z, bool bPreScaledByUIScaleFactor = false ) = 0; // 0x30
	virtual void func7( ) = 0; // 0x38
	virtual void func8( ) = 0; // 0x40
	virtual void func9( ) = 0; // 0x48
	virtual void func10( ) = 0; // 0x50
	virtual void func11( ) = 0; // 0x58
	virtual void func12( ) = 0; // 0x60
	virtual void func13( ) = 0; // 0x68
	virtual void func14( ) = 0; // 0x70
	virtual void func15( ) = 0; // 0x78
	virtual void func16( ) = 0; // 0x80
	virtual void func17( ) = 0; // 0x88
	virtual void func18( ) = 0; // 0x90
	virtual void func19( ) = 0; // 0x98
	virtual void func20( ) = 0; // 0xa0
	virtual void func21( ) = 0; // 0xa8
	virtual void SetOpacity( float a1 ) = 0; // 0xb0
	virtual void func23( ) = 0; // 0xb8
	virtual void func24( ) = 0; // 0xc0
	virtual void func25( ) = 0; // 0xc8
	virtual void func26( ) = 0; // 0xd0
	virtual void func27( ) = 0; // 0xd8
	virtual void func28( ) = 0; // 0xe0
	virtual void func29( ) = 0; // 0xe8
	virtual void func30( ) = 0; // 0xf0
	virtual void func31( ) = 0; // 0xf8
	virtual void func32( ) = 0; // 0x100
	virtual void func33( ) = 0; // 0x108
	virtual void func34( ) = 0; // 0x110
	virtual void func35( ) = 0; // 0x118
	virtual void func36( ) = 0; // 0x120
	virtual void func37( ) = 0; // 0x128
	virtual void func38( ) = 0; // 0x130
	virtual void func39( ) = 0; // 0x138
	virtual void func40( ) = 0; // 0x140
	virtual void func41( ) = 0; // 0x148
	virtual void func42( ) = 0; // 0x150
	virtual void func43( ) = 0; // 0x158
	virtual void func44( ) = 0; // 0x160
	virtual void func45( ) = 0; // 0x168
	virtual void func46( ) = 0; // 0x170
	virtual void func47( ) = 0; // 0x178
	virtual void func48( ) = 0; // 0x180
	virtual void func49( ) = 0; // 0x188
	virtual void SetBackgroundColor( const char* color ) = 0; // 0x190
	virtual void func51( ) = 0; // 0x198
	virtual void func52( ) = 0; // 0x1a0
	virtual void SetForegroundColor( const char* color ) = 0; // 0x1a8
	virtual void func54( ) = 0; // 0x1b0
	virtual void func55( ) = 0; // 0x1b8
	virtual void func56( ) = 0; // 0x1c0
	virtual void func57( ) = 0; // 0x1c8
	virtual void func58( ) = 0; // 0x1d0
	virtual void func59( ) = 0; // 0x1d8
	virtual void func60( ) = 0; // 0x1e0
	virtual void func61( ) = 0; // 0x1e8
	virtual void func62( ) = 0; // 0x1f0
	virtual void func63( ) = 0; // 0x1f8
	virtual void func64( ) = 0; // 0x200
	virtual void func65( ) = 0; // 0x208
	virtual void func66( ) = 0; // 0x210
	virtual void func67( ) = 0; // 0x218
	virtual void func68( ) = 0; // 0x220
	virtual void func69( ) = 0; // 0x228
	virtual void func70( ) = 0; // 0x230
	virtual void func71( ) = 0; // 0x238
	virtual void func72( ) = 0; // 0x240
	virtual void func73( ) = 0; // 0x248
	virtual void func74( ) = 0; // 0x250
	virtual void func75( ) = 0; // 0x258
	virtual void func76( ) = 0; // 0x260
	virtual void func77( ) = 0; // 0x268
	virtual void func78( ) = 0; // 0x270
	virtual void func79( ) = 0; // 0x278
	virtual void func80( ) = 0; // 0x280
	virtual void func81( ) = 0; // 0x288
	virtual void func82( ) = 0; // 0x290
	virtual void func83( ) = 0; // 0x298
	virtual void func84( ) = 0; // 0x2a0
	virtual void func85( ) = 0; // 0x2a8
	virtual void func86( ) = 0; // 0x2b0
	virtual void func87( ) = 0; // 0x2b8
	virtual bool GetVisiblity( bool* out ) = 0; // 0x2c0
	virtual void SetVisibility( bool vis ) = 0; // 0x2c8
};