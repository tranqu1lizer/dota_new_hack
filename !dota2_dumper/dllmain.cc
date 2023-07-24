#include "sdk.hh"
#include <format>
#include <optional>

void dump( ) {
	constexpr const auto mod = "server.dll";
	constexpr const auto cls = "CDOTAGameRules";

	const auto module_scope = CSchemaSystem::GetInstance( ).FindTypeScopeForModule( mod );

	if ( !module_scope )
		return;

	const auto class_info = module_scope->FindDeclaredClass( cls );

	if ( !class_info )
		return;

	char buf[256];
	if ( class_info->m_schema_parent )
		sprintf_s( buf, "struct %s : public %s // size: 0x%X\n{", class_info->m_name, class_info->m_schema_parent->m_class->m_name, class_info->m_size );
	else
		sprintf_s( buf, "struct %s // size: 0x%X\n{", class_info->m_name, class_info->m_size );
	std::cout << buf << "\n";

	for ( auto k = 0; k < class_info->m_align; k++ ) {
		const auto& field = &class_info->m_fields[k];
		int sz;

		if ( !field || field->m_single_inheritance_offset < 0 )
			continue;

		if ( !field->m_type->GetSize( &sz ) || !sz )
			continue;

		const auto str = std::format( "    char {}[{:#x}]; // {:#x};\n", field->m_name, sz, field->m_single_inheritance_offset );
		std::cout << str;
	}
	std::cout << "};";
}

bool __stdcall DllMain( HINSTANCE hModule, std::uint8_t reason, void* ) {
	if ( reason == 1 ) {
		util::allocate_console( );
		setlocale( 0, "" );

		dump( );
	}
	return reason;
}