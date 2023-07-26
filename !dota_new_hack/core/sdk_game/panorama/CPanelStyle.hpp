#include "../NormalClass.hpp"

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
};

class CPanelStyle : VClass {
public:
	bool GetVisibility( bool* out ) {
		return CallVFunc<88, bool>( out );
	}
	
	void SetVisibility( bool v ) {
		CallVFunc<89, void>( v );
	}
};