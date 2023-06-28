#include <iostream>

#include "CEconItem.hpp"

void CEconItem::EnsureCustomDataExists( )
{
	if ( m_pCustomData == NULL )
	{
		m_pCustomData = new CEconItemCustomData( );

		if ( m_dirtyBits.m_bHasEquipSingleton )
		{
			m_pCustomData->m_vecEquipped.add_to_tail( m_EquipInstanceSingleton );
			m_EquipInstanceSingleton = EquippedInstance_t( );
			m_dirtyBits.m_bHasEquipSingleton = false;
		}
		if ( m_dirtyBits.m_bHasAttribSingleton )
		{
			m_pCustomData->m_vecAttributes.add_to_tail( m_CustomAttribSingleton );
			m_dirtyBits.m_bHasAttribSingleton = false;
		}
	}
}

void CEconItem::UnequipFromClass( equipped_class_t unClass ) noexcept
{
    if ( m_dirtyBits.m_bHasEquipSingleton )
    {
        if ( m_EquipInstanceSingleton.m_unEquippedClass == unClass )
        {
            m_dirtyBits.m_bHasEquipSingleton = false;
            m_EquipInstanceSingleton.m_unEquippedClass = 0;
            m_EquipInstanceSingleton.m_unEquippedSlot = std::numeric_limits<std::uint16_t>::quiet_NaN();
        }
    }
    else if ( m_pCustomData )
    {
        for ( int i = 0; i < m_pCustomData->m_vecEquipped.Count( ); i++ )
        {
            if ( m_pCustomData->m_vecEquipped.Element( i ).m_unEquippedClass == unClass )
            {
               // m_pCustomData->m_vecEquipped.FastRemove( i );
                break;
            }
        }
    }
}


void CEconItem::Equip( equipped_class_t unClass, equipped_slot_t unSlot )
{
    UnequipFromClass( unClass );

    if ( m_pCustomData )
    {
        m_pCustomData->m_vecEquipped.add_to_tail( { unClass, unSlot } );
    }
    else if ( !m_dirtyBits.m_bHasEquipSingleton )
    {
        m_EquipInstanceSingleton = { unClass, unSlot };
        m_dirtyBits.m_bHasEquipSingleton = true;
    }
    else
    {
        EnsureCustomDataExists( );
        m_pCustomData->m_vecEquipped.add_to_tail( { unClass, unSlot } );
    }
}