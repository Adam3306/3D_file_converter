#pragma once
#include <vector>
#include <glm.hpp>
#include <string>

class MeshFileConverter
{
public:
	class ConversionStats
	{
	public:
		enum eConversionStat
		{
			CONVERSIONSTAT_CALC_SURFACE,
			CONVERSIONSTAT_CALC_VOLUME,
			CONVERSIONSTAT_CALC_3DPOINT_INSIDE,
		};

		struct sStat
		{
			eConversionStat m_Stat;
			std::string m_StatHint;
		};

		virtual std::string Parse(const std::string& str) = 0;

		std::vector<sStat>	m_Stats;
		std::string			m_Result;
	};

	class ConversionParams
	{
	public:
		enum eConversionParams
		{
			CONVERSIONPARAM_TRANSFORM_TRANSLATE_XYZ,
			CONVERSIONPARAM_TRANSFORM_ROTATE_XYZ,
			CONVERSIONPARAM_TRANSFORM_SCALE_XYZ,
		};

		struct sParam
		{
			eConversionParams	m_Param;
			std::string			m_ParamValaue;
		};
		

		virtual std::string Parse(const std::string& str) = 0;

		std::vector<sParam> m_Params;

	};

	virtual std::string GetDestExtension() = 0;
	virtual std::string GetSrcExtension() = 0;
	virtual std::string Convert(const std::string& filepath, const std::string& stats, const std::string& conversionparams) = 0;
};
