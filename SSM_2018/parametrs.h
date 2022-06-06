#pragma once

class Parametrs
{
public:
	int crops;
	QString func_file_name;
	QString h5_file_name;
	QString h5_table_name;
	QString crops_ini_file;

	int nF;
	int wL;
	int nD;
	int rT;
	int ecovar;
	int print_trace;
	int seed;
	int dividing_dataset;

	int function_mode;// 0 - soltani, 1 - opt
	int optimization_mode;//0- 1- 2

	QString sName;//Name of site or scenario. This name will appear in output
	double Latitude;//Latitude of location. NOTE: south latitude should be NEGETIVE
	double VPDF;//Vapor pressure deficit factor.
	int FixFind; // 1=fixed sowing date, 2=first possible sowing date based on pre-defined conditions.
	double SearchDur; // Specifies the end of sowing window
	double SowWat; // Amount of water in the soil that makes sowing possible (mm)
	double PDEN; // Plant density (# per m2)
	int water; //  1= irrigated 2 = rainfed
	double IRGLVL; // Irrigation level based on FTSW if the farming is irrigated.
	double SOLDEP; // Soil depth (mm)
	double DEP1; // Depth of top layer (mm)
	double SALB; // Soil albedo
	double CN2; // Soil curve number.
	double DRAINF; // Drainage factor
	double SAT; // Soil saturation limit (m3 m-3 or mm mm-1).
	double DUL;// Soil drained upper limit (m3 m-3 or mm mm-1).
	double EXTR; // Soil extractable moisture (m3 m-3 or mm mm-1).
	int MAI1; // Soil moisture availabilty index for top layer. 1=DUL, 0=LL.
	int MAI;// Soil moisture availability index (1=DUL, 0=LL).
	int INSOL; // Initial soil nitrogen that can be uptaken by the crop before BNF is activated (g N m-2)
	double U;// Cumulative evaporation that stage I terminates and stage II starts. Not used by the model (inactive)
	int ROW;
	bool file_mode;// true - with dl or false - without dl
	Parametrs() {}
	void Print()
	{
		std::cout << "Latitude " << Latitude << std::endl;
		std::cout << "VPDF " << VPDF << std::endl;
		std::cout << "FixFind " << FixFind << std::endl;
		std::cout << "SearchDur " << SearchDur << std::endl;
		std::cout << "SowWat " << SowWat << std::endl;
		std::cout << "PDEN " << PDEN << std::endl;
		std::cout << "water " << water << std::endl;
		std::cout << "IRGLVL " << IRGLVL << std::endl;
		std::cout << "SOLDEP " << SOLDEP << std::endl;
		std::cout << "DEP1 " << DEP1 << std::endl;
		std::cout << "SALB " << SALB << std::endl;
		std::cout << "CN2 " << CN2 << std::endl;
		std::cout << "DRAINF " << DRAINF << std::endl;
		std::cout << "SAT " << SAT << std::endl;
		std::cout << "DUL " << DUL << std::endl;
		std::cout << "EXTR " << EXTR << std::endl;
		std::cout << "MAI1 " << MAI1 << std::endl;
		std::cout << "MAI " << MAI << std::endl;
		std::cout << "INSOL " << INSOL << std::endl;
		std::cout << "U " << U << std::endl;
		std::cout << "file_mode " << file_mode << endl;
		std::cout << "print_trace " << print_trace << endl;
		std::cout << "optimization_mode " << optimization_mode << endl;
		std::cout << "nF " << nF << endl;
		std::cout << "nD " << nD << endl;
		std::cout << "wL " << wL << endl;
		std::cout << "rT " << rT << endl;
		std::cout << "function_mode " << function_mode << endl;
		std::cout << "func_file_name " << func_file_name.toStdString() << endl;
		std::cout << "h5_file_name " << h5_file_name.toStdString() << endl;
		std::cout << "h5_table_name " << h5_table_name.toStdString() << endl;
		std::cout << "crops_ini_file " << crops_ini_file.toStdString() << endl;
	}
};
