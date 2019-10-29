#include "pch.h"
#include <iostream>
#include "MeshFileConverterFactory.h"

void MeshFileConverterFactory::ConvertFile(const std::string& src, const std::string& targetextension, const std::string& stats, const std::string& conversionparams)
{
	MeshFileConverter* converter_found = nullptr;

	try
	{
		if (targetextension.length() == 0)
		{
			throw std::exception(std::string("No target extension specified for file: " + src).c_str());
		}

		if (ExtraValidationLayer::IsFilenameValid(src) == false)
		{
			throw std::exception(std::string("File name is not valid for file: " + src).c_str());
		}

		for (size_t ci = 0; ci < m_Converters.size(); ci++)
		{
			MeshFileConverter* converter = m_Converters[ci];
			std::string srcextension = converter->GetSrcExtension();

			std::string src_extension = src.substr(src.length() - srcextension.length(), srcextension.length());

			if (src_extension.compare(srcextension) == 0 &&
				converter->GetDestExtension().compare(targetextension) == 0)
			{
				converter_found = converter;
				break;
			}
		}

		if (converter_found == nullptr)
		{
			// log error event
			throw std::exception(std::string("Converter not found for file: " + src).c_str());
		}

		std::string errors = converter_found->Convert(src, stats, conversionparams);
		if (errors.length() > 0)
		{
			throw std::exception(errors.c_str());
		}
	}
	catch (const std::exception& exp)
	{
		std::string what = exp.what();
		std::ofstream fstream(src + ".log_errors");
		fstream << what;
		std::cout << "ERROR OCCURED: " << what << std::endl;
	}
	catch (...)
	{
		//unknown exception
	}
}

void MeshFileConverterFactory::RegisterConverter(MeshFileConverter* converter)
{
	m_Converters.push_back(converter);
}