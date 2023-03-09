#pragma once
#include <FECore/FEPlotData.h>
#include <FECore/FEElement.h>
#include <FECore/FEDataStream.h>
#include <FEBioMech/FEContinuousElasticDamage.h>
//-----------------------------------------------------------------------------
//! Damage reduction factor
class FEPlotContinuousDamagePlugin : public FEPlotDomainData
{
public:
	FEPlotContinuousDamagePlugin(FEModel* fem);
	bool Save(FEDomain& dom, FEDataStream& a) override;
	bool SetFilter(const char* sz) override;
private:
	std::string	m_prop;
	int			m_propIndex;
	int			m_comp;
};