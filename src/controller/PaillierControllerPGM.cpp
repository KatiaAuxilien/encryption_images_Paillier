/******************************************************************************
 * ICAR_Interns_Library
 *
 * File : PaillierControllerPGM.cpp
 *
 * Description : Implementation of the PaillierControllerPGM class, which is a
 * controller for the Paillier cryptosystem applied to PGM (Portable Gray Map)
 * images.
 *
 *
 * Author : Katia Auxilien
 *
 * Mail : katia.auxilien@mail.fr
 *
 * Date : 29 Mai 2024, 13:57:00
 *
 *******************************************************************************/
#include "../../include/controller/PaillierControllerPGM.hpp"

PaillierControllerPGM::PaillierControllerPGM()
{
	init();
};

PaillierControllerPGM::~PaillierControllerPGM(){};

void PaillierControllerPGM::init()
{
	this->c_file = NULL;
	this->c_key_file = NULL;
	this->model = PaillierModel::getInstance();
	this->view = commandLineInterface::getInstance();
}

const char *PaillierControllerPGM::getCFile() const
{
	return c_file;
}

void PaillierControllerPGM::setCFile(char *newCFile)
{
	delete[] c_file;
	c_file = new char[strlen(newCFile) + 1];
	strcpy(c_file, newCFile);
}

void PaillierControllerPGM::checkParameters(char *arg_in[], int size_arg, bool param[])
{
	// if (arg_in == NULL || param == NULL) // Sécurité pointeurs.
	// {
	// this->view->getInstance()->error_failure("checkParameters : arguments null.");
	// exit(EXIT_FAILURE);
	// }

	this->convertToLower(arg_in, size_arg);

	/********** Initialisation de param[] à false. *************/
	for (int i = 0; i < 6; i++)
	{
		param[i] = false;
	}

	/********** Initialisation de param[] à false. *************/
	for (int i = 0; i < size_arg; i++)
	{
		if (strcmp(arg_in[i], "-h") == 0 || strcmp(arg_in[i], "-help") == 0)
		{
			param[6] = true;
		}
	}
	if (!param[6])
	{
		/**************** First param ******************/
		if (!strcmp(arg_in[1], "e") || !strcmp(arg_in[1], "enc") || !strcmp(arg_in[1], "encrypt") || !strcmp(arg_in[1], "encryption"))
		{
			param[0] = true;
		}
		else if (!strcmp(arg_in[1], "d") || !strcmp(arg_in[1], "dec") || !strcmp(arg_in[1], "decrypt") || !strcmp(arg_in[1], "decryption"))
		{
			param[0] = false;
			param[1] = true;
		}
		else
		{
			this->view->getInstance()->error_failure("The first argument must be e, enc, encrypt, encryption or d, dec, decrypt, decryption (the case don't matter)\n");
			exit(EXIT_FAILURE);
		}
		/**************** ... param ******************/

		bool isFilePGM = false;
		bool isFileBIN = false;

		int i = 2;
		if (param[0] == true && (strcmp(arg_in[i], "-k") && strcmp(arg_in[i], "-key")))
		{
			uint64_t p = this->check_p_q_arg(arg_in[2]);
			if (p == 1)
			{
				exit(EXIT_FAILURE);
			}
			this->model->getInstance()->setP(p);

			uint64_t q = this->check_p_q_arg(arg_in[3]);
			if (q == 1)
			{
				exit(EXIT_FAILURE);
			}
			this->model->getInstance()->setQ(q);

			uint64_t n = p * q;
			this->model->getInstance()->setN(n);
			Paillier<uint64_t, uint64_t> tempPaillier;
			this->model->getInstance()->setPaillierGenerationKey(tempPaillier);

			uint64_t pgc_pq = this->model->getInstance()->getPaillierGenerationKey().gcd_64t(p * q, (p - 1) * (q - 1));

			if (pgc_pq != 1)
			{
				string msg = "pgcd(p * q, (p - 1) * (q - 1))= " + to_string(pgc_pq) + "\np & q arguments must have a gcd = 1. Please retry with others p and q.\n";
				this->getView()->error_failure(msg);
				exit(EXIT_FAILURE);
			}
			uint64_t lambda = this->model->getInstance()->getPaillierGenerationKey().lcm_64t(p - 1, q - 1);

			this->model->getInstance()->setLambda(lambda);

			i = 4;
			isFileBIN = true;
		}

		for (i = i; i < size_arg; i++)
		{
			// TODO : Gérer les cas où il y a deux fois -k ou -? ... dans la ligne de commande. pour éviter les erreurs.

			if ((!isFileBIN && !strcmp(arg_in[i], "-k")) || !strcmp(arg_in[i], "-key") || (param[1] == true && endsWith(arg_in[i], ".bin")))
			{ // TODO : 2 cas où on veut check si il y a un argument .bin après le -k OU après le premier argument d

				if (!strcmp(arg_in[i], "-k") || !strcmp(arg_in[i], "-key"))
				{
					this->setCKeyFile(arg_in[i + 1]);
					param[1] = true;
					i++;
				}
				if (param[1] == true)
				{
					this->setCKeyFile(arg_in[i]);
				}
				/****************** Check .bin file **************************/
				string s_key_file = this->getCKeyFile();
				ifstream file(this->getCKeyFile());
				if (!file || !this->endsWith(s_key_file, ".bin"))
				{
					this->view->getInstance()->error_failure("The argument after -k or dec must be an existing .bin file.\n");
					exit(EXIT_FAILURE);
				}
				isFileBIN = true;
			}
			else if (!strcmp(arg_in[i], "-d") || !strcmp(arg_in[i], "-distr") || !strcmp(arg_in[i], "-distribution"))
			{
				param[2] = true;
			}
			else if (!strcmp(arg_in[i], "-hexp") || !strcmp(arg_in[i], "-histogramexpansion"))
			{
				param[3] = true;
			}
			else if (!strcmp(arg_in[i], "-olsbr32") || !strcmp(arg_in[i], "-optlsbr32"))
			{
				param[4] = true;
			}
			else if (!strcmp(arg_in[i], "-olsbr16") || !strcmp(arg_in[i], "-optlsbr16"))
			{
				param[5] = true;
			}
			else if (this->endsWith(arg_in[i], ".pgm") && !isFilePGM)
			{
				this->setCFile(arg_in[i]);
				string s_file = this->getCFile();
				ifstream file(this->getCFile());
				if (!file)
				{
					this->view->getInstance()->error_failure("The arguments must have an existing .pgm file.\n");
					exit(EXIT_FAILURE);
				}
				isFilePGM = true;
			
			}else{
				this->view->getInstance()->error_failure("The argument "+ std::string(arg_in[i]) +" is not available.\n");
				exit(EXIT_FAILURE);
			}
		}

		if (!isFilePGM)
		{
			this->view->getInstance()->error_failure("The arguments must have a .pgm file.\n");
			exit(EXIT_FAILURE);
		}
		if (param[1] == true && !isFileBIN)
		{
			this->view->getInstance()->error_failure("The argument after -k or dec must be a .bin file.\n");
			exit(EXIT_FAILURE);
		}
	}
}

void PaillierControllerPGM::printHelp()
{
	this->view->getInstance()->help("./PaillierPgm.out\nNAME\n \t./PaillierPgm.out - Encrypt or decrypt .pgm file\n\nSYNOPSIS\n\t./PaillierPgm.out [MODE]... [OPTIONS]... [FILE]...	\n\nDESCRIPTION\n	Program to encrypt or decrypt portable graymap file format.	\n\nOPTIONS	\n\t./Paillier_pgm_main.out encryption [ARGUMENTS] [FILE.PGM]	\n\t./Paillier_pgm_main.out encrypt [ARGUMENTS] [FILE.PGM]	\n\t./Paillier_pgm_main.out enc [ARGUMENTS] [FILE.PGM]	\n\t./Paillier_pgm_main.out e [ARGUMENTS] [FILE.PGM]\n\t\t encrypt file.\n	\n\t./Paillier_pgm_main.out decryption [PRIVATE KEY FILE .BIN] [FILE.PGM] [ARGUMENTS]	\n\t./Paillier_pgm_main.out decrypt [PRIVATE KEY FILE .BIN] [FILE.PGM] [ARGUMENTS]	\n\t./Paillier_pgm_main.out dec [PRIVATE KEY FILE .BIN] [FILE.PGM] [ARGUMENTS]	\n\t./Paillier_pgm_main.out d [PRIVATE KEY FILE .BIN] [FILE.PGM] [ARGUMENTS]*\n\t\tdecrypt file.	\n\t\tThe image to encrypt or to decrypt can be specify after the key or the options, or at the end.	\n	\n\t./Paillier_pgm_main.out encryption [p] [q] [FILE.PGM]	\n\t\t Encryption mode where you specify p and q arguments. p and q are prime number where pgcd(p * q,p-1 * q-1) = 1.	\n\n\t-k, -key	\n\t\t specify usage of private or public key, followed by file.bin, your key file. Encryption mode where you specify your public key file with format .bin.	\n\n\t./Paillier_pgm_main.out encryption -k [PUBLIC KEY FILE .BIN] [FILE.PGM]	\n\t./Paillier_pgm_main.out encryption -key [PUBLIC KEY FILE .BIN] [FILE.PGM]	\n\t./Paillier_pgm_main.out decryption -k [PRIVATE KEY FILE .BIN] [FILE.PGM]	\n\t\tdecryption mode where you specify your private key with format .bin. The option -k is optional, because it\'s obligatory to specify private key at decryption.\n\n\t-distribution, -distr, -d	\n\t\tto split encrypted pixel on two pixel.\n	\n\t-histogramexpansion,-hexp	\n\t\tto specify during **encryption** that we want to transform the histogram befor image encryption.\n\n\t-optlsbr32, -olsbr32\n\tto specify that we want to use bit compression with encrypted through optimized r generation mod(32), so free 5 LSB.\n\n\t-optlsbr16, -olsbr16\n\tto specify that we want to use bit compression with encrypted through optimized r generation mod(16), so free 4 LSB.\n\n");
}

uint8_t PaillierControllerPGM::histogramExpansion(OCTET ImgPixel, bool recropPixels)
{
	uint8_t pixel;
	if (recropPixels)
	{
		uint64_t n = model->getInstance()->getPublicKey().getN();
		pixel = (ImgPixel * n) / 256;
	}
	else
	{
		pixel = ImgPixel;
	}
	return pixel;
}

/*********************** Chiffrement/Déchiffrement ***********************/

uint16_t *PaillierControllerPGM::compressBits_16bpp(uint16_t *ImgInEnc, int nb_lignes, int nb_colonnes, int bitsCompressed)
{
	int nbPixel = nb_colonnes * nb_lignes;
	if (nbPixel > 132710400)
	{
		this->view->getInstance()->error_failure("Maximum image size is 132 710 400 pixels.\n");
		exit(EXIT_FAILURE);
	}

	// Taille max image 15360*8640l
	// std::bitset<2 123 366 400> finalSet;
	std::bitset<2123366400> *finalSet = new std::bitset<2123366400>;

	if(bitsCompressed > 16 || bitsCompressed < 0)
	{
		this->view->getInstance()->error_failure("Bits compressed must be between 0 and 16.\n");
		exit(EXIT_FAILURE);
	}

	int j = 0;
	std::bitset<16> tempSet;

	for (int i = 0; i < nbPixel; i++)
	{
		tempSet = ImgInEnc[i];
		for (int k = 15; k >= bitsCompressed; k--)
		{
			finalSet->set(j, tempSet[k]);
			j++;
		}
	}

	int size_ImgIn11bits = nbPixel * (16 - bitsCompressed);
	int size_ImgOutEnc16bits = ceil((double)size_ImgIn11bits/16);

	uint16_t * ImgOutEnc16bits = new uint16_t[size_ImgOutEnc16bits];

	int k = 0;
	for (int i = 0; i < size_ImgOutEnc16bits; i++)
	{
		std::bitset<16> SetImgOutEnc16bits;
		for (int l = 0; l < 16; l++)
		{
			SetImgOutEnc16bits.set(l, (*finalSet)[k]);
			k++;
		}

		ImgOutEnc16bits[i] = (uint16_t)SetImgOutEnc16bits.to_ulong();
	}

	return ImgOutEnc16bits;
}

uint16_t *PaillierControllerPGM::decompressBits_16bpp(uint16_t *ImgInEnc, int nb_lignes, int nb_colonnes, int nTailleOriginale, int bitsCompressed)
{
	if(bitsCompressed > 16 || bitsCompressed < 0)
	{
		this->view->getInstance()->error_failure("Bits compressed must be between 0 and 16.\n");
		exit(EXIT_FAILURE);
	}

	int sizeComp = nb_lignes * nb_colonnes;
	std::bitset<2123366400> *setTemp = new std::bitset<2123366400>;

	int j = 0;
	for (int i = 0; i < sizeComp; i++)
	{
		std::bitset<16> setImg = ImgInEnc[i];
		for (int k = 0; k < 16; k++)
		{
			setTemp->set(j, setImg[k]);
			j++;
		}
	}

	// std::cout << setTemp << std::endl;

	// step 2 : On écrit ce bitset dans le tableau originalImg
	// int sizeOriginal = nb_lignes * nb_colonnes * 16;
	int sizeOriginal = nTailleOriginale * 16;


	uint16_t *originalImg = new uint16_t[sizeOriginal];

	j = 0;
	for (int i = 0; i < sizeOriginal; i++)
	{
		std::bitset<16> setImg;
		for (int k = 15; k >= bitsCompressed; k--)
		{
			setImg.set(k, (*setTemp)[j]);
			j++;
		}
		originalImg[i] = (uint16_t)setImg.to_ulong();
		// std::cout << setImg << std::endl;
	}

	return originalImg;
}

pair<int, int> PaillierControllerPGM::decomposeDimension(int n){
    int facteur1 = 1, facteur2 = n;
    for (int i = 2; i <= n / 2; i++) {
        if (n % i == 0) {
            int diff = abs(i - n / i);
            int diff_actuelle = abs(facteur1 - facteur2);
            if (diff < diff_actuelle) {
                facteur1 = i;
                facteur2 = n / i;
            }
        }
    }
    return make_pair(facteur1, facteur2);
}



uint8_t *PaillierControllerPGM::compressBits_8bpp(uint16_t *ImgInEnc, int nb_lignes, int nb_colonnes, int bitsCompressed)
{
	int nbPixel = nb_colonnes * nb_lignes;
	if (nbPixel > 132710400)
	{
		this->view->getInstance()->error_failure("Maximum image size is 132 710 400 pixels.\n");
		exit(EXIT_FAILURE);
	}

	// Taille max image 15360*8640l
	// std::bitset<2 123 366 400> finalSet;
	std::bitset<2123366400> *finalSet = new std::bitset<2123366400>;

	if(bitsCompressed > 16 || bitsCompressed < 0)
	{
		this->view->getInstance()->error_failure("Bits compressed must be between 0 and 16.\n");
		exit(EXIT_FAILURE);
	}

	int j = 0;
	std::bitset<16> tempSet;

	for (int i = 0; i < nbPixel; i++)
	{
		tempSet = ImgInEnc[i];
		for (int k = 15; k >= bitsCompressed; k--)
		{
			finalSet->set(j, tempSet[k]);
			j++;
		}
	}

	int size_ImgIn11bits = nbPixel * (16 - bitsCompressed);
	int size_ImgOutEnc16bits = ceil((double)size_ImgIn11bits/16);


	uint8_t * ImgOutEnc8bits = new uint8_t[size_ImgOutEnc16bits *2];

	int k = 0;
	j = 0;
	for (int i = 0; i < size_ImgOutEnc16bits * 2; i++)
	{
		std::bitset<8> SetImgOutEnc8bits;
		for (int l = 0; l < 8; l++)
		{
			SetImgOutEnc8bits.set(l, (*finalSet)[k]);
			k++;
		}
		std::bitset<8> SetImgOutEnc8bits2;
		for (int l = 0; l < 8; l++)
		{
			SetImgOutEnc8bits2.set(l, (*finalSet)[k]);
			k++;
		}

		ImgOutEnc8bits[j] = (uint8_t)SetImgOutEnc8bits.to_ulong();
		j++;
		ImgOutEnc8bits[j] = (uint8_t)SetImgOutEnc8bits2.to_ulong();
		j++;
	}

	return ImgOutEnc8bits;
}

uint16_t *PaillierControllerPGM::decompressBits_8bpp(uint8_t *ImgInEnc, int nb_lignes, int nb_colonnes, int nTailleOriginale, int bitsCompressed)
{
	if(bitsCompressed > 16 || bitsCompressed < 0)
	{
		this->view->getInstance()->error_failure("Bits compressed must be between 0 and 16.\n");
		exit(EXIT_FAILURE);
	}

	int sizeComp = nb_lignes * nb_colonnes;
	std::bitset<2123366400> *setTemp = new std::bitset<2123366400>;

	int l = 0;
	int j = 0;
	for (int i = 0; i < sizeComp; i++)
	{
		std::bitset<8> setImg = ImgInEnc[l];
		for (int k = 0; k < 8; k++)
		{
			setTemp->set(j, setImg[k]);
			j++;
		}
		l++;
		std::bitset<8> setImg2 = ImgInEnc[l];
		for (int k = 0; k < 8 ; k++){
			setTemp->set(j, setImg2[k]);
			j++;
		}
		l++;
	}

	// std::cout << setTemp << std::endl;

	// step 2 : On écrit ce bitset dans le tableau originalImg
	// int sizeOriginal = nb_lignes * nb_colonnes * 16;
	int sizeOriginal = nTailleOriginale * 16;


	uint16_t *originalImg = new uint16_t[sizeOriginal];

	j = 0;
	for (int i = 0; i < sizeOriginal; i++)
	{
		std::bitset<16> setImg;
		for (int k = 15; k >= bitsCompressed; k--)
		{
			setImg.set(k, (*setTemp)[j]);
			j++;
		}
		originalImg[i] = (uint16_t)setImg.to_ulong();
		// std::cout << setImg << std::endl;
	}

	return originalImg;
}
