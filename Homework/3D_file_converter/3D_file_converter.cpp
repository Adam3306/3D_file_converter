#include "pch.h"
#include <iostream>
#include "MeshFileConverterFactory.h"
#include "MeshFileConverter_OBJ_STL.h"

int main()
{
	// Instanciate the general 3D file converter
	MeshFileConverterFactory mfcf;

	// Register one concrete implementation for an OBJ reader and an STL writer.
	mfcf.RegisterConverter(MeshFileConverter_OBJ_STL::GetSingleton());
	
	// Samples
	//mfcf.ConvertFile("cube1.obj", ".stl", "CALC_VOLUME CALC_SURFACE", "TRANSFORM_ROTATE_XYZ TRANSFORM_ROTATE_XYZ");
	//mfcf.ConvertFile("tetrahedron.obj", ".stl", "CALC_VOLUME CALC_SURFACE", "");
	
	std::cout << "Welcome!" << std::endl;
	while (1)
	{
		// TODO: check input is valid
		std::string outFormat;
		std::string fName;
		std::string translations;
		std::string calculations;
		
		std::cout << "First type the name of your 3D file with extension: ";
		std::cin >> fName;
		std::cout << std::endl;
		std::cout << "Type the destination extension (e.g.: .stl): ";
		std::cin >> outFormat;
		std::cout << std::endl;
		std::cout << "Type the calculations you want to count out (You can write more than one just separate it with spaces): " << std::endl;
		std::cout << "<CALC_VOLUME | CALC_SURFACE | CALC_3DPOINT_INSIDE>" << std::endl;
		std::cin.ignore();
		std::getline(std::cin, calculations);
		std::cout << std::endl;
		std::cout << "Type the translations you want do (You can write more than one just separate it with spaces): " << std::endl;
		std::cout << "<ROTATE | TRANSLATE | SCALE>" << std::endl;

		std::getline(std::cin, translations);
		std::cout << std::endl;

		mfcf.ConvertFile(fName, outFormat, calculations, translations);

		char choice[16];
		std::cout << "Do you want to quit? (yes/no)" <<  std::endl;
		std::cin >> choice;
		if (strcmp(choice, "yes") == 0) break;
	}
	return 0;
}
