#include "functions-grammar.h"
#include "data.h"
using namespace std;
using namespace HighFive;
#include <QSettings>

class Nlreg
{
public:
	Nlreg(QString func_file_name, std::vector<std::string> meas, std::vector<std::string> grn, int nF, int wl, int rF, int print_trace = 0, int crops = 0) : funcs_file_name(func_file_name), measurements(meas), gr_names(grn) {
		nFunctions = nF;
		wordLength = wl;
		num_of_climate_vars = measurements.size();
		num_of_gt_vars = gr_names.size();
		PRINT_TRACE = print_trace;
		read_flag = rF;
		read_genotype(crops);
	}
	double get_func_value(vector<double> clim_arg, vector<double> gt_vars)
	{
		double val = 0;
		GrammarNode *retFtn;
		for (size_t i = 0; i < nFunctions; ++i) {
			retFtn = grc->get_nth_tree(i);
			double fval = retFtn->eval(clim_arg);
			val += beta[i] * fval;

			for (size_t j = 0; j < num_of_gt_vars; ++j) {
				val += beta[i*num_of_gt_vars + j + nFunctions] * gt_vars[j] * fval;
			}
		}
		return val;
	}
	void delete_all()
	{
		delete[] phenotype;
		delete[] phenomask;
	}
	double get_func_value(vector<double> clim_arg)
	{
		double val = 0;
		GrammarNode *retFtn;
		for (size_t i = 0; i < nFunctions; ++i) {
			retFtn = grc->get_nth_tree(i);
			double fval = retFtn->eval(clim_arg);
			val += beta[i] * fval;
		}
		return val;
	}
	double get_l1_pen()
	{
		double val = 0;
		for (size_t i = 0; i < nFunctions; ++i) {
			val += (beta[i] > 0) ? beta[i] : -beta[i];
			for (size_t j = 0; j < num_of_gt_vars; ++j) {
				val += (beta[i*num_of_gt_vars + j + nFunctions] > 0) ? beta[i*num_of_gt_vars + j + nFunctions] : -beta[i*num_of_gt_vars + j + nFunctions];
			}
		}
		return val;
	}
	double get_l2_pen()
	{
		double val = 0;
		for (size_t i = 0; i < nFunctions; ++i) {
			val += beta[i] * beta[i];
			for (size_t j = 0; j < num_of_gt_vars; ++j) {
				val += beta[i*num_of_gt_vars + j + nFunctions] * beta[i*num_of_gt_vars + j + nFunctions];
			}
		}
		return val;
	}
	void nlreg_build()
	{
		GrammarNode *retFtn;
		grc = new GrammarContainer(measurements, nFunctions);
		phenotype = new double[nFunctions * wordLength];
		phenomask = new int[nFunctions * wordLength];
		int first = 0, last = wordLength;
		for (int i = 0; i < nFunctions; ++i) {
			std::vector<int> gt = std::vector<int>(genotype.begin() + first, genotype.begin() + last);
			first += wordLength;
			last += wordLength;
			grc->build_nth_tree(gt, climate_var, i, &phenotype[i * wordLength], &phenomask[i * wordLength]);
		}
	}
	void print_trace(std::string func_name, int arg) {
		std::ofstream out_func;
		out_func.open(func_name + "_" + "out_func.txt", std::ios::app);
		GrammarNode *retFtn;
		size_t i, j;
		if (arg == 0) {
			for (i = 0; i < nFunctions + nFunctions * num_of_gt_vars; ++i) {
				out_func << beta[i] << " ";
			}
			out_func << endl;
			out_func << "F= ";
			int flag = 0;
			for (i = 0; i < nFunctions + nFunctions * num_of_gt_vars; ++i) {
				if (fabs(beta[i]) > 0.0) {
					if (flag == 1 && beta[i] > 0) out_func << "+";
					out_func << beta[i];
					j = i;
					if (j < nFunctions) {
						out_func << "*";
						retFtn = grc->get_nth_tree(j);
						retFtn->coprint(out_func);
					}
					else if (j >= nFunctions) {
						int jj = j - nFunctions;
						int ii = jj / num_of_gt_vars;
						int kk = jj % num_of_gt_vars;
						out_func << "*" << gr_names[kk] << "*" << "f[" << ii << "]";
					}
					flag = 1;
				}
			}
			out_func << endl;
			for (size_t i = 0; i < nFunctions * wordLength; ++i) { // + nEcovar
				out_func << phenotype[i] << " ";
			}
			out_func << endl;
			for (size_t i = 0; i < nFunctions * wordLength; ++i) { // + nEcovar
				out_func << phenomask[i] << " ";
			}
			out_func << endl;
		}
		else {
			for (size_t i = 0; i < nFunctions; ++i) {
				retFtn = grc->get_nth_tree(i);
				out_func << i << " ";
				retFtn->coprint(out_func);
				out_func << endl;
			}
		}
		out_func << endl;
		out_func.close();
	}
	void set_genotype(vector<int>& gt)
	{
		genotype = gt;
	}
	void set_beta(vector<double>& be)
	{
		beta = be;
	}
	void set_beta_limit(double& arg)
	{
		MB = arg;
		for (size_t i = nFunctions; i < nFunctions + nFunctions * num_of_gt_vars; ++i) {
			double be = (beta[i] > 0) ? beta[i] : -beta[i];
			beta[i] = (be < MB) ? 0.0 : beta[i];
		}
	}
	void set_climate_vars(vector<double>& concs)
	{
		climate_var = concs;
	}
private:
	int num_of_climate_vars; // Number of columns in the weather table == number of consts to read after func's and betas
	int num_of_gt_vars; // Number of genotype data columns (snp or qtl or locations). Number of betas=number of funcs + number of func * number of gt vars
	int nFunctions;
	int wordLength;
	int PRINT_TRACE = 1;
	int read_flag = 2;//-1
	std::vector<std::string> measurements;
	std::vector<std::string> gr_names;
	GrammarContainer* grc;
	vector<double> climate_var;
	vector<double> beta;
	double MB;
	QString funcs_file_name;
	double *phenotype;
	int *phenomask;
	vector<int> genotype;
	void read_genotype(const int crops) {
		if (crops == 0)
		{
			QFile file(funcs_file_name);
			vector<int> gt;
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
				cout << "Can't open " << funcs_file_name.toStdString() << endl;
				return;
			}
			QTextStream in(&file);
			QString nnm;
			in >> nnm;
			for (size_t i = 0; i < nFunctions * wordLength; ++i) {
				int arg1;
				in >> arg1;
				gt.push_back(arg1);
			}
			if (read_flag > -1) {
				vector<double> be;
				for (size_t i = 0; i < nFunctions + nFunctions * num_of_gt_vars; ++i) {
					double arg2;
					in >> arg2;
					be.push_back(arg2);
				}
				beta = be;
			}
			vector<double> concs;

			for (size_t i = 0; i < 2 * num_of_climate_vars; ++i) {
				double arg3;
				in >> arg3;
				concs.push_back(arg3);
			}
			climate_var = concs;
			if (read_flag > 0) {
				in >> MB;
				for (size_t i = nFunctions; i < nFunctions + nFunctions * num_of_gt_vars; ++i) {
					double be = (beta[i] > 0.0) ? beta[i] : -beta[i];
					beta[i] = (be < MB) ? 0.0 : beta[i];
				}
			}
			genotype = gt;
			file.close();
		}
		else
		{
			QSettings sett(funcs_file_name, QSettings::IniFormat);
			sett.beginGroup("DEEP");
			int size = sett.beginReadArray("x");
			double arg_cv;
			for (int i = 0; i < size; ++i) {
				sett.setArrayIndex(i);
				int arg;
				arg = sett.value("value").toInt();
				genotype.push_back(arg);
			}
			sett.endArray();

			size = sett.beginReadArray("beta");
			if (size != 0)
			{
				for (int i = 0; i < size; ++i) {
					sett.setArrayIndex(i);
					double arg;
					arg = sett.value("value").toDouble();
					beta.push_back(arg);
				}	
				sett.endArray();
			}
			else
			{	
				sett.endArray();
				double arg;
			        arg = sett.value("beta").toDouble();
			        beta.push_back(arg);
			}
			
			arg_cv = sett.value("TP2D", 25).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("TBD", 15).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("Pbase", 40).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("cpp", 18).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("Sbase", 40).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("TCD", 25).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("TP1D", 15).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("Pceil", 40).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("cppx", 18).toDouble();
			climate_var.push_back(arg_cv);
			arg_cv = sett.value("Sceil", 40).toDouble();
			climate_var.push_back(arg_cv);

			MB = sett.value("MB", 1.0).toDouble();
			for (size_t i = nFunctions; i < nFunctions + nFunctions * num_of_gt_vars; ++i) {
				double be = (beta[i] > 0.0) ? beta[i] : -beta[i];
				beta[i] = (be < MB) ? 0.0 : beta[i];
			}
			sett.endGroup();
		}
	}
};
