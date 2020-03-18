#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <exception>
#include <ctime>

/*  
	Кодер хранит g(x), deg(g(x)) = r; 						argv[1]
	Декодер хранит g(x), deg(g(x)) = r, k; 					argv[2]

Note: (1) - используется в коде, 
	  (2) - по заданию (заменила)

	k = l - длина информационного сообщения m(x) 			argv[3]
	r = r - степень порождающего многочлена g(x)
	n = k - длина кодового слова = l + r
	n = k + r

	  (*) k >=< n

	e(x) = argv[4]

type poly: 1 - g(x)
	  	   2 - m(x)
*/
struct Polynom
{
	int max_degree;
	int length;
	std::vector<int> coefs;

};

Polynom f_poly;
Polynom g_poly; 
Polynom g_poly_main; // основа
Polynom m_poly;
Polynom c_poly;
Polynom a_poly;
Polynom e_poly;
Polynom b_poly;
Polynom s_poly;


/* 
	Создание полиномов по входному вектору
*/
Polynom create_polynom(std::string bits, bool is_g);
Polynom create_polynom(std::string bits, int k);
std::vector<Polynom> create_polynom(int len); 

/*
	Операторы для работы с полиномами
*/
std::ostream& operator<<(std::ostream& os, const Polynom& poly);  // вывод
Polynom operator<(const Polynom& a, int deg);					  // сдвиг полинома (равноправно умножению на x^deg)
Polynom operator%(const Polynom& a, const Polynom& b);			  // взятите полинома a модулю b
Polynom operator-(const Polynom& a, const Polynom& b);			  // вычитание
Polynom operator+(const Polynom& a, const Polynom& b);			  // сложение
bool operator==(const Polynom& a, const Polynom& b);			  // проверка на равенство

int C(int n, int m);                                              // бином из n по m (не используется в программе)
int get_ones(const Polynom& poly);                                // подсчет веса вектора (кол-во единиц в векторе), чтобы нечетное кол-во ошибок (для допа)
Polynom dec_to_bin(int num, int len);                             // перевод из 10 в 2 систему счисления, чтобы перебрать все возможные ошибки (для допа)

int 
main(int argc, const char* argv[])
{
	if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL)
	{
		std::cout << "input is null\n";
		exit(1);
	}
	try
	{
		// формирования полиномов по входным данным
		g_poly_main = create_polynom(argv[1], true);
		f_poly = create_polynom(argv[1], true); // для допа 1011 или 1101
		g_poly = f_poly < 1;
		g_poly = g_poly + f_poly; // для допа
		m_poly = create_polynom(argv[2], std::stoi(argv[3]));
	}
	catch(const std::exception& err)
	{
		std::cerr << err.what() << "\n";
		exit(1);
	}
	std::cout << "g(x) = " << g_poly_main;
	std::cout << "m(x) = " << m_poly;

	// формирование c(x) = m(x)*x^r mod g(x)
	Polynom tmp_poly = m_poly < g_poly_main.max_degree;
	std::cout << "m(x) * x^" << g_poly_main.max_degree << " = " << tmp_poly;

	c_poly = tmp_poly % g_poly_main;
	std::cout << "c(x) = " << c_poly;

	// формирование a(x) = m(x)*x^r + c(x)
	a_poly = tmp_poly + c_poly;
	std::cout << "a(x) = " << a_poly;

	e_poly = create_polynom(argv[4], false);
	std::cout << "e(x) = " << e_poly;
	if (e_poly.length != a_poly.length)
	{
		std::cout << "e has incorrect size\n";
		exit(1);
	}
	// формирования b(x) - вектор после прохождения через канал связи 
	// (на входе декодера)
	b_poly = a_poly + e_poly; 
	std::cout << "b(x) = " << b_poly;
	// формирования Е - решение декордера 
	s_poly = b_poly % g_poly_main;
	std::cout << "s(x) = " << s_poly;

	if (s_poly.length != 0)
		std::cout << "E = 1\n";
	else
		std::cout << "E = 0\n";
	std::cout << "--------------------------------------\n\n";

 	///////////////Доп задание
 	// формирование с(х) для f(x) и g(x)
	Polynom tmp_poly_f = m_poly < f_poly.max_degree;
	Polynom tmp_poly_g = m_poly < g_poly.max_degree;
	//std::cout << "\n----------\nfor f_polynom:\n m(x) * fx^" << f_poly.max_degree << " = " << tmp_poly_f;

	Polynom c_poly_f = tmp_poly_f % f_poly;
	Polynom c_poly_g = tmp_poly_g % g_poly;
	//std::cout << "fc(x) = " << c_poly_f;

	// формирование a(х) для f(x) и g(x)
	Polynom a_poly_f = tmp_poly_f + c_poly_f;
	Polynom a_poly_g = tmp_poly_g + c_poly_g;

	//std::cout << "fa(x) = " << a_poly_f;

	// для f(x)
	int err_null_f = 0; // кол-во векторов ошибок, при котороых Е = 0
	// формирование всех векторов ошибок заданной длины
	// т.е длины, равной a(x) (макс.степень + 1)
	// вектора, которые в 10 с.с.ч от 1 до 2^(макс.степень + 1)
	std::vector<Polynom> e_poly_dop_f = create_polynom(a_poly_f.length);

	// вектора ошибок, при которых решение декодера Е = 0
	std::vector<Polynom> err_null_v_f; 
	for (int i = 0; i < (int)e_poly_dop_f.size(); i++)
	{
		if (get_ones(e_poly_dop_f[i]) % 2 != 0)
		{
			Polynom s_tmp_f = (a_poly_f + e_poly_dop_f[i]) % f_poly;
			//std::cout <<"-------------------------------\n";
			//std::cout << "e_dop_f(x) = " << e_poly_dop_f[i];
			//std::cout << "s_tmp_f(x) = " << s_tmp_f;
			//std::cout <<"-------------------------------\n";
			if (s_tmp_f.coefs.size() == 0)
			{
				err_null_f++;
				err_null_v_f.push_back(e_poly_dop_f[i]);
			}
		}

	}

	// аналогично для g(x)
	int err_null_g = 0;
	std::vector<Polynom> e_poly_dop_g = create_polynom(a_poly_g.length);
	std::vector<Polynom> err_null_v_g;
	for (int i = 0; i < (int)e_poly_dop_g.size(); i++)
	{
		if (get_ones(e_poly_dop_g[i]) % 2 != 0)
		{
			Polynom s_tmp_g = (a_poly_g + e_poly_dop_g[i]) % g_poly;
			//std::cout <<"-------------------------------\n";
			//std::cout << "e_dop_g(x) = " << e_poly_dop_g[i];
			//std::cout << "s_tmp_g(x) = " << s_tmp_g;
			//std::cout <<"-------------------------------\n";
			if (s_tmp_g.coefs.size() == 0)
			{
				err_null_g++;
				err_null_v_g.push_back(e_poly_dop_g[i]);
			}
		}

	}

	// наглядный вывод векторов ошибок, при которых Е = 0
	std::cout << "\n\nm(x) = " << m_poly;
 	std::cout << "f(x) = " << f_poly;
 	std::cout << "g(x) = " << g_poly;
 	std::cout << "\n";
 	std::cout << "for f(x): c(x) = " << c_poly_f;
 	std::cout << "for g(x): c(x) = " << c_poly_g;
 	std::cout << "\n";
 	std::cout << "for f(x): a(x) = " << a_poly_f;
 	std::cout << "for g(x): a(x) = " << a_poly_g;
 	std::cout << "\n";

	std::cout << "FOR f(x)\nERROR IS NULL " << err_null_f <<"\n";
	// std::cout << "f(x) = " << f_poly;
	// std::cout << "af(x) = " << a_poly_f << "\n";
	for (int i = 0; i < (int)err_null_v_f.size(); i++)
	{
		std::cout << "a(x) = " << a_poly_f;
		std::cout << "e(x) = " << err_null_v_f[i];
		Polynom s_tmp_f = (a_poly_f + err_null_v_f[i]) % f_poly;
		std::cout << "b(x) = " << a_poly_f + err_null_v_f[i];
		std::cout << "s(x) = " << s_tmp_f << "\n";
	}

	std::cout << "FOR g(x)\nERROR IS NULL " << err_null_g <<"\n";
	std::cout << "g(x) = " << g_poly;
	std::cout << "a(x) = " << a_poly_g << "\n";
	for (int i = 0; i < (int)err_null_v_g.size(); i++)
	{
		std::cout << "e(x) = " << err_null_v_g[i];
		Polynom s_tmp_g = (a_poly_g + err_null_v_g[i]) % g_poly;
		std::cout << "s(x) = " << s_tmp_g << "\n";
	}
	return 0;
}

int get_ones(const Polynom& poly)
{
	int ones = 0;
	for (int i = 0; i < (int)poly.coefs.size(); i++)
		if (poly.coefs[i] == 1)
			ones++;
	return ones;
}

Polynom dec_to_bin(int num, int len)
{
	Polynom res;
	for (int i = 0; i < len; i++)
		res.coefs.push_back(0);
	int i = 0;
	int num_tmp = num;
    while (num_tmp != 0)
    {
       	int remain = num_tmp % 2;
       	res.coefs[res.coefs.size() - i - 1] = remain;
       	i++;
       	num_tmp = num_tmp / 2;
    }
    res.length = len;
    i = 0;
    while (res.coefs[i] != 1 && i < (int)res.coefs.size())
    	i++;
    res.max_degree = res.coefs.size() - i - 1;
    //std::cout << num << " = " << res;
    return res;
}

std::vector<Polynom> create_polynom(int len)
{
	std::vector<Polynom> res;
	for (int i = 1; i < pow(2, len); i++)
	{
		Polynom tmp = dec_to_bin(i, len);
		res.push_back(tmp);
	}
	return res;
}
int C(int n, int m)
{
	int f_n = 1;
	int f_nm = 1;
	int f_m = 1;
	for (int i = 1; i <= n; i++)
		f_n *= i;
	for (int i = 1; i <= n - m; i++)
		f_nm *= i;
	for (int i = 1; i <= m; i++)
		f_m *= i;

	int C_res = f_n/(f_nm * f_m);
	return C_res;
}
Polynom create_polynom(std::string bits, bool is_g)
{
	Polynom res;
	float p = 0;
	bool p_flag = false;
	int begin = 0;
	if (is_g == true)
	{
		if (bits[0] == '1')
			res.length = bits.length();
		else 
			res.length = bits.length() - bits.find_first_of('1');	
		begin = bits.find_first_of('1');
		if (bits.find_first_of('1') != std::string::npos)
			res.max_degree = bits.length() - bits.find_first_of('1') - 1;
		else 
			res.max_degree = 0;
	}
	if (is_g == false)
	{
		if (bits.find_first_of('.') != std::string::npos)
		{
			p_flag = true;
			p = std::stof(bits);
			std::srand(std::time(nullptr));
			for (int i = 0; i < (int)a_poly.coefs.size(); i++)
			{
				//std::srand(std::time(nullptr));
				float p_tmp = (rand() % 100)/(100 * 1.0);
				if (p_tmp < p)
					res.coefs.push_back(0);
				else
					res.coefs.push_back(1);
				std::cout << "current p(" << i << ") = " << p_tmp <<"\n";
			}
			res.length = res.coefs.size();
			res.max_degree = bits.length() - bits.find_first_of('1') - 1;
		}
		else
		{
			res.length = bits.length();
			if (bits.find_first_of('1') != std::string::npos)
				res.max_degree = bits.length() - bits.find_first_of('1') - 1;
			else 
				res.max_degree = 0;
			begin = 0;
		}
	}
	if (p_flag == false)
	{
		for (int i = begin; i < (int)bits.length(); i++)
		{
			if (bits.at(i) != '0' && bits.at(i) != '1')
				throw std::logic_error("Error *create_polynom*: bits[i] != 0 != 1");
			
			if (bits.at(i) == '0')
				res.coefs.push_back(0);
			else
				res.coefs.push_back(1);
		}
	}
	return res;
}

Polynom create_polynom(std::string bits, int k)
{
	if (k <= 0)
		throw std::logic_error("m doesn't exist\n");
	Polynom res;
	if (k < (int)bits.length())
		bits.erase(bits.cbegin(), bits.cbegin() + (bits.length() - k));
	res.length = k;
	if (bits.find_first_of('1') != std::string::npos)
		res.max_degree = bits.length() - bits.find_first_of('1') - 1;
	else 
		res.max_degree = 0;

	for (int i = 0; i < (int)bits.length(); i++)
	{
		if (bits.at(i) != '0' && bits.at(i) != '1')
			throw std::logic_error("Error *create_polynom*: bits[i] != 0 != 1");
		
		if (bits.at(i) == '0')
			res.coefs.push_back(0);
		else
			res.coefs.push_back(1);
	}
	return res;
}

std::ostream& operator<<(std::ostream& os, const Polynom& poly)
{
	if (poly.coefs.size() == 0)
	{
		os << "0\n";
		return os;
	}
	for (int i = 0; i < (int)poly.coefs.size(); i++)
		os << poly.coefs[i];
	os << "   deg = " << poly.max_degree << "  length = " << poly.length << "\n";
    return os;
}

Polynom operator<(const Polynom& a, int deg)
{
	Polynom res;
	res.max_degree = a.max_degree + deg;
	for (int i = 0; i < (int)a.coefs.size(); i++)
		res.coefs.push_back(a.coefs[i]);
	for (int i = 0; i < deg; i++)
		res.coefs.push_back(0);
	res.length = res.coefs.size();
	return res;
}

Polynom operator-(const Polynom& a, const Polynom& b)
{
	Polynom res;
	int z = 0;
	res.max_degree = 0;
	res.length = 0;
	if (a == b) return res;
	for (int i = 0; i < (int)a.coefs.size(); i++)
	{
		if (i < (int)(a.coefs.size() - b.coefs.size()))			
			res.coefs.push_back((a.coefs[i]));		
		else
			res.coefs.push_back(abs((a.coefs[i] - b.coefs[i - (a.coefs.size() - b.coefs.size())]) % 2));
	}
	while (res.coefs[z] == 0)
		z++;
	if (z < (int)res.coefs.size())
		res.coefs.erase(res.coefs.cbegin(), res.coefs.cbegin() + z);
	res.length = res.coefs.size();
	res.max_degree = res.coefs.size() - 1;
	return res;
}

bool operator==(const Polynom& a, const Polynom& b) 
{
	if (a.length != b.length || a.max_degree != b.max_degree)
		return false;

	for (int i = 0; i < a.length; i++)
	{
		if (a.coefs[i] != b.coefs[i])
			return false;
	}
	return true;
}

Polynom operator+(const Polynom& a, const Polynom& b)
{
	Polynom res;
	res.max_degree = a.max_degree > b.max_degree ? a.max_degree : b.max_degree;
	for (int i = 0; i < (int)a.coefs.size(); i++)
	{
		if (i < (int)(a.coefs.size() - b.coefs.size()))			
			res.coefs.push_back((a.coefs[i]));
		
		else
			res.coefs.push_back((a.coefs[i] + b.coefs[i - (a.coefs.size() - b.coefs.size())]) % 2);
	}
	res.length = res.coefs.size();
	return res;
}
Polynom operator%(const Polynom& a, const Polynom& b)
{
	Polynom remain;
	Polynom a_tmp = a;
	while (a_tmp.max_degree >= b.max_degree)
	{
		int deg = a_tmp.max_degree - b.max_degree;
		Polynom tmp = b < deg;
		remain = a_tmp - tmp;
		a_tmp = remain;
	}
	return remain;
}