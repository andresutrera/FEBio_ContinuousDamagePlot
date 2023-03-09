#include <FEBioMech/FESolidMaterial.h>
#include <FECore/FEDomain.h>
#include <FECore/writeplot.h>
#include <FECore/FEDataStream.h>
#include <iostream>
#include <FEBioMech/FEContinuousElasticDamage.h>
#include "FEPlotContinuousDamagePlugin.h"
#include <FEBioMech/FEElasticMixture.h>
//-----------------------------------------------------------------------------
FEPlotContinuousDamagePlugin::FEPlotContinuousDamagePlugin(FEModel* fem) : FEPlotDomainData(fem, PLT_FLOAT, FMT_ITEM)
{
	m_propIndex = 0;
}

//-----------------------------------------------------------------------------

bool FEPlotContinuousDamagePlugin::SetFilter(const char* sz)
{
	m_prop = sz;
	return true;
}

bool FEPlotContinuousDamagePlugin::Save(FEDomain& dom, FEDataStream& a)
{
	// get the material
	FEMaterial* domMat = dom.GetMaterial();
	if (domMat == nullptr) return false;

	// get the fiber damage component
	FEDamageElasticFiber* mat = nullptr;
	if (m_prop.empty()) mat = dynamic_cast<FEDamageElasticFiber*>(domMat);
	else
	{
		ParamString ps(m_prop.c_str());

		//getting the last index from the filter ParamString
		if(ps.count() <= 1) m_propIndex = ps.Index();
		//ps.last() only works if there is more than one element in the array
		else m_propIndex = ps.last().Index();
		//m_propIndex = ps.Index();

		mat = dynamic_cast<FEDamageElasticFiber*>(domMat->GetProperty(ps));
	}
	if (mat == nullptr) return false;

	int NE = dom.Elements();
	for (int i = 0; i < NE; ++i)
	{
		FEElement& el = dom.ElementRef(i);

		double D = 0.0;
		int nint = el.GaussPoints();
		for (int j = 0; j < nint; ++j)
		{
			FEMaterialPoint& mp = *el.GetMaterialPoint(j);
			//This was taken from the damage plot class. it seems that "GetPointData" doesnt work if the material point comes from another class. e.g PreStrainElastic
			FEElasticMixtureMaterialPoint* mmp = mp.ExtractData<FEElasticMixtureMaterialPoint>();
			FEMaterialPoint* pt = nullptr;
			if (mmp)
			{
				pt = mmp->GetPointData(m_propIndex);
			} else
			{
				pt = mp.GetPointData(m_propIndex);
			}
			//FEMaterialPoint* pt = mp.GetPointData(m_propIndex);
			double Dj = mat->Damage(*pt, 0);

			D += Dj;
		}
		D /= (double)nint;

		a << D;
	}

	return true;
}